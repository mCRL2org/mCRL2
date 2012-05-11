// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/atermpp/aterm_io.h"

namespace atermpp
{

/// \brief Make a term from a string pattern.
/// \param pattern A string
/// \return The term constructed from the pattern.
inline
aterm make_term(const std::string& pattern)
{
  return read_from_string(pattern);
}


} // namespace atermpp

#endif // ATERM_UTILITY_H
