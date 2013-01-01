// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/container_utility.h
/// \brief Provides utilities for working with container.

#ifndef MCRL2_ATERMPP_DETAIL_ATERM_LIST_UTILITY_H
#define MCRL2_ATERMPP_DETAIL_ATERM_LIST_UTILITY_H

#include "boost/static_assert.hpp"

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
atermpp::term_list< Expression > operator+(atermpp::term_list< Expression > l, atermpp::term_list< CompatibleExpression > m)
{
  BOOST_STATIC_ASSERT((boost::is_convertible< CompatibleExpression, Expression >::value));

  return atermpp::term_list< Expression >(l+static_cast< atermpp::term_list<Expression > >(m));
}

/// \brief Returns the concatenation of the lists l and m
/// \param l A sequence of data expressions
/// \param m A sequence of data variables
/// \return The concatenation of the lists l and m
/// \note Candidate for moving to atermpp library
template < typename Expression, typename CompatibleExpression >
inline
atermpp::term_list< Expression > operator+(atermpp::term_list< Expression > l, CompatibleExpression const& m)
{
  BOOST_STATIC_ASSERT((boost::is_convertible< CompatibleExpression, Expression >::value));

  return atermpp::push_back(l, static_cast< Expression >(m));
}

/// \brief Returns the concatenation of the lists l and m
/// \param l A sequence of data expressions
/// \param m A sequence of data variables
/// \return The concatenation of the lists l and m
/// \note Candidate for moving to atermpp library
template < typename Expression, typename CompatibleExpression >
inline
atermpp::term_list< Expression > operator+(CompatibleExpression const& m, atermpp::term_list< Expression > l)
{
  BOOST_STATIC_ASSERT((boost::is_convertible< CompatibleExpression, Expression >::value));
  atermpp::term_list< Expression > result=l;
  result.push_front(aterm_cast<Expression>(m));
  return result;
}


} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_APPL_H
