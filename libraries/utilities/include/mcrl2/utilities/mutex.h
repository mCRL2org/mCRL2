// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <mutex>

#include "mcrl2/utilities/configuration.h"

namespace mcrl2::utilities
{

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

} // namespace mcrl2::utilities