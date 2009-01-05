// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

/// \brief Returns the set of all identifier strings occurring in the term t
/// \param t A term
/// \return The set of all identifier strings occurring in the term t
template <typename Term>
std::set<core::identifier_string> find_identifiers(Term t)
{
  std::set<core::identifier_string> result;
  find_all_if(atermpp::aterm_traits<Term>::term(t), core::is_identifier_string, std::inserter(result, result.end()));
  return result;
}
 
/// \brief Returns true if the term has a given variable as subterm.
/// \param t A term
/// \param d A data variable
/// \return True if the term has a given variable as subterm.
template <typename Term>
bool find_data_variable(Term t, const data_variable& d)
{
  return atermpp::partial_find_if(t, detail::compare_term<data_variable>(d), core::detail::gsIsDataVarId) != atermpp::aterm();
}

/// \brief Returns all data variables that occur in the term t
/// This is implementation is more efficient, but there are problems with it...
/// \param t A term
/// \return All data variables that occur in the term t
template <typename Term>
std::set<data_variable> find_all_data_variables2(Term t)
{
  std::set<data_variable> result;
  atermpp::partial_find_all_if(t,
                               is_data_variable,
                               boost::bind(std::logical_or<bool>(), boost::bind(is_data_variable, _1), boost::bind(is_data_operation, _1)),
                               std::inserter(result, result.end())
                              );
  return result;
}

/// \brief Returns all data variables that occur in the term t
/// \param t A term
/// \return All data variables that occur in the term t
template <typename Term>
std::set<data_variable> find_all_data_variables(Term t)
{
  std::set<data_variable> result;
  atermpp::find_all_if(t, is_data_variable, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given sort identifier as subterm.
/// \param t A term
/// \param s A sort identifier
/// \return True if the term has a given sort identifier as subterm.
template <typename Term>
bool find_sort_identifier(Term t, const sort_identifier& s)
{
  return atermpp::find_if(t, boost::bind(std::equal_to<atermpp::aterm_appl>(), s, _1)) != atermpp::aterm();
}

/// \brief Returns all sort identifiers that occur in the term t
/// \param t A term
/// \return All sort identifiers that occur in the term t
template <typename Term>
std::set<sort_identifier> find_all_sort_identifiers(Term t)
{
  std::set<sort_identifier> result;
  atermpp::find_all_if(t, is_sort_identifier, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given sort expression as subterm.
/// \param t A term
/// \param s A sort expression
/// \return True if the term has a given sort expression as subterm.
template <typename Term>
bool find_sort_expression(Term t, const sort_expression& s)
{
  return atermpp::find_if(t, boost::bind(std::equal_to<atermpp::aterm_appl>(), s, _1)) != atermpp::aterm();
}

/// \brief Returns all sort expressions that occur in the term t
/// \param t A term
/// \return All sort expressions that occur in the term t
template <typename Term>
std::set<sort_expression> find_all_sort_expressions(Term t)
{
  std::set<sort_expression> result;
  atermpp::find_all_if(t, is_sort_expression, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given data expression as subterm.
/// \param t A term
/// \param s A data expression
/// \return True if the term has a given data expression as subterm.
template <typename Term>
bool find_data_expression(Term t, const data_expression& s)
{
  return atermpp::find_if(t, boost::bind(std::equal_to<atermpp::aterm_appl>(), s, _1)) != atermpp::aterm();
}

/// \brief Returns all data expressions that occur in the term t
/// \param t A term
/// \return All data expressions that occur in the term t
template <typename Term>
std::set<data_expression> find_all_data_expressions(Term t)
{
  std::set<data_expression> result;
  atermpp::find_all_if(t, is_data_expression, std::inserter(result, result.end()));
  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FIND_H
