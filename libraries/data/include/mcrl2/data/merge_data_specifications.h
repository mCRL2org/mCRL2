// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/merge_data_specifications.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_MERGE_DATA_SPECIFICATIONS_H
#define MCRL2_DATA_MERGE_DATA_SPECIFICATIONS_H

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/typecheck.h"

namespace mcrl2 {

namespace data {

/// \brief Merges two data specifications.  Throws an exception if conflicts are detected.
/// \detail If the data specifications have equal aliases, types, constructors or functions these are merged.
/// \param[in] dataspec1 The first data specification to be merged.
/// \param[out] dataspec2 The second data specification to be merged.
/// \return The merged data specification.
inline
data_specification merge_data_specifications(const data_specification& dataspec1, const data_specification& dataspec2)
{
  data_specification result = dataspec1;
 
  // Merge the sorts.
  for(const basic_sort& s: dataspec2.user_defined_sorts())
  {
    if (std::find(dataspec1.user_defined_sorts().begin(),dataspec1.user_defined_sorts().end(),s)==dataspec1.user_defined_sorts().end())
    {
      result.add_sort(s);
    }
  }

  // Merge the aliases.
  for(const alias& a: dataspec2.user_defined_aliases())
  {
    if (std::find(dataspec1.user_defined_aliases().begin(),dataspec1.user_defined_aliases().end(),a)==dataspec1.user_defined_aliases().end())
    {
      result.add_alias(a);
    }
  }

  // Merge the constructors.
  for(const function_symbol& f: dataspec2.user_defined_constructors())
  {
    if (std::find(dataspec1.user_defined_constructors().begin(),dataspec1.user_defined_constructors().end(),f)==
                                dataspec1.user_defined_constructors().end())
    {
      result.add_constructor(f);
    }
  }

  // Merge the mappings.
  for(const function_symbol& f: dataspec2.user_defined_mappings())
  {
    if (std::find(dataspec1.user_defined_mappings().begin(),dataspec1.user_defined_mappings().end(),f)==dataspec1.user_defined_mappings().end())
    {
      result.add_mapping(f);
    }
  }

  // Merge the data equations.
  for(const data_equation& e: dataspec2.user_defined_equations())
  {
    if (std::find(dataspec1.user_defined_sorts().begin(),dataspec1.user_defined_sorts().end(),e)==dataspec1.user_defined_sorts().end())
    {
      result.add_equation(e);
    }
  }

  data_type_checker type_checker(result);
  try
  {
    // Type check the resulting specification.
    // Throw an exception when there is an issue.
    type_checker();
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string("Merging of two data specifications fails.\n") + e.what());
  }

  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_MERGE_DATA_SPECIFICATIONS_H
