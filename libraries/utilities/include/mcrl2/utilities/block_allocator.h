// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file block_allocator.h
/// \brief Fixed-size block allocator with per-thread allocation state.

#ifndef MCRL2_UTILITIES_BLOCK_ALLOCATOR_H_
#define MCRL2_UTILITIES_BLOCK_ALLOCATOR_H_

#include "mcrl2/utilities/noncopyable.h"
#include "mcrl2/utilities/thread_local.h"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <mutex>
#include <new>
#include <type_traits>
#include <vector>

namespace mcrl2::utilities
{

namespace detail
{

/// Sentinel written to `next` to identify freed entries during consolidation.
static constexpr uintptr_t BlockAllocSentinel = std::numeric_limits<uintptr_t>::max();

/// Number of entries per shared free chunk handed between threads.
static constexpr std::size_t FreeChunkSize = 1000;

/// Union entry: active as `value` while live, as `next` while on a freelist.
template <typename T>
union BlockEntry
{
  T value;
  BlockEntry* next;

  BlockEntry() noexcept : next(nullptr) {}
  ~BlockEntry() noexcept {}
};

/// A fixed-size block of N entries linked in a singly-linked list.
template <typename T, std::size_t N>
struct Block
{
  std::array<BlockEntry<T>, N> data;
  Block<T, N>* next = nullptr;
};

/// The shared block list and free chunks. Protected by the allocator mutex.
template <typename T, std::size_t N>
struct BlockList
{
  Block<T, N>* head = nullptr;

  /// Heads of linked chains of freed entries available for redistribution.
  std::vector<BlockEntry<T>*> free_chunks;

  BlockList() = default;

  ~BlockList() noexcept
  {
    Block<T, N>* block = head;
    while (block != nullptr)
    {
      Block<T, N>* next = block->next;
      delete block;
      block = next;
    }
  }

  BlockList(BlockList&&) = default;
  BlockList& operator=(BlockList&&) = default;
};

/// Per-thread allocation state: current bump block and thread-local freelist.
template <typename T, std::size_t N>
struct ThreadLocalAllocState
{
  Block<T, N>* current_block = nullptr;
  std::size_t bump_offset = N;   ///< N or greater means the block is exhausted.
  BlockEntry<T>* free_head = nullptr;
};

/// No-op mutex used when ThreadSafe=false.
struct no_op_mutex
{
  void lock() noexcept {}
  void unlock() noexcept {}
};

} // namespace detail

/// \brief Fixed-size block allocator compatible with the STL allocator interface.
///
/// Stores blocks of ElementsPerBlock entries, minimising per-allocation overhead.
/// Maintains per-thread state so allocation and deallocation are contention-free
/// on the common path; the shared mutex is only taken when a new block must be
/// allocated or during consolidate().
///
/// consolidate() must not be called concurrently with any allocations or
/// deallocations.
template <class T,
          std::size_t ElementsPerBlock = 1024,
          bool ThreadSafe = false>
class block_allocator : private noncopyable
{
public:
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  template <class U>
  struct rebind
  {
    using other = block_allocator<U, ElementsPerBlock, ThreadSafe>;
  };

  block_allocator() = default;

  T* allocate(size_type n, const void* hint = nullptr)
  {
    if (n != 1 || hint != nullptr)
    {
      throw std::bad_alloc();
    }
    return allocate_object();
  }

  void deallocate(T* p, size_type)
  {
    deallocate_object(p);
  }

  /// \brief Removes empty blocks and returns the number of blocks removed.
  ///
  /// Must not be called concurrently with allocate or deallocate.
  std::size_t consolidate()
  {
    return remove_free_blocks();
  }

private:
  static constexpr std::size_t N = ElementsPerBlock;

  using Entry     = detail::BlockEntry<T>;
  using Block     = detail::Block<T, N>;
  using BlockList = detail::BlockList<T, N>;
  using LocalState = detail::ThreadLocalAllocState<T, N>;
  using MutexType = std::conditional_t<ThreadSafe, std::mutex, detail::no_op_mutex>;

  // When ThreadSafe=true each thread gets its own LocalState via ThreadLocal.
  // When ThreadSafe=false there is only one thread, so a plain struct suffices.
  using AllocState = std::conditional_t<ThreadSafe, ThreadLocal<LocalState>, LocalState>;

  static Entry* sentinel_ptr() noexcept
  {
    return reinterpret_cast<Entry*>(detail::BlockAllocSentinel);
  }

  LocalState& get_local_state()
  {
    if constexpr (ThreadSafe)
    {
      return *m_alloc_state.get_or_mut([] { return LocalState{}; });
    }
    else
    {
      return m_alloc_state;
    }
  }

  template <typename F>
  void for_each_local_state(const F& func)
  {
    if constexpr (ThreadSafe)
    {
      m_alloc_state.for_each_mut(func);
    }
    else
    {
      func(m_alloc_state);
    }
  }

