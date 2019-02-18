// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/generate_lts.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_GENERATE_LTS_H
#define MCRL2_LPS_GENERATE_LTS_H

#include <deque>
#include <iomanip>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "mcrl2/atermpp/indexed_set.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/order_summand_variables.h"
#include "mcrl2/lps/replace_constants_by_variables.h"
#include "mcrl2/lps/resolve_name_clashes.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/utilities/detail/io.h"

namespace mcrl2 {

namespace lps {

/// \brief The skip operation with one argument
struct skip1
{
  template <typename T>
  void operator()(const T&) const {}
};

/// \brief The skip operation with three arguments
struct skip3
{
  template <typename T1, typename T2, typename T3>
  void operator()(const T1&, const T2&, const T3&) const {}
};

// The states are identified by numbers in the interval [0 ... number_of_states).
// For each transition (from, label, to) we have
//    0 <= to < number_of_states
//    0 <= label < action_labels.size()
//    0 <= from < number_of_states.
//
// The special action "tau" is always at the front of action_labels.
/// \brief Simple LTS class
struct labeled_transition_system
{
  struct transition
  {
    std::size_t from;
    std::size_t label;
    std::size_t to;

    transition(std::size_t from_, std::size_t label_, std::size_t to_)
      : from(from_), label(label_), to(to_)
    {}

    bool operator<(const transition& other) const
    {
      return std::tie(from, label, to) < std::tie(other.from, other.label, other.to);
    }
  };

  std::vector<transition> transitions;
  std::vector<std::string> action_labels;
  std::size_t initial_state = 0;
  std::size_t number_of_states = 0;

