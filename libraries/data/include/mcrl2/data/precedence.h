// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/precedence.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_PRECEDENCE_H
#define MCRL2_DATA_PRECEDENCE_H

#include "mcrl2/core/detail/precedence.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/real.h"

namespace mcrl2 {

namespace data {

using namespace core::detail::precedences;

namespace detail {

  inline
  bool is_numeric_cast(const data_expression& x)
  {
    return data::sort_nat::is_pos2nat_application(x)
           || data::sort_int::is_pos2int_application(x)
           || data::sort_real::is_pos2real_application(x)
           || data::sort_int::is_nat2int_application(x)
           || data::sort_real::is_nat2real_application(x)
           || data::sort_real::is_int2real_application(x)
           || data::sort_nat::is_cnat_application(x)
           || data::sort_int::is_cint_application(x)
           || data::sort_real::is_creal_application(x)
           ;
  }

  inline
  data_expression remove_numeric_casts(data_expression x)
  {
    while (is_numeric_cast(x))
    {
    	x = *atermpp::aterm_cast<application>(x).begin();
    }
    return x;
  }

  inline
  bool is_plus(const application& x)
  {
    return sort_int::is_plus_application(remove_numeric_casts(x)) ||
           sort_nat::is_plus_application(remove_numeric_casts(x)) ||
           sort_pos::is_plus_application(remove_numeric_casts(x)) ||
           sort_real::is_plus_application(remove_numeric_casts(x));
  }

  inline
  bool is_minus(const application& x)
  {
    return sort_int::is_minus_application(remove_numeric_casts(x)) ||
           sort_real::is_minus_application(remove_numeric_casts(x));
  }

  inline
  bool is_mod(const application& x)
  {
    return sort_int::is_mod_application(remove_numeric_casts(x)) ||
           sort_nat::is_mod_application(remove_numeric_casts(x));
  }

  inline
  bool is_div(const application& x)
  {
    return sort_int::is_div_application(remove_numeric_casts(x)) ||
           sort_nat::is_div_application(remove_numeric_casts(x));
  }

  inline
  bool is_divmod(const application& x)
  {
    return sort_nat::is_divmod_application(remove_numeric_casts(x));
  }

  inline
  bool is_divides(const application& x)
  {
    return sort_real::is_divides_application(remove_numeric_casts(x));
  }

  inline
  bool is_implies(const application& x)
  {
    return sort_bool::is_implies_application(x);
  }

  inline
  bool is_set_union(const application& x)
  {
    return sort_set::is_union_application(x);
  }

  inline
  bool is_set_difference(const application& x)
  {
    return sort_set::is_difference_application(x);
  }

  inline
  bool is_bag_join(const application& x)
  {
    return sort_bag::is_union_application(x);
  }

  inline
  bool is_bag_difference(const application& x)
  {
    return sort_bag::is_difference_application(x);
  }

  inline
  bool is_and(const application& x)
  {
    return sort_bool::is_and_application(x);
  }

  inline
  bool is_or(const application& x)
  {
    return sort_bool::is_or_application(x);
  }

  inline
  bool is_equal_to(const application& x)
  {
    return data::is_equal_to_application(x);
  }

  inline
  bool is_not_equal_to(const application& x)
  {
    return data::is_not_equal_to_application(x);
  }

  inline
  bool is_less(const application& x)
  {
    return data::is_less_application(x);
  }

  inline
  bool is_less_equal(const application& x)
  {
    return data::is_less_equal_application(x);
  }

  inline
  bool is_greater(const application& x)
  {
    return data::is_greater_application(x);
  }

  inline
  bool is_greater_equal(const application& x)
  {
    return data::is_greater_equal_application(x);
  }

  inline
  bool is_in(const application& x)
  {
    return sort_list::is_in_application(x);
  }

  inline
  bool is_times(const application& x)
  {
    return sort_int::is_times_application(remove_numeric_casts(x));
  }

  inline
  bool is_element_at(const application& x)
  {
    return sort_list::is_element_at_application(x);
  }

  inline
  bool is_set_intersection(const application& x)
  {
    return sort_set::is_intersection_application(x);
  }

  inline
  bool is_bag_intersection(const application& x)
  {
    return sort_bag::is_intersection_application(x);
  }

  inline
  bool is_concat(const application& x)
  {
    return sort_list::is_concat_application(x);
  }

  inline
  bool is_cons_list(data_expression x)
  {
    while (sort_list::is_cons_application(x))
    {
      x = sort_list::right(x);
    }
    return sort_list::is_empty_function_symbol(x);
  }

  inline
  bool is_snoc_list(data_expression x)
  {
    while (sort_list::is_snoc_application(x))
    {
      x = sort_list::left(x);
    }
    return sort_list::is_empty_function_symbol(x);
  }

  inline
  bool is_cons(const application& x)
  {
    return sort_list::is_cons_application(x) && !is_cons_list(x);
  }

  inline
  bool is_snoc(const application& x)
  {
    return sort_list::is_snoc_application(x) && !is_snoc_list(x);
  }

} // namespace detail

inline
int left_precedence(const application& x)
{
  // TODO: this is unexpected, what to do???
  if (sort_real::is_creal_application(x))
  {
    return left_precedence(sort_real::left(x));
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
    return 6;
  }
  else if (detail::is_cons(x))
  {
    return 7;
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
           || detail::is_divmod(x)
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
int right_precedence(const application& x)
{
  return left_precedence(x);
}

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
int left_precedence(const data_expression& x)
{
  if (is_application(x))
  {
    return left_precedence(application(x));
  }
  else if (is_abstraction(x))
  {
    return 1;
  }
  return max_precedence;
}

inline
int right_precedence(const data_expression& x)
{
  return left_precedence(x);
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

#endif // MCRL2_DATA_PRECEDENCE_H
