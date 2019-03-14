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

#ifndef MCRL2_LPS_EXPLORER_H
#define MCRL2_LPS_EXPLORER_H

#include <deque>
#include <iomanip>
#include <limits>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
// #include "mcrl2/lps/exploration_strategy.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/order_summand_variables.h"
#include "mcrl2/lps/replace_constants_by_variables.h"
#include "mcrl2/lps/resolve_name_clashes.h"
#include "mcrl2/lps/state.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/process/timed_multi_action.h"
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

enum class caching { none, local, global };

inline
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
  // exploration_strategy expl_strat;
  bool one_point_rule_rewrite = false;
  bool replace_constants_by_variables = false;
  bool resolve_summand_variable_name_clashes = false;
  bool store_states_as_trees = true;
  bool cached = false;
  bool global_cache = false;
  bool confluence = false;
  bool detect_deadlock = false;
  bool detect_nondeterminism = false;
  bool detect_divergence = false;
  bool detect_action = false;
  bool generate_traces = false;
  std::size_t max_states = std::numeric_limits<std::size_t>::max();
  std::size_t max_traces = 0;
  std::string priority_action;
  std::string trace_prefix;
  std::set<core::identifier_string> trace_actions;
  std::set<std::string> trace_multiaction_strings;
  std::set<lps::multi_action> trace_multiactions;
  std::set<core::identifier_string> actions_internal_for_divergencies;
};

