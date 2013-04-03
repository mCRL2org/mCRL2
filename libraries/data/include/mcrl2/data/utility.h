// Author(s): Jeroen Keiren, Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/utility.h
/// \brief Provides utilities for working with lists.

#ifndef MCRL2_DATA_UTILITY_H
#define MCRL2_DATA_UTILITY_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <utility>

#include "mcrl2/data/assignment.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2
{

namespace data
{

/// \brief Generates fresh variables with names that do not appear in the given context.
/// Caveat: the implementation is very inefficient.
/// \param update_context If true, then generated names are added to the context
inline
variable_list fresh_variables(const variable_list& variables, std::set<std::string>& context, bool update_context = true)
{
  variable_vector result;
  for (variable_list::const_iterator i = variables.begin(); i != variables.end(); ++i)
  {
    utilities::number_postfix_generator generator(std::string(i->name()));
    std::string name;
    do
    {
      name = generator();
    }
    while (context.find(name) != context.end());
    if (update_context)
    {
      context.insert(name);
    }
    result.push_back(variable(name, i->sort()));
  }
  return variable_list(result.begin(),result.end());
}

/// \brief Returns a variable that doesn't appear in context
/// \param context A term
/// \param s A sort expression
/// \param hint A string
/// \return A variable that doesn't appear in context
/// \warning reorganising the identifier context is expensive, consider using an identifier generator
inline
variable fresh_variable(const std::set<core::identifier_string>& ids, sort_expression const& s, std::string const& hint)
{
  set_identifier_generator generator;
  generator.add_identifiers(ids);
  return variable(generator(hint), s);
}

} // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H

