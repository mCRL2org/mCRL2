// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_MEMORY_POOL_H_
#define MCRL2_UTILITIES_MEMORY_POOL_H_

#include "mcrl2/utilities/noncopyable.h"
#include "mcrl2/utilities/spinlock.h"

#include <array>
#include <assert.h>
#include <atomic>
#include <cstdint>
#include <forward_list>
#include <limits>
#include <type_traits>
#include <utility>

namespace mcrl2
{
namespace utilities
{

constexpr static std::uintptr_t FreeListSentinel = std::numeric_limits<std::uintrptr_t>::max();

/// \brief The memory pool allocates elements of size T from blocks.
/// \details When ThreadSafe is true then the thread-safe guarantees will be satisfied.
template <class T, 
          std::size_t ElementsPerBlock = 1024, 
          bool ThreadSafe = false>
class memory_pool : private mcrl2::utilities::noncopyable
{
  /// \brief Stores a given object or an entry in the freelist.
  union Slot
  {
    T object;
    Slot* next;

    Slot() { next = nullptr; }
    ~Slot() {}
  };

public:
  memory_pool()  = default;

  /// \brief Triggers the (possibly non-trivial) destructor of all elements stored
  ///        in the pool.
  ~memory_pool()
  {
    /// Go through the freelist and mark all reachable slots with a special value.
    while (m_first_freeslot != nullptr)
    {
      Slot* currentSlot = m_first_freeslot;
      m_first_freeslot = currentSlot->next;
      currentSlot->next = FreeListSentinel;
    }

    /// For all actual elements stored in the pool trigger the destructor.
    for (auto& block : m_blocks)
    {
      for (auto& slot : block)
      {
        if (slot.next != FreeListSentinel)
        {
          destroy(reinterpret_cast<T*>(&slot));
        }
      }
    }
  }

  /// \brief Reuses memory from block and allocates a new block when
  ///        no slots are free.
  /// \returns A pointer to a block of memory that can store an object of type T.
  T* allocate()
  {
    // Enterting critical section, only allow one process to allocate at the time.
    if (ThreadSafe)
    {
      m_block_mutex.lock();
    }

    if (m_first_freeslot != nullptr)
    {
      Slot* slot = m_first_freeslot;
      m_first_freeslot = slot->next;

      if (ThreadSafe)
      {
        m_block_mutex.unlock();
      }
      return reinterpret_cast<T*>(slot);
    }

    if (m_current_index >= ElementsPerBlock)
    {
      // The whole buffer was used so allocate a new one.
      m_blocks.emplace_front();
      ++m_number_of_blocks;
      m_current_index = 0;
    }

    // The object was last written as this slot is not part of the freelist.
    T* slot = &m_blocks.front()[m_current_index++].object;

    if (ThreadSafe)
    {
      m_block_mutex.unlock();
    }
    assert(contains(slot));
    return slot;
  }

  /// \brief Free the memory used by the given pointer that has been allocated by this pool.
  void deallocate(T* pointer)
  {
    assert(contains(pointer));

    Slot* slot = reinterpret_cast<Slot*>(pointer);
    slot->next = m_first_freeslot;
    m_first_freeslot = slot;
  }

  /// \brief Call the destructor of the pointed to object.
  void destroy(T* p)
  {
    p->~T();
  }

