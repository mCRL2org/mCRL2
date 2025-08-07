// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_SYMBOLIC_UTILITY_H
#define MCRL2_SYMBOLIC_UTILITY_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/symbolic/data_index.h"

#include <vector>
#include <stdint.h>

#include <sylvan_ldd.hpp>

namespace mcrl2::symbolic
{

/// \brief Projects the given vector v on the indices given by used.
template <typename T>
std::vector<T> project(const std::vector<T>& v, const std::vector<std::size_t>& used)
{
  std::vector<T> result;
  result.reserve(used.size());
  for (unsigned long i : used)
  {
    result.push_back(v[i]);
  }
  return result;
}

// Return a permuted copy v' of v with v'[i] = v[permutation[i]]
// N.B. the implementation is not efficient
template <typename Container>
Container permute_copy(const Container& v, const std::vector<std::size_t>& permutation)
{
  using T = typename Container::value_type;

  std::size_t n = v.size();
  assert(permutation.size() == n);
  std::vector<T> v_(v.begin(), v.end());
  std::vector<T> result(n);
  for (std::size_t i = 0; i < n; i++)
  {
    result[i] = v_[permutation[i]];
  }
  return Container(result.begin(), result.end());
}

/// \brief Converts a state vector into a singleton LDD representing that vector.
inline sylvan::ldds::ldd state2ldd(const data::data_expression_list& x, std::vector<data_expression_index>& data_index)
{
  MCRL2_DECLARE_STACK_ARRAY(v, std::uint32_t, x.size());

  auto vi = v.begin();
  auto di = data_index.begin();
  auto xi = x.begin();
  for (; di != data_index.end(); ++vi, ++di, ++xi)
  {
    *vi = di->insert(*xi).first;
  }

  return sylvan::ldds::cube(v.data(), x.size());
}


} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_SYMBOLIC_UTILITY_H