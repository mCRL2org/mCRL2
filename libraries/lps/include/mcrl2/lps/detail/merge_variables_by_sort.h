// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/merge_variables_by_sort.h
/// \brief Merges the variables of summands into one list, joining variables of the same sort, as used for
///        clustering summands and for the ultimate delay.

#ifndef MCRL2_LPS_DETAIL_MERGE_VARIABLES_BY_SORT_H
#define MCRL2_LPS_DETAIL_MERGE_VARIABLES_BY_SORT_H

#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/substitutions/maintain_variables_in_rhs.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

#include <algorithm>

namespace mcrl2::lps::detail
{

/// \brief A renaming of variables, that can be applied capture avoiding.
using variable_renaming = data::maintain_variables_in_rhs<data::mutable_map_substitution<>>;

/// \brief Joins v to the first variable of the same sort in unmatched_variables, which is removed from
///        unmatched_variables.
/// \details If v is joined to a different variable w, v is renamed into w. If unmatched_variables contains no variable
///          of the sort of v, v is a new variable. If its name occurs in merged_variables or in avoid, it is renamed to
///          a fresh variable.
/// \param fresh_variable Returns a fresh variable with the name hint and sort of its argument.
/// \return The variable that v is joined to, or that v is renamed to if it is new, or v itself; and true if v is
///         joined to a variable of unmatched_variables.
template<typename FreshVariableGenerator>
std::pair<data::variable, bool> merge_variable_by_sort(const data::variable& v,
  const data::variable_list& merged_variables,
  data::variable_list& unmatched_variables,
  const data::variable_list& avoid,
  FreshVariableGenerator& fresh_variable)
{
  const data::variable_list::const_iterator match = std::find_if(unmatched_variables.begin(),
    unmatched_variables.end(),
    [&v](const data::variable& w) { return w.sort() == v.sort(); });
  if (match != unmatched_variables.end())
  {
    const data::variable result = *match;
    data::variable_vector remaining_variables(unmatched_variables.begin(), match);
    remaining_variables.insert(remaining_variables.end(), std::next(match), unmatched_variables.end());
    unmatched_variables = data::variable_list(remaining_variables.begin(), remaining_variables.end());
    return {result, true};
  }

  const auto has_name_of_v = [&v](const data::variable& w)
  {
    return w.name() == v.name();
  };
  if (std::any_of(merged_variables.begin(), merged_variables.end(), has_name_of_v)
      || std::any_of(avoid.begin(), avoid.end(), has_name_of_v))
  {
    return {fresh_variable(v), false};
  }
  return {v, false};
}

/// \brief Adds the condition that v equals a default value of its sort to each of the conditions.
/// \details If the representative generator cannot find a default value, the conditions are not changed, and v is
///          unconstrained. As there is no concrete value of its sort, this is unlikely to affect state space
///          generation.
/// \param representative Returns a default value of a sort, or throws a mcrl2::runtime_error.
template<typename RepresentativeGenerator>
data::data_expression_list add_default_value_condition(const data::variable& v,
  const data::data_expression_list& conditions,
  RepresentativeGenerator& representative)
{
  try
  {
    const data::data_expression condition = data::equal_to(v, representative(v.sort()));
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

/// \brief Merges the variables of a summand into the variables merged from the previous summands, joining variables
///        of the same sort.
/// \details A variable of summand_variables is joined to a variable of merged_variables of the same sort that no other
///          variable of summand_variables is joined to. The other variables of summand_variables are added to
///          merged_variables, and renamed to fresh variables if their names occur in merged_variables or in avoid.
///
///          There is a condition per summand, which fixes the merged variables that do not occur in that summand to
///          a default value. The conditions of the previous summands are extended with the condition that the added
///          variables equal a default value, and the condition for this summand, that the variables of
///          merged_variables that no variable is joined to equal a default value, is put in front.
///
///          If merged_variables is empty, summand_variables are merged in reverse order, so that the variables are
///          merged in the same order for all summands (due to a suggestion of Muck van Weerdenburg).
/// \param summand_variables The variables of the summand.
/// \param merged_variables The variables merged from the previous summands, to which the new variables are added.
/// \param conditions The conditions of the previous summands, in reverse order of the summands.
/// \param avoid Variables whose names may not be used for added variables.
/// \param fresh_variable Returns a fresh variable with the name hint and sort of its argument.
/// \param representative Returns a default value of a sort, or throws a mcrl2::runtime_error.
/// \return The renaming of summand_variables into merged_variables.
template<typename FreshVariableGenerator, typename RepresentativeGenerator>
variable_renaming merge_variables_by_sort(const data::variable_list& summand_variables,
  data::variable_list& merged_variables,
  data::data_expression_list& conditions,
  const data::variable_list& avoid,
  FreshVariableGenerator& fresh_variable,
  RepresentativeGenerator& representative)
{
  variable_renaming renaming;
  data::variable_list unmatched_variables = merged_variables;
  data::variable_list result = merged_variables;
  for (const data::variable& v: merged_variables.empty() ? reverse(summand_variables) : summand_variables)
  {
    const std::pair<data::variable, bool> merged
      = merge_variable_by_sort(v, merged_variables, unmatched_variables, avoid, fresh_variable);
    if (merged.first != v)
    {
      renaming[v] = merged.first;
    }
    if (!merged.second)
    {
      result.push_front(merged.first);
      conditions = add_default_value_condition(merged.first, conditions, representative);
    }
  }
  conditions.push_front(default_values_condition(unmatched_variables, representative));
  merged_variables = result;
  return renaming;
}

} // namespace mcrl2::lps::detail

#endif // MCRL2_LPS_DETAIL_MERGE_VARIABLES_BY_SORT_H
