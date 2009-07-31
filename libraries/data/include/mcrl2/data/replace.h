// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/replace.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_DATA_REPLACE_H
#define MCRL2_DATA_REPLACE_H

#include "mcrl2/data/detail/replace.h"

namespace mcrl2 {

  namespace data {

/// \brief Recursively traverses the given term, and applies the replace function to
/// each data variable that is encountered during the traversal.
/// \param[in] container a container with expressions (expression, or container of expressions)
/// \param[in] substitution A replace function
/// \note Binders are ignored with replacements
/// \return The replacement result
template < typename Container, typename Substitution >
Container replace_variables(Container const& container, Substitution substitution)
{
  return detail::variable_replace_helper< typename boost::add_reference< Substitution >::type >(substitution)(container);
}

/// \brief Recursively traverses the given expression or expression container,
/// and applies the replace function to each data variable that is not bound it
/// its context.
/// \param[in] container a container with expressions (expression, or container of expressions)
/// \param[in] substitution the function used for replacing variables
/// \pre for all v in find_free_variables(container) for all x in
/// find_free_variables(substitution(v)) v does not occur in a
/// context C[v] = container in which x is bound
/// \return The expression that results after replacement
template <typename Container, typename Substitution >
Container replace_free_variables(Container const& container, Substitution substitution)
{
  return detail::free_variable_replace_helper< typename boost::add_reference< Substitution >::type >(substitution)(container);
}

/// \brief Recursively traverses the given expression or expression container,
/// and applies the replace function to each data variable that is not bound it
/// its context.
/// \param[in] container a container with expressions (expression, or container of expressions)
/// \param[in] substitution the function used for replacing variables
/// \param[in] bound a set of variables that should be considered as bound
/// \pre for all v in find_free_variables(container) for all x in
/// find_free_variables(substitution(v)) v does not occur in a
/// context C[v] = container in which x is bound
/// \return The expression that results after replacement
/// TODO prevent bound from being copied
template <typename Container, typename Substitution , typename VariableSequence >
Container replace_free_variables(Container const& container, Substitution substitution, VariableSequence const& bound)
{
  return detail::free_variable_replace_helper< typename boost::add_reference< Substitution >::type >(bound, substitution)(container);
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REPLACE_H
