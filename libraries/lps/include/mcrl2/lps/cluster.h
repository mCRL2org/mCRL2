// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/cluster.h
/// \brief Clusters the summands of a linear process with the same action labels into one summand.

#ifndef MCRL2_LPS_CLUSTER_H
#define MCRL2_LPS_CLUSTER_H

#include "mcrl2/data/enumerated_sort_generator.h"
#include "mcrl2/data/real_utilities.h"
#include "mcrl2/data/replace_capture_avoiding_with_an_identifier_generator.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/substitutions/maintain_variables_in_rhs.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/detail/merge_variables.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/stochastic_specification.h"

#include <optional>

namespace mcrl2::lps
{

/// \brief Options for clustering.
struct cluster_options
{
  /// \brief If true, a summand is selected with boolean sum variables and case trees of if functions, instead of a
  ///        sum variable of an enumerated sort and case functions on it.
  bool binary = false;
};

/// \brief Clusters the summands of a linear process with the same action labels, and the same time status, into one
///        summand.
/// \details The summands in a cluster are selected with a sum variable e of an enumerated sort with an element per
///          summand: a term of the clustered summand that differs between the summands is a case function on e
///          applied to the terms of the summands. With binary, the summands are selected with boolean sum variables
///          and case trees of if functions. The sum variables, and the stochastic variables of the summands are
///          merged: variables of the same sort are joined, and the other variables are added, and fixed to a default
///          value in the summands in which they do not occur. Deadlock summands are clustered in the same way.
///
///          Terms that are equal up to global variables are not replaced by a case function.
///
///          The enumerated sorts and case functions are added to the data specification of an enumerated sort
///          generator; fresh names are taken from an identifier generator, that must contain the identifiers of the
///          linear process and its data specification.
class cluster_algorithm
{
protected:
  using renaming_type = data::maintain_variables_in_rhs<data::mutable_map_substitution<>>;

  /// \brief A cluster of n summands that is turned into a single summand. Lists of terms with one term per summand are
  ///        in the reverse order of the summands, as arguments of a case function.
  struct summand_cluster
  {
    summand_cluster(std::size_t enumerated_sort_size_, const data::variable& enumerated_variable_, std::size_t n_)
      : enumerated_sort_size(enumerated_sort_size_),
        enumerated_variable(enumerated_variable_),
        n(n_)
    {}

    /// \brief The number of elements of the enumerated sort that selects a summand, and a variable of that sort.
    std::size_t enumerated_sort_size;
    data::variable enumerated_variable;
    std::size_t n;
    /// \brief The sum variables of the clustered summand: the enumerated variable (with binary the boolean variables
    ///        that select a summand) followed by the merged sum variables of the summands.
    data::variable_list sums;
    /// \brief Per summand, the condition that fixes the merged sum variables that do not occur in it to a default
    ///        value.
    data::data_expression_list absent_sum_variables_conditions;
    /// \brief With binary, the condition that excludes the values of the boolean variables that do not select a
    ///        summand.
    data::data_expression binary_sum_condition;
    /// \brief Per summand, the renaming of its sum variables.
    std::vector<renaming_type> sum_renamings;
    /// \brief For action summands, the merged stochastic variables, per summand the condition that fixes those that
    ///        do not occur in it to a default value, and per summand the renaming of its stochastic variables followed
    ///        by that of its sum variables.
    data::variable_list stochastic_variables;
    data::data_expression_list absent_stochastic_variables_conditions;
    std::vector<renaming_type> renamings;
  };

  const data::data_specification& m_data;
  data::enumerated_sort_generator& m_enumerated_sorts;
  data::set_identifier_generator& m_identifier_generator;
  const std::set<data::variable>& m_global_variables;
  cluster_options m_options;

  data::variable fresh_variable(const std::string& name_hint, const data::sort_expression& sort)
  {
    return data::variable(m_identifier_generator(name_hint), sort);
  }

  data::variable_list merge_variables(const data::variable_list& v1,
    const data::variable_list& v2,
    std::vector<data::variable_list>& renamings_pars,
    std::vector<data::data_expression_list>& renamings_args,
    data::data_expression_list& conditions,
    const data::variable_list& avoid)
  {
    const auto fresh = [this](const data::variable& v)
    {
      return fresh_variable(v.name(), v.sort());
    };
    // A new representative generator is used for each sort, as the data specification can change in between.
    const auto representative = [this](const data::sort_expression& s)
    {
      return data::representative_generator(m_data)(s);
    };
    return detail::merge_variables(v1, v2, renamings_pars, renamings_args, conditions, avoid, fresh, representative);
  }

