// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/infix_precedence.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_INFIX_PRECEDENCE_H
#define MCRL2_DATA_INFIX_PRECEDENCE_H

#include "mcrl2/data/precedence.h"

namespace mcrl2 {

namespace data {

inline
int infix_precedence_left(const application& x)
{
  // TODO: this is unexpected, what to do???
  if (sort_real::is_creal_application(x))
  {
    return infix_precedence_left(atermpp::aterm_cast<application>(sort_real::arg(x)));
  }
  else if (detail::is_implies(x))
  {
    return 3;
  }
  else if (detail::is_or(x))
  {
    return 4;
  }
  else if (detail::is_and(x))
  {
    return 5;
  }
  else if (detail::is_equal_to(x) ||
           detail::is_not_equal_to(x)
          )
  {
    return 6;
  }
  else if (   detail::is_less(x)
           || detail::is_less_equal(x)
           || detail::is_greater(x)
           || detail::is_greater_equal(x)
           || detail::is_in(x)
          )
  {
    return 7;
  }
  else if (detail::is_cons(x))
  {
    return 10;
  }
  else if (detail::is_snoc(x))
  {
    return 8;
  }
  else if (detail::is_concat(x))
  {
    return 9;
  }
  else if (   detail::is_plus(x)
           || detail::is_minus(x)
           || detail::is_set_union(x)
           || detail::is_set_difference(x)
           || detail::is_bag_join(x)
           || detail::is_bag_difference(x)
          )
  {
    return 10;
  }
  else if (   detail::is_div(x)
           || detail::is_mod(x)
           || detail::is_divides(x)
          )
  {
    return 11;
  }
  else if (   detail::is_times(x)
           || detail::is_element_at(x)
           || detail::is_set_intersection(x)
           || detail::is_bag_intersection(x)
          )
  {
    return 12;
  }
  return max_precedence;
}

inline
int infix_precedence_right(const application& x)
{
  // TODO: this is unexpected, what to do???
  if (sort_real::is_creal_application(x))
  {
    return infix_precedence_right(atermpp::aterm_cast<application>(sort_real::arg(x)));
  }
  else if (detail::is_implies(x))
  {
    return 2;
  }
  else if (detail::is_or(x))
  {
    return 3;
  }
  else if (detail::is_and(x))
  {
    return 4;
  }
  else if (detail::is_equal_to(x) ||
           detail::is_not_equal_to(x)
          )
  {
    return 5;
  }
  else if (   detail::is_less(x)
           || detail::is_less_equal(x)
           || detail::is_greater(x)
           || detail::is_greater_equal(x)
           || detail::is_in(x)
          )
  {
    return 7;
  }
  else if (detail::is_cons(x))
  {
    return 8;
  }
  else if (detail::is_snoc(x))
  {
    return 10;
  }
  else if (detail::is_concat(x))
  {
    return 10;
  }
  else if (   detail::is_plus(x)
           || detail::is_minus(x)
           || detail::is_set_union(x)
           || detail::is_set_difference(x)
           || detail::is_bag_join(x)
           || detail::is_bag_difference(x)
          )
  {
    return 11;
  }
  else if (   detail::is_div(x)
           || detail::is_mod(x)
           || detail::is_divides(x)
          )
  {
    return 12;
  }
  else if (   detail::is_times(x)
           || detail::is_element_at(x)
           || detail::is_set_intersection(x)
           || detail::is_bag_intersection(x)
          )
  {
    return 13;
  }
  return max_precedence;
}

inline
int infix_precedence_left(const data_expression& x)
{
  if (is_application(x))
  {
    return infix_precedence_left(application(x));
  }
  else if (is_abstraction(x))
  {
    return 2;
  }
  return max_precedence;
}

inline
int infix_precedence_right(const data_expression& x)
{
  if (is_application(x))
  {
    return infix_precedence_right(application(x));
  }
  else if (is_abstraction(x))
  {
    return 1;
  }
  return max_precedence;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_INFIX_PRECEDENCE_H
