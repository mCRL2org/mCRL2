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

#include <random>
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/lps/detail/instantiate_global_variables.h"
#include "mcrl2/lps/explorer_options.h"
#include "mcrl2/lps/find_representative.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/order_summand_variables.h"
#include "mcrl2/lps/replace_constants_by_variables.h"
#include "mcrl2/lps/resolve_name_clashes.h"
#include "mcrl2/lps/stochastic_state.h"
#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/skip.h"

namespace mcrl2::lps {

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

inline
std::vector<data::data_expression> make_data_expression_vector(const data::data_expression_list& v)
{
  return std::vector<data::data_expression>(v.begin(), v.end());
}

class todo_set
{
  protected:
    std::deque<state> todo;

  public:
    explicit todo_set(const state& init)
      : todo{init}
    {}

    template<typename ForwardIterator>
    todo_set(ForwardIterator first, ForwardIterator last)
      : todo(first, last)
    {}

    virtual ~todo_set() = default;

    virtual state choose_element() = 0;

    virtual void insert(const state& s) = 0;

    virtual void finish_state()
    { }

    bool empty() const
    {
      return todo.empty();
    }

    std::size_t size() const
    {
      return todo.size();
    }
};

class breadth_first_todo_set : public todo_set
{
  public:
    explicit breadth_first_todo_set(const state& init)
      : todo_set(init)
    {}

    template<typename ForwardIterator>
    breadth_first_todo_set(ForwardIterator first, ForwardIterator last)
      : todo_set(first, last)
    {}

    state choose_element() override
    {
      auto s = todo.front();
      todo.pop_front();
      return s;
    }

    void insert(const state& s) override
    {
      todo.push_back(s);
    }
};

class depth_first_todo_set : public todo_set
{
  public:
    explicit depth_first_todo_set(const state& init)
      : todo_set(init)
    {}

    template<typename ForwardIterator>
    depth_first_todo_set(ForwardIterator first, ForwardIterator last)
      : todo_set(first, last)
    {}

    state choose_element() override
    {
      auto s = todo.back();
      todo.pop_back();
      return s;
    }

    void insert(const state& s) override
    {
      todo.push_back(s);
    }
};

class highway_todo_set : public todo_set
{
  protected:
    std::size_t N;
    std::size_t n;
    std::size_t L;
    std::random_device device;
    std::mt19937 generator;

  public:
    explicit highway_todo_set(const state& init, std::size_t N_)
      : todo_set(init),
        N(N_),
        n(0),
        L(1),
        device(),
        generator(device())
    {
    }

    template<typename ForwardIterator>
    highway_todo_set(ForwardIterator first, ForwardIterator last, std::size_t N_)
      : todo_set(first, last),
        N(N_),
        n(0),
        L(todo.size()),
        device(),
        generator(device())
    {
    }

    state choose_element() override
    {
      auto s = todo.front();
      todo.pop_front();
      return s;
    }

    void insert(const state& s) override
    {
      n++;
      if (n <= N)
      {
        todo.push_back(s);
      }
      else
      {
        std::uniform_int_distribution<> distribution(1, n);
        std::size_t k = distribution(generator);
        if (k <= N)
        {
          todo[todo.size() - k] = s;
        }
      }
    }

    void finish_state() override
    {
      L--;
      if (L == 0)
      {
        L = todo.size();
        n = 0;
      }
    }
};

template <typename Summand>
const stochastic_distribution& summand_distribution(const Summand& /* summand */)
{
  static stochastic_distribution empty_distribution;
  return empty_distribution;
}

template <>
const stochastic_distribution& summand_distribution(const lps::stochastic_action_summand& summand)
{
  return summand.distribution();
}

inline
const stochastic_distribution& initial_distribution(const lps::specification& /* lpsspec */)
{
  static stochastic_distribution empty_distribution;
  return empty_distribution;
}

inline
const stochastic_distribution& initial_distribution(const lps::stochastic_specification& lpsspec)
{
  return lpsspec.initial_process().distribution();
}

struct explorer_summand
{
  data::variable_list variables;
  data::data_expression condition;
  lps::multi_action multi_action;
  stochastic_distribution distribution;
  std::vector<data::data_expression> next_state;
  std::size_t index;