  /// \brief Applies sigma to x, renaming a bound variable only if it would capture a variable of a right hand side of
  ///        sigma. Fresh names come from the identifier generator.
  data::data_expression apply_renaming(const data::data_expression& x, renaming_type& sigma)
  {
    return data::replace_variables_capture_avoiding_with_an_identifier_generator(x, sigma, m_identifier_generator);
  }

  bool is_global_variable(const data::data_expression& d) const
  {
    return data::is_variable(d)
           && m_global_variables.find(atermpp::down_cast<data::variable>(d)) != m_global_variables.end();
  }

  static bool summands_can_be_clustered(const stochastic_action_summand& summand1,
    const stochastic_action_summand& summand2)
  {
    // The multi-actions are ordered, so they have the same action labels if the sequences of labels are equal.
    const process::action_list& actions1 = summand1.multi_action().actions();
    const process::action_list& actions2 = summand2.multi_action().actions();
    return summand1.has_time() == summand2.has_time()
           && std::equal(actions1.begin(),
             actions1.end(),
             actions2.begin(),
             actions2.end(),
             [](const process::action& a1, const process::action& a2) { return a1.label() == a2.label(); });
  }

  static bool summands_can_be_clustered(const deadlock_summand& summand1, const deadlock_summand& summand2)
  {
    return summand1.has_time() == summand2.has_time();
  }

  /// \brief The value of var in the assignments, which is var if it is not assigned.
  static data::data_expression assigned_value(const data::variable& var, const data::assignment_list& assignments)
  {
    const data::assignment_list::const_iterator i = std::find_if(assignments.begin(),
      assignments.end(),
      [&var](const data::assignment& a) { return a.lhs() == var; });
    return i == assignments.end() ? data::data_expression(var) : i->rhs();
  }

  /// \brief The assignments of the values to the parameters, without assignments of a parameter to itself, unless the
  ///        parameter is one of the sum or stochastic variables.
  static data::assignment_list make_optimised_assignment_list(const data::variable_list& parameters,
    const data::data_expression_list& values,
    const data::variable_list& sum_variables,
    const data::variable_list& stochastic_variables)
  {
    assert(parameters.size() == values.size());
    data::data_expression_list::const_iterator i = values.begin();
    return data::assignment_list(
      parameters.begin(),
      parameters.end(),
      [&i](const data::variable& p) { return data::assignment(p, *i++); },
      [&sum_variables, &stochastic_variables](const data::assignment& a)
      {
        return a.lhs() != a.rhs()
               || std::find(sum_variables.begin(), sum_variables.end(), a.rhs()) != sum_variables.end()
               || std::find(stochastic_variables.begin(), stochastic_variables.end(), a.rhs())
                    != stochastic_variables.end();
      });
  }

  /// \brief Adds the renaming of pars into args to sigma. The first assignment to a variable counts.
  static void
  add_renaming(renaming_type& sigma, const data::variable_list& pars, const data::data_expression_list& args)
  {
    assert(pars.size() == args.size());
    data::data_expression_list::const_iterator arg = args.begin();
    for (const data::variable& par: pars)
    {
      if (sigma(par) == par) // sigma is not defined for par.
      {
        sigma[par] = *arg;
      }
      ++arg;
    }
  }

  /// \brief Adds the assignments of other to sigma for the variables that sigma does not assign yet.
  static void add_renaming(renaming_type& sigma, const renaming_type& other)
  {
    for (const std::pair<const data::variable, data::data_expression>& assignment: other)
    {
      if (sigma(assignment.first) == assignment.first)
      {
        sigma[assignment.first] = assignment.second;
      }
    }
  }

  static const data::data_expression& summand_time(const stochastic_action_summand& summand)
  {
    return summand.multi_action().time();
  }

  static const data::data_expression& summand_time(const deadlock_summand& summand)
  {
    return summand.deadlock().time();
  }

  template<class T>
  static bool all_equal(const atermpp::term_list<T>& l)
  {
    return std::all_of(l.begin(), l.end(), [&l](const T& x) { return x == l.front(); });
  }

