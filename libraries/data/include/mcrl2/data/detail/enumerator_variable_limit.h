// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/enumerator_variable_limit.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_ENUMERATOR_VARIABLE_LIMIT_H
#define MCRL2_DATA_DETAIL_ENUMERATOR_VARIABLE_LIMIT_H

#include <cstddef>

namespace mcrl2 {

namespace data {

namespace detail {

// Stores the maximum number of variables that may be used during enumeration.
template <class T> // note, T is only a dummy
struct enumerator_variable_limit
{
  static std::size_t max_enumerator_variables;
};

// Initialization
template <class T>
std::size_t enumerator_variable_limit<T>::max_enumerator_variables = 1000;

inline
void set_enumerator_variable_limit(std::size_t size)
{
  enumerator_variable_limit<std::size_t>::max_enumerator_variables = size;
}

inline
std::size_t get_enumerator_variable_limit()
{
  return enumerator_variable_limit<std::size_t>::max_enumerator_variables;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_ENUMERATOR_VARIABLE_LIMIT_H
