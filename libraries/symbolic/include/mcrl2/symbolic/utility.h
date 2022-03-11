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

#include <vector>
#include <stdint.h>

namespace mcrl2::symbolic
{

/// \brief Projects the given vector v on the indices given by used.
template <typename T>
std::vector<T> project(const std::vector<T>& v, const std::vector<std::size_t>& used)
{
  std::vector<T> result;
  result.reserve(used.size());
  for (std::size_t i = 0; i < used.size(); i++)
  {
    result.push_back(v[used[i]]);
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

} // namespace mcrl2::symbolic

#endif // MCRL2_SYMBOLIC_UTILITY_H