// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/utility.h
/// \brief Utility functions for terms.

#ifndef ATERM_UTILITY_H
#define ATERM_UTILITY_H

#include <string>
#include "mcrl2/atermpp/aterm_string.h"

namespace atermpp
{

  /// Remove leading and trailing quotes from a quoted aterm_string.
  ///
  inline
  std::string unquote(aterm_string t)
  {
    std::string s(t);
    assert(s.size() >= 2 && *s.begin() == '"' && *s.rbegin() == '"');
    return std::string(++s.begin(), --s.end());
  }

} // namespace atermpp

#endif // ATERM_UTILITY_H
