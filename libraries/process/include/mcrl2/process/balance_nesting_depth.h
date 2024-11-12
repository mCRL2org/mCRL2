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

namespace mcrl2 {

namespace process {

namespace detail
{
struct balance_summands_builder
    : public process_expression_builder<balance_summands_builder>
{
  typedef process_expression_builder<balance_summands_builder> super;
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
void balance_summands(T& x, typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr)
{
  detail::balance_summands_builder f;
  f.update(x);
}

/// \brief Reduces the nesting depth of the choice operator
template <typename T>
T balance_summands(const T& x, typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr)
{
  T result;
  detail::balance_summands_builder f;
  f.apply(result, x);
  return result;
}

namespace detail
{
struct balance_merge_builder : public process_expression_builder<balance_merge_builder>
{
  typedef process_expression_builder<balance_merge_builder> super;
  using super::apply;

  template <class T>
  void apply(T& result, const process::merge& x)
  {
    std::vector<process_expression> merges = split_merges(x);
    process_expression new_merge;
    for (process_expression& merge : merges)
    {
      super::apply(new_merge, merge);
      merge = new_merge;
    }

    result = utilities::detail::join_balanced<process_expression>(merges.begin(),
        merges.end(),
        [](const process::process_expression& x, const process_expression& y) { return merge(x, y); });
  }
};

} // namespace detail

/// \brief Reduces the nesting depth of the merge operator
template <typename T>
void balance_merge(T& x, typename std::enable_if<!std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr)
{
  detail::balance_merge_builder f;
  f.update(x);
}

/// \brief Reduces the nesting depth of the choice operator
template <typename T>
T balance_merge(const T& x, typename std::enable_if<std::is_base_of<atermpp::aterm, T>::value>::type* = nullptr)
{
  T result;
  detail::balance_merge_builder f;
  f.apply(result, x);
  return result;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_BALANCE_NESTING_DEPTH_H
