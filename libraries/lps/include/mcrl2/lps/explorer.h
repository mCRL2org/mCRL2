// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/explorer.h
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
#include "mcrl2/lps/explorer_options.h"
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
    default: os.setstate(std::ios_base::failbit);
  }
  return os;
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

inline
std::vector<data::data_expression> make_data_expression_vector(const data::data_expression_list& v)
{
  return std::vector<data::data_expression>(v.begin(), v.end());
}

class explorer
{
  public:
    struct explorer_summand
    {
      data::variable_list variables;
      data::data_expression condition;
      process::timed_multi_action multi_action;
      std::vector<data::data_expression> next_state;
      std::size_t index;

      // attributes for caching
      caching cache_strategy;
      std::vector<data::variable> gamma;
      atermpp::function_symbol f_gamma;
      mutable std::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> local_cache;

      explorer_summand(const lps::action_summand& summand, std::size_t summand_index, const data::variable_list& process_parameters, caching cache_strategy_)
        : variables(summand.summation_variables()),
          condition(summand.condition()),
          multi_action(summand.multi_action().actions(), summand.multi_action().time()),
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

  protected:
    typedef data::enumerator_list_element_with_substitution<> enumerator_element;

    const explorer_options& options;
    data::rewriter r;
    mutable data::mutable_indexed_substitution<> sigma;
    data::enumerator_identifier_generator id_generator;
    data::enumerator_algorithm<> E;
    std::vector<data::variable> m_process_parameters;
    std::size_t n; // n = process_parameters.size()
    data::data_expression_list m_initial_state;
    bool m_recursive = false;

    std::vector<explorer_summand> m_regular_summands;
    std::vector<explorer_summand> m_confluent_summands;

    // N.B. The keys are stored in term_appl instead of data_expression_list for performance reasons.
    std::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> global_cache;

    std::unordered_map<lps::state, std::size_t> m_discovered;

    volatile bool must_abort = false;

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
      data::data_expression_list process_parameter_undo = process_parameter_values();

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
          set_process_parameter_values(process_parameter_undo);
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

