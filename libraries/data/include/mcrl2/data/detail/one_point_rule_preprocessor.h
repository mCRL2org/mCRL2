// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/one_point_rule_preprocessor.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_ONE_POINT_RULE_PREPROCESSOR_H
#define MCRL2_DATA_DETAIL_ONE_POINT_RULE_PREPROCESSOR_H

#include "mcrl2/data/join.h"

namespace mcrl2::data::detail
{

struct
one_point_rule_preprocessor
{
  data::data_expression operator()(const data::data_expression& x) const
  {
    if (data::sort_bool::is_not_application(x)) // x == !y
    {
      const data::data_expression& y = unary_operand1(x);
      if (data::sort_bool::is_not_application(y))
      {
        return (*this)(unary_operand1(y));
      }
      else if (data::sort_bool::is_and_application(y))
      {
        std::set<data::data_expression> args = data::split_and(y);
        std::vector<data::data_expression> result;
        for (const data::data_expression& arg: args)
        {
          result.push_back((*this)(data::sort_bool::not_(arg)));
        }
        return data::join_or(result.begin(), result.end());
      }
      else if (data::sort_bool::is_or_application(y))
      {
        std::set<data::data_expression> args = data::split_or(y);
        std::vector<data::data_expression> result;
        for (const data::data_expression& arg: args)
        {
          result.push_back((*this)(data::sort_bool::not_(arg)));
        }
        return data::join_and(result.begin(), result.end());
      }
      else if (data::is_equal_to_application(y))
      {
        return data::not_equal_to(binary_left(atermpp::down_cast<application>(y)), binary_right(atermpp::down_cast<application>(y)));
      }
      else if (data::is_not_equal_to_application(y))
      {
        return data::equal_to(binary_left(atermpp::down_cast<application>(y)), binary_right(atermpp::down_cast<application>(y)));
      }
    }
    return x;
  }
};

} // namespace mcrl2::data::detail

#endif // MCRL2_DATA_DETAIL_ONE_POINT_RULE_PREPROCESSOR_H
