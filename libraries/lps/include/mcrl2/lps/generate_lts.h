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
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/detail/io.h"

namespace mcrl2 {

namespace lps {

/// \brief The skip operation with a variable number of arguments
struct skip
{
  template<typename... Args>
  void operator()(const Args&...) const {}
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

struct generate_lts_options
{
  data::rewrite_strategy rewrite_strategy = data::jitty;
  bool one_point_rule_rewrite = false;
  bool replace_constants_by_variables = false;
  bool resolve_summand_variable_name_clashes = false;
  bool store_states_as_trees = true;
  bool cached = false;
  bool confluence = false;
};

inline
std::ostream& operator<<(std::ostream& out, const generate_lts_options& options)
{
  out << "cached = " << std::boolalpha << options.cached << std::endl;
  out << "confluence = " << std::boolalpha << options.confluence << std::endl;
  out << "one_point_rule_rewrite = " << std::boolalpha << options.one_point_rule_rewrite << std::endl;
  out << "resolve_summand_variable_name_clashes = " << std::boolalpha << options.resolve_summand_variable_name_clashes << std::endl;
  out << "replace_constants_by_variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "rewrite_strategy = " << std::boolalpha << options.rewrite_strategy << std::endl;
  out << "store_states_as_trees = " << std::boolalpha << options.store_states_as_trees << std::endl;
  return out;
}

template <typename StateType>
inline
void add_assignments(data::mutable_indexed_substitution<>& sigma, const data::variable_list& v, const StateType& e)
{
  assert(v.size() == e.size());
  auto vi = v.begin();
  auto ei = e.begin();
  for (; vi != v.end(); ++vi, ++ei)
  {
    sigma[*vi] = *ei;
  }
}

inline
void remove_assignments(data::mutable_indexed_substitution<>& sigma, const data::variable_list& v)
{
  for (const data::variable& vi: v)
  {
    sigma[vi] = vi;
  }
}

inline
data::data_expression_list substitute(data::mutable_indexed_substitution<>& sigma, const data::variable_list& v)
{
  return data::data_expression_list{v.begin(), v.end(), [&](const data::variable& x) { return sigma(x); }};
}

class lts_generator
{
  protected:
    typedef data::enumerator_list_element_with_substitution<> enumerator_element;
    struct next_state_summand;

    specification lpsspec;
    data::rewriter r;
    mutable data::mutable_indexed_substitution<> sigma;
    data::enumerator_identifier_generator id_generator;
    data::enumerator_algorithm<data::rewriter, data::rewriter> E;
    const data::variable_list& process_parameters;
    std::size_t n;
    bool apply_confluence_reduction;

    std::vector<next_state_summand> next_state_summands;
    std::vector<next_state_summand> confluent_summands;

    struct next_state_summand
    {
      data::variable_list variables;
      data::data_expression condition;
      process::action_list actions;
      data::data_expression_list next_state;

      data::variable_list gamma; // used for caching

      next_state_summand(const lps::action_summand& summand, const data::variable_list& process_parameters)
        : variables(summand.summation_variables()),
          condition(summand.condition()),
          actions(summand.multi_action().actions()),
          next_state(summand.next_state(process_parameters))
      {
        gamma = free_variables(summand.condition(), process_parameters);
      }

      template <typename T>
      data::variable_list free_variables(const T& x, const data::variable_list& v)
      {
        using utilities::detail::contains;
        std::set<data::variable> FV = data::find_free_variables(x);
        std::vector<data::variable> result;
        for (const data::variable& vi: v)
        {
          if (contains(FV, vi))
          {
            result.push_back(vi);
          }
        }
        return data::variable_list{result.begin(), result.end()};
      }
    };

    void preprocess(const generate_lts_options& options)
    {
      detail::instantiate_global_variables(lpsspec);
      lps::order_summand_variables(lpsspec);
      if (options.resolve_summand_variable_name_clashes)
      {
        resolve_summand_variable_name_clashes(lpsspec);
      }
      if (options.one_point_rule_rewrite)
      {
        one_point_rule_rewrite(lpsspec);
      }
      if (options.replace_constants_by_variables)
      {
        replace_constants_by_variables(lpsspec, r, sigma);
      }
    }

