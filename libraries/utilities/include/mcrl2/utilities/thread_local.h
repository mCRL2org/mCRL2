// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Based on the Rust thread_local crate by Amanieu d'Antras
// (Licensed under Apache License, Version 2.0 <LICENSE-APACHE or
// http://apache.org/licenses/LICENSE-2.0> or the MIT license <LICENSE-MIT or
// http://opensource.org/licenses/MIT>, at your option)

#ifndef MCRL2_UTILITIES_THREAD_LOCAL_H_
#define MCRL2_UTILITIES_THREAD_LOCAL_H_

#include <array>
#include <atomic>
#include <bit>
#include <cstddef>
#include <iterator>
#include <new>
#include <stdexcept>
#include <type_traits>

#include "mcrl2/utilities/noncopyable.h"

namespace mcrl2::utilities
{

namespace detail
{

/// \brief Get a unique identifier for the current thread.
inline std::size_t get_thread_id()
{
  static thread_local std::size_t thread_id_cache = 0;
  if (thread_id_cache == 0)
  {
    static std::atomic<std::size_t> next_id(1);
    thread_id_cache = next_id.fetch_add(1, std::memory_order_relaxed);
  }
  return thread_id_cache;
}

/// \brief Information about a thread's position in the bucket structure.
struct ThreadBucket
{
  std::size_t id;       ///< Unique thread identifier
  std::size_t bucket;   ///< Which bucket this thread's entry is in
  std::size_t index;    ///< Index within the bucket
  std::size_t bucket_size; ///< Size of the current bucket

  ThreadBucket(std::size_t thread_id)
    : id(thread_id)
  {
    // Compute bucket and index from thread ID using bit operations
    // Bucket n contains 2^n elements
    if (thread_id == 0)
    {
      bucket = 0;
      index = 0;
      bucket_size = 1;
    }
    else
    {
      bucket = std::bit_width(thread_id);
      std::size_t offset = thread_id - (1ULL << (bucket - 1));
      index = offset;
      bucket_size = 1ULL << bucket;
    }
  }
};

/// \brief Entry in the thread-local bucket.
template <typename T>
struct Entry
{
  std::atomic<bool> present{false};
  std::aligned_storage_t<sizeof(T), alignof(T)> value;

  ~Entry()
  {
    if (present.load(std::memory_order_acquire))
    {
      reinterpret_cast<T*>(&value)->~T();
    }
  }
};

} // namespace detail

/// \brief Per-object thread-local storage container.
/// \details This class provides per-object thread-local storage, allowing
///          each thread to have its own copy of a value. Unlike std::thread_local
///          which is static, this allows creating multiple independent thread-local
///          storages within a single program.
///
///          Per-thread objects are not destroyed when a thread exits. Instead,
///          objects are only destroyed when the ThreadLocal container is destroyed.
///
///          Thread-local values can be iterated over using iter() and iter_mut()
///          methods (the latter requires a mutable borrow, guaranteeing thread safety).
template <typename T>
class ThreadLocal : private noncopyable
{
private:
  using Entry = detail::Entry<T>;

  // Compute the number of buckets needed (based on pointer width)
  static constexpr std::size_t POINTER_WIDTH = sizeof(std::size_t) * 8;
  static constexpr std::size_t BUCKETS = POINTER_WIDTH - 1;

  std::array<std::atomic<Entry*>, BUCKETS> buckets;
  std::atomic<std::size_t> values_count{0};

  /// \brief Helper to deallocate a bucket.
  static void deallocate_bucket(Entry* bucket_ptr, std::size_t /* size */)
  {
    delete[] bucket_ptr;
  }

  /// \brief Helper to allocate a bucket.
  static Entry* allocate_bucket(std::size_t size)
  {
    Entry* bucket = new Entry[size];
    return bucket;
  }

  /// \brief Get the value for the given thread, if it exists.
  const T* get_inner(const detail::ThreadBucket& thread) const
  {
    Entry* bucket_ptr = buckets[thread.bucket].load(std::memory_order_acquire);
    if (!bucket_ptr)
    {
      return nullptr;
    }

    Entry& entry = bucket_ptr[thread.index];
    if (entry.present.load(std::memory_order_relaxed))
    {
      return reinterpret_cast<const T*>(&entry.value);
    }
    return nullptr;
  }

  /// \brief Insert a value for the given thread.
  const T* insert(const detail::ThreadBucket& thread, T value)
  {
    std::atomic<Entry*>& bucket_atomic_ptr = buckets[thread.bucket];
    Entry* bucket_ptr = bucket_atomic_ptr.load(std::memory_order_acquire);

    // Allocate the bucket if it doesn't exist
    if (!bucket_ptr)
    {
      Entry* new_bucket = allocate_bucket(thread.bucket_size);

      Entry* expected = nullptr;
      if (bucket_atomic_ptr.compare_exchange_strong(
            expected, new_bucket, std::memory_order_acq_rel, std::memory_order_acquire))
      {
        bucket_ptr = new_bucket;
      }
      else
      {
        // Another thread allocated before us, use theirs
        deallocate_bucket(new_bucket, thread.bucket_size);
        bucket_ptr = expected;
      }
    }

    // Insert the value into the bucket
    Entry& entry = bucket_ptr[thread.index];
    new (&entry.value) T(std::move(value));
    entry.present.store(true, std::memory_order_release);

    values_count.fetch_add(1, std::memory_order_release);

    return reinterpret_cast<const T*>(&entry.value);
  }

public:
  /// \brief Create a new empty ThreadLocal container.
  ThreadLocal()
  {
    for (std::size_t i = 0; i < BUCKETS; ++i)
    {
      buckets[i].store(nullptr, std::memory_order_relaxed);
    }
  }