inline
std::ostream& operator<<(std::ostream& out, const generate_lts_options& options)
{
  out << "rewrite_strategy = " << options.rewrite_strategy << std::endl;
// out << "expl_strat = " << options.expl_strat << std::endl;
  out << "cached = " << std::boolalpha << options.cached << std::endl;
  out << "global-cache = " << std::boolalpha << options.global_cache << std::endl;
  out << "confluence = " << std::boolalpha << options.confluence << std::endl;
  out << "one_point_rule_rewrite = " << std::boolalpha << options.one_point_rule_rewrite << std::endl;
  out << "resolve_summand_variable_name_clashes = " << std::boolalpha << options.resolve_summand_variable_name_clashes << std::endl;
  out << "replace_constants_by_variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "store_states_as_trees = " << std::boolalpha << options.store_states_as_trees << std::endl;
  out << "detect_deadlock = " << std::boolalpha << options.detect_deadlock << std::endl;
  out << "detect_nondeterminism = " << std::boolalpha << options.detect_nondeterminism << std::endl;
  out << "detect_divergence = " << std::boolalpha << options.detect_divergence << std::endl;
  out << "detect_action = " << std::boolalpha << options.detect_action << std::endl;
  out << "generate_traces = " << std::boolalpha << options.generate_traces << std::endl;
  out << "max_states = " << options.max_states << std::endl;
  out << "max_traces = " << options.max_traces << std::endl;
  out << "priority_action = " << options.priority_action << std::endl;
  out << "trace_prefix = " << options.trace_prefix << std::endl;
  out << "trace_actions = " << core::detail::print_set(options.trace_actions) << std::endl;
  out << "trace_multiaction_strings = " << core::detail::print_set(options.trace_multiaction_strings) << std::endl;
  out << "trace_multiactions = " << core::detail::print_set(options.trace_multiactions) << std::endl;
  out << "actions_internal_for_divergencies = " << core::detail::print_set(options.actions_internal_for_divergencies) << std::endl;
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

// This class automatically undoes assignments to variables in the substitution sigma
// as soon as it goes out of scope.
template <typename VariableSequence>
struct substitution_undo
{
  data::mutable_indexed_substitution<>& sigma;
  const VariableSequence& variables;
  std::vector<data::data_expression> expressions;

  substitution_undo(data::mutable_indexed_substitution<>& sigma_, const VariableSequence & variables_)
    : sigma(sigma_), variables(variables_)
  {
    expressions.reserve(variables_.size());
    for (const data::variable& v: variables_)
    {
      expressions.push_back(sigma(v));
    }
  }

  ~substitution_undo()
  {
    add_assignments(sigma, variables, expressions);
  }
};

class explorer
{
  protected:
    typedef data::enumerator_list_element_with_substitution<> enumerator_element;
    struct next_state_summand;

    const generate_lts_options& options;
    data::rewriter r;
    mutable data::mutable_indexed_substitution<> sigma;
    data::enumerator_identifier_generator id_generator;
    data::enumerator_algorithm<> E;
    std::vector<data::variable> m_process_parameters;
    std::size_t n; // n = process_parameters.size()
    data::data_expression_list m_initial_state;

    std::vector<next_state_summand> m_regular_summands;
    std::vector<next_state_summand> m_confluent_summands;
    std::vector<next_state_summand> m_empty_summands;

    // N.B. The keys are stored in term_appl instead of data_expression_list for performance reasons.
    std::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> global_cache;

    std::unordered_map<lps::state, std::size_t> m_discovered;

    volatile bool must_abort = false;

    struct next_state_summand
    {
      data::variable_list variables;
      data::data_expression condition;
      process::timed_multi_action actions;
      std::vector<data::data_expression> next_state;
      std::size_t index;

      // attributes for caching
      caching cache_strategy;
      std::vector<data::variable> gamma;
      atermpp::function_symbol f_gamma;
      mutable std::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> local_cache;

      next_state_summand(const lps::action_summand& summand, std::size_t summand_index, const data::variable_list& process_parameters, caching cache_strategy_)
        : variables(summand.summation_variables()),
          condition(summand.condition()),
          actions(summand.multi_action().actions(), summand.multi_action().time()),
          next_state(make_data_expression_vector(summand.next_state(process_parameters))),
          index(summand_index),
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

    // This function is based an iterative version of Tarjan's strongly connected components algorithm.
    // It returns the smallest node of the first SCC that is detected. The first SCC is a TSCC, meaning
    // that it has no outgoing edges. In a confluent tau graph there is only one TSCC, so this should
    // guarantee a unique representative.
    // N.B. The implementation is based on https://llbit.se/?p=3379
    template <typename SummandSequence>
    lps::state find_representative(lps::state& u0, const SummandSequence& summands)
    {
      using utilities::detail::contains;
      data::data_expression_list process_parameter_undo = substitute(sigma, m_process_parameters);

      std::vector<lps::state> stack;
      std::map<lps::state, std::size_t> low;
      std::map<lps::state, std::size_t> disc;

      std::map<lps::state, std::vector<lps::state>> successors;
      std::vector<std::pair<lps::state, std::size_t>> work;

      successors[u0] = generate_successors(u0, summands);
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
            successors[v] = generate_successors(v, summands);
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
          add_assignments(sigma, m_process_parameters, process_parameter_undo);
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
    }

    process::timed_multi_action rewrite_action_list(const process::timed_multi_action& a) const
    {
      const process::action_list& actions = a.actions();
      const data::data_expression& time = a.time();
      return
        process::timed_multi_action(
          process::action_list(
            actions.begin(),
            actions.end(),
            [&](const process::action& a)
            {
              const auto& args = a.arguments();
              return process::action(a.label(), data::data_expression_list(args.begin(), args.end(), [&](const data::data_expression& x) { return r(x, sigma); }));
            }
          ),
          a.has_time() ? r(time, sigma) : time
        );
    }

    // Generates outgoing transitions for a summand, and reports them via the callback function examine_transition.
    // It is assumed that the substitution sigma contains the assignments corresponding to the current state.
    template <typename SummandSequence, typename ExamineTransition = skip>
    void generate_transitions(
      const next_state_summand& summand,
      const SummandSequence& confluent_summands,
      ExamineTransition examine_transition = ExamineTransition()
    )
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
                        process::timed_multi_action a = rewrite_action_list(summand.actions);
                        lps::state d1 = rewrite_state(summand.next_state);
                        if (!confluent_summands.empty())
                        {
                          d1 = find_representative(d1, confluent_summands);
                        }
                        examine_transition(a, d1);
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
          process::timed_multi_action a = rewrite_action_list(summand.actions);
          lps::state d1 = rewrite_state(summand.next_state);
          if (!confluent_summands.empty())
          {
            d1 = find_representative(d1, confluent_summands);
          }
          examine_transition(a, d1);
        }
      }
      remove_assignments(sigma, summand.variables);
    }

    // pre: d0 is in normal form
    template <typename SummandSequence>
    std::vector<lps::state> generate_successors(
      const lps::state& d0,
      const SummandSequence& summands,
      const SummandSequence& confluent_summands = SummandSequence()
    )
    {
      std::vector<lps::state> result;
      add_assignments(sigma, m_process_parameters, d0);
      for (const next_state_summand& summand: summands)
      {
        generate_transitions(
          summand,
          confluent_summands,
          [&](const process::timed_multi_action& /* a */, const lps::state& d1)
          {
            result.push_back(d1);
          }
        );
        remove_assignments(sigma, summand.variables);
      }
      return result;
    }

  public:
    explorer(const specification& lpsspec, const generate_lts_options& options_)
      : options(options_),
        r(lpsspec.data(), data::used_data_equation_selector(lpsspec.data(), lps::find_function_symbols(lpsspec), lpsspec.global_variables()), options.rewrite_strategy),
        E(r, lpsspec.data(), r, id_generator, false)
    {
      lps::specification lpsspec_ = preprocess(lpsspec);
      auto params = lpsspec_.process().process_parameters();
      m_process_parameters = std::vector<data::variable>(params.begin(), params.end());
      n = m_process_parameters.size();
      m_initial_state = lpsspec_.initial_process().state(lpsspec_.process().process_parameters());
      core::identifier_string ctau{"ctau"};
      const auto& lpsspec_summands = lpsspec_.process().action_summands();
      for (std::size_t i = 0; i < lpsspec_summands.size(); i++)
      {
        const action_summand& summand = lpsspec_summands[i];
        auto cache_strategy = options.cached ? (options.global_cache ? lps::caching::global : lps::caching::local) : lps::caching::none;
        if (summand.multi_action().actions().size() == 1 && summand.multi_action().actions().front().label().name() == ctau)
        {
          m_confluent_summands.emplace_back(summand, i, lpsspec_.process().process_parameters(), cache_strategy);
        }
        else
        {
          m_regular_summands.emplace_back(summand, i, lpsspec_.process().process_parameters(), cache_strategy);
        }
      }
    }

    ~explorer() = default;

    // pre: d0 is in normal form
    template <typename SummandSequence,
      typename DiscoverState = skip,
      typename ExamineTransition = skip,
      typename StartState = skip,
      typename FinishState = skip
    >
    void generate_state_space(
      lps::state& d0,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      std::unordered_map<lps::state, std::size_t>& discovered,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      StartState start_state = StartState(),
      FinishState finish_state = FinishState()
    )
    {
      std::deque<lps::state> todo;
      discovered.clear();

      if (!confluent_summands.empty())
      {
        d0 = find_representative(d0, confluent_summands);
      }
      std::size_t d0_index = discovered.size();
      discovered.insert(std::make_pair(d0, d0_index));
      discover_state(d0, d0_index);
      todo.push_back(d0);

      while (!todo.empty() && !must_abort)
      {
        const lps::state& s = todo.front();
        todo.pop_front();
        std::size_t s_index = discovered.find(s)->second;
        start_state(s, s_index);

        add_assignments(sigma, m_process_parameters, s);
        for (const next_state_summand& summand: regular_summands)
        {
          generate_transitions(
            summand,
            confluent_summands,
            [&](const process::timed_multi_action& a, const lps::state& s1)
            {
              auto j = discovered.find(s1);
              if (j == discovered.end())
              {
                std::size_t k = discovered.size();
                j = discovered.insert(std::make_pair(s1, k)).first;
                discover_state(s1, k);
                todo.push_back(s1);
              }
              std::size_t s1_index = j->second;
              examine_transition(s_index, a, s1_index, s1, summand.index);
            }
          );
        }
        finish_state(s, s_index);
      }
    }

    /// \brief Generates the state space, and reports all discovered states and transitions by means of callback
    /// functions.
    /// \param discover_state Is invoked when a state is encountered for the first time.
    /// \param examine_transition Is invoked on every transition.
    /// \param start_state Is invoked on a state right before its outgoing transitions are being explored.
    /// \param finish_state Is invoked on a state after all of its outgoing transitions have been explored.
    template <
      typename DiscoverState = skip,
      typename ExamineTransition = skip,
      typename StartState = skip,
      typename FinishState = skip
    >
    void generate_state_space(
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      StartState start_state = StartState(),
      FinishState finish_state = FinishState()
    )
    {
      lps::state d0 = rewrite_state(m_initial_state);
      generate_state_space(d0, m_regular_summands, m_confluent_summands, m_discovered, discover_state, examine_transition, start_state, finish_state);
    }

    /// \brief Generates outgoing transitions for a given state.
    std::vector<std::pair<lps::multi_action, lps::state>> generate_transitions(const lps::state& d0)
    {
      data::data_expression_list process_parameter_undo = substitute(sigma, m_process_parameters);
      std::vector<std::pair<lps::multi_action, lps::state>> result;
      add_assignments(sigma, m_process_parameters, d0);
      for (const next_state_summand& summand: m_regular_summands)
      {
        generate_transitions(
          summand,
          m_confluent_summands,
          [&](const process::timed_multi_action& a, const lps::state& d1)
          {
            result.emplace_back(lps::multi_action(a), d1);
          }
        );
        remove_assignments(sigma, summand.variables);
      }
      add_assignments(sigma, m_process_parameters, process_parameter_undo);
      return result;
    }

    /// \brief Generates outgoing transitions for a given state.
    std::vector<std::pair<lps::multi_action, lps::state>> generate_transitions(const data::data_expression_list& init)
    {
      lps::state d0 = rewrite_state(init);
      return generate_transitions(d0);
    }

    /// \brief Generates outgoing transitions for a given state, reachable via the summand with index i.
    std::vector<std::pair<lps::multi_action, lps::state>> generate_transitions(const data::data_expression_list& init, std::size_t i)
    {
      data::data_expression_list process_parameter_undo = substitute(sigma, m_process_parameters);
      lps::state d0 = rewrite_state(init);
      std::vector<std::pair<lps::multi_action, lps::state>> result;
      add_assignments(sigma, m_process_parameters, d0);
      generate_transitions(
        m_regular_summands[i],
        m_confluent_summands,
        [&](const process::timed_multi_action& a, const lps::state& d1)
        {
          result.emplace_back(lps::multi_action(a), d1);
        }
      );
      remove_assignments(sigma, m_regular_summands[i].variables);
      add_assignments(sigma, m_process_parameters, process_parameter_undo);
      return result;
    }

    /// \brief Returns a mapping containing all discovered states.
    const std::unordered_map<lps::state, std::size_t>& state_map() const
    {
      return m_discovered;
    }

    /// \brief Abort the function generate_state_space.
    void abort()
    {
      must_abort = true;
    }
};

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_EXPLORER_H