    process::timed_multi_action rewrite_action(const process::timed_multi_action& a) const
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
      const explorer_summand& summand,
      const SummandSequence& confluent_summands,
      ExamineTransition examine_transition = ExamineTransition()
    )
    {
      if (!m_recursive)
      {
        id_generator.clear();
      }
      if (summand.cache_strategy == caching::none)
      {
        data::data_expression condition = r(summand.condition, sigma);
        if (!data::is_false(condition))
        {
          E.enumerate(enumerator_element(summand.variables, condition),
                      sigma,
                      [&](const enumerator_element& p) {
                        p.add_assignments(summand.variables, sigma, r);
                        process::timed_multi_action a = rewrite_action(summand.multi_action);
                        lps::state d1 = rewrite_state(summand.next_state);
                        if (!confluent_summands.empty())
                        {
                          d1 = find_representative(d1, confluent_summands);
                        }
                        if (m_recursive)
                        {
                          remove_assignments(sigma, summand.variables);
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
          process::timed_multi_action a = rewrite_action(summand.multi_action);
          lps::state d1 = rewrite_state(summand.next_state);
          if (!confluent_summands.empty())
          {
            d1 = find_representative(d1, confluent_summands);
          }
          if (m_recursive)
          {
            remove_assignments(sigma, summand.variables);
          }
          examine_transition(a, d1);
        }
      }
      if (!m_recursive)
      {
        remove_assignments(sigma, summand.variables);
      }
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
      for (const explorer_summand& summand: summands)
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

    template <typename T>
    T next_todo(std::deque<T>& todo)
    {
      if (options.search_strategy == es_breadth)
      {
        auto s = todo.front();
        todo.pop_front();
        return s;
      }
      else
      {
        auto s = todo.back();
        todo.pop_back();
        return s;
      }
    }

  public:
    explorer(const specification& lpsspec, const explorer_options& options_)
      : options(options_),
        r(lpsspec.data(), data::used_data_equation_selector(lpsspec.data(), lps::find_function_symbols(lpsspec), lpsspec.global_variables()), options.rewrite_strategy),
        E(r, lpsspec.data(), r, id_generator, false)
    {
      lps::specification lpsspec_ = preprocess(lpsspec);
      const auto& params = lpsspec_.process().process_parameters();
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
    void generate_untimed_state_space(
      bool recursive,
      const lps::state& d0,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      std::unordered_map<lps::state, std::size_t>& discovered,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      StartState start_state = StartState(),
      FinishState finish_state = FinishState()
    )
    {
      m_recursive = recursive;
      std::deque<lps::state> todo;
      discovered.clear();
      std::size_t d0_index = 0;
      discovered.insert(std::make_pair(d0, d0_index));
      discover_state(d0, d0_index);
      todo.push_back(d0);

      while (!todo.empty() && !must_abort)
      {
        lps::state s = next_todo(todo);
        std::size_t s_index = discovered.find(s)->second;
        start_state(s, s_index);
        add_assignments(sigma, m_process_parameters, s);
        for (const explorer_summand& summand: regular_summands)
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
              examine_transition(s, s_index, a, s1, s1_index, summand.index);
            }
          );
        }
        finish_state(s, s_index, todo.size());
      }
      must_abort = false;
    }

    bool less_time(const data::data_expression& t0, const data::data_expression& t1)
    {
std::cout << "LESS (" << t0 << ", " << t1 << ") = " << r(data::less(t0, t1)) << std::endl;
      return r(data::less(t0, t1)) == data::sort_bool::true_();
    }

    // pre: d0 is in normal form
    template <typename SummandSequence,
      typename DiscoverState = skip,
      typename ExamineTransition = skip,
      typename StartState = skip,
      typename FinishState = skip
    >
    void generate_timed_state_space(
      bool recursive,
      const lps::state& d0,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      std::unordered_map<lps::state, std::size_t>& discovered,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      StartState start_state = StartState(),
      FinishState finish_state = FinishState()
    )
    {
      m_recursive = recursive;
      std::deque<std::pair<lps::state, data::data_expression>> todo;
      discovered.clear();
      std::size_t d0_index = 0;
      discovered.insert(std::make_pair(d0, d0_index));
      discover_state(d0, d0_index);
      data::data_expression zero = data::sort_real::creal(data::sort_int::cint(data::sort_nat::c0()), data::sort_pos::c1());
      todo.emplace_back(d0, zero);

      while (!todo.empty() && !must_abort)
      {
        std::pair<lps::state, data::data_expression> q = next_todo(todo);
        const lps::state& s = q.first;
        const data::data_expression& T = q.second;
        std::size_t s_index = discovered.find(s)->second;
        start_state(s, s_index);
        add_assignments(sigma, m_process_parameters, s);
        for (const explorer_summand& summand: regular_summands)
        {
          generate_transitions(
            summand,
            confluent_summands,
            [&](const process::timed_multi_action& a, const lps::state& s1)
            {
              if (a.has_time() && less_time(a.time(), T))
              {
                return;
              }
              data::data_expression T1 = a.has_time() ? a.time() : T;
              auto j = discovered.find(s1);
              if (j == discovered.end())
              {
                std::size_t k = discovered.size();
                j = discovered.insert(std::make_pair(s1, k)).first;
                discover_state(s1, k);
                todo.emplace_back(s1, T);
              }
              std::size_t s1_index = j->second;
              examine_transition(s, s_index, a, s1, s1_index, summand.index);
            }
          );
        }
        finish_state(s, s_index, todo.size());
      }
      must_abort = false;
    }

    // pre: d0 is in normal form
    template <typename SummandSequence,
      typename DiscoverState = skip,
      typename ExamineTransition = skip,
      typename StartState = skip,
      typename FinishState = skip
    >
    void generate_state_space(
      bool timed,
      bool recursive,
      const lps::state& d0,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      std::unordered_map<lps::state, std::size_t>& discovered,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      StartState start_state = StartState(),
      FinishState finish_state = FinishState()
    )
    {
      if (timed)
      {
        generate_timed_state_space(recursive, d0, regular_summands, confluent_summands, discovered, discover_state, examine_transition, start_state, finish_state);
      }
      else
      {
        generate_untimed_state_space(recursive, d0, regular_summands, confluent_summands, discovered, discover_state, examine_transition, start_state, finish_state);
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
      bool timed,
      bool recursive,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      StartState start_state = StartState(),
      FinishState finish_state = FinishState()
    )
    {
      lps::state d0 = rewrite_state(m_initial_state);
      if (!m_confluent_summands.empty())
      {
        d0 = find_representative(d0, m_confluent_summands);
      }
      generate_state_space(timed, recursive, d0, m_regular_summands, m_confluent_summands, m_discovered, discover_state, examine_transition, start_state, finish_state);
    }

    /// \brief Generates outgoing transitions for a given state.
    std::vector<std::pair<lps::multi_action, lps::state>> generate_transitions(const lps::state& d0)
    {
      data::data_expression_list process_parameter_undo = process_parameter_values();
      std::vector<std::pair<lps::multi_action, lps::state>> result;
      add_assignments(sigma, m_process_parameters, d0);
      for (const explorer_summand& summand: m_regular_summands)
      {
        generate_transitions(
          summand,
          m_confluent_summands,
          [&](const process::timed_multi_action& a, const lps::state& d1)
          {
            result.emplace_back(lps::multi_action(a.actions(), a.time()), d1);
          }
        );
        // remove_assignments(sigma, summand.variables);
      }
      set_process_parameter_values(process_parameter_undo);
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
      data::data_expression_list process_parameter_undo = process_parameter_values();
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
      set_process_parameter_values(process_parameter_undo);
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

    const std::vector<explorer_summand>& regular_summands() const
    {
      return m_regular_summands;
    }

    const std::vector<explorer_summand>& confluent_summands() const
    {
      return m_confluent_summands;
    }

    const std::vector<data::variable>& process_parameters() const
    {
      return m_process_parameters;
    }

    data::data_expression_list process_parameter_values() const
    {
      return data::data_expression_list{m_process_parameters.begin(), m_process_parameters.end(), [&](const data::variable& x) { return sigma(x); }};
    }

    void set_process_parameter_values(const data::data_expression_list& values)
    {
      add_assignments(sigma, m_process_parameters, values);
    }
};

inline
std::ostream& operator<<(std::ostream& out, const explorer::explorer_summand& s)
{
  return out << "(" << s.condition << ") -> " << s.multi_action << " . P(" << core::detail::print_arguments(s.next_state) << ")";
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_EXPLORER_H
