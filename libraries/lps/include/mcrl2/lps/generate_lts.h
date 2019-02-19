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
  bool cached = false;
};

inline
std::ostream& operator<<(std::ostream& out, const generate_lts_options& options)
{
  out << "cached = " << std::boolalpha << options.cached << std::endl;
  out << "one_point_rule_rewrite = " << std::boolalpha << options.one_point_rule_rewrite << std::endl;
  out << "resolve_summand_variable_name_clashes = " << std::boolalpha << options.resolve_summand_variable_name_clashes << std::endl;
  out << "replace_constants_by_variables = " << std::boolalpha << options.replace_constants_by_variables << std::endl;
  out << "rewrite_strategy = " << std::boolalpha << options.rewrite_strategy << std::endl;
  out << "store_states_as_trees = " << std::boolalpha << options.store_states_as_trees << std::endl;
  return out;
}

template <typename StateType>
class lts_generator
{
  protected:
    typedef data::enumerator_list_element_with_substitution<> enumerator_element;
    typedef StateType state_type;

    const specification& original_lpsspec;
    data::rewriter r;
    mutable data::mutable_indexed_substitution<> sigma;
    data::enumerator_identifier_generator id_generator;
    data::enumerator_algorithm<data::rewriter, data::rewriter> E;

    void preprocess(specification& lpsspec, const generate_lts_options& options) const
    {
      detail::instantiate_global_variables(lpsspec);
      lps::order_summand_variables(lpsspec);
      if (options.resolve_summand_variable_name_clashes) { resolve_summand_variable_name_clashes(lpsspec); }
      if (options.one_point_rule_rewrite)                { one_point_rule_rewrite(lpsspec); }
      if (options.replace_constants_by_variables)        { replace_constants_by_variables(lpsspec, r, sigma); }
    }

    struct next_state_summand
    {
      data::variable_list variables;
      data::data_expression condition;
      process::action_list actions;
      data::data_expression_list next_state;
      std::list<data::data_expression_list> solutions;

      explicit next_state_summand(const lps::action_summand& summand, const data::variable_list& process_parameters)
        : variables(summand.summation_variables()),
          condition(summand.condition()),
          actions(summand.multi_action().actions()),
          next_state(summand.next_state(process_parameters))
      {}

      /// \brief Returns all valuations of variables that make condition become true.
      virtual const std::list<data::data_expression_list>& generate_solutions(const data::enumerator_algorithm<data::rewriter, data::rewriter>& E,
                                                                              const data::rewriter& r,
                                                                              data::mutable_indexed_substitution<>& sigma
                                                                             )
      {
        solutions.clear();
        data::data_expression c = r(condition, sigma);
        if (!data::is_false(c))
        {
          E.enumerate(enumerator_element(variables, c),
                      sigma,
                      [&](const enumerator_element& p)
                      {
                        solutions.push_back(p.assign_expressions(variables, r));
                        return false;
                      },
                      data::is_false
          );
        }
        return solutions;
      }

      void add_assignments(data::mutable_indexed_substitution<>& sigma, const data::data_expression_list& e) const
      {
        assert(variables.size() == e.size());
        auto vi = variables.begin();
        auto ei = e.begin();
        for (; vi != variables.end(); ++vi, ++ei)
        {
          sigma[*vi] = *ei;
        }
      }

      void remove_assignments(data::mutable_indexed_substitution<>& sigma) const
      {
        for (const data::variable& v: variables)
        {
          sigma[v] = v;
        }
      }
    };

    // Summand that caches enumerator solutions
    struct next_state_summand_with_caching: public next_state_summand
    {
      typedef next_state_summand super;
      using super::solutions;

      std::unordered_map<data::data_expression_list, std::list<data::data_expression_list>> solution_cache;
      data::variable_list free_variables; // process parameters that occur free in condition

      next_state_summand_with_caching(const lps::action_summand& summand, const data::variable_list& process_parameters)
        : super(summand, process_parameters)
      {
        using utilities::detail::contains;
        std::set<data::variable> FV = data::find_free_variables(summand.condition());
        std::vector<data::variable> result;
        for (const data::variable& v: process_parameters)
        {
          if (contains(FV, v))
          {
            result.push_back(v);
          }
        }
        free_variables = data::variable_list(result.begin(), result.end());
      }

      const std::list<data::data_expression_list>& generate_solutions(const data::enumerator_algorithm<data::rewriter, data::rewriter>& E,
                                                                      const data::rewriter& r,
                                                                      data::mutable_indexed_substitution<>& sigma
                                                                     ) override
      {
        data::data_expression_list key{free_variables.begin(), free_variables.end(), [&](const data::variable& x) { return sigma(x); }};
        auto i = solution_cache.find(key);
        if (i != solution_cache.end())
        {
          return i->second;
        }
        super::generate_solutions(E, r, sigma);
        solution_cache[key] = solutions;
        return solutions;
      }
    };

    template <typename NextStateSummand, typename ReportState = skip1, typename ReportTransition = skip3>
    void generate(const generate_lts_options& options,
                  ReportState report_state = ReportState(),
                  ReportTransition report_transition = ReportTransition()
    ) const
    {
      lps::specification lpsspec = original_lpsspec;
      preprocess(lpsspec, options);
      const data::variable_list& process_parameters = lpsspec.process().process_parameters();

      std::vector<NextStateSummand> next_state_summands;
      for (const action_summand& summand: lpsspec.process().action_summands())
      {
        next_state_summands.push_back(NextStateSummand(summand, process_parameters));
      }

      atermpp::indexed_set<state_type> discovered;
      std::deque<std::size_t> todo;
      const std::size_t n = process_parameters.size();

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

        for (NextStateSummand& summand: next_state_summands)
        {
          for (const data::data_expression_list& e: summand.generate_solutions(E, r, sigma))
          {
            summand.add_assignments(sigma, e);
            process::action_list a = rewrite_action_list(summand.actions);
            state_type d1 = rewrite_state(summand.next_state);
            auto j = discovered.put(d1);
            if (j.second)
            {
              todo.push_back(j.first);
              report_state(d1);
            }
            report_transition(i, a, j.first);
          }
          summand.remove_assignments(sigma);
        }
      }
    }

  public:
    lts_generator(const specification& lpsspec, data::rewrite_strategy rewrite_strategy)
      : original_lpsspec(lpsspec),
        r(lpsspec.data(), data::used_data_equation_selector(lpsspec.data(), lps::find_function_symbols(lpsspec), lpsspec.global_variables()), rewrite_strategy),
        E(r, lpsspec.data(), r, id_generator, false)
    {}

    template <typename ReportState = skip1, typename ReportTransition = skip3>
    void generate(const generate_lts_options& options,
                  ReportState report_state = ReportState(),
                  ReportTransition report_transition = ReportTransition()
                 ) const
    {
      if (options.cached)
      {
        generate<next_state_summand_with_caching>(options, report_state, report_transition);
      }
      else
      {
        generate<next_state_summand>(options, report_state, report_transition);
      }
    }
};

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

  lts_generator<state_type> generator(lpsspec, options.rewrite_strategy);
  generator.generate(options,
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