  /// \brief Destructor that cleans up all allocated buckets.
  ~ThreadLocal()
  {
    for (std::size_t i = 0; i < BUCKETS; ++i)
    {
      Entry* bucket_ptr = buckets[i].load(std::memory_order_relaxed);
      if (bucket_ptr)
      {
        deallocate_bucket(bucket_ptr, 1ULL << i);
      }
    }
  }

  /// \brief Get the thread-local value for the current thread, if it exists.
  const T* get() const
  {
    detail::ThreadBucket thread(detail::get_thread_id());
    return get_inner(thread);
  }

  /// \brief Get the thread-local value for the current thread, or create it if it doesn't exist.
  template <typename F>
  const T* get_or(F&& create)
  {
    detail::ThreadBucket thread(detail::get_thread_id());
    const T* val = get_inner(thread);
    if (val)
    {
      return val;
    }
    return insert(thread, std::forward<F>(create)());
  }

  /// \brief Get the thread-local value for the current thread, or create it if it doesn't exist.
  /// \returns The value, or nullptr if creation failed.
  template <typename F>
  const T* get_or_try(F&& create)
  {
    detail::ThreadBucket thread(detail::get_thread_id());
    const T* val = get_inner(thread);
    if (val)
    {
      return val;
    }
    try
    {
      return insert(thread, std::forward<F>(create)());
    }
    catch (...)
    {
      return nullptr;
    }
  }

  /// \brief Iterator over thread-local values.
  class Iter
  {
  private:
    const ThreadLocal* m_thread_local;
    mutable std::size_t bucket = 0;
    mutable std::size_t bucket_size = 1;
    mutable std::size_t index = 0;
    mutable std::size_t yielded = 0;

    /// \brief Advance to the next bucket.
    void next_bucket() const
    {
      bucket_size <<= 1;
      bucket += 1;
      index = 0;
    }

  public:
    using difference_type = std::ptrdiff_t;
    using value_type = const T;
    using pointer = const T*;
    using reference = const T&;
    using iterator_category = std::forward_iterator_tag;

    Iter(const ThreadLocal* tl, bool begin)
      : m_thread_local(tl)
    {
      if (!begin)
      {
        bucket = BUCKETS;
      }
    }

    const T& operator*() const
    {
      while (bucket < BUCKETS)
      {
        Entry* bucket_ptr = m_thread_local->buckets[bucket].load(std::memory_order_acquire);
        if (!bucket_ptr)
        {
          next_bucket();
          continue;
        }

        while (index < bucket_size)
        {
          Entry& entry = bucket_ptr[index];
          ++index;
          if (entry.present.load(std::memory_order_acquire))
          {
            return *reinterpret_cast<const T*>(&entry.value);
          }
        }

        next_bucket();
      }

      throw std::out_of_range("ThreadLocal iterator out of bounds");
    }

    const T* operator->() const
    {
      return &**this;
    }

    Iter& operator++()
    {
      while (bucket < BUCKETS)
      {
        Entry* bucket_ptr = m_thread_local->buckets[bucket].load(std::memory_order_acquire);
        if (!bucket_ptr)
        {
          next_bucket();
          continue;
        }

        while (index < bucket_size)
        {
          Entry& entry = bucket_ptr[index];
          ++index;
          if (entry.present.load(std::memory_order_acquire))
          {
            ++yielded;
            return *this;
          }
        }

        next_bucket();
      }

      return *this;
    }

    Iter operator++(int)
    {
      Iter tmp = *this;
      ++*this;
      return tmp;
    }

    bool operator==(const Iter& other) const
    {
      return m_thread_local == other.m_thread_local && bucket == other.bucket && index == other.index;
    }

    bool operator!=(const Iter& other) const
    {
      return !(*this == other);
    }
  };

  /// \brief Get an iterator over all thread-local values.
  Iter begin() const
  {
    return Iter(this, true);
  }

  /// \brief Get the end iterator.
  Iter end() const
  {
    return Iter(this, false);
  }

  /// \brief Get a mutable pointer to the thread-local value for the current thread, or create it.
  template <typename F>
  T* get_or_mut(F&& create)
  {
    return const_cast<T*>(get_or(std::forward<F>(create)));
  }

  /// \brief Apply func to every present thread-local value via a mutable reference.
  /// \pre Requires exclusive access to this ThreadLocal (no concurrent operations).
  template <typename F>
  void for_each_mut(const F& func)
  {
    for (std::size_t i = 0; i < BUCKETS; ++i)
    {
      Entry* bucket_ptr = buckets[i].load(std::memory_order_acquire);
      if (!bucket_ptr)
      {
        continue;
      }

      std::size_t bucket_size = 1ULL << i;
      for (std::size_t j = 0; j < bucket_size; ++j)
      {
        Entry& entry = bucket_ptr[j];
        if (entry.present.load(std::memory_order_acquire))
        {
          func(*reinterpret_cast<T*>(&entry.value));
        }
      }
    }
  }

  /// \brief Clear all thread-local values.
  void clear()
  {
    for (std::size_t i = 0; i < BUCKETS; ++i)
    {
      Entry* bucket_ptr = buckets[i].load(std::memory_order_relaxed);
      if (bucket_ptr)
      {
        deallocate_bucket(bucket_ptr, 1ULL << i);
        buckets[i].store(nullptr, std::memory_order_relaxed);
      }
    }
    values_count.store(0, std::memory_order_relaxed);
  }

  /// \brief Get the number of thread-local values (approximate).
  std::size_t size() const
  {
    return values_count.load(std::memory_order_acquire);
  }
};

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_THREAD_LOCAL_H_
