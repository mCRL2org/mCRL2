// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_DETAIL_SHARED_MUTEX_H
#define MCRL2_UTILITIES_DETAIL_SHARED_MUTEX_H

#include "mcrl2/utilities/noncopyable.h"

#include <atomic>
#include <mutex>

namespace mcrl2::utilities
{

// Forward declaration
class shared_mutex;

/// The pool that can hand out shared mutex instances.
/// Locking and unlocking is only performed when GlobalThreadSafe is set to true.
class shared_mutex_pool : private mcrl2::utilities::noncopyable
{
private:
  friend class shared_mutex;

  /// \returns Wait for the mutex to unlock.
  void wait()
  {
    std::unique_lock lock(m_mutex);
  }

  /// \brief Acquires exlusive access by the given shared mutex.
  void lock_impl(shared_mutex* shared_mutex);

  /// \brief Acquires exlusive access by the given shared mutex.
  void unlock_impl();

  /// Adds a shared mutex to the pool.
  void register_mutex(shared_mutex* mutex);
  
  // Removes a shared mutex from the pool
  void unregister_mutex(shared_mutex* mutex);

  std::vector<shared_mutex*> m_shared_mutexes;

  /// \brief Mutex for adding/removing shared_guards.
  std::mutex m_mutex;
};


/// A shared lock guard for the shared_mutex.
class shared_guard : private mcrl2::utilities::noncopyable
{
public:
  
  /// Locks the guard again explicitly.
  void lock_shared();

  /// Unlocks the acquired shared guard explicitly. Otherwise, performed in destructor.
  void unlock_shared();

  ~shared_guard()
  {
    if (is_locked)
    {
      unlock_shared();
    }
  }

private:
  friend class shared_mutex;

  shared_guard(shared_mutex& mutex)
    : m_mutex(mutex)
  {}

  shared_mutex& m_mutex;
  bool is_locked = true;
};

/// An exclusive lock guard for the shared_mutex.
class lock_guard : private mcrl2::utilities::noncopyable
{
public:
  /// Unlocks the acquired shared guard explicitly. Otherwise, performed in destructor.
  void unlock();

  ~lock_guard()
  {
    if (is_locked)
    {
      unlock();
    }
  }

private:
  friend class shared_mutex;

  lock_guard(shared_mutex& mutex)
    : m_mutex(mutex)
  {}

  shared_mutex& m_mutex;
  bool is_locked = true;
};


/// An implementation of a shared mutex (also called readers-write lock in the literature) based on
/// the notion of busy and forbidden flags.
class shared_mutex : private mcrl2::utilities::noncopyable
{
public:
  shared_mutex(shared_mutex_pool& pool)
    : m_pool(pool) 
  { 
    m_pool.register_mutex(this);
  }

  ~shared_mutex() 
  { 
    m_pool.unregister_mutex(this);
  }

  shared_mutex(shared_mutex&& other)
    : m_pool(other.m_pool)
  {
    m_pool.register_mutex(this);
    m_pool.unregister_mutex(&other);
  }
  
  // Obtain exclusive access to the busy-forbidden lock.
  inline
  lock_guard lock()
  {
    m_pool.lock_impl(this);
    return lock_guard(*this);
  }

  /// Acquires a shared lock on this instance, returns a shared guard that keeps the lock until it is destroyed.
  /// Or alternative, unlock_shared is called explicitly.
  inline
  shared_guard lock_shared()
  {
    lock_shared_impl();
    return shared_guard(*this);
  }

  /// \returns True iff the shared mutex is in the shared section
  bool is_shared_locked() const
  {
    return m_lock_depth != 0;
  }

private:
  friend class lock_guard;
  friend class shared_guard;
  friend class shared_mutex_pool;

  // Release exclusive access to the busy-forbidden lock.
  inline
  void unlock()
  {
    m_pool.unlock_impl();
  }
    
  inline
  void lock_shared_impl()
  {
    if (mcrl2::utilities::detail::GlobalThreadSafe && m_lock_depth == 0)
    {
      assert(!m_busy_flag);
      m_busy_flag.store(true);

      // Wait for the forbidden flag to become false.
      while (m_forbidden_flag.load())
      {
        m_busy_flag = false;
        m_pool.wait();
        m_busy_flag = true;
      }
    }

    ++m_lock_depth;
  }

  // Release shared access to the busy-forbidden lock.
  inline
  void unlock_shared()
  {
    assert(!mcrl2::utilities::detail::GlobalThreadSafe || m_lock_depth > 0);

    --m_lock_depth;
    if (mcrl2::utilities::detail::GlobalThreadSafe && m_lock_depth == 0)
    {
      assert(m_busy_flag);
      m_busy_flag.store(false, std::memory_order_release);
    }
  }

  /// \brief Blocks until the thread pool is not busy.
  inline
  void wait_for_busy() const
  {
    while (m_busy_flag.load());
  }

  /// \returns True iff the thread aterm pool has its busy flag set. 
  inline
  bool is_busy() const
  {
    return m_busy_flag.load();
  }

  inline
  void set_forbidden(bool value)
  {
    m_forbidden_flag.store(value);
  }

  /// \brief A boolean flag indicating whether this thread is working inside the global aterm pool.
  std::atomic<bool> m_busy_flag = false;
  std::atomic<bool> m_forbidden_flag = false;

  /// \brief It can happen that un/lock_shared calls are nested, so keep track of the nesting depth and only
  ///        actually perform un/locking at the root.
  std::size_t m_lock_depth = 0;

  shared_mutex_pool& m_pool;
};

/// \brief Acquires exlusive access by the given shared mutex.
inline
void shared_mutex_pool::lock_impl(shared_mutex* shared_mutex)
{
  if constexpr (mcrl2::utilities::detail::GlobalThreadSafe)
  {
    // Only one thread can halt everything.
    m_mutex.lock();

    assert(std::find(m_shared_mutexes.begin(), m_shared_mutexes.end(), shared_mutex) != m_shared_mutexes.end());
    // Indicate that threads must wait.
    for (auto& mutex : m_shared_mutexes)
    {
      if (mutex != shared_mutex)
      {
        mutex->set_forbidden(true);
      }
    }

    // Wait for all pools to indicate that they are not busy.
    for (const auto& mutex : m_shared_mutexes)
    {
      if (mutex != shared_mutex)
      {
        mutex->wait_for_busy();
      }
    }
  }
}

inline
void shared_mutex_pool::unlock_impl()
{
  if constexpr (mcrl2::utilities::detail::GlobalThreadSafe)
  {
    for (auto& mutex : m_shared_mutexes)
    {
      mutex->set_forbidden(false);
    }

    m_mutex.unlock();
  }
}

inline
void shared_mutex_pool::register_mutex(shared_mutex* mutex)
{
  std::lock_guard guard(m_mutex);
  m_shared_mutexes.emplace_back(mutex);
}

inline void shared_mutex_pool::unregister_mutex(shared_mutex* mutex) 
{
  std::lock_guard guard(m_mutex);
  auto it = std::find(m_shared_mutexes.begin(), m_shared_mutexes.end(), mutex);
  assert(it != m_shared_mutexes.end());

  m_shared_mutexes.erase(it);
}

inline
void shared_guard::unlock_shared()
{
  m_mutex.unlock_shared();
  is_locked = false;
}

inline 
void shared_guard::lock_shared()
{
  // Uses the internal implementation since we don't need a shared_guard.
  m_mutex.lock_shared_impl();
  is_locked = true;
}

inline
void lock_guard::unlock()
{
  m_mutex.unlock();
  is_locked = false;
}

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_DETAIL_SHARED_MUTEX_H
