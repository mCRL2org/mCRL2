// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/enumerator_iteration_limit.h
/// \brief Stores a static variable that indicates the number of iterations
/// allowed during enumeration

#ifndef MCRL2_DATA_DETAIL_ENUMERATOR_ITERATION_LIMIT_H
#define MCRL2_DATA_DETAIL_ENUMERATOR_ITERATION_LIMIT_H

#include <cstddef>

namespace mcrl2::data::detail
{

// Stores the maximum number of iterations that may be performed during enumeration.
template <class T> // note, T is only a dummy
struct enumerator_iteration_limit
{
  static std::size_t max_enumerator_iterations;
};

// Initialization
template <class T>
std::size_t enumerator_iteration_limit<T>::max_enumerator_iterations = 1000;

inline
void set_enumerator_iteration_limit(std::size_t size)
{
  enumerator_iteration_limit<std::size_t>::max_enumerator_iterations = size;
}

inline
std::size_t get_enumerator_iteration_limit()
{
  return enumerator_iteration_limit<std::size_t>::max_enumerator_iterations;
}

} // namespace mcrl2::data::detail

#endif // MCRL2_DATA_DETAIL_ENUMERATOR_ITERATION_LIMIT_H
