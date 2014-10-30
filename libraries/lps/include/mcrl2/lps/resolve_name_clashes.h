// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/resolve_name_clashes.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_RESOLVE_NAME_CLASHES_H
#define MCRL2_LPS_RESOLVE_NAME_CLASHES_H

#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/replace.h"

namespace mcrl2 {

namespace lps {

namespace detail {

// returns the names of the variables in v
template <typename VariableContainer>
std::set<core::identifier_string> variable_names(const VariableContainer& v)
{
  std::set<core::identifier_string> result;
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    result.insert(i->name());
  }
  return result;
}

// returns the names of variables in v that are also in w
template <typename VariableContainer>
std::set<core::identifier_string> variable_name_clashes(const VariableContainer& v, const std::set<core::identifier_string>& w)
{
  std::set<core::identifier_string> result;
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    if (w.find(i->name()) != w.end())
    {
      result.insert(i->name());
    }
  }
  return result;
}

// resolves name clashes in an action_summand or deadlock_summand
template <typename Summand>
void resolve_summand_variable_name_clashes(Summand& summand, const std::set<core::identifier_string>& process_parameter_names)
{
  const data::variable_list& summation_variables = summand.summation_variables();
  std::set<core::identifier_string> v = variable_name_clashes(summation_variables, process_parameter_names);
  if (!v.empty())
  {
    data::mutable_map_substitution<> sigma;
    std::set<core::identifier_string> context = variable_names(lps::find_all_variables(summand));
    data::set_identifier_generator generator;
    generator.add_identifiers(context);
    for (auto i = summation_variables.begin(); i != summation_variables.end(); ++i)
    {
      if (process_parameter_names.find(i->name()) != process_parameter_names.end())
      {
        sigma[*i] = data::variable(generator(i->name()), i->sort());
      }
    }
    lps::replace_all_variables(summand, sigma);
  }
}

} // namespace detail

/// \brief Renames summand variables such that there are no name clashes between summand variables and process parameters
template <typename Specification>
void resolve_summand_variable_name_clashes(Specification& spec)
{
  auto& proc = spec.process();
  std::set<core::identifier_string> process_parameter_names = detail::variable_names(proc.process_parameters());

  auto& action_summands = proc.action_summands();
  for (auto i = action_summands.begin(); i != action_summands.end(); ++i)
  {
    detail::resolve_summand_variable_name_clashes(*i, process_parameter_names);
  }

  deadlock_summand_vector& deadlock_summands = proc.deadlock_summands();
  for (auto i = deadlock_summands.begin(); i != deadlock_summands.end(); ++i)
  {
    detail::resolve_summand_variable_name_clashes(*i, process_parameter_names);
  }
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_RESOLVE_NAME_CLASHES_H
