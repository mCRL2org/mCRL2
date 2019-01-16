// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_WORKAROUNDS_H
#define MCRL2_UTILITIES_WORKAROUNDS_H

#include <algorithm>

namespace mcrl2
{
namespace workaround
{
  /// \brief This is a workaround for the return by value diagnostic (clang -Wreturn-std-move).
  /// Description: The user probably expected a move, but they're not getting a move, perhaps because
  /// the type of "x" is different from the return type of the function.
  /// \details This constraint might be lifted by newer C++ standards and then this workaround can easily be removed.
  template<typename T>
  T&& return_std_move(T& t)
  {
    return std::move(t);
  }
}
}

#endif // MCRL2_UTILITIES_WORKAROUNDS_H


