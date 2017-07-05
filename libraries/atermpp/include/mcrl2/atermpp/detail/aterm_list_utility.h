// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/aterm_list_utility.h
/// \brief Provides the + operator on lists.

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_LIST_UTILITY_H
#define MCRL2_ATERMPP_DETAIL_ATERM_LIST_UTILITY_H

#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{
namespace detail
{

/// \brief Returns the concatenation of the lists l and m
/// \param l A sequence of data expressions
/// \param m A sequence of data variables
/// \return The concatenation of the lists l and m
/// \note Candidate for moving to atermpp library
template < typename Expression, typename CompatibleExpression >
inline
atermpp::term_list< Expression > operator+(const atermpp::term_list< Expression > &l, const atermpp::term_list< CompatibleExpression > &m)
{
  static_assert(std::is_convertible< CompatibleExpression, Expression >::value,"Concatenated lists must be of the same type (1)");

  return atermpp::term_list< Expression >(l+container_cast< atermpp::term_list< Expression > >(m));
}

/// \brief Returns the concatenation of the lists l and m
/// \param l A sequence of data expressions
/// \param m A sequence of data variables
/// \return The concatenation of the lists l and m
/// \note Candidate for moving to atermpp library
template < typename Expression, typename CompatibleExpression >
inline
atermpp::term_list< Expression > operator+(atermpp::term_list< Expression > l, const CompatibleExpression &m)
{
  l.push_back(down_cast< Expression >(m));
  return l;
}

/// \brief Returns the concatenation of the lists l and m
/// \param l A sequence of data expressions
/// \param m A sequence of data variables
/// \return The concatenation of the lists l and m
/// \note Candidate for moving to atermpp library
template < typename Expression, typename CompatibleExpression >
inline
atermpp::term_list< Expression > operator+(const CompatibleExpression &m, atermpp::term_list< Expression > l)
{
  l.push_front(m);
  return l;
}


} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_APPL_H