    // Confluence reduction based on S.C.C. Blom, Partial tau-confluence for
    // Efficient State Space Generation, Technical Report SEN-R0123, CWI, Amsterdam, 2001
    lps::state find_representative(const lps::state& state) const
    {
      data::data_expression_list process_parameter_values = substitute(sigma, process_parameters);
      lps::state u = state;

      std::map<lps::state, std::size_t> depth;
      std::map<lps::state, std::size_t> low;
      std::map<lps::state, std::list<lps::state>> successors;
      std::map<lps::state, lps::state> predecessor;

      std::size_t d = 0;
      depth[u] = 0;

      while (true)
      {
        if (depth[u] == 0)
        {
          d++;
          depth[u] = d;
          low[u] = d;
          successors[u] = std::list<lps::state>();

          add_assignments(sigma, process_parameters, u);
          for (const auto& summand: confluent_summands)
          {
            data::data_expression c = r(summand.condition, sigma);
            if (!data::is_false(c))
            {
              E.enumerate(enumerator_element(summand.variables, c),
                          sigma,
                          [&](const enumerator_element& p)
                          {
                            p.add_assignments(summand.variables, sigma, r);
                            lps::state v = rewrite_state(summand.next_state);
                            successors[u].push_back(v);
                            if (depth.find(v) == depth.end())
                            {
                              depth[v] = 0;
                            }
                            p.remove_assignments(summand.variables, sigma);
                            return false;
                          },
                          data::is_false
              );
            }
          }
        }
        if (successors[u].empty())
        {
          if (depth[u] == low[u])
          {
            // undo changes to sigma
            add_assignments(sigma, process_parameters, process_parameter_values);
            return u;
          }
          lps::state w = predecessor[u];
          low[w] = std::min(low[u], low[w]);
          u = w;
        }
        else
        {
          lps::state v = successors[u].front();
          successors[u].pop_front();
          if (depth[v] == 0)
          {
            predecessor[v] = u;
            u = v;
          }
          else if (depth[v] < depth[u])
          {
            low[u] = std::min(low[u], depth[v]);
          }
        }
      }
    }

    lps::state rewrite_state(const data::data_expression_list& v) const
    {
      return lps::state(v.begin(), n, [&](const data::data_expression& x) { return r(x, sigma); });
    };

    process::action_list rewrite_action_list(const process::action_list& actions) const
    {
      return process::action_list(
        actions.begin(),
        actions.end(),
        [&](const process::action& a)
        {
          const auto& args = a.arguments();
          return process::action(a.label(), data::data_expression_list(args.begin(), args.end(), [&](const data::data_expression& x) { return r(x, sigma); }));
        }
      );
    };