  /// \brief Prepends the boolean variables that select one of n summands to tail. The condition excludes the values of
  ///        these variables that do not select a summand.
  data::variable_list make_binary_sums(std::size_t n, data::data_expression& condition, const data::variable_list& tail)
  {
    assert(n > 1);
    data::variable_list result = tail;
    condition = data::sort_bool::true_();
    for (n = n - 1; n > 0; n = n / 2)
    {
      const data::variable sum_variable = fresh_variable("e", data::sort_bool::bool_());
      result.push_front(sum_variable);
      if ((n % 2) == 0)
      {
        condition = data::lazy::and_(sum_variable, condition);
      }
      else
      {
        condition = data::lazy::or_(sum_variable, condition);
      }
    }
    return result;
  }

  data::data_expression construct_binary_case_tree_rec(std::size_t n,
    const data::variable_list& sums,
    data::data_expression_list& terms,
    const data::sort_expression& sort)
  {
    assert(!terms.empty());

    if (n == 0)
    {
      const data::data_expression t = terms.front();
      terms.pop_front();
      return t;
    }

    assert(!sums.empty());
    const data::data_expression t = construct_binary_case_tree_rec(n / 2, sums.tail(), terms, sort);
    if (terms.empty())
    {
      return t;
    }

    const data::data_expression t1 = construct_binary_case_tree_rec(n / 2, sums.tail(), terms, sort);
    if (t == t1)
    {
      return t;
    }
    return data::application(m_enumerated_sorts.case_function(2, sort), sums.front(), t, t1);
  }

  /// \brief A case tree over the boolean variables at the front of sums that selects one of the n terms.
  data::data_expression construct_binary_case_tree(std::size_t n,
    const data::variable_list& sums,
    data::data_expression_list terms,
    const data::sort_expression& sort)
  {
    return construct_binary_case_tree_rec(n - 1, sums, terms, sort);
  }

  /// \brief The sum variables of the summands are merged. They are renamed away from the parameters if needed.
  template<typename Summand>
  void
  merge_sum_variables(summand_cluster& c, const std::vector<Summand>& summands, const data::variable_list& parameters)
  {
    std::vector<data::variable_list> rename_list_pars;
    std::vector<data::data_expression_list> rename_list_args;
    for (const Summand& summand: summands)
    {
      c.sums = merge_variables(summand.summation_variables(),
        c.sums,
        rename_list_pars,
        rename_list_args,
        c.absent_sum_variables_conditions,
        parameters);
    }
    c.sum_renamings.resize(c.n);
    for (std::size_t i = 0; i < c.n; ++i)
    {
      add_renaming(c.sum_renamings[i], rename_list_pars[i], rename_list_args[i]);
    }

    if (m_options.binary)
    {
      c.sums = make_binary_sums(c.n, c.binary_sum_condition, c.sums);
    }
    else
    {
      c.sums.push_front(c.enumerated_variable);
    }
  }

  /// \brief The stochastic variables of the summands are merged. They are renamed away from the parameters and the sum
  ///        variables if needed. In the distribution and the next state, the renaming of the stochastic variables takes
  ///        precedence over the renaming of the sum variables.
  void merge_stochastic_variables(summand_cluster& c,
    const stochastic_action_summand_vector& summands,
    const data::variable_list& parameters)
  {
    std::set<data::variable> all_sum_variables;
    for (const stochastic_action_summand& summand: summands)
    {
      all_sum_variables.insert(summand.summation_variables().begin(), summand.summation_variables().end());
    }
    const data::variable_list avoid
      = parameters + data::variable_list(all_sum_variables.begin(), all_sum_variables.end());

    std::vector<data::variable_list> rename_list_pars;
    std::vector<data::data_expression_list> rename_list_args;
    for (const stochastic_action_summand& summand: summands)
    {
      c.stochastic_variables = merge_variables(summand.distribution().variables(),
        c.stochastic_variables,
        rename_list_pars,
        rename_list_args,
        c.absent_stochastic_variables_conditions,
        avoid);
    }
    c.renamings.resize(c.n);
    for (std::size_t i = 0; i < c.n; ++i)
    {
      add_renaming(c.renamings[i], rename_list_pars[i], rename_list_args[i]);
      add_renaming(c.renamings[i], c.sum_renamings[i]);
    }
  }

  /// \brief The terms of the summands, in reverse order, as arguments of a case function.
  static data::data_expression_list case_arguments(const data::data_expression_vector& terms)
  {
    return data::data_expression_list(terms.rbegin(), terms.rend());
  }

