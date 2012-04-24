// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/utility.h
/// \brief Utility functions.

#ifndef ATERM_DETAIL_UTILITY_H
#define ATERM_DETAIL_UTILITY_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{

namespace detail
{

/// \brief Converts a string to an ATerm string
/// \param s A string
/// \return The converted string
inline
ATermAppl str2appl(std::string const& s)
{
  return ATmakeAppl0(AFun(s.c_str(), 0, true));
}

} // namespace detail

} // namespace atermpp

#endif // ATERM_DETAIL_UTILITY_H
