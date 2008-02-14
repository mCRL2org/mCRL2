// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/find.h
/// \brief Search functions of the data library.

#ifndef MCRL2_DATA_FIND_H
#define MCRL2_DATA_FIND_H

#include <set>
#include <iterator>
#include <functional>
#include <boost/bind.hpp>
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/detail/data_functional.h"

namespace mcrl2 {

namespace data {

using atermpp::aterm;
using atermpp::aterm_traits;

/// \brief Returns the set of all identifier strings occurring in the term t
template <typename Term>
std::set<core::identifier_string> find_identifiers(Term t)
{
  std::set<core::identifier_string> result;
  find_all_if(aterm_traits<Term>::term(t), core::is_identifier_string, std::inserter(result, result.end()));
  return result;
}
 
/// Returns true if the term has a given variable as subterm.
template <typename Term>
bool find_data_variable(Term t, const data_variable& d)
{
  return atermpp::partial_find_if(t, detail::compare_data_variable(d), core::detail::gsIsDataVarId) != atermpp::aterm();
}

/// \brief Returns all data variables that occur in the term t
template <typename Term>
std::set<data_variable> find_all_data_variables(Term t)
{
  std::set<data_variable> variables;
  atermpp::find_all_if(t, is_data_variable, std::inserter(variables, variables.end()));
  return variables;

  // find all data variables in t
  // std::set<data_variable> variables;
  // atermpp::partial_find_all_if(t,
  //                              is_data_variable,
  //                              boost::bind(std::logical_or<bool>(), boost::bind(is_data_variable, _1), boost::bind(is_data_operation, _1)),
  //                              std::inserter(variables, variables.end())
  //                             );
  // return variables;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FIND_H
