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

#include <algorithm>
#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>
#include <vector>

#include "mcrl2/utilities/noncopyable.h"
#include "mcrl2/utilities/configuration.h"
#include "mcrl2/utilities/hardware_interference_size.h"

namespace mcrl2::utilities
{

// Forward declaration.
class shared_mutex;

/// A shared lock guard for the shared_mutex.
class shared_guard : private mcrl2::utilities::noncopyable
{
public:
  
  /// Locks the guard again explicitly.
  inline
  void lock_shared();

  /// Unlocks the acquired shared guard explicitly. Otherwise, performed in destructor.
  inline
  void unlock_shared();

  ~shared_guard()
  {
    if (m_is_locked)
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
  bool m_is_locked = true;
};

/// An exclusive lock guard for the shared_mutex.
class lock_guard : private mcrl2::utilities::noncopyable
{
public:
  /// Unlocks the acquired shared guard explicitly. Otherwise, performed in destructor.
  void unlock();

  bool try_lock();

  ~lock_guard()
  {
    if (m_is_locked)
    {
      unlock();
    }
  }

  bool owns_lock() const
  {
    return m_is_locked;
  }

private:
  friend class shared_mutex;

  lock_guard(shared_mutex& mutex, bool locked=true)
    : m_mutex(mutex), m_is_locked(locked)
  {}

  shared_mutex& m_mutex;
  bool m_is_locked;
};

struct shared_mutex_data 
{
  /// \brief The list of other mutexes.
  std::vector<shared_mutex*> other;

  /// \brief Mutex for adding/removing shared_guards.
  std::mutex mutex;  
  
  /// Adds a shared mutex to the data.
  inline
  void register_mutex(shared_mutex* shared_mutex)
  {
    std::lock_guard guard(mutex);
    other.emplace_back(shared_mutex);
  }
  
  // Removes a shared mutex from the data
  inline void unregister_mutex(shared_mutex* shared_mutex) 
  {
    std::lock_guard guard(mutex);
    auto it = std::find(other.begin(), other.end(), shared_mutex);
    assert(it != other.end());

    other.erase(it);
  }
};

/// An implementation of a shared mutex (also called readers-write lock in the literature) based on
/// the notion of busy and forbidden flags.
class alignas(hardware_destructive_interference_size) shared_mutex
{
public:
  shared_mutex()
    : m_shared(std::make_shared<shared_mutex_data>())
  {
    m_shared->register_mutex(this);
  }

  ~shared_mutex() 
  { 
    m_shared->unregister_mutex(this);
  }

  /// The copy/move constructor/assignment should not be called while any lock_guard or shared_guard is alive.
  shared_mutex(const shared_mutex& other)
    : m_shared(other.m_shared)
  {
    m_shared->register_mutex(this);
  }

  shared_mutex(shared_mutex&& other) noexcept
      : m_shared(other.m_shared)
  {
    m_shared->register_mutex(this);
  }

  shared_mutex& operator=(const shared_mutex& other)
  {
    if (this != &other)
    {
      // Remove ourselves, and register into the other shared.
      m_shared->unregister_mutex(this);

      m_shared = other.m_shared;
      m_shared->register_mutex(this);
    }
    return *this;
  }

  shared_mutex& operator=(shared_mutex&& other) noexcept
  {
    if (this != &other)
    {
      m_shared->unregister_mutex(this);

      m_shared = other.m_shared;
      m_shared->register_mutex(this);
      m_shared->unregister_mutex(&other);
    }
    return *this;
  }

  // Obtain exclusive access to the busy-forbidden lock.
  inline
  lock_guard lock()
  {
    lock_impl();
    return lock_guard(*this);
  }

  // Try to obtain exclusive access to the busy-forbidden lock without blocking.
  // Returns a lock_guard with is_locked set to true if successful, false otherwise.
  inline
  lock_guard try_lock()
  {
    bool acquired = try_lock_impl();
    return lock_guard(*this, acquired);
  }

  // Release exclusive access to the busy-forbidden lock.
  inline
  void unlock()
  {
    unlock_impl();
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

  inline
  void set_forbidden_flags() 
  {    
    // Shared and exclusive sections MUST be disjoint.
    assert(!m_busy_flag);

    assert(std::find(m_shared->other.begin(), m_shared->other.end(), this) != m_shared->other.end());

    // Indicate that threads must wait.
    for (shared_mutex* mutex : m_shared->other)
    {
      if (mutex != this)
      {
        mutex->set_forbidden(true);
      }
    }

    // Wait for all pools to indicate that they are not busy.
    for (const shared_mutex* mutex : m_shared->other)
    {
      if (mutex != this)
      {
        mutex->wait_for_busy();
      }
    }
  }

  inline
  void lock_impl() 
  {    
    if constexpr (mcrl2::utilities::detail::GlobalThreadSafe)
    {
      // Only one thread can halt everything.
      m_shared->mutex.lock();
      set_forbidden_flags();
    }
  }

  inline
  bool try_lock_impl() 
  {    
    if constexpr (mcrl2::utilities::detail::GlobalThreadSafe)
    {
      // Try to obtain the lock. If the surrounding mutex cannot be locked, this fails.
      if (!m_shared->mutex.try_lock())
      {
        return false;
      }

      set_forbidden_flags();
    }
    return true;
  }

// REMOVE
inline std::size_t get_lock_depth() { return m_lock_depth; }
      
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

        // Wait for the global lock.
        m_shared->mutex.lock();
        m_shared->mutex.unlock();
        
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

private:
  friend class lock_guard;
  friend class shared_guard;
  friend class shared_mutex_pool;

  void unlock_impl()
  {
    if constexpr (mcrl2::utilities::detail::GlobalThreadSafe)
    {
      for (shared_mutex* mutex : m_shared->other)
      {
        mutex->set_forbidden(false);
      }

      m_shared->mutex.unlock();
    }
  }

  /// \returns True iff the shared mutex has its busy flag set. 
  inline
  bool is_busy() const
  {
    return m_busy_flag.load();
  }

  /// \brief Waits for the busy flag to become false.
  inline
  void wait_for_busy() const
  {
    while (m_busy_flag.load()) { /* wait */ };
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

  std::shared_ptr<shared_mutex_data> m_shared;
};

inline
void shared_guard::lock_shared()   
{    
  // Uses the internal implementation since we don't need a shared_guard.
  m_mutex.lock_shared_impl();
  m_is_locked = true;
}

inline
void shared_guard::unlock_shared()
{    
  m_mutex.unlock_shared();
  m_is_locked = false;
}

inline
bool lock_guard::try_lock()
{
  return m_mutex.try_lock_impl();
}

inline
void lock_guard::unlock()
{
  m_mutex.unlock();
  m_is_locked = false;
}

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_DETAIL_SHARED_MUTEX_H
