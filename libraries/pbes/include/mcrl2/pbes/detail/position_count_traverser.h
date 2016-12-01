// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/position_count_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_POSITION_COUNT_TRAVERSER_H
#define MCRL2_PBES_DETAIL_POSITION_COUNT_TRAVERSER_H

#include "mcrl2/pbes/traverser.h"

namespace mcrl2 {

namespace pbes_system {

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

struct position_count_traverser: public pbes_expression_traverser<position_count_traverser>
{
  typedef pbes_expression_traverser<position_count_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  position_counter counter;

  template <typename T>
  void visit(const T& x)
  {
    counter.increase();
    super::apply(x);
    counter.decrease();
  }

  void apply(const and_& x)
  {
    visit(x);
  }

  void apply(const or_& x)
  {
    visit(x);
  }

  void apply(const imp& x)
  {
    visit(x);
  }

  void apply(const exists& x)
  {
    visit(x);
  }

  void apply(const forall& x)
  {
    visit(x);
  }

  void apply(const propositional_variable_instantiation& x)
  {
    visit(x);
  }

  void apply(const data::data_expression& x)
  {
    visit(x);
  }
};

inline
std::vector<std::size_t> position_counts(const pbes& x)
{
  detail::position_count_traverser f;
  f.apply(x);
  return f.counter.ycounts;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_POSITION_COUNT_TRAVERSER_H
