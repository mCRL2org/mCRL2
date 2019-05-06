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
#include <memory>
#include <random>
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
#include "mcrl2/utilities/unused.h"

namespace mcrl2::lps {

struct enumerator_error: public mcrl2::runtime_error
{
  explicit enumerator_error(const std::string& message): mcrl2::runtime_error(message)
  { }
};

inline
const data::data_expression& real_zero()
{
  static data::data_expression result = data::sort_real::creal(data::sort_int::cint(data::sort_nat::c0()), data::sort_pos::c1());
  return result;
}

inline
const data::data_expression& real_one()
{
  static data::data_expression result = data::sort_real::creal(data::sort_int::cint(data::sort_nat::cnat(data::sort_pos::c1())), data::sort_pos::c1());
  return result;
}

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
  assert(v.size() <= e.size());
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

// invariant: the elements of states must be unique
// invariant: the elements of probabilities must be >= 0
// invariant: the elements of probabilities must sum up to 1
// invariant: |probabilities| = |states|
struct stochastic_state
{
  // TODO: use a more efficient representation
  std::vector<data::data_expression> probabilities;
  std::vector<state> states;

  stochastic_state() = default;

  explicit stochastic_state(const state& s)
    : probabilities{real_one()}, states{s}
  {}

  void push_back(const data::data_expression& probability, const state& s)
  {
    probabilities.push_back(probability);
    states.push_back(s);
  }
};

inline
void check_stochastic_state(const stochastic_state& /* s */)
{
  // TODO
}

struct explorer_summand
{
  data::variable_list variables;
  data::data_expression condition;
  process::timed_multi_action multi_action;
  stochastic_distribution distribution;
  std::vector<data::data_expression> next_state;
  std::size_t index;

  // attributes for caching
  caching cache_strategy;
  std::vector<data::variable> gamma;
  atermpp::function_symbol f_gamma;
  mutable std::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> local_cache;

  template <typename ActionSummand>
  explorer_summand(const ActionSummand& summand, std::size_t summand_index, const data::variable_list& process_parameters, caching cache_strategy_)
    : variables(summand.summation_variables()),
      condition(summand.condition()),
      multi_action(summand.multi_action().actions(), summand.multi_action().time()),
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
    lps::multi_action(summand.multi_action.actions(), summand.multi_action.time()),
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
    std::unordered_map<atermpp::term_appl<data::data_expression>, std::list<data::data_expression_list>> global_cache;
    std::unordered_map<state, std::size_t> m_discovered;

    // used by make_timed_state, to avoid needless creation of vectors
    std::vector<data::data_expression> timed_state;

    Specification preprocess(const Specification& lpsspec)
    {
      Specification result = lpsspec;
      detail::instantiate_global_variables(result);
      lps::order_summand_variables(result);
      if (m_options.resolve_summand_variable_name_clashes)
      {
        resolve_summand_variable_name_clashes(result);
      }
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

    // This function is based an iterative version of Tarjan's strongly connected components algorithm.
    // It returns the smallest node of the first SCC that is detected. The first SCC is a TSCC, meaning
    // that it has no outgoing edges. In a confluent tau graph there is only one TSCC, so this should
    // guarantee a unique representative.
    // N.B. The implementation is based on https://llbit.se/?p=3379
    template <typename SummandSequence>
    state find_representative(state& u0, const SummandSequence& summands)
    {
      using utilities::detail::contains;
      data::data_expression_list process_parameter_undo = process_parameter_values();

      std::vector<state> stack;
      std::map<state, std::size_t> low;
      std::map<state, std::size_t> disc;

      std::map<state, std::vector<state>> successors;
      std::vector<std::pair<state, std::size_t>> work;

      successors[u0] = generate_successors(u0, summands);
      work.emplace_back(std::make_pair(u0, 0));

      while (!work.empty())
      {
        state u = work.back().first;
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
        const std::vector<state>& succ = successors[u];
        for (std::size_t j = i; j < succ.size(); j++)
        {
          const state& v = succ[j];
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
          state result = u;
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
          state v = u;
          u = work.back().first;
          low[u] = std::min(low[u], low[v]);
        }
      }
      throw mcrl2::runtime_error("find_representative did not find a solution");
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
        remove_assignments(m_sigma, distribution.variables());
        check_stochastic_state(result);
      }
      else
      {
        result.probabilities.push_back(real_one());
        result.states.push_back(compute_state(next_state));
      }
      return result;
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
        data::data_expression condition = data::replace_variables(summand.condition, m_sigma);

        remove_assignments(m_sigma, m_process_parameters);
        remove_assignments(m_sigma, summand.variables);
        data::data_expression reduced_condition = m_rewr(summand.condition, m_sigma);

        std::string printed_condition = data::pp(condition).substr(0, 300);

        throw enumerator_error("Expression " + data::pp(reduced_condition) +
                               " does not rewrite to true or false in the condition "
                               + printed_condition
                               + (printed_condition.size() >= 300 ? "..." : ""));
      }
    }

