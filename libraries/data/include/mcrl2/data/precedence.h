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
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/multiple_possible_sorts.h"
#include "mcrl2/data/unknown_sort.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/real.h"

namespace mcrl2 {

namespace data {

using namespace core::detail::precedences;

inline
bool is_cons_list(data_expression x)
{
  while (sort_list::is_cons_application(x))
  {
    x = sort_list::tail(x);
  }
  return sort_list::is_nil_function_symbol(x);
}

inline
bool is_snoc_list(data_expression x)
{
  while (sort_list::is_snoc_application(x))
  {
    x = sort_list::rtail(x);
  }
  return sort_list::is_nil_function_symbol(x);
}

inline
int precedence(const data_expression& x)
{
  if (is_application(x))
  {
    if (sort_bool::is_implies_application(x))
    {
      return 2;
    }
    else if (sort_bool::is_and_application(x)
             || sort_bool::is_or_application(x)
            )
    {
      return 3;
    }
    else if (data::is_equal_to_application(x)
             || data::is_not_equal_to_application(x)
            )
    {
      return 4;
    }
    else if (data::is_less_application(x)
             || data::is_less_equal_application(x)
             || data::is_greater_application(x)
             || data::is_greater_equal_application(x)
             || sort_list::is_in_application(x)
            )
    {
      return 5;
    }
    else if (sort_list::is_cons_application(x))
    {
      return is_cons_list(x) ? max_precedence : 6;
    }
    else if (sort_list::is_snoc_application(x))
    {
      return is_snoc_list(x) ? max_precedence : 7;
    }
    else if (sort_list::is_concat_application(x))
    {
      return 8;
    }
    else if (sort_real::is_plus_application(x)
             || sort_real::is_minus_application(x)
             || sort_set::is_setunion_application(x)
             || sort_set::is_setdifference_application(x)
             || sort_bag::is_bagjoin_application(x)
             || sort_bag::is_bagdifference_application(x)
            )
    {
      return 9;
    }
    else if (sort_int::is_div_application(x)
             || sort_int::is_mod_application(x)
             || sort_real::is_divides_application(x)
            )
    {
      return 10;
    }
    else if (sort_int::is_times_application(x)
             || sort_list::is_element_at_application(x)
             || sort_set::is_setintersection_application(x)
             || sort_bag::is_bagintersect_application(x)
            )
    {
      return 11;
    }
  }
  return max_precedence;
}

inline
int infix_precedence_left(const data_expression& x)
{
  if (is_application(x))
  {
    if (sort_bool::is_implies_application(x))
    {
      return 3;
    }
    else if (sort_bool::is_and_application(x)
             || sort_bool::is_or_application(x)
            )
    {
      return 4;
    }
    else if (data::is_equal_to_application(x)
             || data::is_not_equal_to_application(x)
            )
    {
      return 5;
    }
    else if (data::is_less_application(x)
             || data::is_less_equal_application(x)
             || data::is_greater_application(x)
             || data::is_greater_equal_application(x)
             || sort_list::is_in_application(x)
            )
    {
      return 6;
    }
    else if (sort_list::is_cons_application(x))
    {
      return is_cons_list(x) ? max_precedence : 9;
    }
    else if (sort_list::is_snoc_application(x))
    {
      return is_snoc_list(x) ? max_precedence : 7;
    }
    else if (sort_list::is_concat_application(x))
    {
      return 8;
    }
    else if (sort_real::is_plus_application(x)
             || sort_real::is_minus_application(x)
             || sort_set::is_setunion_application(x)
             || sort_set::is_setdifference_application(x)
             || sort_bag::is_bagjoin_application(x)
             || sort_bag::is_bagdifference_application(x)
            )
    {
      return 9;
    }
    else if (sort_int::is_div_application(x)
             || sort_int::is_mod_application(x)
             || sort_real::is_divides_application(x)
            )
    {
      return 10;
    }
    else if (sort_int::is_times_application(x)
             || sort_list::is_element_at_application(x)
             || sort_set::is_setintersection_application(x)
             || sort_bag::is_bagintersect_application(x)
            )
    {
      return 11;
    }
  }
  return -1;
}

inline
int infix_precedence_right(const data_expression& x)
{
  if (is_application(x))
  {
    if (sort_bool::is_implies_application(x))
    {
      return 2;
    }
    else if (sort_bool::is_and_application(x)
             || sort_bool::is_or_application(x)
            )
    {
      return 3;
    }
    else if (data::is_equal_to_application(x)
             || data::is_not_equal_to_application(x)
            )
    {
      return 4;
    }
    else if (data::is_less_application(x)
             || data::is_less_equal_application(x)
             || data::is_greater_application(x)
             || data::is_greater_equal_application(x)
             || sort_list::is_in_application(x)
            )
    {
      return 6;
    }
    else if (sort_list::is_cons_application(x))
    {
      return is_cons_list(x) ? max_precedence : 6;
    }
    else if (sort_list::is_snoc_application(x))
    {
      return is_snoc_list(x) ? max_precedence : 9;
    }
    else if (sort_list::is_concat_application(x))
    {
      return 9;
    }
    else if (sort_real::is_plus_application(x)
             || sort_real::is_minus_application(x)
             || sort_set::is_setunion_application(x)
             || sort_set::is_setdifference_application(x)
             || sort_bag::is_bagjoin_application(x)
             || sort_bag::is_bagdifference_application(x)
            )
    {
      return 10;
    }
    else if (sort_int::is_div_application(x)
             || sort_int::is_mod_application(x)
             || sort_real::is_divides_application(x)
            )
    {
      return 11;
    }
    else if (sort_int::is_times_application(x)
             || sort_list::is_element_at_application(x)
             || sort_set::is_setintersection_application(x)
             || sort_bag::is_bagintersect_application(x)
            )
    {
      return 12;
    }
  }
  return -1;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_PRECEDENCE_H
