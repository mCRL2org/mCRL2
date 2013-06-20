// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/data/standard.h"

namespace mcrl2 {

namespace data {

namespace detail {

struct
one_point_rule_preprocessor
{
  data::data_expression operator()(const data::data_expression& x) const
  {
    if (data::sort_bool::is_not_application(x)) // x == !y
    {
      data::data_expression y = unary_operand1(x);
      if (data::sort_bool::is_not_application(y))
      {
        return (*this)(unary_operand1(y));
      }
      else if (data::sort_bool::is_and_application(y))
      {
        std::set<data::data_expression> args = data::split_and(y);
        std::vector<data::data_expression> result;
        for (std::set<data::data_expression>::iterator i = args.begin(); i != args.end(); ++i)
        {
          result.push_back((*this)(data::sort_bool::not_(*i)));
        }
        return data::join_or(result.begin(), result.end());
      }
      else if (data::sort_bool::is_or_application(y))
      {
        std::set<data::data_expression> args = data::split_or(y);
        std::vector<data::data_expression> result;
        for (std::set<data::data_expression>::iterator i = args.begin(); i != args.end(); ++i)
        {
          result.push_back((*this)(data::sort_bool::not_(*i)));
        }
        return data::join_and(result.begin(), result.end());
      }
      else if (data::is_equal_to_application(y))
      {
        return data::not_equal_to(binary_left(atermpp::aterm_cast<application>(y)), binary_right(atermpp::aterm_cast<application>(y)));
      }
      else if (data::is_not_equal_to_application(y))
      {
        return data::equal_to(binary_left(atermpp::aterm_cast<application>(y)), binary_right(atermpp::aterm_cast<application>(y)));
      }
    }
    return x;
  }
};

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_ONE_POINT_RULE_PREPROCESSOR_H
