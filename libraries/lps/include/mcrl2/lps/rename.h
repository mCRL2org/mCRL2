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
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/detail/algorithm.h"

namespace lps {

/// Renames the process parameters in the process p, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
linear_process rename_process_parameters(const linear_process& p, const std::set<identifier_string>& forbidden_names, const std::string& postfix = "_S")
{
  std::set<identifier_string> used_names = forbidden_names;
    
  // add the free variables and summation variables of p to the used names
  used_names.insert(boost::make_transform_iterator(p.free_variables().begin(), detail::data_variable_name()),
                    boost::make_transform_iterator(p.free_variables().begin(), detail::data_variable_name()));
  for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
  {
    used_names.insert(
        boost::make_transform_iterator(i->summation_variables().begin(), detail::data_variable_name()),
        boost::make_transform_iterator(i->summation_variables().end(),   detail::data_variable_name())
       );
  }

  std::vector<data_variable> src;  // contains the variables that need to be renamed
  std::vector<data_variable> dest; // contains the corresponding replacements
  lps::set_identifier_generator generator;
  generator.add_identifiers(used_names);

  for (data_variable_list::iterator i = p.process_parameters().begin(); i != p.process_parameters().end(); ++i)
  {
    if (used_names.find(i->name()) != used_names.end()) // name clash!
    {
      // save the old and new value in the src and dest arrays
      src.push_back(*i);
      std::string name = generator(std::string(i->name()) + postfix);
      dest.push_back(data_variable(name, i->sort()));
    }
  }
  return atermpp::partial_replace(p, detail::make_data_variable_replacer(src, dest));
}

/// Renames the free variables in the process p, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
linear_process rename_free_variables(const linear_process& p, const std::set<identifier_string>& forbidden_names, const std::string& postfix = "_S")
{
  std::set<identifier_string> used_names = forbidden_names;
    
  // add the process parameters and summation variables of p to the forbidden names
  used_names.insert(boost::make_transform_iterator(p.process_parameters().begin(), detail::data_variable_name()),
                    boost::make_transform_iterator(p.process_parameters().begin(), detail::data_variable_name()));
  for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
  {
    used_names.insert(
        boost::make_transform_iterator(i->summation_variables().begin(), detail::data_variable_name()),
        boost::make_transform_iterator(i->summation_variables().end(),   detail::data_variable_name())
       );
  }

  std::vector<data_variable> src;  // contains the variables that need to be renamed
  std::vector<data_variable> dest; // contains the corresponding replacements
  lps::set_identifier_generator generator;
  generator.add_identifiers(used_names);

  for (data_variable_list::iterator i = p.process_parameters().begin(); i != p.process_parameters().end(); ++i)
  {
    if (used_names.find(i->name()) != used_names.end()) // name clash!
    {
      // save the old and new value in the src and dest arrays
      src.push_back(*i);
      std::string name = generator(std::string(i->name()) + postfix);
      dest.push_back(data_variable(name, i->sort()));
    }
  }
  return atermpp::partial_replace(p, detail::make_data_variable_replacer(src, dest));
}

/// Renames the summation variables in the process p, such that none of them
/// appears in forbidden_names. Postfix is used as a hint for the new name.
inline
linear_process rename_summation_variables(const linear_process& p, const std::set<identifier_string>& forbidden_names, const std::string& postfix = "_S")
{
  atermpp::vector<summand> new_summands;

  std::set<identifier_string> used_names = forbidden_names;
    
  // add the process parameters and free variables of p to the forbidden names
  used_names.insert(boost::make_transform_iterator(p.process_parameters().begin(), detail::data_variable_name()),
                    boost::make_transform_iterator(p.process_parameters().begin(), detail::data_variable_name()));
  used_names.insert(boost::make_transform_iterator(p.free_variables().begin(), detail::data_variable_name()),
                    boost::make_transform_iterator(p.free_variables().begin(), detail::data_variable_name()));

  for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
  {
    std::vector<data_variable> src;  // contains the variables that need to be renamed
    std::vector<data_variable> dest; // contains the corresponding replacements
    lps::set_identifier_generator generator;
    generator.add_identifiers(used_names);

    for (data_variable_list::iterator j = i->summation_variables().begin(); j != i->summation_variables().end(); ++j)
    {
      if (used_names.find(j->name()) != used_names.end()) // name clash!
      {
        // save the old and new value in the src and dest arrays
        src.push_back(*j);
        std::string name = generator(std::string(j->name()) + postfix);
        dest.push_back(data_variable(name, j->sort()));
      }
    }
    new_summands.push_back(atermpp::partial_replace(*i, detail::make_data_variable_name_replacer(src, dest)));
  }
  
  return set_summands(p, summand_list(new_summands.begin(), new_summands.end()));
}

} // namespace lps

#endif // MCRL2_LPS_RENAME_H
