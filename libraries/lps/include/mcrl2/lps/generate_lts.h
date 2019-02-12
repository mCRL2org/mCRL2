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
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <utility>
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/specification.h"

namespace std
{

/// \brief specialization of the standard std::hash function.
template<>
struct hash<mcrl2::lps::multi_action>
{
  std::size_t operator()(const mcrl2::lps::multi_action& x) const
  {
    std::size_t seed = std::hash<atermpp::aterm>()(x.actions());
    if (!x.actions().empty())
    {
      seed = std::hash<atermpp::aterm>()(x.arguments()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    if (x.has_time())
    {
      seed = std::hash<atermpp::aterm>()(x.time()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

}

namespace mcrl2 {

namespace lps {

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
  std::size_t initial_state;
  std::size_t number_of_states;

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

/// \brief Simple algorithm to generate an LTS from a linear process specification
/// \param report_transition Callback function that reports the transtions that are found
/// \pre lpsspec may not have any 'global' variables
template <typename ReportTransition>
void generate_lts(const specification& lpsspec, const data::rewriter& r, ReportTransition report_transition)
{
  typedef data::data_expression_list state_type;
  typedef data::enumerator_list_element_with_substitution<data::data_expression> enumerator_element;

  data::mutable_indexed_substitution<> sigma;
  const data::variable_list& process_parameters = lpsspec.process().process_parameters();
  state_type d0 = lpsspec.initial_process().state(process_parameters);

  std::deque<state_type> todo{d0};
  std::unordered_set<state_type> explored{d0};

  auto rewrite_data_expression_list = [&](const data::data_expression_list& v)
  {
    return data::data_expression_list(v.begin(), v.end(), [&](const data::data_expression& x) { return r(x, sigma); });
  };

  auto rewrite_action = [&](const process::action& a)
  {
    return process::action(a.label(), rewrite_data_expression_list(a.arguments()));
  };

  auto rewrite_multi_action = [&](const lps::multi_action& m)
  {
    const process::action_list& actions = m.actions();
    if (m.has_time())
    {
      return lps::multi_action(process::action_list(actions.begin(), actions.end(), [&](const process::action& a) { return rewrite_action(a); }), r(m.time(), sigma));
    }
    else
    {
      return lps::multi_action(process::action_list(actions.begin(), actions.end(), [&](const process::action& a) { return rewrite_action(a); }));
    }
  };

  bool accept_solutions_with_variables = true;
  data::enumerator_identifier_generator id_generator;
  data::enumerator_algorithm<data::rewriter, data::rewriter> E(r, lpsspec.data(), r, id_generator, accept_solutions_with_variables);

  while (!todo.empty())
  {
    state_type d = todo.front();
    todo.pop_front();

    auto di = d.begin();
    auto pi = process_parameters.begin();
    for (; di != d.end(); ++di, ++pi)
    {
      sigma[*pi] = *di;
    }

    for (const action_summand& summand: lpsspec.process().action_summands())
    {
      const data::variable_list& variables = summand.summation_variables();
      E.enumerate(enumerator_element(variables, summand.condition()),
                  sigma,
                  [&](const enumerator_element& p)
                  {
                    if (!is_true(p.expression()))
                    {
                      return false;
                    }
                    p.add_assignments(variables, sigma, r);
                    multi_action a = rewrite_multi_action(summand.multi_action());
                    state_type d1 = rewrite_data_expression_list(summand.next_state(process_parameters));
                    if (explored.find(d1) == explored.end())
                    {
                      todo.push_back(d1);
                      explored.insert(d1);
                    }
                    report_transition(d, a, d1);
                    return false;
                  },
                  data::is_false,
                  data::is_true
      );
    }
  }
}

inline
void generate_lts(const specification& lpsspec, const data::rewriter& r, labeled_transition_system& result)
{
  typedef data::data_expression_list state_type;

  std::unordered_map<data::data_expression_list, std::size_t> states;
  std::unordered_map<lps::multi_action, std::size_t> actions;

  auto add_state = [&](const state_type& s)
  {
    auto i = states.find(s);
    if (i == states.end())
    {
      i = states.emplace(std::make_pair(s, states.size())).first;
    }
    return i->second;
  };

  auto add_action = [&](const lps::multi_action& m)
  {
    auto i = actions.find(m);
    if (i == actions.end())
    {
      i = actions.emplace(std::make_pair(m, actions.size())).first;
    }
    return i->second;
  };

  lps::multi_action tau;
  add_action(tau);

  const data::variable_list& process_parameters = lpsspec.process().process_parameters();
  state_type d0 = lpsspec.initial_process().state(process_parameters);
  result.initial_state = add_state(d0);

  generate_lts(lpsspec,
               r,
               [&](const state_type& d, const lps::multi_action& m, const state_type& d1)
               {
                 std::size_t from = states.find(d)->second;
                 std::size_t label = add_action(m);
                 std::size_t to = add_state(d1);
                 result.add_transition(from, label, to);
               }
  );

  result.number_of_states = states.size();
  result.action_labels.resize(actions.size());
  for (const auto& p: actions)
  {
    result.action_labels[p.second] = lps::pp(p.first);
  }
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_GENERATE_LTS_H