  T* allocate_object()
  {
    LocalState& state = get_local_state();

    // Fast path 1: pop from the thread-local freelist.
    if (state.free_head != nullptr)
    {
      Entry* e = state.free_head;
      state.free_head = e->next;
      return reinterpret_cast<T*>(e);
    }

    // Fast path 2: bump-allocate from the current thread's block.
    if (state.current_block != nullptr && state.bump_offset < N)
    {
      return reinterpret_cast<T*>(&state.current_block->data[state.bump_offset++]);
    }

    // Refill the thread-local freelist from a shared chunk.
    if (refill_local_free(state) && state.free_head != nullptr)
    {
      Entry* e = state.free_head;
      state.free_head = e->next;
      return reinterpret_cast<T*>(e);
    }

    // Slow path: allocate a new block under the mutex.
    return allocate_new_block(state);
  }

  bool refill_local_free(LocalState& state)
  {
    Entry* chunk;
    {
      std::lock_guard<MutexType> lock(m_mutex);
      if (m_block_list.free_chunks.empty())
      {
        return false;
      }
      chunk = m_block_list.free_chunks.back();
      m_block_list.free_chunks.pop_back();
    }
    state.free_head = chunk;
    return true;
  }

  T* allocate_new_block(LocalState& state)
  {
    std::lock_guard<MutexType> lock(m_mutex);
    Block* block = new Block;
    block->next = m_block_list.head;
    m_block_list.head = block;

    state.current_block = block;
    state.bump_offset = 1;

    return reinterpret_cast<T*>(&block->data[0]);
  }

  void deallocate_object(T* p)
  {
    LocalState& state = get_local_state();
    Entry* e = reinterpret_cast<Entry*>(p);
    e->next = state.free_head;
    state.free_head = e;
  }

  std::size_t remove_free_blocks()
  {
    // Step 1: Walk every thread-local freelist, mark each entry with the
    // sentinel, then reset the per-thread state so threads get fresh blocks
    // after consolidation.
    for_each_local_state([](LocalState& state) {
      Entry* e = state.free_head;
      while (e != nullptr)
      {
        Entry* next = e->next;
        e->next = block_allocator::sentinel_ptr();
        e = next;
      }

      // Mark the entries of the current block that were never bump-allocated
      // (the range [bump_offset, N)) as free too. These slots are not on any
      // freelist and would otherwise neither be reclaimed nor returned to the
      // shared free chunks, leaking them (and pinning their block) until the
      // allocator is destroyed.
      if (state.current_block != nullptr)
      {
        for (std::size_t i = state.bump_offset; i < N; ++i)
        {
          state.current_block->data[i].next = block_allocator::sentinel_ptr();
        }
      }

      state.free_head = nullptr;
      state.current_block = nullptr;
      state.bump_offset = N;
    });

    std::lock_guard<MutexType> lock(m_mutex);

    // Step 2: Mark entries in shared free chunks with the sentinel.
    for (Entry* chunk : m_block_list.free_chunks)
    {
      Entry* e = chunk;
      while (e != nullptr)
      {
        Entry* next = e->next;
        e->next = sentinel_ptr();
        e = next;
      }
    }
    m_block_list.free_chunks.clear();

    // Step 3: Remove blocks where every entry carries the sentinel (all freed).
    std::size_t removed = 0;
    Block** prev = &m_block_list.head;
    while (*prev != nullptr)
    {
      Block* block = *prev;
      bool all_free = true;
      for (const Entry& e : block->data)
      {
        if (e.next != sentinel_ptr())
        {
          all_free = false;
          break;
        }
      }

      if (all_free)
      {
        *prev = block->next;
        delete block;
        ++removed;
      }
      else
      {
        prev = &block->next;
      }
    }

    // Step 4: Rebuild shared free chunks from sentinel-marked entries in
    // surviving blocks, in chains of FreeChunkSize.
    Entry* chunk_head = nullptr;
    Entry* chunk_tail = nullptr;
    std::size_t chunk_len = 0;

    for (Block* block = m_block_list.head; block != nullptr; block = block->next)
    {
      for (Entry& e : block->data)
      {
        if (e.next == sentinel_ptr())
        {
          e.next = nullptr;
          if (chunk_tail != nullptr)
          {
            chunk_tail->next = &e;
          }
          else
          {
            chunk_head = &e;
          }
          chunk_tail = &e;
          ++chunk_len;

          if (chunk_len == detail::FreeChunkSize)
          {
            m_block_list.free_chunks.push_back(chunk_head);
            chunk_head = nullptr;
            chunk_tail = nullptr;
            chunk_len = 0;
          }
        }
      }
    }

    if (chunk_head != nullptr)
    {
      m_block_list.free_chunks.push_back(chunk_head);
    }

    return removed;
  }

  MutexType m_mutex;
  BlockList m_block_list;
  AllocState m_alloc_state;
};

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_BLOCK_ALLOCATOR_H_
