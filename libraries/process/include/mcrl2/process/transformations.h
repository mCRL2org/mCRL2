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

inline
action make_dummy_action()
{
  return action(action_label(core::identifier_string("dummy"), {}), {});
}

inline
bool is_dummy_action(const process_expression& x)
{
  return is_action(x) && x == make_dummy_action();
}

inline
bool has_action_prefix(const process_expression& x)
{
  return is_action(x) || (is_seq(x) && has_action_prefix(atermpp::down_cast<seq>(x).left()));
}

inline
process_expression make_if_then(const data::data_expression& b, const process_expression& x)
{
  if (is_if_then(x))
  {
    const auto& x_ = atermpp::down_cast<if_then>(x);
    return if_then(data::and_(b, x_.condition()), x_.then_case());
  }
  else if (is_choice(x))
  {
    std::vector<process_expression> summands = split_summands(x);
    for (auto& summand: summands)
    {
      summand = make_if_then(b, summand);
    }
    return join_summands(summands.begin(), summands.end());
  }
  return if_then(b, x);
}

inline
process_expression make_guarded(const process_expression& x, const action& dummy)
{
  if (process::is_action(x))
  {
    return x;
  }
  else if (process::is_process_instance(x))
  {
    return seq(dummy, x);
  }
  else if (process::is_process_instance_assignment(x))
  {
    return seq(dummy, x);
  }
  else if (process::is_delta(x))
  {
    return x;
  }
  else if (process::is_tau(x))
  {
    return x;
  }
  else if (process::is_sum(x))
  {
    const auto& x_ = atermpp::down_cast<process::sum>(x);
    return sum(x_.variables(), make_guarded(x_.operand(), dummy));
  }
  else if (process::is_block(x))
  {
    const auto& x_ = atermpp::down_cast<process::block>(x);
    return block(x_.block_set(), make_guarded(x_.operand(), dummy));
  }
  else if (process::is_hide(x))
  {
    const auto& x_ = atermpp::down_cast<process::hide>(x);
    return hide(x_.hide_set(), make_guarded(x_.operand(), dummy));
  }
  else if (process::is_rename(x))
  {
    const auto& x_ = atermpp::down_cast<process::rename>(x);
    return rename(x_.rename_set(), make_guarded(x_.operand(), dummy));
  }
  else if (process::is_comm(x))
  {
    const auto& x_ = atermpp::down_cast<process::comm>(x);
    return comm(x_.comm_set(), make_guarded(x_.operand(), dummy));
  }
  else if (process::is_allow(x))
  {
    const auto& x_ = atermpp::down_cast<process::allow>(x);
    return allow(x_.allow_set(), make_guarded(x_.operand(), dummy));
  }
  else if (process::is_sync(x))
  {
    const auto& x_ = atermpp::down_cast<process::sync>(x);
    return sync(make_guarded(x_.left(), dummy), make_guarded(x_.right(), dummy));
  }
  else if (process::is_at(x))
  {
    const auto& x_ = atermpp::down_cast<process::at>(x);
    return at(make_guarded(x_.operand(), dummy), x_.time_stamp());
  }
  else if (process::is_seq(x))
  {
    const auto& x_ = atermpp::down_cast<process::seq>(x);
    if (is_action(x_.left()))
    {
      return x;
    }
    return seq(x_.left(), make_guarded(x_.right(), dummy));
  }
  else if (process::is_if_then(x))
  {
    const auto& x_ = atermpp::down_cast<process::if_then>(x);
    return if_then(x_.condition(), make_guarded(x_.then_case(), dummy));
  }
  else if (process::is_if_then_else(x))
  {
    const auto& x_ = atermpp::down_cast<process::if_then_else>(x);
    return if_then_else(x_.condition(), make_guarded(x_.then_case(), dummy), make_guarded(x_.else_case(), dummy));
  }
  else if (process::is_merge(x))
  {
    const auto& x_ = atermpp::down_cast<process::merge>(x);
    return merge(make_guarded(x_.left(), dummy), make_guarded(x_.right(), dummy));
  }
  else if (process::is_left_merge(x))
  {
    const auto& x_ = atermpp::down_cast<process::left_merge>(x);
    return left_merge(make_guarded(x_.left(), dummy), make_guarded(x_.right(), dummy));
  }
  else if (process::is_choice(x))
  {
    const auto& x_ = atermpp::down_cast<process::choice>(x);
    return choice(make_guarded(x_.left(), dummy), make_guarded(x_.right(), dummy));
  }
  throw mcrl2::runtime_error("make_guarded: unsupported case");
}

// N.B. Does not preserve branching bisimilarity!
inline
process_expression make_seq(const action &a, const process_expression &x)
{
  if (is_if_then(x))
  {
    const auto& x_ = atermpp::down_cast<if_then>(x);
    return if_then(x_.condition(), make_seq(a, x_.then_case()));
  }
  else if (is_if_then_else(x))
  {
    const auto& x_ = atermpp::down_cast<if_then_else>(x);
    return if_then_else(x_.condition(), make_seq(a, x_.then_case()), make_seq(a, x_.else_case()));
  }
  else if (is_seq(x))
  {
    const auto& x_ = atermpp::down_cast<seq>(x);
    return seq(make_seq(a, x_.left()), x_.right());
  }
  else if (is_choice(x))
  {
    auto summands = split_summands(x);
    for (auto& summand: summands)
    {
      summand = make_seq(a, summand);
    }
    return join_summands(summands.begin(), summands.end());
  }
  else if (is_sum(x) && a.arguments().empty()) // conservative
  {
    const auto& x_ = atermpp::down_cast<sum>(x);
    return sum(x_.variables(), make_seq(a, x_.operand()));
  }
  return seq(a, x);
}

// b -> x <> y   =>   (b -> x) + (!b -> y)
// b -> (c -> x) =>   (b && c) -> x
// b -> (x + y)  =>   (b -> x) + (b -> y)
struct expand_if_then_else_builder: public process_expression_builder<expand_if_then_else_builder>
{
  typedef process_expression_builder<expand_if_then_else_builder> super;
  using super::apply;

  process_expression apply(const process::if_then_else& x)
  {
    process_expression then_case = apply(x.then_case());
    process_expression else_case = apply(x.else_case());
    auto result = choice(make_if_then(x.condition(), then_case), make_if_then(data::not_(x.condition()), else_case));
    return result;
  }

  process_expression apply(const process::if_then& x)
  {
    process_expression then_case = apply(x.then_case());
    return make_if_then(x.condition(), then_case);
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
      return make_seq(atermpp::down_cast<action>(left), right);
    }
    else
    {
      return seq(left, right);
    }
  }
};

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

inline
void make_guarded(process_specification& procspec, const action& dummy)
{
  for (process_equation& eqn: procspec.equations())
  {
    eqn = process_equation(eqn.identifier(), eqn.formal_parameters(), detail::make_guarded(eqn.expression(), dummy));
  }
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TRANSFORMATIONS_H
