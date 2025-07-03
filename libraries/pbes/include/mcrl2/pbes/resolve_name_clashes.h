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



namespace mcrl2::pbes_system {

namespace detail {

template<bool allow_ce>
inline
void resolve_summand_variable_name_clashes(pre_srf_summand<allow_ce>& summand, const std::set<core::identifier_string>& process_parameter_names, data::set_identifier_generator& generator)
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

    if constexpr (allow_ce)
    {
      // Replace on pbes_expressions
      summand.condition() = pbes_system::replace_all_variables(summand.condition(), sigma);
    }
    else
    {
      // Replace on data expressions.
      summand.condition() = lps::replace_all_variables(summand.condition(), sigma);
    }
    
    summand.variable() = pbes_system::replace_all_variables(summand.variable(), sigma);
  }
}

} // namespace detail

/// \brief Renames summand variables such that there are no name clashes between summand variables and process parameters
template<bool allow_ce>
inline
void resolve_summand_variable_name_clashes(detail::pre_srf_pbes<allow_ce>& pbesspec, const data::variable_list& process_parameters)
{
  std::set<core::identifier_string> process_parameter_names = lps::detail::variable_names(process_parameters);

  data::set_identifier_generator generator;

  // Here we cannot yet convert the full srf_pbes to pbes since it's not well typed, so instead find the identifiers separately.
  for (const detail::pre_srf_equation<allow_ce>& eq : pbesspec.equations())
  {
    generator.add_identifiers(pbes_system::find_identifiers(eq.to_pbes()));
  }
  generator.add_identifiers(pbes_system::find_identifiers(pbesspec.initial_state()));
  generator.add_identifiers(data::function_and_mapping_identifiers(pbesspec.data()));

  for (auto& equation: pbesspec.equations())
  {
    for (auto& summand: equation.summands())
    {
      detail::resolve_summand_variable_name_clashes(summand, process_parameter_names, generator);
    }
  }
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_RESOLVE_NAME_CLASHES_H