  // attributes for caching
  caching cache_strategy;
  std::vector<data::variable> gamma;
  atermpp::function_symbol f_gamma;
  mutable utilities::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> local_cache;

  template <typename ActionSummand>
  explorer_summand(const ActionSummand& summand, std::size_t summand_index, const data::variable_list& process_parameters, caching cache_strategy_)
    : variables(summand.summation_variables()),
      condition(summand.condition()),
      multi_action(summand.multi_action()),
      distribution(summand_distribution(summand)),
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

inline
std::ostream& operator<<(std::ostream& out, const explorer_summand& summand)
{
  return out << lps::stochastic_action_summand(
    summand.variables,
    summand.condition,
    summand.multi_action,
    data::make_assignment_list(summand.variables, summand.next_state),
    summand.distribution
  );
}

struct abortable
{
  virtual void abort() = 0;
};

template <bool Stochastic, bool Timed, typename Specification>
class explorer: public abortable
{
  public:
    using state_type = typename std::conditional<Stochastic, stochastic_state, state>::type;
    using state_index_type = typename std::conditional<Stochastic, std::list<std::size_t>, std::size_t>::type;
    static constexpr bool is_stochastic = Stochastic;
    static constexpr bool is_timed = Timed;

  protected:
    using enumerator_element = data::enumerator_list_element_with_substitution<>;

    struct transition
    {
      lps::multi_action action;
      state_type state;

      transition(lps::multi_action  action_, const state_type& state_)
       : action(std::move(action_)), state(state_)
      {}
    };

    const explorer_options& m_options;
    data::rewriter m_rewr;
    mutable data::mutable_indexed_substitution<> m_sigma;
    data::enumerator_identifier_generator m_id_generator;
    data::enumerator_algorithm<> m_enumerator;
    std::vector<data::variable> m_process_parameters;
    std::size_t m_n; // m_n = m_process_parameters.size()
    data::data_expression_list m_initial_state;
    lps::stochastic_distribution m_initial_distribution;
    bool m_recursive = false;
    std::vector<explorer_summand> m_regular_summands;
    std::vector<explorer_summand> m_confluent_summands;

    volatile bool m_must_abort = false;

    // N.B. The keys are stored in term_appl instead of data_expression_list for performance reasons.
    utilities::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> global_cache;
    utilities::indexed_set<state> m_discovered;

    // used by make_timed_state, to avoid needless creation of vectors
    mutable std::vector<data::data_expression> timed_state;

    Specification preprocess(const Specification& lpsspec)
    {
      Specification result = lpsspec;
      detail::instantiate_global_variables(result);
      lps::order_summand_variables(result);
      resolve_summand_variable_name_clashes(result); // N.B. This is a required preprocessing step.
      if (m_options.one_point_rule_rewrite)
      {
        one_point_rule_rewrite(result);
      }
      if (m_options.replace_constants_by_variables)
      {
        replace_constants_by_variables(result, m_rewr, m_sigma);
      }
      return result;
    }

    // Evaluates whether t0 <= t1
    bool less_equal(const data::data_expression& t0, const data::data_expression& t1) const
    {
      return m_rewr(data::less_equal(t0, t1)) == data::sort_bool::true_();
    }

    // Find a unique representative in the confluent tau-graph reachable from u0.
    template <typename SummandSequence>
    state find_representative(state& u0, const SummandSequence& summands)
    {
      bool recursive_undo = m_recursive;
      m_recursive = true;
      data::data_expression_list process_parameter_undo = process_parameter_values();
      state result = lps::find_representative(u0, [&](const state& u) { return generate_successors(u, summands); });
      set_process_parameter_values(process_parameter_undo);
      m_recursive = recursive_undo;
      return result;
    }

    template <typename DataExpressionSequence>
    state compute_state(const DataExpressionSequence& v) const
    {
      return state(v.begin(), m_n, [&](const data::data_expression& x) { return m_rewr(x, m_sigma); });
    }

