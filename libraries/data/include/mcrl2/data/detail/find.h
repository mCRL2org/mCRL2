// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/find.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_FIND_H
#define MCRL2_DATA_DETAIL_FIND_H

#include "mcrl2/data/find.h"

namespace mcrl2 {

namespace data {

namespace detail {

/// \brief Returns all names of data variables that occur in the term t
/// \param t A term
/// \return RETURN_DESCRIPTION
template <typename Term>
std::set<core::identifier_string> find_variable_names(Term t)
{
  // find all data variables in t
  std::set<data_variable> variables;
  atermpp::find_all_if(t, is_data_variable, std::inserter(variables, variables.end()));

  std::set<core::identifier_string> result;
  for (std::set<data_variable>::iterator j = variables.begin(); j != variables.end(); ++j)
  {
    result.insert(j->name());
  }
  return result;
}

/// \brief Returns all names of data variables that occur in the term t
/// \param t A term
/// \return RETURN_DESCRIPTION
template <typename Term>
std::set<std::string> find_variable_name_strings(Term t)
{
  // find all data variables in t
  std::set<data_variable> variables;
  atermpp::find_all_if(t, is_data_variable, std::inserter(variables, variables.end()));

  std::set<std::string> result;
  for (std::set<data_variable>::iterator j = variables.begin(); j != variables.end(); ++j)
  {
    result.insert(j->name());
  }
  return result;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_FIND_H
