// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/resolve_name_clashes.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_RESOLVE_NAME_CLASHES_H
#define MCRL2_LPS_RESOLVE_NAME_CLASHES_H

#include "mcrl2/lps/replace.h"

namespace mcrl2::lps
{

namespace detail {

// returns the names of the variables in v
template <typename VariableContainer>
std::set<core::identifier_string> variable_names(const VariableContainer& vars)
{
  std::set<core::identifier_string> result;
  for (const data::variable& v: vars)
  {
    result.insert(v.name());
  }
  return result;
}

// returns the names of variables in v that are also in w
template <typename VariableContainer>
std::set<core::identifier_string> variable_name_clashes(const VariableContainer& vars, const std::set<core::identifier_string>& w)
{
  std::set<core::identifier_string> result;
  for (const data::variable& v: vars)
  {
    if (w.find(v.name()) != w.end())
    {
      result.insert(v.name());
    }
  }
  return result;
}

inline
void resolve_summand_variable_name_clashes(action_summand& summand, 
                                           const std::set<core::identifier_string>& process_parameter_names, 
                                           data::set_identifier_generator& generator)
{
  const data::variable_list& summation_variables = summand.summation_variables();
  std::set<core::identifier_string> names = variable_name_clashes(summation_variables, process_parameter_names);
  if (!names.empty())
  {
    data::mutable_map_substitution<> sigma;
    for (const data::variable& v: summation_variables)
    {
      if (process_parameter_names.find(v.name()) != process_parameter_names.end())
      {
        sigma[v] = data::variable(generator(v.name()), v.sort());
      }
    }
    if (!sigma.empty())
    {
      summand.summation_variables()=data::replace_variables(summand.summation_variables(),sigma);
      lps::replace_variables(summand, sigma);
    }
  }
}

inline
void resolve_summand_variable_name_clashes(deadlock_summand& summand, 
                                           const std::set<core::identifier_string>& process_parameter_names, 
                                           data::set_identifier_generator& generator)
{
  const data::variable_list& summation_variables = summand.summation_variables();
  std::set<core::identifier_string> names = variable_name_clashes(summation_variables, process_parameter_names);
  if (!names.empty())
  {
    data::mutable_map_substitution<> sigma;
    for (const data::variable& v: summation_variables)
    {
      if (process_parameter_names.find(v.name()) != process_parameter_names.end())
      {
        sigma[v] = data::variable(generator(v.name()), v.sort());
      }
    }
    lps::replace_all_variables(summand, sigma);
  }
}

inline
void resolve_summand_variable_name_clashes(stochastic_action_summand& summand, 
                                           const std::set<core::identifier_string>& process_parameter_names, 
                                           data::set_identifier_generator& generator)
{
  data::mutable_map_substitution<> sigma;
  std::set<core::identifier_string> summation_names;

  // handle the summation variables
  for (const data::variable& v: summand.summation_variables())
  {
    if (process_parameter_names.find(v.name()) != process_parameter_names.end())
    {
      sigma[v] = data::variable(generator(v.name()), v.sort());
    }
    summation_names.insert(v.name());
  }
  if (!sigma.empty())
  {
    lps::replace_variables(summand, sigma);
    summand.summation_variables()=lps::replace_variables(summand.summation_variables(),sigma);
  }

  // handle the distribution variables
  sigma.clear();

  for (const data::variable& v: summand.distribution().variables())
  {
    if (process_parameter_names.find(v.name()) != process_parameter_names.end() ||
        summation_names.find(v.name()) != summation_names.end())  // Check stochastic variables also with respect to the summand variables. 
    {
      sigma[v] = data::variable(generator(v.name()), v.sort());
    }
  }
  if (!sigma.empty())
  {
    summand.distribution() = lps::replace_all_variables(summand.distribution(), sigma);
    summand.assignments() = lps::replace_variables(summand.assignments(), sigma);
  }
}

} // namespace detail

/// \brief Renames summand variables such that there are no name clashes between summand variables and process parameters
template <typename Specification>
void resolve_summand_variable_name_clashes(Specification& spec)
{
  typename Specification::process_type& proc = spec.process();
  std::set<core::identifier_string> process_parameter_names = detail::variable_names(proc.process_parameters());

  data::set_identifier_generator generator;
  generator.add_identifiers(lps::find_identifiers(spec));
  generator.add_identifiers(data::function_and_mapping_identifiers(spec.data()));

  for (typename Specification::process_type::action_summand_type& s: proc.action_summands())
  {
    detail::resolve_summand_variable_name_clashes(s, process_parameter_names, generator);
  }

  for (deadlock_summand& s: proc.deadlock_summands())
  {
    detail::resolve_summand_variable_name_clashes(s, process_parameter_names, generator);
  }
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_RESOLVE_NAME_CLASHES_H