    template <typename DataExpressionSequence>
    stochastic_state compute_stochastic_state(const stochastic_distribution& distribution, const DataExpressionSequence& next_state) const
    {
      stochastic_state result;
      if (distribution.is_defined())
      {
        m_enumerator.enumerate(enumerator_element(distribution.variables(), distribution.distribution()),
                    m_sigma,
                    [&](const enumerator_element& p) {
                      p.add_assignments(distribution.variables(), m_sigma, m_rewr);
                      result.probabilities.push_back(p.expression());
                      result.states.push_back(compute_state(next_state));
                      return false;
                    },
                    [](const data::data_expression& x) { return x == real_zero(); }
        );
        data::remove_assignments(m_sigma, distribution.variables());
        if (m_options.check_probabilities)
        {
          check_stochastic_state(result, m_rewr);
        }
      }
      else
      {
        result.probabilities.push_back(real_one());
        result.states.push_back(compute_state(next_state));
      }
      return result;
    }

    lps::multi_action rewrite_action(const lps::multi_action& a) const
    {
      const process::action_list& actions = a.actions();
      const data::data_expression& time = a.time();
      return
        lps::multi_action(
          process::action_list(
            actions.begin(),
            actions.end(),
            [&](const process::action& a)
            {
              const auto& args = a.arguments();
              return process::action(a.label(), data::data_expression_list(args.begin(), args.end(), [&](const data::data_expression& x) { return m_rewr(x, m_sigma); }));
            }
          ),
          a.has_time() ? m_rewr(time, m_sigma) : time
        );
    }

    void check_enumerator_solution(const enumerator_element& p, const explorer_summand& summand)
    {
      if (p.expression() != data::sort_bool::true_())
      {
        std::string printed_condition = data::pp(p.expression());
        data::remove_assignments(m_sigma, m_process_parameters);
        data::remove_assignments(m_sigma, summand.variables);
        data::data_expression reduced_condition = m_rewr(summand.condition, m_sigma);
        throw data::enumerator_error("Condition " + data::pp(reduced_condition) +
                                     " does not rewrite to true or false. Culprit: "
                                     + printed_condition.substr(0,300)
                                     + (printed_condition.size() > 300 ? "..." : ""));
      }
    }

    // Generates outgoing transitions for a summand, and reports them via the callback function report_transition.
    // It is assumed that the substitution sigma contains the assignments corresponding to the current state.
    template <typename SummandSequence, typename ReportTransition = utilities::skip>
    void generate_transitions(
      const explorer_summand& summand,
      const SummandSequence& confluent_summands,
      ReportTransition report_transition = ReportTransition()
    )
    {
      if (!m_recursive)
      {
        m_id_generator.clear();
      }
      if (summand.cache_strategy == caching::none)
      {
        data::data_expression condition = m_rewr(summand.condition, m_sigma);
        if (!data::is_false(condition))
        {
          m_enumerator.enumerate(enumerator_element(summand.variables, condition),
                      m_sigma,
                      [&](const enumerator_element& p) {
                        check_enumerator_solution(p, summand);
                        p.add_assignments(summand.variables, m_sigma, m_rewr);
                        lps::multi_action a = rewrite_action(summand.multi_action);
                        state_type s1;
                        if constexpr (Stochastic)
                        {
                          s1 = compute_stochastic_state(summand.distribution, summand.next_state);
                        }
                        else
                        {
                          s1 = compute_state(summand.next_state);
                          if (!confluent_summands.empty())
                          {
                            s1 = find_representative(s1, confluent_summands);
                          }
                        }
                        if (m_recursive)
                        {
                          data::remove_assignments(m_sigma, summand.variables);
                        }
                        report_transition(a, s1);
                        return false;
                      },
                      data::is_false
          );
        }
      }
      else
      {
        auto key = summand.compute_key(m_sigma);
        auto& cache = summand.cache_strategy == caching::global ? global_cache : summand.local_cache;
        auto q = cache.find(key);
        if (q == cache.end())
        {
          data::data_expression condition = m_rewr(summand.condition, m_sigma);
          std::list<data::data_expression_list> solutions;
          if (!data::is_false(condition))
          {
            m_enumerator.enumerate(enumerator_element(summand.variables, condition),
                        m_sigma,
                        [&](const enumerator_element& p) {
                          check_enumerator_solution(p, summand);
                          solutions.push_back(p.assign_expressions(summand.variables, m_rewr));
                          return false;
                        },
                        data::is_false
            );
          }
          q = cache.insert({key, solutions}).first;
        }
        for (const data::data_expression_list& e: q->second)
        {
          data::add_assignments(m_sigma, summand.variables, e);
          lps::multi_action a = rewrite_action(summand.multi_action);
          state_type s1;
          if constexpr (Stochastic)
          {
            s1 = compute_stochastic_state(summand.distribution, summand.next_state);
          }
          else
          {
            s1 = compute_state(summand.next_state);
            if (!confluent_summands.empty())
            {
              s1 = find_representative(s1, confluent_summands);
            }
          }
          if (m_recursive)
          {
            data::remove_assignments(m_sigma, summand.variables);
          }
          report_transition(a, s1);
        }
      }
      if (!m_recursive)
      {
        data::remove_assignments(m_sigma, summand.variables);
      }
    }

