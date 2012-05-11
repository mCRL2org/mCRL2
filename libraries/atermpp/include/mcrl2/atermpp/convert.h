// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/convert.h
/// \brief Conversion utilities for converting between the containers with
///    expressions and term lists that contain expressions

#ifndef MCRL2_ATERMPP_CONVERT_H
#define MCRL2_ATERMPP_CONVERT_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/container_utility.h"

namespace atermpp
{

/// \cond INTERNAL_DOCS
namespace detail
{

template < typename Container >
struct container_value
{
  typedef typename Container::value_type type;
};

template < >
struct container_value< aterm_list >
{
  typedef atermpp::aterm type;
};

template < typename TargetContainer, typename SourceContainer,
         typename TargetExpression = typename container_value< TargetContainer >::type,
         typename SourceExpression = typename container_value< SourceContainer >::type >
struct converter
{
  template < typename Container >
  static TargetContainer convert(Container const& l)
  {
    return TargetContainer(l.begin(), l.end());
  }
};

// Specialisation for aterm_list
/* template < typename TargetContainer, typename TargetExpression >
struct converter< TargetContainer, aterm_list, TargetExpression, atermpp::aterm > :
  public converter< TargetContainer, atermpp::term_list< TargetExpression >, TargetExpression, atermpp::aterm >
{

  static TargetContainer convert(aterm_list l)
  {
    return converter< TargetContainer, atermpp::term_list< TargetExpression >, TargetExpression, atermpp::aterm >::convert(atermpp::term_list< TargetExpression >(l));
  }
}; */

// Copy to from term list to term list
template < typename TargetExpression, typename SourceExpression >
struct converter< atermpp::term_list< TargetExpression >,
    atermpp::term_list< SourceExpression >,
    TargetExpression, SourceExpression >
{

  static atermpp::term_list< TargetExpression >
  convert(atermpp::term_list< SourceExpression > const& r)
  {
    return atermpp::term_list< TargetExpression >(r);
  }
};

} // namespace detail
/// \endcond

/// \brief Convert container with expressions to a new container with expressions
template < typename TargetContainer, typename SourceContainer >
TargetContainer convert(SourceContainer const& c)
{
  return detail::converter< TargetContainer, SourceContainer >::convert(c);
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_CONVERT_H