  /// \brief Selects one of the terms, which are given in the reverse order of the summands, with the case function of
  ///        the enumerated sort on sort, or with binary with a case tree over the boolean sum variables at the front
  ///        of c.sums.
  data::data_expression
  case_term(const summand_cluster& c, const data::data_expression_list& terms, const data::sort_expression& sort)
  {
    if (m_options.binary)
    {
      return construct_binary_case_tree(c.n, c.sums, terms, sort);
    }
    data::data_expression_list arguments = terms;
    arguments.push_front(c.enumerated_variable);
    return data::application(m_enumerated_sorts.case_function(c.enumerated_sort_size, sort), arguments);
  }

  /// \brief The terms of the summands in a cluster can be represented by a single term if all terms that are not
  ///        global variables are equal. This is the first term that is not a global variable, or the last term if all
  ///        terms are global variables.
  std::optional<data::data_expression> equal_up_to_global_variables(const data::data_expression_vector& terms) const
  {
    const data::data_expression_vector::const_iterator first = std::find_if(terms.begin(),
      terms.end(),
      [this](const data::data_expression& t) { return !is_global_variable(t); });
    if (first == terms.end())
    {
      return terms.back();
    }
    if (std::all_of(first,
          terms.end(),
          [this, &first](const data::data_expression& t) { return t == *first || is_global_variable(t); }))
    {
      return *first;
    }
    return std::nullopt;
  }

  /// \brief The values of a parameter in the next states of the summands in a cluster can be represented by a single
  ///        term if they are all equal, or if they are all global variables. A global variable is not replaced by a
  ///        different term: this leads to smaller case functions, but bigger and less structured state spaces, as
  ///        parameters are less often set to default values. Constant elimination can less often be applied, as it
  ///        does not have subtle knowledge of case functions.
  std::optional<data::data_expression> equal_next_state_values(const data::data_expression_vector& terms) const
  {
    if (std::all_of(terms.begin(), terms.end(), [&terms](const data::data_expression& t) { return t == terms.front(); })
        || std::all_of(terms.begin(),
          terms.end(),
          [this](const data::data_expression& t) { return is_global_variable(t); }))
    {
      return terms.front();
    }
    return std::nullopt;
  }

  /// \brief The common term of the terms of the summands if they are equal up to global variables, and a case term
  ///        otherwise.
  data::data_expression
  clustered_term(const summand_cluster& c, const data::data_expression_vector& terms, const data::sort_expression& sort)
  {
    const std::optional<data::data_expression> equal_term = equal_up_to_global_variables(terms);
    return equal_term ? *equal_term : case_term(c, case_arguments(terms), sort);
  }

  /// \brief The condition that fixes the merged variables that do not occur in the selected summand to a default
  ///        value, given the conditions per summand.
  data::data_expression absent_variables_condition(const summand_cluster& c,
    const data::data_expression_list& conditions)
  {
    return all_equal(conditions) ? conditions.front() : case_term(c, conditions, data::sort_bool::bool_());
  }

  template<typename Summand>
  data::data_expression clustered_condition(summand_cluster& c, const std::vector<Summand>& summands)
  {
    data::data_expression_vector conditions;
    for (std::size_t i = 0; i < c.n; ++i)
    {
      conditions.push_back(apply_renaming(summands[i].condition(), c.sum_renamings[i]));
    }
    if (m_options.binary)
    {
      return data::lazy::and_(case_term(c, c.absent_sum_variables_conditions, data::sort_bool::bool_()),
        data::lazy::and_(c.binary_sum_condition, case_term(c, case_arguments(conditions), data::sort_bool::bool_())));
    }
    return data::lazy::and_(absent_variables_condition(c, c.absent_sum_variables_conditions),
      clustered_term(c, conditions, data::sort_bool::bool_()));
  }

  /// \brief The actions of the clustered summand. All summands have the same action labels.
  process::action_list clustered_actions(summand_cluster& c, const stochastic_action_summand_vector& summands)
  {
    // The arguments of all actions of each summand, renamed.
    std::vector<data::data_expression_vector> arguments(c.n);
    for (std::size_t i = 0; i < c.n; ++i)
    {
      for (const process::action& a: summands[i].multi_action().actions())
      {
        for (const data::data_expression& argument: a.arguments())
        {
          arguments[i].push_back(apply_renaming(argument, c.sum_renamings[i]));
        }
      }
    }

    std::vector<process::action> result;
    std::size_t argument_index = 0;
    for (const process::action& a: summands.front().multi_action().actions())
    {
      data::data_expression_vector result_arguments;
      for (const data::data_expression& argument: a.arguments())
      {
        data::data_expression_vector terms;
        for (const data::data_expression_vector& summand_arguments: arguments)
        {
          terms.push_back(summand_arguments[argument_index]);
        }
        result_arguments.push_back(clustered_term(c, terms, argument.sort()));
        ++argument_index;
      }
      result.emplace_back(a.label(), data::data_expression_list(result_arguments.begin(), result_arguments.end()));
    }
    return process::action_list(result.begin(), result.end());
  }

