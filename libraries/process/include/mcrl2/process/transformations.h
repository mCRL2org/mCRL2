// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/transformations.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_TRANSFORMATIONS_H
#define MCRL2_PROCESS_TRANSFORMATIONS_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/join.h"

namespace mcrl2 {

namespace process {

namespace detail {

struct expand_if_then_else_builder: public process_expression_builder<expand_if_then_else_builder>
{
  typedef process_expression_builder<expand_if_then_else_builder> super;
  using super::apply;

  process_expression apply(const process::if_then_else& x)
  {
    process_expression then_case = apply(x.then_case());
    process_expression else_case = apply(x.else_case());
    return choice(if_then(x.condition(), then_case), if_then(data::not_(x.condition()), else_case));
  }
};

struct expand_if_then_choice_builder: public process_expression_builder<expand_if_then_choice_builder>
{
  typedef process_expression_builder<expand_if_then_choice_builder> super;
  using super::apply;

  process_expression apply(const process::if_then& x)
  {
    process_expression then_case = apply(x.then_case());
    if (is_choice(then_case))
    {
      std::vector<process_expression> summands = split_summands(then_case);
      for (auto& summand: summands)
      {
        summand = if_then(x.condition(), summand);
      }
      return join_summands(summands.begin(), summands.end());
    }
    return if_then(x.condition(), then_case);
  }
};

struct remove_nested_if_then_builder: public process_expression_builder<remove_nested_if_then_builder>
{
  typedef process_expression_builder<remove_nested_if_then_builder> super;
  using super::apply;

  process_expression apply(const process::if_then& x)
  {
    process_expression then_case = apply(x.then_case());
    if (is_if_then(then_case))
    {
      const if_then& nested_then_case = atermpp::down_cast<if_then>(then_case);
      data::data_expression condition = data::and_(x.condition(), nested_then_case.condition());
      return if_then(condition, nested_then_case.then_case());
    }
    return if_then(x.condition(), then_case);
  }
};

struct convert_process_instances_builder: public process_expression_builder<convert_process_instances_builder>
{
  typedef process_expression_builder<convert_process_instances_builder> super;
  using super::apply;

  std::map<process_identifier, const process_equation*> equation_index;

  convert_process_instances_builder(process_specification& procspec)
  {
    for (const process_equation& eqn: procspec.equations())
    {
      equation_index[eqn.identifier()] = &eqn;
    }
  }

  process_expression apply(const process::process_instance& x)
  {
    const data::variable_list& d = equation_index.find(x.identifier())->second->formal_parameters();
    const data::data_expression_list& e = x.actual_parameters();
    auto di = d.begin();
    auto ei = e.begin();
    std::vector<data::assignment> assignments;
    for (; di != d.end(); ++di, ++ei)
    {
      if (*di != *ei)
      {
        assignments.emplace_back(*di, *ei);
      }
    }
    return process_instance_assignment(x.identifier(), data::assignment_list(assignments.begin(), assignments.end()));
  }
};

inline
process_expression push_action_inside(const action& a, const process_expression& x)
{
  if (is_if_then(x))
  {
    const if_then& x_ = atermpp::down_cast<if_then>(x);
    return if_then(x_.condition(), push_action_inside(a, x_.then_case()));
  }
  else if (is_if_then_else(x))
  {
    const if_then_else& x_ = atermpp::down_cast<if_then_else>(x);
    return if_then_else(x_.condition(), push_action_inside(a, x_.then_case()), push_action_inside(a, x_.else_case()));
  }
  else if (is_seq(x))
  {
    const seq& x_ = atermpp::down_cast<seq>(x);
    return seq(push_action_inside(a, x_.left()), x_.right());
  }
  else if (is_choice(x))
  {
    auto summands = split_summands(x);
    for (auto& summand: summands)
    {
      summand = push_action_inside(a, summand);
    }
    return join_summands(summands.begin(), summands.end());
  }
  else if (is_sum(x) && a.arguments().empty()) // conservative
  {
    const sum& x_ = atermpp::down_cast<sum>(x);
    return sum(x_.variables(), push_action_inside(a, x_.operand()));
  }
  return seq(a, x);
}

struct push_actions_inside_builder: public process_expression_builder<push_actions_inside_builder>
{
  typedef process_expression_builder<push_actions_inside_builder> super;
  using super::apply;

  process_expression apply(const process::seq& x)
  {
    process_expression left  = apply(x.left());
    process_expression right = apply(x.right());
    if (is_action(left))
    {
      return push_action_inside(atermpp::down_cast<action>(left), right);
    }
    else
    {
      return seq(left, right);
    }
  }
};

inline
bool is_dummy_action(const process_expression& x)
{
  return is_action(x) && atermpp::down_cast<action>(x).label() == action_label(core::identifier_string("dummy"), {});
}

inline
bool has_action_prefix(const process_expression& x)
{
  return is_action(x) || (is_seq(x) && has_action_prefix(atermpp::down_cast<seq>(x).left()));
}

struct remove_dummy_action_builder: public process_expression_builder<remove_dummy_action_builder>
{
  typedef process_expression_builder<remove_dummy_action_builder> super;
  using super::apply;

  process_expression apply(const process::seq& x)
  {
    process_expression left  = apply(x.left());
    process_expression right = apply(x.right());
    if (is_dummy_action(left) && has_action_prefix(right))
    {
      return right;
    }
    return seq(left, right);
  }
};

} // namespace detail

inline
void expand_if_then_else(process_specification& procspec)
{
  detail::expand_if_then_else_builder f;
  f.update(procspec);
}

inline
void expand_if_then_choice(process_specification& procspec)
{
  detail::expand_if_then_choice_builder f;
  f.update(procspec);
}

inline
void remove_nested_if_then(process_specification& procspec)
{
  detail::remove_nested_if_then_builder f;
  f.update(procspec);
}

inline
void convert_process_instances(process_specification& procspec)
{
  detail::convert_process_instances_builder f(procspec);
  f.update(procspec);
}

inline
void push_actions_inside(process_specification& procspec)
{
  detail::push_actions_inside_builder f;
  f.update(procspec);
}

inline
void remove_dummy_action(process_specification& procspec)
{
  detail::remove_dummy_action_builder f;
  f.update(procspec);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TRANSFORMATIONS_H
