#ifndef ATERMPP_DETAIL_TYPE_TRAITS_H
#define ATERMPP_DETAIL_TYPE_TRAITS_H

#include <type_traits>
#include <set>
#include <vector>
#include <list>

namespace atermpp
{

namespace detail
{

// Condition for recognising types that represent containers
template < typename T >
struct is_container_impl : public std::false_type
{ };

template < typename T >
struct is_container_impl<std::list<T> > : public std::true_type
{ };

template < typename T >
struct is_container_impl< std::set< T > > : public std::true_type
{ };

template < typename T >
struct is_container_impl< std::multiset< T > > : public std::true_type
{ };

template < typename T >
struct is_container_impl< std::vector< T > > : public std::true_type
{ };

template < bool C, typename Container, typename Value >
struct lazy_check_value_type : public std::false_type
{ };

template < typename Container, typename ValueType >
struct lazy_check_value_type< true, Container, ValueType > : public
    std::is_convertible< typename Container::value_type, ValueType >
{ };

template < typename T >
struct is_set_impl : public std::false_type
  { };

template < typename T >
struct is_set_impl< std::set< T > > : public std::true_type
  { };

template < typename T >
struct is_set_impl< std::multiset< T > > :public std::true_type
  { };

} // namespace detail

} // namespace atermpp

#endif // ATERMPP_DETAIL_TYPE_TRAITS_H

