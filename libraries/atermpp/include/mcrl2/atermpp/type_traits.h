// Author(s): unknown
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/type_traits.h
/// \brief A class containing some type traits. 

#ifndef ATERMPP_TYPE_TRAITS_H
#define ATERMPP_TYPE_TRAITS_H

#include "mcrl2/atermpp/detail/type_traits_impl.h"

namespace atermpp
{

/// type condition for use with std::enable_if
/// T the type to be tested
/// \pre V is void or T::value_type convertible to V
template < typename T, typename V = void >
struct is_container;

/// type condition for use with std::enable_if
/// T is the container type
template < typename T >
struct is_container< T, void > : public
    detail::is_container_impl< typename std::remove_reference< typename std::remove_const< T >::type >::type >
  { };

template < typename T, typename V >
struct is_container : public detail::lazy_check_value_type< is_container< T, void >::value, T, V >
  { };

/// type condition for use with std::enable_if
/// T the type to be tested
/// \pre V is void or T::value_type convertible to V
template < typename T, typename V = void >
struct enable_if_container : public
    std::enable_if< is_container< T, V >::value >
  { };

/// type condition for use with std::enable_if
/// T the type to be tested
/// \pre V is void or T::value_type convertible to V
template < typename T, typename V = void >
struct disable_if_container : public
    std::enable_if< !is_container< T, V >::value >
  { };

// type condition for use with std::enable_if
template < typename T >
struct is_set : public
    detail::is_set_impl< typename std::remove_reference< typename std::remove_const< T >::type >::type >
  { };

template < template < typename O > class C, typename E >
struct unary_template_swap
{
  typedef C< E > type;
};

} // namespace atermpp

#endif // ATERMPP_TYPE_TRAITS_H
