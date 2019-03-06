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

enum class caching { none, local, global };

std::ostream& operator<<(std::ostream& os, caching c)
{
  switch(c)
  {
    case caching::none: os << "none"; break;
    case caching::local: os << "local"; break;
    case caching::global: os << "global"; break;
    default    : os.setstate(std::ios_base::failbit);
  }
  return os;
}

struct generate_lts_options
{
  data::rewrite_strategy rewrite_strategy = data::jitty;
  bool one_point_rule_rewrite = false;
  bool replace_constants_by_variables = false;
  bool resolve_summand_variable_name_clashes = false;
  bool store_states_as_trees = true;
  bool cached = false;
  bool global_cache = false;
  bool confluence = false;
};

inline
std::ostream& operator<<(std::ostream& out, const generate_lts_options& options)
{
  out << "cached = " << std::boolalpha << options.cached << std::endl;
  out << "global-cache = " << std::boolalpha << options.global_cache << std::endl;
  out << "confluence = " << std::boolalpha << options.confluence << std::endl;
  out << "one_point_rule_rewrite = " << std::boolalpha << options.one_point_rule_rewrite << std::endl;
  out << "resolve_summand_variable_name_clashes = " << std::boolalpha << options.resolve_summand_variable_name_clashes << std::endl;
  out << "replace_constants_by_variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "rewrite_strategy = " << std::boolalpha << options.rewrite_strategy << std::endl;
  out << "store_states_as_trees = " << std::boolalpha << options.store_states_as_trees << std::endl;
  return out;
}

template <typename VariableSequence, typename DataExpressionSequence>
inline
void add_assignments(data::mutable_indexed_substitution<>& sigma, const VariableSequence& v, const DataExpressionSequence& e)
{
  assert(v.size() == e.size());
  auto vi = v.begin();
  auto ei = e.begin();
  for (; vi != v.end(); ++vi, ++ei)
  {
    sigma[*vi] = *ei;
  }
}

template <typename VariableSequence>
inline
void remove_assignments(data::mutable_indexed_substitution<>& sigma, const VariableSequence& v)
{
  for (const data::variable& vi: v)
  {
    sigma[vi] = vi;
  }
}

template <typename VariableSequence>
inline
data::data_expression_list substitute(data::mutable_indexed_substitution<>& sigma, const VariableSequence & v)
{
  return data::data_expression_list{v.begin(), v.end(), [&](const data::variable& x) { return sigma(x); }};
}

inline
std::vector<data::data_expression> make_data_expression_vector(const data::data_expression_list& v)
{
  return std::vector<data::data_expression>(v.begin(), v.end());
}

class lts_generator
{
  protected:
    typedef data::enumerator_list_element_with_substitution<> enumerator_element;
    struct next_state_summand;

    generate_lts_options options;
    data::rewriter r;
    mutable data::mutable_indexed_substitution<> sigma;
    data::enumerator_identifier_generator id_generator;
    data::enumerator_algorithm<data::rewriter, data::rewriter> E;
    std::vector<data::variable> process_parameters;
    std::size_t n; // n = process_parameters.size()
    data::data_expression_list initial_state;

    std::vector<next_state_summand> summands;
    std::vector<next_state_summand> confluent_summands;

    // N.B. The keys are stored in term_appl instead of data_expression_list for performance reasons.
    std::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> global_cache;

    atermpp::indexed_set<lps::state> discovered;

    struct next_state_summand
    {
      data::variable_list variables;
      data::data_expression condition;
      process::action_list actions;
      std::vector<data::data_expression> next_state;

      // attributes for caching
      caching cache_strategy;
      std::vector<data::variable> gamma;
      atermpp::function_symbol f_gamma;
      mutable std::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> local_cache;

      next_state_summand(const lps::action_summand& summand, const data::variable_list& process_parameters, caching cache_strategy_)
        : variables(summand.summation_variables()),
          condition(summand.condition()),
          actions(summand.multi_action().actions()),
          next_state(make_data_expression_vector(summand.next_state(process_parameters))),
          cache_strategy(cache_strategy_)
      {
        gamma = free_variables(summand.condition(), process_parameters);
        if (cache_strategy_ == caching::global)
        {
          gamma.insert(gamma.begin(), data::variable());
        }
        f_gamma = atermpp::function_symbol("@gamma", gamma.size());
      }

      template <typename T>
      std::vector<data::variable> free_variables(const T& x, const data::variable_list& v)
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
        return result;
      }

