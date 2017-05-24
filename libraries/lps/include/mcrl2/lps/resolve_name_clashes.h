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
void resolve_summand_variable_name_clashes(Summand& summand, const std::set<core::identifier_string>& process_parameter_names, data::set_identifier_generator& generator)
{
  const data::variable_list& summation_variables = summand.summation_variables();
  std::set<core::identifier_string> v = variable_name_clashes(summation_variables, process_parameter_names);
  if (!v.empty())
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

} // namespace detail

/// \brief Renames summand variables such that there are no name clashes between summand variables and process parameters
template <typename Specification>
void resolve_summand_variable_name_clashes(Specification& spec)
{
  auto& proc = spec.process();
  std::set<core::identifier_string> process_parameter_names = detail::variable_names(proc.process_parameters());

  data::set_identifier_generator generator;
  generator.add_identifiers(lps::find_identifiers(spec));
  generator.add_identifiers(data::function_and_mapping_identifiers(spec.data()));

  for (action_summand& s: proc.action_summands())
  {
    detail::resolve_summand_variable_name_clashes(s, process_parameter_names, generator);
  }

  for (deadlock_summand& s: proc.deadlock_summands())
  {
    detail::resolve_summand_variable_name_clashes(s, process_parameter_names, generator);
  }
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_RESOLVE_NAME_CLASHES_H