  /// \brief Frees blocks that are no longer storing elements of T.
  /// \returns The number of blocks that have been removed.
  /*std::size_t consolidate()
  {
    /// Go through the freelist and mark all reachable slots with a special value.
    std::size_t freeListLength = count_freelist();
    while (m_first_freeslot != nullptr)
    {
      Slot* currentSlot = m_first_freeslot;
      m_first_freeslot = currentSlot->next;
      currentSlot->next = FreeListSentinel;
    }

    /// Iterate over all blocks and reconstruct the freelist.
    std::size_t numberOfErasedBlocks = 0;

    // Keep track of the last block to be able to erase the current block.
    auto lastBlock = m_blocks.before_begin();

    // The last element of the freelist in the last_block.
    Slot* lastBlockFreeSlot = nullptr;

    auto end = m_blocks.end();
    for (auto it = m_blocks.begin(); it != end; ++it)
    {
      Block& block = *it;

      // Indicate that current block only contains slots in the freelist.
      bool blockOnlyFreelist = true;

      Slot* lastFreeSlot = lastBlockFreeSlot;
      // Iterate over the slots in a block and reconstruct the freelist.
      for (auto& slot : block)
      {
        if (slot.next == FreeListSentinel)
        {
          if (m_first_freeslot == nullptr)
          {
            // Find the first element in the freelist.
            m_first_freeslot = &slot;
            lastFreeSlot = m_first_freeslot;
          }

          // The current slot is the next element in the freelist.
          lastFreeSlot->next = &slot;

          // This is now the end of the list.
          lastFreeSlot = &slot;
          lastFreeSlot->next = nullptr;

          // Check that the length of the new freelist match.
          --freeListLength;
          assert(freeListLength >= 0);
        }
        else
        {
          // Found an element that is not part of the freelist.
          blockOnlyFreelist = false;
        }
      }

      /// Erase blocks that only have elements in the freelist.
      if (blockOnlyFreelist)
      {
        if (it == m_blocks.before_begin())
        {
          // The first block will be removed, so reset firstFreeSlot as it might point into this block.
          m_first_freeslot = nullptr;
        }

        // The current block only has elements in the freelist, so erase it.
        it = m_blocks.erase_after(lastBlock);
        --m_number_of_blocks;

        // Reset the end of the freelist, because it would point into the erased block.
        lastBlockFreeSlot->next = nullptr;

        ++numberOfErasedBlocks;
      }

      lastBlockFreeSlot = lastFreeSlot;
      lastBlock = it;
    }

    return numberOfErasedBlocks;
  }*/

  /// \returns True when thi memory pool has space for at least one more element without allocating
  ///          new memory.
  bool has_free_slots() const noexcept
  {
    // The freelist is not full, i.e. points to some slot or the index has not yet reached the end.
    return m_current_index < ElementsPerBlock || m_first_freeslot != nullptr;
  }

  /// \returns The total number of elements that could be stored in this memory pool.
  std::size_t capacity() const noexcept
  {
    return m_number_of_blocks * ElementsPerBlock;
  }

  // Move constructor and assignment are possible.
  memory_pool(memory_pool&& other) = default;
  memory_pool& operator=(memory_pool&& other) = default;

private:

  /// \brief An array that stores ElementsPerBlock number of objects of type T.
  using Block = std::array<Slot, ElementsPerBlock>;

  /// \brief Indicates the head of the freelist.
  using SlotType = typename std::conditional<ThreadSafe, std::atomic<Slot*>, Slot*>::type;
  SlotType m_first_freeslot = nullptr;

  /// \brief The last slot in the first block that has never been returned by allocate.
  using SizeType = typename std::conditional<ThreadSafe, std::atomic<std::size_t>, std::size_t>::type;
  SizeType m_current_index = ElementsPerBlock;

  /// \brief The list of blocks allocated by this pool.
  std::forward_list<Block> m_blocks;

  /// \brief Ensures that the block list is only modified by a single thread.
  mcrl2::utilities::spinlock m_block_mutex = {};

  /// \brief Equal to the size of the blocks array to prevent iterating over the block list.
  SizeType m_number_of_blocks = 0;

  /// \returns The length of the freelist.
  std::size_t count_freelist()
  {
    Slot* currentSlot = m_first_freeslot;
    std::size_t freelist_length = 0;
    while (currentSlot != nullptr)
    {
      currentSlot = currentSlot->next;
      ++freelist_length;
    }
    return freelist_length;
  }

  /// \returns Check whether the pointer is contained in this memory pool.
  bool contains(T* p)
  {
    std::uintptr_t pointer = reinterpret_cast<std::uintptr_t>(p);
    for (auto& block : m_blocks)
    {
      std::uintptr_t firstSlot = reinterpret_cast<std::uintptr_t>(block.data());
      if (firstSlot <= pointer && pointer < firstSlot + sizeof(T) * ElementsPerBlock)
      {
        assert((pointer - firstSlot) % sizeof(T) == 0);
        return true;
      }
    }

    return false;
  }

  /// \returns True if the given pointer is already in the freelist.
  bool in_freelist(T* pointer)
  {
    Slot* slot = reinterpret_cast<Slot*>(pointer);
    Slot* currentSlot = m_first_freeslot;
    while (currentSlot != nullptr)
    {
      if (slot == currentSlot)
      {
        return true;
      }

      currentSlot = currentSlot->next;
    }

    return false;
  }
};

} // namespace utilities
} // namespace mcrl2

#endif // MCRL2_UTILITIES_MEMORY_POOL_H_