    // Generates outgoing transitions for a summand, and reports them via the callback function report_transition.
    // It is assumed that the substitution sigma contains the assignments corresponding to the current state.
    template <typename SummandSequence, typename ReportTransition = skip>
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
                        process::timed_multi_action a = rewrite_action(summand.multi_action);
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
                          remove_assignments(m_sigma, summand.variables);
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
          add_assignments(m_sigma, summand.variables, e);
          process::timed_multi_action a = rewrite_action(summand.multi_action);
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
            remove_assignments(m_sigma, summand.variables);
          }
          report_transition(a, s1);
        }
      }
      if (!m_recursive)
      {
        remove_assignments(m_sigma, summand.variables);
      }
    }

    // pre: d0 is in normal form
    template <typename SummandSequence>
    std::vector<state> generate_successors(
      const state& s0,
      const SummandSequence& summands,
      const SummandSequence& confluent_summands = SummandSequence()
    )
    {
      std::vector<state> result;
      add_assignments(m_sigma, m_process_parameters, s0);
      for (const explorer_summand& summand: summands)
      {
        generate_transitions(
          summand,
          confluent_summands,
          [&](const process::timed_multi_action& /* a */, const state& s1)
          {
            result.push_back(s1);
          }
        );
        remove_assignments(m_sigma, summand.variables);
      }
      return result;
    }

    std::set<data::function_symbol> add_less_equal_symbol(std::set<data::function_symbol> s) const
    {
      std::set<data::function_symbol> result = std::move(s);
      result.insert(data::less_equal(data::sort_real::real_()));
      return result;
    }

    bool less_equal(const data::data_expression& t0, const data::data_expression& t1)
    {
      return m_rewr(data::less_equal(t0, t1)) == data::sort_bool::true_();
    }

    std::unique_ptr<todo_set> make_todo_set(const state& init)
    {
      switch (m_options.search_strategy)
      {
        case lps::es_breadth: return std::make_unique<breadth_first_todo_set>(init);
        case lps::es_depth: return std::make_unique<depth_first_todo_set>(init);
        case lps::es_highway: return std::make_unique<highway_todo_set>(init, m_options.todo_max);
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
        case lps::es_highway: return std::make_unique<highway_todo_set>(first, last, m_options.todo_max);
        default: throw mcrl2::runtime_error("unsupported search strategy");
      }
    }

  public:
    explorer(const Specification& lpsspec, const explorer_options& options_)
      : m_options(options_),
        m_rewr(lpsspec.data(),
          data::used_data_equation_selector(lpsspec.data(), add_less_equal_symbol(lps::find_function_symbols(lpsspec)), lpsspec.global_variables()),
          m_options.rewrite_strategy),
        m_enumerator(m_rewr, lpsspec.data(), m_rewr, m_id_generator, false)
    {
      Specification lpsspec_ = preprocess(lpsspec);
      const auto& params = lpsspec_.process().process_parameters();
      m_process_parameters = std::vector<data::variable>(params.begin(), params.end());
      m_n = m_process_parameters.size();
      timed_state.resize(m_n + 1);
      m_initial_state = lpsspec_.initial_process().state(lpsspec_.process().process_parameters());
      m_initial_distribution = initial_distribution(lpsspec_);
      core::identifier_string ctau{"ctau"};
      const auto& lpsspec_summands = lpsspec_.process().action_summands();
      for (std::size_t i = 0; i < lpsspec_summands.size(); i++)
      {
        const auto& summand = lpsspec_summands[i];
        auto cache_strategy = m_options.cached ? (m_options.global_cache ? lps::caching::global : lps::caching::local) : lps::caching::none;
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

    // Returns the concatenation of s and [t]
    state make_timed_state(const state& s, const data::data_expression& t)
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
      typename DiscoverState = skip,
      typename ExamineTransition = skip,
      typename StartState = skip,
      typename FinishState = skip,
      typename DiscoverInitialState = skip
    >
    void generate_state_space(
      bool recursive,
      const StateType& s0,
      const SummandSequence& regular_summands,
      const SummandSequence& confluent_summands,
      std::unordered_map<state, std::size_t>& discovered,
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
          std::size_t s_index = discovered.size();
          discovered.insert(std::make_pair(s, s_index));
          discover_state(s, s_index);
          s0_index.push_back(s_index);
        }
        discover_initial_state(s0_, s0_index);
      }
      else
      {
        todo = make_todo_set(s0);
        std::size_t s0_index = 0;
        discovered.insert(std::make_pair(s0, s0_index));
        discover_state(s0, s0_index);
      }

      while (!todo->empty() && !m_must_abort)
      {
        state s = todo->choose_element();
        std::size_t s_index = discovered.find(s)->second;
        start_state(s, s_index);
        add_assignments(m_sigma, m_process_parameters, s);
        for (const explorer_summand& summand: regular_summands)
        {
          generate_transitions(
            summand,
            confluent_summands,
            [&](const process::timed_multi_action& a, const state_type& s1)
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
                for (const state& s1_: S1)
                {
                  auto j = discovered.find(s1_);
                  if (j == discovered.end())
                  {
                    todo->insert(s1_);
                    std::size_t k = discovered.size();
                    j = discovered.insert(std::make_pair(s1_, k)).first;
                    discover_state(s1_, k);
                  }
                  s1_index.push_back(j->second);
                }
                examine_transition(s, s_index, a, s1, s1_index, summand.index);
              }
              else
              {
                auto j = discovered.find(s1);
                if (j == discovered.end())
                {
                  std::size_t k = discovered.size();
                  if constexpr (Timed)
                  {
                    const data::data_expression& t = s[m_n];
                    data::data_expression t1 = a.has_time() ? a.time() : t;
                    state s1_at_t1 = make_timed_state(s1, t1);
                    j = discovered.insert(std::make_pair(s1_at_t1, k)).first;
                    discover_state(s1_at_t1, k);
                    todo->insert(s1_at_t1);
                  }
                  else
                  {
                    j = discovered.insert(std::make_pair(s1, k)).first;
                    discover_state(s1, k);
                    todo->insert(s1);
                  }
                }
                std::size_t s1_index = j->second;
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
      typename DiscoverState = skip,
      typename ExamineTransition = skip,
      typename StartState = skip,
      typename FinishState = skip,
      typename DiscoverInitialState = skip
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
      add_assignments(m_sigma, m_process_parameters, d0);
      for (const explorer_summand& summand: m_regular_summands)
      {
        generate_transitions(
          summand,
          m_confluent_summands,
          [&](const process::timed_multi_action& a, const state_type& d1)
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
      add_assignments(m_sigma, m_process_parameters, d0);
      generate_transitions(
        m_regular_summands[i],
        m_confluent_summands,
        [&](const process::timed_multi_action& a, const state_type& d1)
        {
          result.emplace_back(lps::multi_action(a), d1);
        }
      );
      remove_assignments(m_sigma, m_regular_summands[i].variables);
      set_process_parameter_values(process_parameter_undo);
      return result;
    }

    /// \brief Abort the state space generation
    void abort() override
    {
      m_must_abort = true;
    }

    /// \brief Returns a mapping containing all discovered states.
    const std::unordered_map<state, std::size_t>& state_map() const
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
      add_assignments(m_sigma, m_process_parameters, values);
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_EXPLORER_H
