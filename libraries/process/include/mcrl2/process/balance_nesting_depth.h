// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/balance_nesting_depth.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_BALANCE_NESTING_DEPTH_H
#define MCRL2_PROCESS_BALANCE_NESTING_DEPTH_H

#include "mcrl2/process/builder.h"
#include "mcrl2/process/join.h"
#include "mcrl2/utilities/detail/join.h"

namespace mcrl2::process
{

namespace detail
{
struct balance_summands_builder
    : public process_expression_builder<balance_summands_builder>
{
  using super = process_expression_builder<balance_summands_builder>;
  using super::apply;

  template <class T>
  void apply(T& result, const process::choice& x)
  {
    std::vector<process_expression> summands = split_summands(x);
    process_expression new_summand;
    for (process_expression& summand: summands)
    {
      super::apply(new_summand, summand);
      summand = new_summand;
    }
    
    result = utilities::detail::join_balanced<process_expression>(
        summands.begin(), 
        summands.end(),
        [](const process::process_expression& x, const process_expression& y) {
          return choice(x, y);
        });
  }
};

} // namespace detail

/// \brief Reduces the nesting depth of the choice operator
template <typename T>
void balance_summands(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  detail::balance_summands_builder f;
  f.update(x);
}

/// \brief Reduces the nesting depth of the choice operator
template <typename T>
T balance_summands(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  detail::balance_summands_builder f;
  f.apply(result, x);
  return result;
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_BALANCE_NESTING_DEPTH_H
