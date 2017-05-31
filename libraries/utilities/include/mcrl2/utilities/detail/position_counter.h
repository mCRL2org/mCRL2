// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/detail/position_counter.h
/// \brief add your file description here.

#ifndef MCRL2_UTILITIES_DETAIL_POSITION_COUNTER_H
#define MCRL2_UTILITIES_DETAIL_POSITION_COUNTER_H

#include <cstddef>
#include <vector>

namespace mcrl2 {

namespace utilities {

namespace detail {

// Registers the current position (x, y) in a tree, and counts the number of nodes on every depth
struct position_counter
{
  const std::size_t undefined = std::size_t(-1);
  std::size_t x = 0;
  std::size_t y = undefined; // the depth
  std::vector<std::size_t> ycounts;  // counts the number of nodes at a certain depth

  void increase()
  {
    if (y == undefined)
    {
      y = 0;
    }
    else
    {
      y++;
    }
    if (y == ycounts.size())
    {
      ycounts.push_back(1);
      x = 0;
    }
    else
    {
      x = ycounts[y]++;
    }
  }

  void decrease()
  {
    y--;
  }

  bool at(std::size_t x0, std::size_t y0)
  {
    return x == x0 && y == y0;
  }
};

} // namespace detail

} // namespace utilities

} // namespace mcrl2

#endif // MCRL2_UTILITIES_DETAIL_POSITION_COUNTER_H
