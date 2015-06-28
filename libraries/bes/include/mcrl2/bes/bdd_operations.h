// Author(s): XIAO Qi
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/bdd_operations.h
/// \brief Operations on BDDs.

#ifndef MCRL2_BDD_OPERATIONS_H
#define MCRL2_BDD_OPERATIONS_H

#include "mcrl2/bes/bdd_expression.h"

namespace mcrl2 {

namespace bdd {

bdd_expression ordered_and(const bdd_expression& l, const bdd_expression &r)
{
  using accessors::left;
  using accessors::right;
  // Basic cases.
  if (is_false(l) || is_false(r))
  {
    return false_();
  }
  else if (is_true(l))
  {
    return r;
  }
  else if (is_true(r))
  {
    return l;
  }
  // Now both l and r are if's
  const core::identifier_string& p = atermpp::down_cast<if_>(l).name();
  const core::identifier_string& q = atermpp::down_cast<if_>(r).name();
  if (p == q)
  {
    return if_(p,
        ordered_and(left(l), left(r)), ordered_and(right(l), right(r)));
  }
  else if (p < q)
  {
    return if_(p, ordered_and(left(l), r), ordered_and(right(l), r));
  }
  else // q < p
  {
    return if_(q, ordered_and(l, left(r)), ordered_and(l, right(r)));
  }
}

bdd_expression ordered_or(const bdd_expression& l, const bdd_expression& r)
{
  using accessors::left;
  using accessors::right;
  // Basic cases.
  if (is_true(l) || is_true(r))
  {
    return true_();
  }
  else if (is_false(l))
  {
    return r;
  }
  else if (is_false(r))
  {
    return l;
  }
  // Now both l and r are if's
  const core::identifier_string& p = atermpp::down_cast<if_>(l).name();
  const core::identifier_string& q = atermpp::down_cast<if_>(r).name();
  if (p == q)
  {
    return if_(p,
        ordered_or(left(l), left(r)), ordered_or(right(l), right(r)));
  }
  else if (p < q)
  {
    return if_(p, ordered_or(left(l), r), ordered_or(right(l), r));
  }
  else // q < p
  {
    return if_(q, ordered_or(l, left(r)), ordered_or(l, right(r)));
  }
}

} // namespace bdd

} // namespace mcrl2

#endif // MCRL2_BDD_OPERATIONS_H
