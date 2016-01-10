// Author(s): Xiao Qi
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
  const if_& l_if = atermpp::down_cast<if_>(l);
  const if_& r_if = atermpp::down_cast<if_>(r);
  const core::identifier_string& p = l_if.name();
  const core::identifier_string& q = r_if.name();
  if (p == q)
  {
    return if_(p,
        ordered_and(l_if.left(), r_if.left()), ordered_and(l_if.right(),r_if.right()));
  }
  else if (p < q)
  {
    return if_(p, ordered_and(l_if.left(), r), ordered_and(l_if.right(), r));
  }
  else // q < p
  {
    return if_(q, ordered_and(l, r_if.left()), ordered_and(l, r_if.right()));
  }
}

bdd_expression ordered_or(const bdd_expression& l, const bdd_expression& r)
{
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
  const if_& l_if = atermpp::down_cast<if_>(l);
  const if_& r_if = atermpp::down_cast<if_>(r);
  const core::identifier_string& p = l_if.name();
  const core::identifier_string& q = r_if.name();
  if (p == q)
  {
    return if_(p,
        ordered_or(l_if.left(), r_if.left()), ordered_or(l_if.right(), r_if.right()));
  }
  else if (p < q)
  {
    return if_(p, ordered_or(l_if.left(), r), ordered_or(l_if.right(), r));
  }
  else // q < p
  {
    return if_(q, ordered_or(l, r_if.left()), ordered_or(l, r_if.right()));
  }
}

} // namespace bdd

} // namespace mcrl2

#endif // MCRL2_BDD_OPERATIONS_H
