// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/resolve_name_clashes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_RESOLVE_NAME_CLASHES_H
#define MCRL2_PBES_RESOLVE_NAME_CLASHES_H

#include "mcrl2/lps/resolve_name_clashes.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/srf_pbes.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
void resolve_summand_variable_name_clashes(srf_summand& summand, const std::set<core::identifier_string>& process_parameter_names, data::set_identifier_generator& generator)
{
  const data::variable_list& summation_variables = summand.parameters();
  std::set<core::identifier_string> names = lps::detail::variable_name_clashes(summation_variables, process_parameter_names);
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
    summand.parameters() = lps::replace_all_variables(summand.parameters(), sigma);
    summand.condition() = lps::replace_all_variables(summand.condition(), sigma);
    summand.variable() = pbes_system::replace_all_variables(summand.variable(), sigma);
  }
}

} // namespace detail

/// \brief Renames summand variables such that there are no name clashes between summand variables and process parameters
inline
void resolve_summand_variable_name_clashes(srf_pbes& pbesspec, const data::variable_list& process_parameters)
{
  std::set<core::identifier_string> process_parameter_names = lps::detail::variable_names(process_parameters);

  data::set_identifier_generator generator;
  generator.add_identifiers(pbes_system::find_identifiers(pbesspec.to_pbes()));
  generator.add_identifiers(data::function_and_mapping_identifiers(pbesspec.data()));

  for (auto& equation: pbesspec.equations())
  {
    for (auto& summand: equation.summands())
    {
      detail::resolve_summand_variable_name_clashes(summand, process_parameter_names, generator);
    }
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_RESOLVE_NAME_CLASHES_H