  void add_transition(std::size_t from, std::size_t label, std::size_t to)
  {
    transitions.emplace_back(from, label, to);
  }
};

/// \brief Print a labeled_transition_system in .aut format.
inline
std::ostream& operator<<(std::ostream& out, const labeled_transition_system& x)
{
  out << "des (" << x.initial_state << ',' << x.transitions.size() << ',' << x.number_of_states << ")\n";
  for (const auto& t: x.transitions)
  {
    out << '(' << t.from << ",\"" << x.action_labels[t.label] << "\"," << t.to << ")\n";
  }
  return out;
}

template <typename StateType, class Transformer>
inline
data::data_expression_list
make_state(const data::data_expression_list& x,
           std::size_t /* n */,
           Transformer f,
           typename std::enable_if<std::is_same<StateType, data::data_expression_list>::value>::type* = nullptr)
{
  return data::data_expression_list(x.begin(), x.end(), [&](const data::data_expression& x) { return f(x); });
}

template <typename StateType, class Transformer>
inline
lps::state
make_state(const data::data_expression_list& x,
           std::size_t n,
           Transformer f,
           typename std::enable_if<std::is_same<StateType, lps::state>::value>::type* = nullptr)
{
  return lps::state(x.begin(), n, [&](const data::data_expression& x) { return f(x); });
}

struct generate_lts_options
{
  data::rewrite_strategy rewrite_strategy = data::jitty;
  bool one_point_rule_rewrite = false;
  bool replace_constants_by_variables = false;
  bool resolve_summand_variable_name_clashes = false;
  bool store_states_as_trees = true;
};

inline
std::ostream& operator<<(std::ostream& out, const generate_lts_options& options)
{
  out << "one_point_rule_rewrite = " << std::boolalpha << options.one_point_rule_rewrite << std::endl;
  out << "resolve_summand_variable_name_clashes = " << std::boolalpha << options.resolve_summand_variable_name_clashes << std::endl;
  out << "replace_constants_by_variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "rewrite_strategy = " << std::boolalpha << options.rewrite_strategy << std::endl;
  out << "store_states_as_trees = " << std::boolalpha << options.store_states_as_trees << std::endl;
  return out;
}

/// \brief Simple algorithm to generate an LTS from a linear process specification
/// \param report_state Callback function that reports the states that are disovered. Every state is reported only once.
/// \param report_transition Callback function that reports the transitions that are found.
template <typename StateType, typename ReportState = skip1, typename ReportTransition = skip3>
void generate_lts(const specification& lpsspec_,
                  const generate_lts_options& options,
                  ReportState report_state = ReportState(),
                  ReportTransition report_transition = ReportTransition()
                 )
{
  typedef data::enumerator_list_element_with_substitution<> enumerator_element;
  typedef StateType state_type;

  specification lpsspec = lpsspec_;
  atermpp::indexed_set<state_type> discovered;
  std::deque<std::size_t> todo;
  data::mutable_indexed_substitution<> sigma;
  const data::variable_list& process_parameters = lpsspec.process().process_parameters();
  const std::size_t n = process_parameters.size();
  bool accept_solutions_with_variables = false;

  data::rewriter r(lpsspec.data(), data::used_data_equation_selector(lpsspec.data(), lps::find_function_symbols(lpsspec), lpsspec.global_variables()), options.rewrite_strategy);

  // preprocessing
  detail::instantiate_global_variables(lpsspec);
  lps::order_summand_variables(lpsspec);
  if (options.resolve_summand_variable_name_clashes)  { resolve_summand_variable_name_clashes(lpsspec); }
  if (options.one_point_rule_rewrite)                 { one_point_rule_rewrite(lpsspec); }
  if (options.replace_constants_by_variables)         { replace_constants_by_variables(lpsspec, r, sigma); }

  data::enumerator_identifier_generator id_generator;
  data::enumerator_algorithm<data::rewriter, data::rewriter> E(r, lpsspec.data(), r, id_generator, accept_solutions_with_variables);

  auto rewrite_data_expression_list = [&](const data::data_expression_list& v)
  {
    return data::data_expression_list(v.begin(), v.end(), [&](const data::data_expression& x) { return r(x, sigma); });
  };

  auto rewrite_state = [&](const data::data_expression_list& v)
  {
    return make_state<state_type>(v, n, [&](const data::data_expression& x) { return r(x, sigma); });
  };

  auto rewrite_action = [&](const process::action& a)
  {
    return process::action(a.label(), rewrite_data_expression_list(a.arguments()));
  };

  auto rewrite_action_list = [&](const process::action_list& actions)
  {
    return process::action_list(actions.begin(), actions.end(), [&](const process::action& a) { return rewrite_action(a); });
  };

  state_type d0 = rewrite_state(lpsspec.initial_process().state(process_parameters));
  report_state(d0);
  auto k = discovered.put(d0);
  todo.push_back(k.first);

  while (!todo.empty())
  {
    std::size_t i = todo.front();
    todo.pop_front();
    const state_type& d = discovered.get(i);

    auto di = d.begin();
    auto pi = process_parameters.begin();
    for (; di != d.end(); ++di, ++pi)
    {
      sigma[*pi] = *di;
    }

    for (const action_summand& summand: lpsspec.process().action_summands())
    {
      const data::variable_list& variables = summand.summation_variables();
      data::data_expression condition = r(summand.condition(), sigma);
      if (!data::is_false(condition))
      {
        E.enumerate(enumerator_element(variables, condition),
                    sigma,
                    [&](const enumerator_element& p)
                    {
                      p.add_assignments(variables, sigma, r);
                      process::action_list a = rewrite_action_list(summand.multi_action().actions());
                      state_type d1 = rewrite_state(summand.next_state(process_parameters));
                      p.remove_assignments(variables, sigma);
                      auto j = discovered.put(d1);
                      if (j.second)
                      {
                        todo.push_back(j.first);
                        report_state(d1);
                      }
                      report_transition(i, a, j.first);
                      return false;
                    },
                    data::is_false
        );
      }
    }
  }
}

template <typename StateType>
void generate_labeled_transition_system(const specification& lpsspec,
                                        const generate_lts_options& options,
                                        labeled_transition_system& result
                                       )
{
  typedef process::action_list action_type;
  typedef StateType state_type;

  std::size_t number_of_states = 0;
  std::unordered_map<action_type, std::size_t> actions;

  auto add_action = [&](const action_type& a)
  {
    auto i = actions.find(a);
    if (i == actions.end())
    {
      i = actions.emplace(std::make_pair(a, actions.size())).first;
    }
    return i->second;
  };

  lps::multi_action tau;
  add_action(tau.actions());

  generate_lts<state_type>(lpsspec,
                           options,
                           [&](const state_type&)
                           {
                             number_of_states++;
                           },
                           [&](std::size_t from, const action_type& a, std::size_t to)
                           {
                             std::size_t label = add_action(a);
                             result.add_transition(from, label, to);
                           }
  );

  result.initial_state = 0;
  result.number_of_states = number_of_states;
  result.action_labels.resize(actions.size());
  for (const auto& p: actions)
  {
    result.action_labels[p.second] = lps::pp(lps::multi_action(p.first));
  }
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_GENERATE_LTS_H
