// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/set_operations.h
/// \brief Set operations on term lists.

#ifndef MCRL2_ATERMPP_SET_OPERATIONS_H
#define MCRL2_ATERMPP_SET_OPERATIONS_H

#include <set>
#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{

/// \brief Returns the union of v and w.
/// \param v A term list.
/// \param w A term list.
/// \return The union of the term lists, interpreted as sets.
template <typename Term>
term_list<Term> term_list_union(const term_list<Term>& v, const term_list<Term>& w)
{
  if (v.empty())
  {
    return w;
  }
  if (w.empty())
  {
    return v;
  }
  std::set<Term> result;
  result.insert(v.begin(), v.end());
  result.insert(w.begin(), w.end());
  return term_list<Term>(result.begin(), result.end());
}

/// \brief Returns v minus w.
/// \param v A term list.
/// \param w A term list.
/// \return The difference of the term lists, interpreted as sets.
template <typename Term>
term_list<Term> term_list_difference(const term_list<Term>& v, const term_list<Term>& w)
{
  if (w.empty())
  {
    return v;
  }
  if (v.empty())
  {
    return v;
  }
  std::set<Term> result;
  result.insert(v.begin(), v.end());
  for (typename term_list<Term>::const_iterator i = w.begin(); i != w.end(); ++i)
  {
    result.erase(*i);
  }
  return term_list<Term>(result.begin(), result.end());
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_SET_OPERATIONS_H