      atermpp::term_appl<data::data_expression> compute_key(data::mutable_indexed_substitution<>& sigma) const
      {
        if (cache_strategy == caching::global)
        {
          bool is_first_element = true;
          return atermpp::term_appl<data::data_expression>(f_gamma, gamma.begin(), gamma.end(),
                                                           [&](const data::variable& x)
                                                           {
                                                             if (is_first_element)
                                                             {
                                                               is_first_element = false;
                                                               return condition;
                                                             }
                                                             return sigma(x);
                                                           }
          );
        }
        else
        {
          return atermpp::term_appl<data::data_expression>(f_gamma, gamma.begin(), gamma.end(),
                                                           [&](const data::variable& x)
                                                           {
                                                             return sigma(x);
                                                           }
          );
        }
      }
    };

    lps::specification preprocess(const specification& lpsspec)
    {
      lps::specification result = lpsspec;
      detail::instantiate_global_variables(result);
      lps::order_summand_variables(result);
      if (options.resolve_summand_variable_name_clashes)
      {
        resolve_summand_variable_name_clashes(result);
      }
      if (options.one_point_rule_rewrite)
      {
        one_point_rule_rewrite(result);
      }
      if (options.replace_constants_by_variables)
      {
        replace_constants_by_variables(result, r, sigma);
      }
      return result;
    }

    // pre: d0 is in normal form
    template <typename SummandSequence>
    std::vector<lps::state> generate_successors(const lps::state& d0, const SummandSequence& summands)
    {
      std::vector<lps::state> result;
      add_assignments(sigma, process_parameters, d0);

      for (const next_state_summand& summand: summands)
      {
        id_generator.clear();
        if (summand.cache_strategy == caching::none)
        {
          data::data_expression condition = r(summand.condition, sigma);
          if (!data::is_false(condition))
          {
            E.enumerate(enumerator_element(summand.variables, condition),
                        sigma,
                        [&](const enumerator_element& p)
                        {
                          p.add_assignments(summand.variables, sigma, r);
                          lps::state d1 = rewrite_state(summand.next_state);
                          result.push_back(d1);
                          return false;
                        },
                        data::is_false
            );
          }
        }
        else
        {
          auto key = summand.compute_key(sigma);
          auto& cache = summand.cache_strategy == caching::global ? global_cache : summand.local_cache;
          auto q = cache.find(key);
          if (q == cache.end())
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
            q = cache.insert({key, solutions}).first;
          }
          for (const data::data_expression_list& e: q->second)
          {
            add_assignments(sigma, summand.variables, e);
            lps::state d1 = rewrite_state(summand.next_state);
            result.push_back(d1);
          }
        }
        remove_assignments(sigma, summand.variables);
      }

      return result;
    }

    // pre: d0 is in normal form
    template <typename SummandSequence>
    std::vector<std::pair<lps::multi_action, lps::state>> generate_transitions(const lps::state& d0, const SummandSequence& summands)
    {
      std::vector<std::pair<lps::multi_action, lps::state>> result;
      add_assignments(sigma, process_parameters, d0);

      for (const next_state_summand& summand: summands)
      {
        id_generator.clear();
        if (summand.cache_strategy == caching::none)
        {
          data::data_expression condition = r(summand.condition, sigma);
          if (!data::is_false(condition))
          {
            E.enumerate(enumerator_element(summand.variables, condition),
                        sigma,
                        [&](const enumerator_element& p)
                        {
                          p.add_assignments(summand.variables, sigma, r);
                          process::action_list a = rewrite_action_list(summand.actions);
                          lps::state d1 = rewrite_state(summand.next_state);
                          result.emplace_back(lps::multi_action(a), d1);
                          return false;
                        },
                        data::is_false
            );
          }
        }
        else
        {
          auto key = summand.compute_key(sigma);
          auto& cache = summand.cache_strategy == caching::global ? global_cache : summand.local_cache;
          auto q = cache.find(key);
          if (q == cache.end())
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
            q = cache.insert({key, solutions}).first;
          }
          for (const data::data_expression_list& e: q->second)
          {
            add_assignments(sigma, summand.variables, e);
            process::action_list a = rewrite_action_list(summand.actions);
            lps::state d1 = rewrite_state(summand.next_state);
            result.emplace_back(lps::multi_action(a), d1);
          }
        }
        remove_assignments(sigma, summand.variables);
      }
      return result;
    }

    // This function is based an iterative version of Tarjan's strongly connected components algorithm.
    // It returns the smallest node of the first SCC that is detected. The first SCC is a TSCC, meaning
    // that it has no outgoing edges. In a confluent tau graph there is only one TSCC, so this should
    // guarantee a unique representative.
    // N.B. The implementation is based on https://llbit.se/?p=3379
    lps::state find_representative(lps::state& u0)
    {
      using utilities::detail::contains;
      data::data_expression_list process_parameter_values = substitute(sigma, process_parameters);

      std::vector<lps::state> stack;
      std::map<lps::state, std::size_t> low;
      std::map<lps::state, std::size_t> disc;

      std::map<lps::state, std::vector<lps::state>> successors;
      std::vector<std::pair<lps::state, std::size_t>> work;

      successors[u0] = generate_successors(u0, confluent_summands);
      work.emplace_back(std::make_pair(u0, 0));

      while (!work.empty())
      {
        lps::state u = work.back().first;
        std::size_t i = work.back().second;
        work.pop_back();

        if (i == 0)
        {
          std::size_t k = disc.size();
          disc[u] = k;
          low[u] = k;
          stack.push_back(u);
        }

        bool recurse = false;
        const std::vector<lps::state>& succ = successors[u];
        for (std::size_t j = i; j < succ.size(); j++)
        {
          const lps::state& v = succ[j];
          if (disc.find(v) == disc.end())
          {
            successors[v] = generate_successors(v, confluent_summands);
            work.emplace_back(std::make_pair(u, j + 1));
            work.emplace_back(std::make_pair(v, 0));
            recurse = true;
            break;
          }
          else if (contains(stack, v))
          {
            low[u] = std::min(low[u], disc[v]);
          }
        }
        if (recurse)
        {
          continue;
        }
        if (disc[u] == low[u])
        {
          // an SCC has been found; return the node with the minimum value in this SCC
          lps::state result = u;
          while (true)
          {
            const auto& v = stack.back();
            if (v == u)
            {
              break;
            }
            if (v < result)
            {
              result = v;
            }
            stack.pop_back();
          }

          // undo changes to sigma
          add_assignments(sigma, process_parameters, process_parameter_values);
          return result;
        }
        if (!work.empty())
        {
          lps::state v = u;
          u = work.back().first;
          low[u] = std::min(low[u], low[v]);
        }
      }
      throw mcrl2::runtime_error("find_representative did not find a solution");
    }

    template <typename DataExpressionSequence>
    lps::state rewrite_state(const DataExpressionSequence& v) const
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

    // pre: d0 is in normal form
    template <typename SummandSequence, typename ReportState = skip, typename ReportTransition = skip>
    void generate_state_space(
      lps::state& d0,
      bool use_confluence_reduction,
      const SummandSequence& summands,
      ReportState report_state = ReportState(),
      ReportTransition report_transition = ReportTransition())
    {
      std::deque<std::size_t> todo;
      discovered.clear();

      if (use_confluence_reduction)
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
        for (const next_state_summand& summand: summands)
        {
          id_generator.clear();
          if (summand.cache_strategy == caching::none)
          {
            data::data_expression condition = r(summand.condition, sigma);
            if (!data::is_false(condition))
            {
              E.enumerate(enumerator_element(summand.variables, condition),
                          sigma,
                          [&](const enumerator_element& p) {
                            p.add_assignments(summand.variables, sigma, r);
                            process::action_list a = rewrite_action_list(summand.actions);
                            lps::state d1 = rewrite_state(summand.next_state);
                            if (use_confluence_reduction)
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
                            return false;
                          },
                          data::is_false
              );
            }
          }
          else
          {
            auto key = summand.compute_key(sigma);
            auto& cache = summand.cache_strategy == caching::global ? global_cache : summand.local_cache;
            auto q = cache.find(key);
            if (q == cache.end())
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
              q = cache.insert({key, solutions}).first;
            }
            for (const data::data_expression_list& e: q->second)
            {
              add_assignments(sigma, summand.variables, e);
              process::action_list a = rewrite_action_list(summand.actions);
              lps::state d1 = rewrite_state(summand.next_state);
              if (use_confluence_reduction)
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
          }
          remove_assignments(sigma, summand.variables);
        }
      }
    }

  public:
    lts_generator(const specification& lpsspec, const generate_lts_options& options_)
      : options(options_),
        r(lpsspec.data(), data::used_data_equation_selector(lpsspec.data(), lps::find_function_symbols(lpsspec), lpsspec.global_variables()), options.rewrite_strategy),
        E(r, lpsspec.data(), r, id_generator, false)
    {
      lps::specification lpsspec_ = preprocess(lpsspec);
      auto params = lpsspec_.process().process_parameters();
      process_parameters = std::vector<data::variable>(params.begin(), params.end());
      n = process_parameters.size();
      initial_state = lpsspec_.initial_process().state(lpsspec_.process().process_parameters());
      core::identifier_string ctau{"ctau"};
      for (const action_summand& summand: lpsspec_.process().action_summands())
      {
        auto cache_strategy = options.cached ? (options.global_cache ? lps::caching::global : lps::caching::local) : lps::caching::none;
        if (summand.multi_action().actions().size() == 1 && summand.multi_action().actions().front().label().name() == ctau)
        {
          confluent_summands.emplace_back(summand, lpsspec_.process().process_parameters(), cache_strategy);
        }
        else
        {
          summands.emplace_back(summand, lpsspec_.process().process_parameters(), cache_strategy);
        }
      }
    }

    ~lts_generator() = default;

    /// \brief Generates the state space, and reports all discovered states and transitions by means of callback
    /// functions.
    template <typename ReportState = skip, typename ReportTransition = skip>
    void generate_state_space(ReportState report_state = ReportState(), ReportTransition report_transition = ReportTransition())
    {
      lps::state d0 = rewrite_state(initial_state);
      generate_state_space(d0, options.confluence, summands, report_state, report_transition);
    }

    /// \brief Generates outgoing transitions for a given state.
    std::vector<std::pair<lps::multi_action, lps::state>> generate_transitions(const data::data_expression_list& init)
    {
      lps::state d0 = rewrite_state(init);
      return generate_transitions(d0, summands);
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
  generator.generate_state_space(
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
