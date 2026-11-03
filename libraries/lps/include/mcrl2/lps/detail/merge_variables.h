// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/merge_variables.h
/// \brief Merges lists of variables of summands, as used for clustering summands and for the ultimate delay.

#ifndef MCRL2_LPS_DETAIL_MERGE_VARIABLES_H
#define MCRL2_LPS_DETAIL_MERGE_VARIABLES_H

#include "mcrl2/data/standard_utility.h"

#include <algorithm>

namespace mcrl2::lps::detail
{

/// \brief Joins var to the first variable of the same sort in matching_list, which is removed from matching_list.
/// \details If var is joined to a different variable w, the renaming of var into w is added to pars and args. If
///          matching_list contains no variable of the sort of var, var is a new variable. If its name occurs in
///          variables or in avoid, it is renamed to a fresh variable, and this renaming is added to pars and args.
/// \param fresh_variable Returns a fresh variable with the name hint and sort of its argument.
/// \return True if var is joined to a variable in matching_list, and false if it is new.
template<typename FreshVariableGenerator>
bool merge_variable(data::variable& var,
  const data::variable_list& variables,
  data::variable_list& matching_list,
  data::variable_list& pars,
  data::data_expression_list& args,
  const data::variable_list& avoid,
  FreshVariableGenerator& fresh_variable)
{
  const data::variable_list::const_iterator match = std::find_if(matching_list.begin(),
    matching_list.end(),
    [&var](const data::variable& w) { return w.sort() == var.sort(); });
  if (match != matching_list.end())
  {
    if (var != *match)
    {
      pars.push_front(var);
      args.push_front(*match);
    }
    data::variable_vector remaining_variables(matching_list.begin(), match);
    remaining_variables.insert(remaining_variables.end(), std::next(match), matching_list.end());
    matching_list = data::variable_list(remaining_variables.begin(), remaining_variables.end());
    return true;
  }

  const auto has_name_of_var = [&var](const data::variable& w)
  {
    return w.name() == var.name();
  };
  if (std::any_of(variables.begin(), variables.end(), has_name_of_var)
      || std::any_of(avoid.begin(), avoid.end(), has_name_of_var))
  {
    pars.push_front(var);
    var = fresh_variable(var);
    args.push_front(var);
  }
  return false;
}

/// \brief Adds the condition that var equals a default value of its sort to each of the conditions.
/// \details If the representative generator cannot find a default value, the conditions are not changed, and var is
///          unconstrained. As there is no concrete value of its sort, this is unlikely to affect state space
///          generation.
/// \param representative Returns a default value of a sort, or throws a mcrl2::runtime_error.
template<typename RepresentativeGenerator>
data::data_expression_list extend_conditions(const data::variable& var,
  const data::data_expression_list& conditions,
  RepresentativeGenerator& representative)
{
  try
  {
    const data::data_expression condition = data::equal_to(var, representative(var.sort()));
    return data::data_expression_list(conditions.begin(),
      conditions.end(),
      [&condition](const data::data_expression& c) { return data::lazy::and_(condition, c); });
  }
  catch (mcrl2::runtime_error&)
  {
    return conditions;
  }
}

/// \brief The condition that the variables equal default values of their sorts. Variables of sorts for which the
///        representative generator cannot find a default value are unconstrained.
template<typename RepresentativeGenerator>
data::data_expression default_values_condition(const data::variable_list& variables,
  RepresentativeGenerator& representative)
{
  data::data_expression result = data::sort_bool::true_();
  for (const data::variable& v: variables)
  {
    try
    {
      result = data::lazy::and_(result, data::equal_to(v, representative(v.sort())));
    }
    catch (mcrl2::runtime_error&) // NOLINT(bugprone-empty-catch)
    {
      // No default value of sort v.sort() could be found. No condition is added.
    }
  }
  return result;
}

/// \brief Merges the variables v1 of a summand into the variables v2 merged from the previous summands.
/// \details A variable of v1 is joined to a variable of v2 of the same sort that no other variable of v1 is joined
///          to. Other variables of v1 are added to the result, and renamed if their names occur in v2 or in avoid.
///          The renaming of the variables of v1 is added to renamings_pars and renamings_args. The conditions per
///          previous summand are extended with the condition that the added variables equal a default value, and the
///          condition for this summand, that the variables of v2 that no variable is joined to equal a default value,
///          is put in front.
///
///          If v2 is empty, v1 is merged in reverse order, so that the variables are merged in the same order for all
///          summands (due to a suggestion of Muck van Weerdenburg).
/// \param fresh_variable Returns a fresh variable with the name hint and sort of its argument.
/// \param representative Returns a default value of a sort, or throws a mcrl2::runtime_error.
/// \return The merged variables.
template<typename FreshVariableGenerator, typename RepresentativeGenerator>
data::variable_list merge_variables(const data::variable_list& v1,
  const data::variable_list& v2,
  std::vector<data::variable_list>& renamings_pars,
  std::vector<data::data_expression_list>& renamings_args,
  data::data_expression_list& conditions,
  const data::variable_list& avoid,
  FreshVariableGenerator& fresh_variable,
  RepresentativeGenerator& representative)
{
  data::data_expression_list renaming_args;
  data::variable_list renaming_pars;
  data::variable_list matching_list = v2;

  data::variable_list result = v2;
  for (const data::variable& v: v2.empty() ? reverse(v1) : v1)
  {
    data::variable var = v;
    if (!merge_variable(var, v2, matching_list, renaming_pars, renaming_args, avoid, fresh_variable))
    {
      result.push_front(var);
      conditions = extend_conditions(var, conditions, representative);
    }
  }
  conditions.push_front(default_values_condition(matching_list, representative));
  renamings_pars.push_back(renaming_pars);
  renamings_args.push_back(renaming_args);
  return result;
}

} // namespace mcrl2::lps::detail

#endif // MCRL2_LPS_DETAIL_MERGE_VARIABLES_H
