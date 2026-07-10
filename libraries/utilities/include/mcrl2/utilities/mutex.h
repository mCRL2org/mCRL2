// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_MUTEX_H_
#define MCRL2_UTILITIES_MUTEX_H_

#include <concepts>
#include <mutex>

#include "mcrl2/utilities/configuration.h"

namespace mcrl2::utilities
{

namespace detail
{

/// \brief The Cpp17BasicLockable named requirement; the standard library provides no concept for it.
template<typename Mutex>
concept IsBasicLockable = requires(Mutex m) {
  { m.lock() } -> std::same_as<void>;
  { m.unlock() } -> std::same_as<void>;
};

} // namespace detail

/// \brief This is simply an exclusive lock based on the standard library with the ability to perform no locks when 
///        thread safety is not desired.
class mutex 
{
public:
  void lock() {
    if constexpr (mcrl2::utilities::detail::GlobalThreadSafe) {
        m_mutex.lock();
    }
  }

  void unlock()
  {        
    if constexpr (mcrl2::utilities::detail::GlobalThreadSafe) {
        m_mutex.unlock();
    }
  }

private:
    std::mutex m_mutex;
};

// The interface must behave as std::mutex, such that the standard library lock guards apply.
static_assert(detail::IsBasicLockable<mutex>,
    "mutex must satisfy Cpp17BasicLockable, like std::mutex, for use with std::lock_guard");

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_MUTEX_H_
