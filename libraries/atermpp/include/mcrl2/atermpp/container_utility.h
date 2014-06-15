// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/container_utility.h
/// \brief Provides utilities for working with container.

#ifndef MCRL2_ATERMPP_CONTAINER_UTILITY_H
#define MCRL2_ATERMPP_CONTAINER_UTILITY_H

#include <algorithm>
#include <list>
#include <set>
#include <vector>

#include "boost/assert.hpp"
#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_convertible.hpp"
#include "boost/call_traits.hpp"
#include "boost/type_traits/remove_reference.hpp"
#include "boost/type_traits/add_reference.hpp"
#include "boost/iterator/iterator_adaptor.hpp"
#include "boost/iterator/iterator_facade.hpp"

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace atermpp
{

/// \cond INTERNAL_DOCS
namespace detail
{

// Condition for recognising types that represent containers
template < typename T >
struct is_container_impl
{
  typedef boost::false_type type;
};

template < typename T >
struct is_container_impl<std::list<T> >
{
  typedef boost::true_type type;
};

template < typename T >
struct is_container_impl< std::set< T > >
{
  typedef boost::true_type type;
};

template < typename T >
struct is_container_impl< std::multiset< T > >
{
  typedef boost::true_type type;
};

template < typename T >
struct is_container_impl< std::vector< T > >
{
  typedef boost::true_type type;
};

template < typename T >
struct is_container_impl< atermpp::term_list< T > >
{
  typedef boost::true_type type;
};

template < bool C, typename Container, typename Value >
struct lazy_check_value_type
{
  typedef boost::false_type type;
};

template < typename Container, typename ValueType >
struct lazy_check_value_type< true, Container, ValueType >
{
  typedef typename boost::is_convertible< typename Container::value_type, ValueType >::type type;
};

/// type condition for use with boost::enable_if
/// T the type to be tested
/// \pre V is void or T::value_type convertible to V
template < typename T, typename V = void >
struct is_container;

/// type condition for use with boost::enable_if
/// T is the container type
template < typename T >
struct is_container< T, void > : public
    is_container_impl< typename boost::remove_reference< typename boost::remove_const< T >::type >::type >
  { };

template < typename T, typename V >
struct is_container
{
  typedef typename lazy_check_value_type< is_container< T, void >::type::value, T, V >::type type;
};

/// type condition for use with boost::enable_if
/// T the type to be tested
/// \pre V is void or T::value_type convertible to V
template < typename T, typename V = void >
struct enable_if_container : public
    boost::enable_if< typename atermpp::detail::is_container< T, V >::type >
  {};

/// type condition for use with boost::enable_if
/// T the type to be tested
/// \pre V is void or T::value_type convertible to V
template < typename T, typename V = void >
struct disable_if_container : public
    boost::disable_if< typename atermpp::detail::is_container< T, V >::type >
  {};

template < typename T >
struct is_set_impl
{
  typedef boost::false_type type;
};

template < typename T >
struct is_set_impl< std::set< T > >
{
  typedef boost::true_type type;
};

template < typename T >
struct is_set_impl< std::multiset< T > >
{
  typedef boost::true_type type;
};

// type condition for use with boost::enable_if
template < typename T >
struct is_set : public is_set_impl< typename boost::remove_reference< typename boost::remove_const< T >::type >::type >
  { };

/// The Boost.phoenix library has a nice implementation that can also be used...
/// For our limited purposes this is enough though
template < typename Result >
struct construct
{
  typedef Result result_type;

  template < typename A >
  Result operator()(typename boost::call_traits< A >::param_type a) const
  {
    return Result(a);
  }

  template < typename A, typename A1 >
  Result operator()(A a, A1 a1) const
  {
    return Result(a, a1);
  }
};

} // namespace detail

/// \brief Constructs a vector with element type T of one argument.
///
/// \param[in] t1 The first element of the vector.
template <typename T>
std::vector<T> make_vector(const T& t1)
{
  std::vector<T> v;
  v.push_back(t1);
  return v;
}

/// \brief Constructs a vector with element type T of two arguments.
///
/// \param[in] t1 The first element of the vector.
/// \param[in] t2 The second element of the vector.
template <typename T>
std::vector<T> make_vector(const T& t1, const T& t2)
{
  std::vector<T> v;
  v.push_back(t1);
  v.push_back(t2);
  return v;
}

/// \brief Constructs a vector with element type T of three arguments.
///
/// \param[in] t1 The first element of the vector.
/// \param[in] t2 The second element of the vector.
/// \param[in] t3 The third element of the vector.
template <typename T>
std::vector<T> make_vector(const T& t1, const T& t2, const T& t3)
{
  std::vector<T> v;
  v.push_back(t1);
  v.push_back(t2);
  v.push_back(t3);
  return v;
}

/// \brief Constructs a vector with element type T of four arguments.
///
/// \param[in] t1 The first element of the vector.
/// \param[in] t2 The second element of the vector.
/// \param[in] t3 The third element of the vector.
/// \param[in] t4 The fourth element of the vector.
template <typename T>
std::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4)
{
  std::vector<T> v;
  v.push_back(t1);
  v.push_back(t2);
  v.push_back(t3);
  v.push_back(t4);
  return v;
}