    template <typename SummandSequence>
    std::list<transition> out_edges(const state& s, const SummandSequence& regular_summands, const SummandSequence& confluent_summands)
    {
      std::list<transition> transitions;
      data::add_assignments(m_sigma, m_process_parameters, s);
      for (const explorer_summand& summand: regular_summands)
      {
        generate_transitions(
          summand,
          confluent_summands,
          [&](const lps::multi_action& a, const state_type& s1)
          {
            if constexpr (Timed)
            {
              const data::data_expression& t = s[m_n];
              if (a.has_time() && less_equal(a.time(), t))
              {
                return;
              }
              data::data_expression t1 = a.has_time() ? a.time() : t;
              state s1_at_t1 = make_timed_state(s1, t1);
              transitions.emplace_back(a, s1_at_t1);
            }
            else
            {
              transitions.emplace_back(a, s1);
            }
          }
        );
      }
      return transitions;
    }

    // pre: s0 is in normal form
    template <typename SummandSequence>
    std::vector<state> generate_successors(
      const state& s0,
      const SummandSequence& summands,
      const SummandSequence& confluent_summands = SummandSequence()
    )
    {
      std::vector<state> result;
      data::add_assignments(m_sigma, m_process_parameters, s0);
      for (const explorer_summand& summand: summands)
      {
        generate_transitions(
          summand,
          confluent_summands,
          [&](const lps::multi_action& /* a */, const state& s1)
          {
            result.push_back(s1);
          }
        );
        data::remove_assignments(m_sigma, summand.variables);
      }
      return result;
    }

    // Add operations on reals that are needed for the exploration.
    std::set<data::function_symbol> add_real_operators(std::set<data::function_symbol> s) const
    {
      std::set<data::function_symbol> result = std::move(s);
      result.insert(data::less_equal(data::sort_real::real_()));
      result.insert(data::greater_equal(data::sort_real::real_()));
      result.insert(data::sort_real::plus(data::sort_real::real_(), data::sort_real::real_()));
      return result;
    }

    std::unique_ptr<todo_set> make_todo_set(const state& init)
    {
      switch (m_options.search_strategy)
      {
        case lps::es_breadth: return std::make_unique<breadth_first_todo_set>(init);
        case lps::es_depth: return std::make_unique<depth_first_todo_set>(init);
        case lps::es_highway: return std::make_unique<highway_todo_set>(init, m_options.highway_todo_max);
        default: throw mcrl2::runtime_error("unsupported search strategy");
      }
    }

    template <typename ForwardIterator>
    std::unique_ptr<todo_set> make_todo_set(ForwardIterator first, ForwardIterator last)
    {
      switch (m_options.search_strategy)
      {
        case lps::es_breadth: return std::make_unique<breadth_first_todo_set>(first, last);
        case lps::es_depth: return std::make_unique<depth_first_todo_set>(first, last);
        case lps::es_highway: return std::make_unique<highway_todo_set>(first, last, m_options.highway_todo_max);
        default: throw mcrl2::runtime_error("unsupported search strategy");
      }
    }

    data::rewriter construct_rewriter(const Specification& lpsspec, bool remove_unused_rewrite_rules)
    {
      if (remove_unused_rewrite_rules)
      {
        return data::rewriter(lpsspec.data(),
          data::used_data_equation_selector(lpsspec.data(), add_real_operators(lps::find_function_symbols(lpsspec)), lpsspec.global_variables()),
          m_options.rewrite_strategy);
      }
      else
      {
        return data::rewriter(lpsspec.data(), m_options.rewrite_strategy);
      }
    }

