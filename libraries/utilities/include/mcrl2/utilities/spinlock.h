// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_SPINLOCK_H_
#define MCRL2_UTILITIES_SPINLOCK_H_

#include <atomic>

namespace mcrl2
{
namespace utilities
{

/// \brief Implements a very simple spinlock.
class spinlock
{
public:

  /// \brief Busy waits until the lock is unlocked.
  void lock()
  {
    while (m_flag.test_and_set()) {};
  }

  /// \brief Tries to lock the spinlock, but also returns immediately.
  /// \returns Whether the lock was acquired succesfully.
  bool try_lock()
  {
    return m_flag.test_and_set();
  }

  /// \brief Frees the lock.
  void unlock()
  {
    m_flag.clear();
  }

private:
  std::atomic_flag m_flag;
};

} // namespace utilities.
} // namespace mcrl2.

#endif // MCRL2_UTILITIES_SPINLOCK_H_