/// \brief Constructs a vector with element type T of four arguments.
///
/// \param[in] t1 The first element of the vector.
/// \param[in] t2 The second element of the vector.
/// \param[in] t3 The third element of the vector.
/// \param[in] t4 The fourth element of the vector.
/// \param[in] t5 The fifth element of the vector.
template <typename T>
std::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4, const T& t5)
{
  std::vector<T> v;
  v.push_back(t1);
  v.push_back(t2);
  v.push_back(t3);
  v.push_back(t4);
  v.push_back(t5);
  return v;
}

/// \brief Constructs a vector with element type T of four arguments.
///
/// \param[in] t1 The first element of the vector.
/// \param[in] t2 The second element of the vector.
/// \param[in] t3 The third element of the vector.
/// \param[in] t4 The fourth element of the vector.
/// \param[in] t5 The fifth element of the vector.
/// \param[in] t6 The sixth element of the vector.
template <typename T>
std::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4, const T& t5, const T& t6)
{
  std::vector<T> v;
  v.push_back(t1);
  v.push_back(t2);
  v.push_back(t3);
  v.push_back(t4);
  v.push_back(t5);
  v.push_back(t6);
  return v;
}

/// \brief Constructs a vector with element type T of four arguments.
///
/// \param[in] t1 The first element of the vector.
/// \param[in] t2 The second element of the vector.
/// \param[in] t3 The third element of the vector.
/// \param[in] t4 The fourth element of the vector.
/// \param[in] t5 The fifth element of the vector.
/// \param[in] t6 The sixth element of the vector.
/// \param[in] t7 The seventh element of the vector.
template <typename T>
std::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4, const T& t5, const T& t6, const T& t7)
{
  std::vector<T> v;
  v.push_back(t1);
  v.push_back(t2);
  v.push_back(t3);
  v.push_back(t4);
  v.push_back(t5);
  v.push_back(t6);
  v.push_back(t7);
  return v;
}

/// \brief Constructs a vector with element type T of four arguments.
///
/// \param[in] t1 The first element of the vector.
/// \param[in] t2 The second element of the vector.
/// \param[in] t3 The third element of the vector.
/// \param[in] t4 The fourth element of the vector.
/// \param[in] t5 The fifth element of the vector.
/// \param[in] t6 The sixth element of the vector.
/// \param[in] t7 The seventh element of the vector.
/// \param[in] t8 The eighth element of the vector.
template <typename T>
std::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4, const T& t5, const T& t6, const T& t7, const T& t8)
{
  std::vector<T> v;
  v.push_back(t1);
  v.push_back(t2);
  v.push_back(t3);
  v.push_back(t4);
  v.push_back(t5);
  v.push_back(t6);
  v.push_back(t7);
  v.push_back(t8);
  return v;
}

} // namespace atermpp

#endif // MCRL2_ATERMPP_CONTAINER_UTILITY_H
