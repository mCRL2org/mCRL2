// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_traits.h
/// \brief Traits class for terms.

#ifndef MCRL2_ATERMPP_ATERM_TRAITS_H
#define MCRL2_ATERMPP_ATERM_TRAITS_H

#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <utility>
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{

// class aterm;

template < typename T >
struct non_aterm_traits
{
  /// \brief Returns the aterm that corresponds to the term t.
  /// \param t A term
  /// \return The aterm that corresponds to the term t.
  static T term(const T& t)
  {
    return t;
  }
};

template < typename T, typename C = void >
struct select_traits_base
{
  typedef non_aterm_traits< T > base_type;
};

/// \brief Traits class for terms. It is used to specify how the term interacts
/// with the garbage collector, and how it can be converted to an aterm.
template <typename T>
struct aterm_traits: public select_traits_base< T >::base_type
  {};

/// \cond INTERNAL_DOCS
template <>
struct aterm_traits<aterm>
{
  static aterm term(aterm t)
  {
    return t;
  }
};

template <>
struct aterm_traits<aterm_list>
{
  static aterm term(aterm_list t)
  {
    return t;
  }
};

template <class Term>
struct aterm_traits<atermpp::term_list<Term> >
{
  static aterm term(aterm_appl t)
  {
    return t;
  }
};

template <>
struct aterm_traits<aterm_int>
{
  static aterm term(aterm_int t)
  {
    return t;
  }
};

template<typename T1, typename T2>
struct aterm_traits<std::pair<T1, T2> >
{
};
/// \endcond

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_TRAITS_H
