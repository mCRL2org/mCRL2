// Author(s): Wieger Wesselink, Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_TYPE_TRAITS_H
#define MCRL2_UTILITIES_TYPE_TRAITS_H

#include <type_traits>
#include <functional>

namespace mcrl2::utilities
{

/// \brief A typetrait that is std::true_type iff std::begin() and std::end() can be called on type T
template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<decltype(std::begin(std::declval<T>())),
                                  decltype(std::end(std::declval<T>()))
                                 >
                  > : std::true_type {};

/// \brief A typetrait that is std::true_type iff the given type has the iterator traits.
template<typename T, typename = void>
struct is_iterator : std::false_type {};

template<typename T>
struct is_iterator<T, typename std::enable_if<!std::is_same<typename std::iterator_traits<T>::value_type, void>::value>::type>
    : std::true_type
{};

/// \brief Checks whether condition holds for all types passed as variadic template.
template<typename... Conds>
struct forall: std::true_type
{};

template<typename Cond, typename... Conds>
struct forall<Cond, Conds...>
    : std::conditional<Cond::value, forall<Conds...>, std::false_type>::type
{};

// Helpers
template <typename T>
constexpr bool is_iterable_v = is_iterable<T>::value;

template <typename T>
constexpr bool is_iterator_v = is_iterator<T>::value;

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_TYPE_TRAITS_H