    bool is_confluent_tau(const multi_action& a)
    {
      if (a.actions().empty())
      {
        return m_options.confluence_action == "tau";
      }
      else if (a.actions().size() == 1)
      {
        return std::string(a.actions().front().label().name()) == m_options.confluence_action;
      }
      return false;
    }

  public:
    explorer(const Specification& lpsspec, const explorer_options& options_)
      : m_options(options_),
        m_rewr(construct_rewriter(lpsspec, m_options.remove_unused_rewrite_rules)),
        m_enumerator(m_rewr, lpsspec.data(), m_rewr, m_id_generator, false)
    {
      Specification lpsspec_ = preprocess(lpsspec);
      const auto& params = lpsspec_.process().process_parameters();
      m_process_parameters = std::vector<data::variable>(params.begin(), params.end());
      m_n = m_process_parameters.size();
      timed_state.resize(m_n + 1);
      m_initial_state = lpsspec_.initial_process().expressions();
      m_initial_distribution = initial_distribution(lpsspec_);

      // Split the summands in regular and confluent summands
      const auto& lpsspec_summands = lpsspec_.process().action_summands();
      for (std::size_t i = 0; i < lpsspec_summands.size(); i++)
      {
        const auto& summand = lpsspec_summands[i];
        auto cache_strategy = m_options.cached ? (m_options.global_cache ? lps::caching::global : lps::caching::local) : lps::caching::none;
        if (is_confluent_tau(summand.multi_action()))
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

    // Returns the concatenation of s and [t]
    state make_timed_state(const state& s, const data::data_expression& t) const
    {
      std::copy(s.begin(), s.end(), timed_state.begin());
      timed_state.back() = t;
      return state(timed_state.begin(), m_n + 1);
    }

    state_type make_state(const state& s) const
    {
      if constexpr (Stochastic)
      {
        return stochastic_state(s);
      }
      else
      {
        return s;
      }
    }

    const state_type& make_state(const stochastic_state& s) const
    {
      return s;
    }

    // pre: s0 is in normal form
    template <
      typename StateType,
      typename SummandSequence,
      typename DiscoverState = utilities::skip,
      typename ExamineTransition = utilities::skip,
      typename StartState = utilities::skip,
      typename FinishState = utilities::skip,
      typename DiscoverInitialState = utilities::skip
    >
    void generate_state_space(
      bool recursive,
      const StateType& s0,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      utilities::indexed_set<state>& discovered,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      StartState start_state = StartState(),
      FinishState finish_state = FinishState(),
      DiscoverInitialState discover_initial_state = DiscoverInitialState()
    )
    {
      utilities::mcrl2_unused(discover_initial_state); // silence unused parameter warning

      m_recursive = recursive;
      std::unique_ptr<todo_set> todo;
      discovered.clear();

      if constexpr (Stochastic)
      {
        state_type s0_ = make_state(s0);
        const auto& S = s0_.states;
        todo = make_todo_set(S.begin(), S.end());
        discovered.clear();
        std::list<std::size_t> s0_index;
        for (const state& s: S)
        {
          // TODO: join duplicate targets
          std::size_t s_index = discovered.index(s);
          if (s_index >= discovered.size())
          {
            s_index = discovered.insert(s).first;
            discover_state(s, s_index);
          }
          s0_index.push_back(s_index);
        }
        discover_initial_state(s0_, s0_index);
      }
      else
      {
        todo = make_todo_set(s0);
        std::size_t s0_index = discovered.insert(s0).first;
        discover_state(s0, s0_index);
      }

      while (!todo->empty() && !m_must_abort)
      {
        state s = todo->choose_element();
        std::size_t s_index = discovered.index(s);
        start_state(s, s_index);
        data::add_assignments(m_sigma, m_process_parameters, s);
        for (const explorer_summand& summand: regular_summands)
        {
          generate_transitions(
            summand,
            confluent_summands,
            [&](const lps::multi_action& a, const state_type& s1)
            {
              if constexpr (Timed)
              {
                const data::data_expression& t = s[m_n];
                if (a.has_time() && less_equal(a.time(), t))
                {
                  return;
                }
              }
              if constexpr (Stochastic)
              {
                std::list<std::size_t> s1_index;
                const auto& S1 = s1.states;
                // TODO: join duplicate targets
                for (const state& s1_: S1)
                {
                  std::size_t k = discovered.index(s1_);
                  if (k >= discovered.size())
                  {
                    todo->insert(s1_);
                    k = discovered.insert(s1_).first;
                    discover_state(s1_, k);
                  }
                  s1_index.push_back(k);
                }
                examine_transition(s, s_index, a, s1, s1_index, summand.index);
              }
              else
              {
                std::size_t s1_index = discovered.index(s1);
                if (s1_index >= discovered.size())
                {
                  if constexpr (Timed)
                  {
                    const data::data_expression& t = s[m_n];
                    data::data_expression t1 = a.has_time() ? a.time() : t;
                    state s1_at_t1 = make_timed_state(s1, t1);
                    s1_index = discovered.insert(s1_at_t1).first;
                    discover_state(s1_at_t1, s1_index);
                    todo->insert(s1_at_t1);
                  }
                  else
                  {
                    s1_index = discovered.insert(s1).first;
                    discover_state(s1, s1_index);
                    todo->insert(s1);
                  }
                }
                examine_transition(s, s_index, a, s1, s1_index, summand.index);
              }
            }
          );
        }
        finish_state(s, s_index, todo->size());
        todo->finish_state();
      }
      m_must_abort = false;
    }

    /// \brief Generates the state space, and reports all discovered states and transitions by means of callback
    /// functions.
    /// \param discover_state Is invoked when a state is encountered for the first time.
    /// \param examine_transition Is invoked on every transition.
    /// \param start_state Is invoked on a state right before its outgoing transitions are being explored.
    /// \param finish_state Is invoked on a state after all of its outgoing transitions have been explored.
    template <
      typename DiscoverState = utilities::skip,
      typename ExamineTransition = utilities::skip,
      typename StartState = utilities::skip,
      typename FinishState = utilities::skip,
      typename DiscoverInitialState = utilities::skip
    >
    void generate_state_space(
      bool recursive,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      StartState start_state = StartState(),
      FinishState finish_state = FinishState(),
      DiscoverInitialState discover_initial_state = DiscoverInitialState()
    )
    {
      state_type s0;
      if constexpr (Stochastic)
      {
        s0 = compute_stochastic_state(m_initial_distribution, m_initial_state);
      }
      else
      {
        s0 = compute_state(m_initial_state);
        if (!m_confluent_summands.empty())
        {
          s0 = find_representative(s0, m_confluent_summands);
        }
        if constexpr (Timed)
        {
          s0 = make_timed_state(s0, real_zero());
        }
      }
      generate_state_space(recursive, s0, m_regular_summands, m_confluent_summands, m_discovered, discover_state, examine_transition, start_state, finish_state, discover_initial_state);
    }

    /// \brief Generates outgoing transitions for a given state.
    std::vector<std::pair<lps::multi_action, state_type>> generate_transitions(const state& d0)
    {
      data::data_expression_list process_parameter_undo = process_parameter_values();
      std::vector<std::pair<lps::multi_action, state_type>> result;
      data::add_assignments(m_sigma, m_process_parameters, d0);
      for (const explorer_summand& summand: m_regular_summands)
      {
        generate_transitions(
          summand,
          m_confluent_summands,
          [&](const lps::multi_action& a, const state_type& d1)
          {
            result.emplace_back(lps::multi_action(a.actions(), a.time()), d1);
          }
        );
      }
      set_process_parameter_values(process_parameter_undo);
      return result;
    }

    /// \brief Generates outgoing transitions for a given state.
    std::vector<std::pair<lps::multi_action, state>> generate_transitions(const data::data_expression_list& init)
    {
      state d0 = compute_state(init);
      return generate_transitions(d0);
    }

    /// \brief Generates outgoing transitions for a given state, reachable via the summand with index i.
    std::vector<std::pair<lps::multi_action, state_type>> generate_transitions(const data::data_expression_list& init, std::size_t i)
    {
      data::data_expression_list process_parameter_undo = process_parameter_values();
      state d0 = compute_state(init);
      std::vector<std::pair<lps::multi_action, state_type>> result;
      data::add_assignments(m_sigma, m_process_parameters, d0);
      generate_transitions(
        m_regular_summands[i],
        m_confluent_summands,
        [&](const lps::multi_action& a, const state_type& d1)
        {
          result.emplace_back(lps::multi_action(a), d1);
        }
      );
      data::remove_assignments(m_sigma, m_regular_summands[i].variables);
      set_process_parameter_values(process_parameter_undo);
      return result;
    }

    // pre: s0 is in normal form
    // N.B. Does not support stochastic specifications!
    template <
      typename SummandSequence,
      typename DiscoverState = utilities::skip,
      typename ExamineTransition = utilities::skip,
      typename TreeEdge = utilities::skip,
      typename BackEdge = utilities::skip,
      typename ForwardOrCrossEdge = utilities::skip,
      typename FinishState = utilities::skip
    >
    void generate_state_space_dfs_recursive(
      const state& s0,
      std::unordered_set<state> gray,
      std::unordered_set<state>& discovered,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      TreeEdge tree_edge = TreeEdge(),
      BackEdge back_edge = BackEdge(),
      ForwardOrCrossEdge forward_or_cross_edge = ForwardOrCrossEdge(),
      FinishState finish_state = FinishState()
    )
    {
      using utilities::detail::contains;

      // invariants:
      // - s not in discovered => color(s) = white
      // - s in discovered && s in gray => color(s) = gray
      // - s in discovered && s not in gray => color(s) = black

      gray.insert(s0);
      discovered.insert(s0);
      discover_state(s0);

      for (const transition& tr: out_edges(s0, regular_summands, confluent_summands))
      {
        if (m_must_abort)
        {
          break;
        }

        const auto&[a, s1] = tr;
        examine_transition(s0, a, s1);

        if (discovered.find(s1) == discovered.end())
        {
          tree_edge(s0, a, s1);
          if constexpr (Timed)
          {
            const data::data_expression& t = s0[m_n];
            data::data_expression t1 = a.has_time() ? a.time() : t;
            state s1_at_t1 = make_timed_state(s1, t1);
            discovered.insert(s1_at_t1);
          }
          else
          {
            discovered.insert(s1);
          }
          generate_state_space_dfs_recursive(s1, gray, discovered, regular_summands, confluent_summands, discover_state, examine_transition, tree_edge, back_edge, forward_or_cross_edge, finish_state);
        }
        else if (contains(gray, s1))
        {
          back_edge(s0, a, s1);
        }
        else
        {
          forward_or_cross_edge(s0, a, s1);
        }
      }
      gray.erase(s0);
      finish_state(s0);
    }

    template <
      typename DiscoverState = utilities::skip,
      typename ExamineTransition = utilities::skip,
      typename TreeEdge = utilities::skip,
      typename BackEdge = utilities::skip,
      typename ForwardOrCrossEdge = utilities::skip,
      typename FinishState = utilities::skip
    >
    void generate_state_space_dfs_recursive(
      bool recursive,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      TreeEdge tree_edge = TreeEdge(),
      BackEdge back_edge = BackEdge(),
      ForwardOrCrossEdge forward_or_cross_edge = ForwardOrCrossEdge(),
      FinishState finish_state = FinishState()
    )
    {
      m_recursive = recursive;
      std::unordered_set<state> gray;
      std::unordered_set<state> discovered;

      state s0 = compute_state(m_initial_state);
      if (!m_confluent_summands.empty())
      {
        s0 = find_representative(s0, m_confluent_summands);
      }
      if constexpr (Timed)
      {
        s0 = make_timed_state(s0, real_zero());
      }
      generate_state_space_dfs_recursive(s0, gray, discovered, m_regular_summands, m_confluent_summands, discover_state, examine_transition, tree_edge, back_edge, forward_or_cross_edge, finish_state);
      m_recursive = false;
    }

    // pre: s0 is in normal form
    // N.B. Does not support stochastic specifications!
    template <
      typename SummandSequence,
      typename DiscoverState = utilities::skip,
      typename ExamineTransition = utilities::skip,
      typename TreeEdge = utilities::skip,
      typename BackEdge = utilities::skip,
      typename ForwardOrCrossEdge = utilities::skip,
      typename FinishState = utilities::skip
    >
    void generate_state_space_dfs_iterative(
      const state& s0,
      std::unordered_set<state>& discovered,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      TreeEdge tree_edge = TreeEdge(),
      BackEdge back_edge = BackEdge(),
      ForwardOrCrossEdge forward_or_cross_edge = ForwardOrCrossEdge(),
      FinishState finish_state = FinishState()
    )
    {
      using utilities::detail::contains;

      // invariants:
      // - s not in discovered => color(s) = white
      // - s in discovered && s in todo => color(s) = gray
      // - s in discovered && s not in todo => color(s) = black

      std::vector<std::pair<state, std::list<transition>>> todo;

      todo.emplace_back(s0, out_edges(s0, regular_summands, confluent_summands));
      discovered.insert(s0);
      discover_state(s0);

      while (!todo.empty() && !m_must_abort)
      {
        const state* s = &todo.back().first;
        std::list<transition>* E = &todo.back().second;
        while (!E->empty())
        {
          transition e = E->front();
          const auto& a = e.action;
          const auto& s1 = e.state;
          E->pop_front();
          examine_transition(*s, a, s1);

          if (discovered.find(s1) == discovered.end())
          {
            tree_edge(*s, a, s1);
            if constexpr (Timed)
            {
              const data::data_expression& t = (*s)[m_n];
              data::data_expression t1 = a.has_time() ? a.time() : t;
              state s1_at_t1 = make_timed_state(s1, t1);
              discovered.insert(s1_at_t1);
              discover_state(s1_at_t1);
            }
            else
            {
              discovered.insert(s1);
              discover_state(s1);
            }
            todo.emplace_back(s1, out_edges(s1, regular_summands, confluent_summands));
            s = &todo.back().first;
            E = &todo.back().second;
          }
          else
          {
            if (std::find_if(todo.begin(), todo.end(), [&](const std::pair<state, std::list<transition>>& p) { return s1 == p.first; }) != todo.end())
            {
              back_edge(*s, a, s1);
            }
            else
            {
              forward_or_cross_edge(*s, a, s1);
            }
          }
        }
        todo.pop_back();
        finish_state(*s);
      }
      m_must_abort = false;
    }

    template <
      typename DiscoverState = utilities::skip,
      typename ExamineTransition = utilities::skip,
      typename TreeEdge = utilities::skip,
      typename BackEdge = utilities::skip,
      typename ForwardOrCrossEdge = utilities::skip,
      typename FinishState = utilities::skip
    >
    void generate_state_space_dfs_iterative(
      bool recursive,
      DiscoverState discover_state = DiscoverState(),
      ExamineTransition examine_transition = ExamineTransition(),
      TreeEdge tree_edge = TreeEdge(),
      BackEdge back_edge = BackEdge(),
      ForwardOrCrossEdge forward_or_cross_edge = ForwardOrCrossEdge(),
      FinishState finish_state = FinishState()
    )
    {
      m_recursive = recursive;
      std::unordered_set<state> discovered;

      state s0 = compute_state(m_initial_state);
      if (!m_confluent_summands.empty())
      {
        s0 = find_representative(s0, m_confluent_summands);
      }
      if constexpr (Timed)
      {
        s0 = make_timed_state(s0, real_zero());
      }
      generate_state_space_dfs_iterative(s0, discovered, m_regular_summands, m_confluent_summands, discover_state, examine_transition, tree_edge, back_edge, forward_or_cross_edge, finish_state);
      m_recursive = false;
    }

    /// \brief Abort the state space generation
    void abort() override
    {
      m_must_abort = true;
    }

    /// \brief Returns a mapping containing all discovered states.
    const utilities::indexed_set<state>& state_map() const
    {
      return m_discovered;
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
      return data::data_expression_list{m_process_parameters.begin(), m_process_parameters.end(), [&](const data::variable& x) { return m_sigma(x); }};
    }

    void set_process_parameter_values(const data::data_expression_list& values)
    {
      data::add_assignments(m_sigma, m_process_parameters, values);
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_EXPLORER_H