  /// \brief The time of the clustered summand, if all summands have a time.
  template<typename Summand>
  data::data_expression clustered_time(summand_cluster& c, const std::vector<Summand>& summands)
  {
    data::data_expression_vector times;
    for (std::size_t i = 0; i < c.n; ++i)
    {
      times.push_back(apply_renaming(summand_time(summands[i]), c.sum_renamings[i]));
    }
    if (m_options.binary)
    {
      return case_term(c, case_arguments(times), data::sort_real::real_());
    }
    return clustered_term(c, times, data::sort_real::real_());
  }

  /// \brief The distribution of the clustered summand.
  data::data_expression clustered_distribution(summand_cluster& c, const stochastic_action_summand_vector& summands)
  {
    data::data_expression_vector distributions;
    for (std::size_t i = 0; i < c.n; ++i)
    {
      distributions.push_back(apply_renaming(summands[i].distribution().distribution(), c.renamings[i]));
    }
    if (m_options.binary)
    {
      const data::data_expression condition
        = case_term(c, c.absent_stochastic_variables_conditions, data::sort_bool::bool_());
      const data::data_expression distribution = case_term(c, case_arguments(distributions), data::sort_real::real_());
      if (condition == data::sort_bool::true_())
      {
        return distribution;
      }
      return data::sort_real::optimized_times(
        data::if_(condition, data::sort_real::real_one(), data::sort_real::real_zero()),
        distribution);
    }

    const std::optional<data::data_expression> equal_distribution = equal_up_to_global_variables(distributions);
    const data::data_expression distribution
      = equal_distribution ? *equal_distribution
                           : case_term(c, case_arguments(distributions), data::sort_real::real_());
    // The distribution is multiplied by the condition on the stochastic variables that do not occur in a summand.
    // This factor is only left out if the distributions are equal and the condition is true.
    if (equal_distribution && all_equal(c.absent_stochastic_variables_conditions)
        && c.absent_stochastic_variables_conditions.front() == data::sort_bool::true_())
    {
      return distribution;
    }
    return data::sort_real::optimized_times(
      data::if_(absent_variables_condition(c, c.absent_stochastic_variables_conditions),
        data::sort_real::real_one(),
        data::sort_real::real_zero()),
      distribution);
  }

  /// \brief The next state of the clustered summand.
  data::assignment_list clustered_next_state(summand_cluster& c,
    const stochastic_action_summand_vector& summands,
    const data::variable_list& parameters)
  {
    data::data_expression_vector next_state;
    for (const data::variable& parameter: parameters)
    {
      data::data_expression_vector values;
      for (std::size_t i = 0; i < c.n; ++i)
      {
        values.push_back(apply_renaming(assigned_value(parameter, summands[i].assignments()), c.renamings[i]));
      }
      const std::optional<data::data_expression> equal_value = equal_next_state_values(values);
      next_state.push_back(equal_value ? *equal_value : case_term(c, case_arguments(values), parameter.sort()));
    }
    return make_optimised_assignment_list(parameters,
      data::data_expression_list(next_state.begin(), next_state.end()),
      c.sums,
      c.stochastic_variables);
  }

  /// \brief Turns summands, with the same action labels and time status, into a single summand.
  template<typename Summand>
  Summand make_clustered_summand(const std::vector<Summand>& summands, const data::variable_list& parameters)
  {
    // With binary, the enumerated sort is Bool and the summand is selected with other boolean variables; the
    // enumerated variable is still generated, so that the fresh names are the same as without binary.
    const std::size_t enumerated_sort_size = m_options.binary ? 2 : summands.size();
    const data::sort_expression& enumerated_sort = m_enumerated_sorts(enumerated_sort_size);
    summand_cluster c(enumerated_sort_size, fresh_variable("e", enumerated_sort), summands.size());
    merge_sum_variables(c, summands, parameters);
    const data::data_expression condition = clustered_condition(c, summands);

    // The summands in a cluster either all have a time, or none has (summands_can_be_clustered).
    const bool has_time = summands.front().has_time();
    assert(std::all_of(summands.begin(),
      summands.end(),
      [has_time](const Summand& summand) { return summand.has_time() == has_time; }));

    if constexpr (std::is_same_v<Summand, stochastic_action_summand>)
    {
      const process::action_list actions = clustered_actions(c, summands);
      const multi_action multiaction
        = has_time ? multi_action(actions, clustered_time(c, summands)) : multi_action(actions);
      merge_stochastic_variables(c, summands, parameters);
      const data::data_expression distribution = clustered_distribution(c, summands);
      return stochastic_action_summand(c.sums,
        condition,
        multiaction,
        clustered_next_state(c, summands, parameters),
        stochastic_distribution(c.stochastic_variables, distribution));
    }
    else
    {
      return deadlock_summand(c.sums, condition, has_time ? deadlock(clustered_time(c, summands)) : deadlock());
    }
  }

