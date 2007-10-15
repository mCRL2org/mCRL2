// Author(s): Wieger Wesselink and Jan Friso Groote
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/rename.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_RENAME_H
#define MCRL2_LPS_RENAME_H

#include <set>
#include <vector>
#include <boost/iterator/transform_iterator.hpp>
#include "atermpp/make_list.h"
#include "atermpp/algorithm.h"
#include "atermpp/vector.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/postfix_identifier_generator.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/lps/detail/linear_process_utility.h"
#include "mcrl2/lps/detail/sorted_sequence_algorithm.h"

namespace lps {

/// Renames the process parameters in the process p using the given identifier generator.
template <typename IdentifierGenerator>
linear_process rename_process_parameters(const linear_process& p, IdentifierGenerator& generator)
{
  std::set<identifier_string> forbidden_names = detail::set_union(
    detail::free_variable_names(p),
    detail::summand_variable_names(p)
  );
  
  std::vector<data_variable> src;  // contains the variables that need to be renamed
  std::vector<data_variable> dest; // contains the corresponding replacements
  generator.add_identifiers(forbidden_names);

  for (data_variable_list::iterator i = p.process_parameters().begin(); i != p.process_parameters().end(); ++i)
  {
    identifier_string new_name = generator(i->name());
    if (new_name != i->name())
    {
      // save the old and new value in the src and dest arrays
      src.push_back(*i);
      dest.push_back(data_variable(new_name, i->sort()));
    }
  }
  return atermpp::partial_replace(p, detail::make_data_variable_replacer(src, dest));
}

/// Renames the process parameters in the process p, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
inline
linear_process rename_process_parameters(const linear_process& p, const std::set<identifier_string>& forbidden_names, const std::string& postfix = "_S")
{
  lps::postfix_identifier_generator generator(postfix);
  generator.add_identifiers(forbidden_names);
  return rename_process_parameters(p, generator);
}

/// Renames the free variables in the process p using the given identifier generator.
template <typename IdentifierGenerator>
linear_process rename_free_variables(const linear_process& p, IdentifierGenerator& generator)
{
  std::set<identifier_string> forbidden_names = detail::set_union(
    detail::process_parameter_names(p),
    detail::summand_variable_names(p)
  );

  std::vector<data_variable> src;  // contains the variables that need to be renamed
  std::vector<data_variable> dest; // contains the corresponding replacements
  generator.add_identifiers(forbidden_names);

  for (data_variable_list::iterator i = p.process_parameters().begin(); i != p.process_parameters().end(); ++i)
  {
    identifier_string new_name = generator(i->name());
    if (new_name != i->name())
    {
      // save the old and new value in the src and dest arrays
      src.push_back(*i);
      dest.push_back(data_variable(new_name, i->sort()));
    }
  }
  return atermpp::partial_replace(p, detail::make_data_variable_replacer(src, dest));
}

/// Renames the free variables in the process p, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
inline
linear_process rename_free_variables(const linear_process& p, const std::set<identifier_string>& forbidden_names, const std::string& postfix = "_S")
{
  lps::postfix_identifier_generator generator(postfix);
  generator.add_identifiers(forbidden_names);
  return rename_free_variables(p, generator);
}

/// Renames the summation variables in the process p using the given identifier generator.
template <typename IdentifierGenerator>
linear_process rename_summation_variables(const linear_process& p, IdentifierGenerator& generator)
{
  atermpp::vector<summand> new_summands;

  std::set<identifier_string> forbidden_names = detail::set_union(
    detail::process_parameter_names(p),
    detail::free_variable_names(p)
  );
  generator.add_identifiers(forbidden_names);

  for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
  {
    std::vector<data_variable> src;  // contains the variables that need to be renamed
    std::vector<data_variable> dest; // contains the corresponding replacements

    for (data_variable_list::iterator j = i->summation_variables().begin(); j != i->summation_variables().end(); ++j)
    {
      identifier_string new_name = generator(j->name());
      if (new_name != j->name())
      {
        // save the old and new value in the src and dest arrays
        src.push_back(*j);
        dest.push_back(data_variable(new_name, j->sort()));
      }
    }
    new_summands.push_back(atermpp::partial_replace(*i, detail::make_data_variable_replacer(src, dest)));
  }
  
  return set_summands(p, summand_list(new_summands.begin(), new_summands.end()));
}

/// Renames the summation variables in the process p, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
inline
linear_process rename_summation_variables(const linear_process& p, const std::set<identifier_string>& forbidden_names, const std::string& postfix = "_S")
{
  lps::postfix_identifier_generator generator(postfix);
  generator.add_identifiers(forbidden_names);
  return rename_summation_variables(p, generator);
}

} // namespace lps

#endif // MCRL2_LPS_RENAME_H
