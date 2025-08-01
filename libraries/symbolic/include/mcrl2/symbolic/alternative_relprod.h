// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_SYMBOLIC_ALTERNATIVE_RELPROD_H
#define MCRL2_SYMBOLIC_ALTERNATIVE_RELPROD_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/symbolic/summand_group.h"

#include <sylvan_ldd.hpp>

#include <limits>
#include <stdint.h>
#include <vector>

namespace mcrl2::symbolic
{

constexpr std::uint32_t relprod_ignore = std::numeric_limits<std::uint32_t>::max(); // used by alternative_relprod/relprev

// A very inefficient implementation of relprod, that matches the specification closely
inline sylvan::ldds::ldd alternative_relprod(const sylvan::ldds::ldd& todo, const summand_group& R)
{
  using namespace sylvan::ldds;

  auto split = [&](const std::vector<std::uint32_t>& xy)
  {
    std::vector<std::uint32_t> x;
    std::vector<std::uint32_t> y;
    for (std::size_t j: R.read_pos)
    {
      x.push_back(xy[j]);
    }
    for (std::size_t j: R.write_pos)
    {
      y.push_back(xy[j]);
    }
    return std::make_pair(x, y);
  };

  auto match = [&](const std::vector<std::uint32_t>& x, const std::vector<std::uint32_t>& x_)
  {
    for (std::size_t j = 0; j < x_.size(); j++)
    {
      if (x[R.read[j]] != x_[j])
      {
        return false;
      }
    }
    return true;
  };

  auto replace = [&](std::vector<std::uint32_t> x, const std::vector<std::uint32_t>& y_)
  {
    for (std::size_t j = 0; j < y_.size(); j++)
    {
      if (y_[j] != relprod_ignore)
      {
        x[R.write[j]] = y_[j];
      }
    }
    return x;
  };

  ldd result = empty_set();

  auto todo_elements = ldd_solutions(todo);
  for (const std::vector<std::uint32_t>& xy: ldd_solutions(R.L))
  {
    auto [x_, y_] = split(xy);
    for (const auto& x: todo_elements)
    {
      if (match(x, x_))
      {
        auto y = replace(x, y_);
        result = union_cube(result, y);
      }
    }
  }
  return result;
}

// A very inefficient implementation of relprev, that matches the specification closely
inline sylvan::ldds::ldd
alternative_relprev(const sylvan::ldds::ldd& Y, const summand_group& R, const sylvan::ldds::ldd& X)
{
  using namespace sylvan::ldds;

  ldd result = empty_set();
  for (const std::vector<std::uint32_t>& x_values: ldd_solutions(X))
  {
    ldd x = cube(x_values);
    ldd y = alternative_relprod(x, R);
    if (intersect(y, Y) != empty_set())
    {
      result = union_(result, x);
    }
  }
  return result;
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_SYMBOLIC_ALTERNATIVE_RELPROD_H