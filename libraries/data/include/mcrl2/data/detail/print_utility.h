// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/print_utility.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_PRINT_UTILITY_H
#define MCRL2_DATA_DETAIL_PRINT_UTILITY_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/function_update.h"
#include "mcrl2/data/lambda.h"

namespace mcrl2 {

namespace data {

namespace detail {

/// \pre BoolExpr is a boolean expression, SortExpr is of type Pos, Nat, Int or
//     Real.
/// \return if(BoolExpr, 1, 0) of sort SortExpr
inline
data::data_expression bool_to_numeric(data::data_expression const& e, data::sort_expression const& s)
{
  // TODO Maybe enforce that SortExpr is a PNIR sort
  return data::if_(e, data::function_symbol("1", s), data::function_symbol("0", s));
}

inline
data_expression reconstruct_pos_mult(const data_expression& x, std::vector<char>& result)
{
  data_expression reconstruction_result;
  if (data::sort_pos::is_c1_function_symbol(x))
  {
    //x is 1; return result
    reconstruction_result = data::function_symbol(data::detail::vector_number_to_string(result), data::sort_pos::pos());
  }
  else if (data::sort_pos::is_cdub_application(x))
  {
    //x is of the form cDub(b,p); return (result*2)*v(p) + result*v(b)
    data_expression bool_arg = sort_pos::left(x);
    data_expression pos_arg = sort_pos::right(x);
    std::vector<char> double_result = result;
    data::detail::decimal_number_multiply_by_two(double_result);
    pos_arg = reconstruct_pos_mult(pos_arg, double_result);
    if (data::sort_bool::is_false_function_symbol(bool_arg))
    {
      //result*v(b) = 0
      reconstruction_result = pos_arg;
    }
    else if (data::sort_bool::is_true_function_symbol(bool_arg))
    {
      //result*v(b) = result
      reconstruction_result = data::sort_real::plus(pos_arg,
                                                    data::function_symbol(data::detail::vector_number_to_string(result), data::sort_pos::pos()));
    }
    else if (data::detail::vector_number_to_string(result) == "1")
    {
      //result*v(b) = v(b)
      reconstruction_result = data::sort_real::plus(pos_arg, bool_to_numeric(bool_arg, data::sort_nat::nat()));
    }
    else
    {
      //result*v(b)
      reconstruction_result = data::sort_real::plus(pos_arg,
                                                    data::sort_real::times(data::function_symbol(data::detail::vector_number_to_string(result), data::sort_nat::nat()),
                                                    bool_to_numeric(bool_arg, data::sort_nat::nat())));
    }
  }
  else
  {
    //x is not a Pos constructor
    if (data::detail::vector_number_to_string(result) == "1")
    {
      reconstruction_result = x;
    }
    else
    {
      reconstruction_result = data::sort_real::times(data::function_symbol(data::detail::vector_number_to_string(result), data::sort_pos::pos()), x);
    }
  }
  return reconstruction_result;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_PRINT_UTILITY_H
