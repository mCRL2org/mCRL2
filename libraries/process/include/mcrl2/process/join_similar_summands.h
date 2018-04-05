// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/join_similar_summands.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_JOIN_SIMILAR_SUMMANDS_H
#define MCRL2_PROCESS_JOIN_SIMILAR_SUMMANDS_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/standard_numbers_utility.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/join.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace process {

namespace detail {

// returns l + [x]
inline
data::data_expression list_append(const data::data_expression& l, const data::data_expression& x)
{
  // TODO: x.sort() should probably be replaced by the element sort of l
  return data::sort_list::snoc(x.sort(), l, x);
}

inline
std::vector<data::variable> common_left_hand_sides(const std::vector<process_instance_assignment>& x)
{
  std::set<data::variable> result;
  for (const process_instance_assignment& x_i: x)
  {
    for (const data::assignment& a: x_i.assignments())
    {
      result.insert(a.lhs());
    }
  }
  return std::vector<data::variable>(result.begin(), result.end());
}

// creates a 'cons' list of the elements in x
inline
data::data_expression make_cons_list(const std::vector<data::data_expression>& x)
{
  data::data_expression result = data::sort_list::empty(x.front().sort());
  for (const data::data_expression& x_i: x)
  {
    result = list_append(result, x_i);
  }
  return result;
}


// Input: [c(e1), ..., c(en)]
// Returns c(e) such that c(e)[q := i] equals c(ei)
inline
data::data_expression join_conditions(const std::vector<data::data_expression>& conditions, const data::variable& q)
{
  return data::sort_list::element_at(data::sort_bool::bool_(), make_cons_list(conditions), q);
}

// Input: [a(e1), ..., a(en)]
// Returns a(e) such that a(e)[q := i] equals a(ei)
inline
action join_actions(const std::vector<action>& actions, const data::variable& q)
{
  std::size_t m = actions.front().arguments().size();
  const action_label& label = actions.front().label();

  std::vector<std::vector<data::data_expression>> v(m);
  for (const action& a: actions)
  {
    std::size_t i = 0;
    for (const auto& arg: a.arguments())
    {
      v[i].push_back(arg);
      i++;
    }
  }

  std::vector<data::data_expression> expressions;
  for (const std::vector<data::data_expression>& v_i: v)
  {
    const data::sort_expression& sort = v_i.front().sort();
    expressions.push_back(data::sort_list::element_at(sort, make_cons_list(v_i), q));
  }

  return action(label, data::data_expression_list(expressions.begin(), expressions.end()));
}

// Joins sequences of the shape Q(e1) + Q(e2) + ... + Q(en)
// Returns an expression of the shape Q(e), such that Q(e)[q := i] equals Q(ei)
inline
process_instance_assignment join_process_instances(const std::vector<process_instance_assignment>& x, const data::variable& q)
{
  // make assignments
  std::vector<data::variable> left_hand_sides = common_left_hand_sides(x);
  std::size_t m = left_hand_sides.size();
  std::map<data::variable, std::size_t> index;
  for (std::size_t i = 0; i < m; i++)
  {
    index[left_hand_sides[i]] = i;
  }
  std::vector<data::data_expression> default_expressions(left_hand_sides.begin(), left_hand_sides.end());
  std::vector<data::data_expression> w;

  // start with empty list for each variable
  for (const data::variable& v: left_hand_sides)
  {
    w.push_back(data::sort_list::empty(v.sort()));
  }

  // append the right hand sides of process instance assignments
  for (const process_instance_assignment& x_i: x)
  {
    std::vector<data::data_expression> expressions = default_expressions;
    for (const data::assignment& a: x_i.assignments())
    {
      expressions[index[a.lhs()]] = a.rhs();
    }
    for (std::size_t i = 0; i < m; i++)
    {
      w[i] = list_append(w[i], expressions[i]);
    }
  }

  // make assignment list
  data::assignment_vector assignments;
  for (std::size_t i = 0; i < m; i++)
  {
    assignments.push_back(data::assignment(left_hand_sides[i], data::sort_list::element_at(left_hand_sides[i].sort(), w[i], q)));
  }
  return process_instance_assignment(x.front().identifier(), data::assignment_list(assignments.begin(), assignments.end()));
}

// Joins sequences of the shape c1 -> Q(e1) + c2 -> Q(e2) + ... + cn -> Q(en)
// Returns an expression of the shape c -> Q(e), such that c -> Q(e)[q := i] equals ci -> Q(ei)
inline
process_expression join_condition_process_instances(const std::vector<process_expression>& x, std::size_t lowerbound)
{
  std::size_t N = x.size();
  if (N <= lowerbound)
  {
    return join_summands(x.begin(), x.end());
  }

  std::vector<data::data_expression> C;
  std::vector<process_instance_assignment> X;
  for (const process_expression& x_i: x)
  {
    const data::data_expression& c = atermpp::down_cast<if_then>(x_i).condition();
    const process_expression& then_case = atermpp::down_cast<if_then>(x_i).then_case();
    const auto& Q = atermpp::down_cast<process_instance_assignment>(then_case);
    C.push_back(c);
    X.push_back(Q);
  }

  data::variable q(core::identifier_string("q"), data::sort_nat::nat());
  data::data_expression n = data::sort_nat::nat(std::to_string(N));
  data::data_expression c = join_conditions(C, q);
  process_instance_assignment Q = join_process_instances(X, q);
  return sum({ q }, if_then(data::less(q, n), if_then(c, Q)));
}

// Joins sequences of the shape a(f1).Q1(e1) + ... + a(fn).Qn(en)
inline
process_expression join_action_process_instances(const std::vector<process_expression>& x, std::size_t lowerbound)
{
  std::size_t N = x.size();
  if (N <= lowerbound)
  {
    return join_summands(x.begin(), x.end());
  }

  std::vector<action> A;
  std::vector<process_instance_assignment> X;
  for (const process_expression& x_i: x)
  {
    const process_expression& left = atermpp::down_cast<seq>(x_i).left();
    const process_expression& right = atermpp::down_cast<seq>(x_i).right();
    const auto& a = atermpp::down_cast<action>(left);
    const auto& Q = atermpp::down_cast<process_instance_assignment>(right);
    A.push_back(a);
    X.push_back(Q);
  }

  data::variable q(core::identifier_string("q"), data::sort_nat::nat());
  data::data_expression n = data::sort_nat::nat(std::to_string(N));
  action a = join_actions(A, q);
  process_instance_assignment Q = join_process_instances(X, q);
  return sum({ q }, if_then(data::less(q, n), seq(a, Q)));
}

// Joins sequences of the shape c1 -> a(f1).Q1(e1) + ... + cn -> a(fn).Qn(en)
inline
process_expression join_condition_action_process_instances(const std::vector<process_expression>& x, std::size_t lowerbound)
{
  std::size_t N = x.size();
  if (N <= lowerbound)
  {
    return join_summands(x.begin(), x.end());
  }

  std::vector<data::data_expression> C;
  std::vector<action> A;
  std::vector<process_instance_assignment> X;
  for (const process_expression& x_i: x)
  {
    const data::data_expression& c = atermpp::down_cast<if_then>(x_i).condition();
    const process_expression& then_case = atermpp::down_cast<if_then>(x_i).then_case();
    const process_expression& left = atermpp::down_cast<seq>(then_case).left();
    const process_expression& right = atermpp::down_cast<seq>(then_case).right();
    const auto& a = atermpp::down_cast<action>(left);
    const auto& Q = atermpp::down_cast<process_instance_assignment>(right);
    C.push_back(c);
    A.push_back(a);
    X.push_back(Q);
  }

  data::variable q(core::identifier_string("q"), data::sort_nat::nat());
  data::data_expression n = data::sort_nat::nat(std::to_string(N));
  data::data_expression c = join_conditions(C, q);
  action a = join_actions(A, q);
  process_instance_assignment Q = join_process_instances(X, q);
  return sum({ q }, if_then(data::less(q, n), if_then(c, seq(a, Q))));
}

// Joins sequences of the shape Q + Q + ... + Q and a.Q + a.Q + ... + a.Q.
inline
process_expression join_similar_summands(const process_expression& x, std::size_t lowerbound)
{
  if (!is_choice(x))
  {
    return x;
  }

  std::vector<process_expression> summands;
  std::map<process_identifier, std::vector<process_instance_assignment>> process_summands;
  std::map<process_identifier, std::vector<process_expression>> condition_summands;
  std::map<action_label, std::map<process_identifier, std::vector<process_expression>>> action_summands;
  std::map<action_label, std::map<process_identifier, std::vector<process_expression>>> condition_action_summands;

  // separate the summands
  auto x_split = split_summands(x);
  if (x_split.size() <= lowerbound)
  {
    return x;
  }
  for (const process_expression& x_i: x_split)
  {
    if (is_process_instance_assignment(x_i))
    {
      const auto& xi = atermpp::down_cast<process_instance_assignment>(x_i);
      process_summands[xi.identifier()].push_back(xi);
mCRL2log(log::debug) << "<process_summand>" << x_i << std::endl;
      continue;
    }
    if (is_seq(x_i))
    {
      const process_expression& left = atermpp::down_cast<seq>(x_i).left();
      const process_expression& right = atermpp::down_cast<seq>(x_i).right();
      if (is_action(left) && (is_process_instance_assignment(right)))
      {
        const auto& a = atermpp::down_cast<action>(left);
        const auto& Q = atermpp::down_cast<process_instance_assignment>(right);
        action_summands[a.label()][Q.identifier()].push_back(x_i);
        continue;
mCRL2log(log::debug) << "<action_summand>" << x_i << std::endl;
      }
    }
    if (is_if_then(x_i))
    {
      const process_expression& then_case = atermpp::down_cast<if_then>(x_i).then_case();
      if (is_process_instance_assignment(then_case))
      {
        const auto& xi = atermpp::down_cast<process_instance_assignment>(then_case);
        condition_summands[xi.identifier()].push_back(x_i);
mCRL2log(log::debug) << "<condition_summand>" << x_i << std::endl;
        continue;
      }
      if (is_seq(then_case))
      {
        const process_expression& left = atermpp::down_cast<seq>(then_case).left();
        const process_expression& right = atermpp::down_cast<seq>(then_case).right();
        if (is_action(left) && (is_process_instance_assignment(right)))
        {
          const auto& a = atermpp::down_cast<action>(left);
          const auto& Q = atermpp::down_cast<process_instance_assignment>(right);
          condition_action_summands[a.label()][Q.identifier()].push_back(x_i);
mCRL2log(log::debug) << "<condition_action_summand>" << x_i << std::endl;
          continue;
        }
      }
    }
mCRL2log(log::debug) << "<other>" << x_i << std::endl;
    summands.push_back(x_i);
  }

  // add process_summands to summands
  for (const auto& p: process_summands)
  {
    const std::vector<process_instance_assignment>& v = p.second;
    std::size_t N = v.size();
    if (N <= lowerbound)
    {
      summands.push_back(join_summands(v.begin(), v.end()));
    }
    else
    {
      data::variable q(core::identifier_string("q"), data::sort_nat::nat());
      data::data_expression n = data::sort_nat::nat(std::to_string(N));
      summands.push_back(sum({ q }, if_then(data::less(q, n), join_process_instances(v, q))));
    }
  }

  // add condition_summands to summands
  for (const auto& p: condition_summands)
  {
    summands.push_back(join_condition_process_instances(p.second, lowerbound));
  }

  // add action_summands to summands
  for (const auto& p: action_summands)
  {
    for (const auto& q: p.second)
    {
      summands.push_back(join_action_process_instances(q.second, lowerbound));
    }
  }

  // add condition_action_summands to summands
  for (const auto& p: condition_action_summands)
  {
    for (const auto& q: p.second)
    {
      summands.push_back(join_condition_action_process_instances(q.second, lowerbound));
    }
  }

mCRL2log(log::debug) << "<result>" << core::detail::print_set(summands) << std::endl;
  return join_summands(summands.begin(), summands.end());
}

struct join_similar_summands_builder: public process_expression_builder<join_similar_summands_builder>
{
  typedef process_expression_builder<join_similar_summands_builder> super;
  using super::apply;

  std::size_t lowerbound;

  join_similar_summands_builder(std::size_t lowerbound_)
    : lowerbound(lowerbound_)
  {}

  process_expression apply(const process::choice& x)
  {
    std::vector<process_expression> v = split_summands(x);
    for (process_expression& v_i: v)
    {
      v_i = apply(v_i);
    }
    process_expression x1 = join_summands(v.begin(), v.end());
    return join_similar_summands(x1, lowerbound);
  }
};

} // namespace detail

// Replaces sequences of similar summands by sum expressions in equations like
//
//   P = Q + Q + ... + Q
//   P = c -> Q + c -> Q + ... + c -> Q
//   P = a.Q + a.Q + ... + a.Q
//   P = c -> a.Q + c -> a.Q + ... + c -> a.Q
//
// The value lowerbound is the minimal length of a sequence that is joined.
inline
void join_similar_summands(process_specification& procspec, std::size_t lowerbound = 5)
{
  detail::join_similar_summands_builder f(lowerbound);
  for (process_equation& eqn: procspec.equations())
  {
    eqn = process_equation(eqn.identifier(), eqn.formal_parameters(), f.apply(eqn.expression()));
  }
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_JOIN_SIMILAR_SUMMANDS_H
