// Author(s): Wieger Wesselink and Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/rename.h
/// \brief Algorithms for renaming parameters in lps data structures.

#ifndef MCRL2_LPS_RENAME_H
#define MCRL2_LPS_RENAME_H

#include <set>
#include <vector>
#include <utility>
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/postfix_identifier_generator.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/lps/detail/linear_process_utility.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"

namespace mcrl2 {

namespace lps {

/// \cond INTERNAL_DOCS
/// Generates a renaming of process parameters of the process p. The parameters are
/// renamed according to the given identifier generator, and well typedness constraints
/// are taken into account. The result consists of two vectors (src, dest) that specify
/// the renaming src[i] := dest[i].
template <typename IdentifierGenerator>
std::pair<std::vector<data::data_variable>, std::vector<data::data_variable> >
rename_process_parameters_helper(const linear_process& p, IdentifierGenerator& generator)
{
  std::set<core::identifier_string> forbidden_names = data::detail::set_union(
    detail::free_variable_names(p),
    detail::summand_variable_names(p)
  );
  
  std::vector<data::data_variable> src;  // contains the variables that need to be renamed
  std::vector<data::data_variable> dest; // contains the corresponding replacements
  generator.add_identifiers(forbidden_names);

  for (data::data_variable_list::iterator i = p.process_parameters().begin(); i != p.process_parameters().end(); ++i)
  {
    core::identifier_string new_name = generator(i->name());
    if (new_name != i->name())
    {
      // save the old and new value in the src and dest arrays
      src.push_back(*i);
      dest.push_back(data::data_variable(new_name, i->sort()));
    }
  }
  return std::make_pair(src, dest);
}
/// \endcond

/// \brief Renames the process parameters in the process p using the given identifier generator.
/// \param p A linear process
/// \param generator A generator for fresh identifiers
/// \return The rename result
template <typename IdentifierGenerator>
linear_process rename_process_parameters(const linear_process& p, IdentifierGenerator& generator)
{
  std::pair<std::vector<data::data_variable>, std::vector<data::data_variable> > r = rename_process_parameters_helper(p, generator);
  return atermpp::partial_replace(p, lps::detail::make_data_variable_replacer(r.first, r.second));
}

/// \brief Renames the process parameters in the process p, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
/// \param p A linear process
/// \param forbidden_names A set of names
/// \param postfix A string
/// \return The rename result
inline
linear_process rename_process_parameters(const linear_process& p, const std::set<core::identifier_string>& forbidden_names, const std::string postfix)
{
  data::postfix_identifier_generator generator(postfix);
  generator.add_identifiers(forbidden_names);
  return rename_process_parameters(p, generator);
}

/// \brief Renames the process parameters in the specification spec using the given identifier generator.
/// \param spec A linear process specification
/// \param generator A generator for fresh identifiers
/// \return The rename result
template <typename IdentifierGenerator>
specification rename_process_parameters(const specification& spec, IdentifierGenerator& generator)
{
  std::pair<std::vector<data::data_variable>, std::vector<data::data_variable> > r = rename_process_parameters_helper(spec.process(), generator);

  linear_process new_process              = atermpp::partial_replace(spec.process()        , lps::detail::make_data_variable_replacer(r.first, r.second));
  process_initializer new_initial_process = atermpp::partial_replace(spec.initial_process(), lps::detail::make_data_variable_replacer(r.first, r.second));
    
  specification result = spec;
  result = set_lps(result, new_process);
  result = set_initial_process(result, new_initial_process);
  return result;
}

/// \brief Renames the process parameters in the specification spec, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
/// \param spec A linear process specification
/// \param forbidden_names A set of names
/// \param postfix A string
/// \return The rename result
inline
specification rename_process_parameters(const specification& spec, const std::set<core::identifier_string>& forbidden_names, const std::string postfix)
{
  data::postfix_identifier_generator generator(postfix);
  generator.add_identifiers(forbidden_names);
  return rename_process_parameters(spec, generator);
}

/// \brief Renames the free variables in the process p using the given identifier generator.
/// \param p A linear process
/// \param generator A generator for fresh identifiers
/// \return The rename result
template <typename IdentifierGenerator>
linear_process rename_free_variables(const linear_process& p, IdentifierGenerator& generator)
{
  std::set<core::identifier_string> forbidden_names = data::detail::set_union(
    detail::process_parameter_names(p),
    detail::summand_variable_names(p)
  );

  std::vector<data::data_variable> src;  // contains the variables that need to be renamed
  std::vector<data::data_variable> dest; // contains the corresponding replacements
  generator.add_identifiers(forbidden_names);

  for (data::data_variable_list::iterator i = p.free_variables().begin(); i != p.free_variables().end(); ++i)
  {
    core::identifier_string new_name = generator(i->name());
    if (new_name != i->name())
    {
      // save the old and new value in the src and dest arrays
      src.push_back(*i);
      dest.push_back(data::data_variable(new_name, i->sort()));
    }
  }
  return atermpp::partial_replace(p, lps::detail::make_data_variable_replacer(src, dest));
}

/// \brief Renames the free variables in the process p, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
/// \param p A linear process
/// \param forbidden_names A set of names
/// \param postfix A string
/// \return The rename result
inline
linear_process rename_free_variables(const linear_process& p, const std::set<core::identifier_string>& forbidden_names, const std::string& postfix)
{
  data::postfix_identifier_generator generator(postfix);
  generator.add_identifiers(forbidden_names);
  return rename_free_variables(p, generator);
}

/// \brief Renames the summation variables in the process p using the given identifier generator.
/// \param p A linear process
/// \param generator A generator for fresh identifiers
/// \return The rename result
template <typename IdentifierGenerator>
linear_process rename_summation_variables(const linear_process& p, IdentifierGenerator& generator)
{
  atermpp::vector<summand> new_summands;

  std::set<core::identifier_string> forbidden_names = data::detail::set_union(
    lps::detail::process_parameter_names(p),
    lps::detail::free_variable_names(p)
  );
  generator.add_identifiers(forbidden_names);

  for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
  {
    std::vector<data::data_variable> src;  // contains the variables that need to be renamed
    std::vector<data::data_variable> dest; // contains the corresponding replacements

    for (data::data_variable_list::iterator j = i->summation_variables().begin(); j != i->summation_variables().end(); ++j)
    {
      core::identifier_string new_name = generator(j->name());
      if (new_name != j->name())
      {
        // save the old and new value in the src and dest arrays
        src.push_back(*j);
        dest.push_back(data::data_variable(new_name, j->sort()));
      }
    }
    new_summands.push_back(atermpp::partial_replace(*i, lps::detail::make_data_variable_replacer(src, dest)));
  }
  
  return set_summands(p, summand_list(new_summands.begin(), new_summands.end()));
}

/// \brief Renames the summation variables in the process p, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
/// \param p A linear process
/// \param forbidden_names A set of names
/// \param postfix A string
/// \return The rename result
inline
linear_process rename_summation_variables(const linear_process& p, const std::set<core::identifier_string>& forbidden_names, const std::string& postfix)
{
  data::postfix_identifier_generator generator(postfix);
  generator.add_identifiers(forbidden_names);
  return rename_summation_variables(p, generator);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_RENAME_H