    template <typename ReportState = skip, typename ReportTransition = skip>
    void generate_default(ReportState report_state = ReportState(), ReportTransition report_transition = ReportTransition())
    {
      atermpp::indexed_set<lps::state> discovered;
      std::deque<std::size_t> todo;

      lps::state d0 = rewrite_state(lpsspec.initial_process().state(process_parameters));
      if (apply_confluence_reduction)
      {
        d0 = find_representative(d0);
      }
      report_state(d0);
      auto k = discovered.put(d0);
      todo.push_back(k.first);
      while (!todo.empty())
      {
        std::size_t i = todo.front();
        todo.pop_front();
        const lps::state& d = discovered.get(i);

        add_assignments(sigma, process_parameters, d);
        for (const auto& summand: next_state_summands)
        {
          data::data_expression c = r(summand.condition, sigma);
          if (!data::is_false(c))
          {
            E.enumerate(enumerator_element(summand.variables, c),
                        sigma,
                        [&](const enumerator_element& p)
                        {
                          p.add_assignments(summand.variables, sigma, r);
                          process::action_list a = rewrite_action_list(summand.actions);
                          lps::state d1 = rewrite_state(summand.next_state);
                          if (apply_confluence_reduction)
                          {
                            d1 = find_representative(d1);
                          }
                          p.remove_assignments(summand.variables, sigma);
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

    template <typename ReportState = skip, typename ReportTransition = skip>
    void generate_cached(ReportState report_state = ReportState(), ReportTransition report_transition = ReportTransition())
    {
      // global cache for solutions of conditions
      std::unordered_map<data::data_expression_list, std::list<data::data_expression_list>> enumerator_cache;

      atermpp::indexed_set<lps::state> discovered;
      std::deque<std::size_t> todo;

      lps::state d0 = rewrite_state(lpsspec.initial_process().state(process_parameters));
      if (apply_confluence_reduction)
      {
        d0 = find_representative(d0);
      }
      report_state(d0);
      auto k = discovered.put(d0);
      todo.push_back(k.first);
      while (!todo.empty())
      {
        std::size_t i = todo.front();
        todo.pop_front();
        const lps::state& d = discovered.get(i);

        add_assignments(sigma, process_parameters, d);
        for (auto& summand: next_state_summands)
        {
          data::data_expression_list key = substitute(sigma, summand.gamma);
          key.push_front(summand.condition);
          auto q = enumerator_cache.find(key);
          if (q == enumerator_cache.end())
          {
            data::data_expression condition = r(summand.condition, sigma);
            std::list<data::data_expression_list> solutions;
            if (!data::is_false(condition))
            {
              E.enumerate(enumerator_element(summand.variables, condition),
                          sigma,
                          [&](const enumerator_element& p) {
                            solutions.push_back(p.assign_expressions(summand.variables, r));
                            return false;
                          },
                          data::is_false
              );
            }
            q = enumerator_cache.insert({key, solutions}).first;
          }
          for (const data::data_expression_list& e: q->second)
          {
            add_assignments(sigma, summand.variables, e);
            process::action_list a = rewrite_action_list(summand.actions);
            lps::state d1 = rewrite_state(summand.next_state);
            if (apply_confluence_reduction)
            {
              d1 = find_representative(d1);
            }
            auto j = discovered.put(d1);
            if (j.second)
            {
              todo.push_back(j.first);
              report_state(d1);
            }
            report_transition(i, a, j.first);
          }
          remove_assignments(sigma, summand.variables);
        }
      }
    }

  public:
    lts_generator(const specification& lpsspec_, const generate_lts_options& options)
      : lpsspec(lpsspec_),
        r(lpsspec.data(), data::used_data_equation_selector(lpsspec.data(), lps::find_function_symbols(lpsspec), lpsspec.global_variables()), options.rewrite_strategy),
        E(r, lpsspec.data(), r, id_generator, false),
        process_parameters(lpsspec.process().process_parameters()),
        n(process_parameters.size()),
        apply_confluence_reduction(options.confluence)
    {
      preprocess(options);
      core::identifier_string ctau{"ctau"};
      for (const action_summand& summand: lpsspec.process().action_summands())
      {
        if (summand.multi_action().actions().size() == 1 && summand.multi_action().actions().front().label().name() == ctau)
        {
          confluent_summands.emplace_back(summand, process_parameters);
        }
        else
        {
          next_state_summands.emplace_back(summand, process_parameters);
        }
      }
    }

    ~lts_generator() = default;

    template <typename ReportState = skip, typename ReportTransition = skip>
    void generate(const generate_lts_options& options, ReportState report_state = ReportState(), ReportTransition report_transition = ReportTransition())
    {
      if (options.cached)
      {
        generate_cached(report_state, report_transition);
      }
      else
      {
        generate_default(report_state, report_transition);
      }
    }
};

void generate_labeled_transition_system(const specification& lpsspec,
                                        const generate_lts_options& options,
                                        labeled_transition_system& result
                                       )
{
  typedef process::action_list action_type;

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

  lts_generator generator(lpsspec, options);
  generator.generate(options,
                     [&](const lps::state&)
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