  /// \brief Clusters the summands that can be clustered with each other into one summand.
  template<typename Summand>
  std::vector<Summand> cluster_summands(const std::vector<Summand>& summands, const data::variable_list& parameters)
  {
    constexpr bool is_action_summand = std::is_same_v<Summand, stochastic_action_summand>;

    std::vector<Summand> result;
    std::vector<Summand> remaining_summands = summands;
    for (const Summand& summand1: summands)
    {
      // The remaining summands that can be clustered with summand1.
      std::vector<Summand> cluster;
      std::vector<Summand> other_summands;
      for (const Summand& summand2: remaining_summands)
      {
        if (summands_can_be_clustered(summand1, summand2))
        {
          cluster.push_back(summand2);
        }
        else
        {
          other_summands.push_back(summand2);
        }
      }
      remaining_summands.swap(other_summands);

      if (cluster.size() > 1)
      {
        result.push_back(make_clustered_summand(cluster, parameters));
      }
      else if (cluster.size() == 1)
      {
        // A single action summand is put in front, a single deadlock summand at the end.
        result.insert(is_action_summand ? result.begin() : result.end(), cluster.front());
      }
    }
    assert(remaining_summands.empty());
    return result;
  }

public:
  /// \brief Constructor.
  /// \param data The data specification of the linear process.
  /// \param enumerated_sorts The generator of enumerated sorts and case functions. It adds them to its data
  ///        specification, which is normally data.
  /// \param identifier_generator The generator for fresh names. It must contain the identifiers of the linear process
  ///        and its data specification.
  /// \param global_variables The global variables of the linear process.
  /// \param options The options for clustering.
  cluster_algorithm(const data::data_specification& data,
    data::enumerated_sort_generator& enumerated_sorts,
    data::set_identifier_generator& identifier_generator,
    const std::set<data::variable>& global_variables,
    const cluster_options& options)
    : m_data(data),
      m_enumerated_sorts(enumerated_sorts),
      m_identifier_generator(identifier_generator),
      m_global_variables(global_variables),
      m_options(options)
  {}

  /// \brief Clusters the action summands and the deadlock summands of a linear process with the given parameters.
  void run(stochastic_action_summand_vector& action_summands,
    deadlock_summand_vector& deadlock_summands,
    const data::variable_list& parameters)
  {
    action_summands = cluster_summands(action_summands, parameters);
    deadlock_summands = cluster_summands(deadlock_summands, parameters);
  }
};

/// \brief Clusters the summands of the linear process of spec with the same action labels into one summand. The
///        enumerated sorts and case functions are added to the data specification of spec.
inline void cluster(stochastic_specification& spec, const cluster_options& options = cluster_options())
{
  data::set_identifier_generator identifier_generator;
  identifier_generator.add_identifiers(lps::find_identifiers(spec));
  // find_identifiers does not find the identifiers in the data specification.
  const data::data_specification& data = spec.data();
  identifier_generator.add_identifiers(data::find_identifiers(data.equations()));
  identifier_generator.add_identifiers(data::find_identifiers(data.user_defined_aliases()));
  identifier_generator.add_identifiers(data::find_identifiers(data.sorts()));
  identifier_generator.add_identifiers(data::find_identifiers(data.constructors()));
  identifier_generator.add_identifiers(data::find_identifiers(data.mappings()));

  data::enumerated_sort_generator enumerated_sorts(spec.data(), identifier_generator);
  cluster_algorithm algorithm(spec.data(), enumerated_sorts, identifier_generator, spec.global_variables(), options);
  algorithm.run(spec.process().action_summands(),
    spec.process().deadlock_summands(),
    spec.process().process_parameters());
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_CLUSTER_H
