// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/solve_structure_graph.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H
#define MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H

#include <set>
#include "mcrl2/pbes/structure_graph.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

// Returns true if the vertex u satisfies the conditions for being added to the attractor set A.
inline
bool is_attractor_conjunctive(const structure_graph::vertex& u, const std::set<const structure_graph::vertex*>& A)
{
  using utilities::detail::contains;

  if (u.decoration != structure_graph::d_disjunction)
  {
    return true;
  }
  if (u.decoration != structure_graph::d_conjunction)
  {
    for (const structure_graph::vertex* v: u.successors)
    {
      if (!contains(A, v))
      {
        return false;
      }
    }
    return true;
  }
  return false;
}

// Returns true if the vertex u satisfies the conditions for being added to the attractor set A.
inline
bool is_attractor_disjunctive(const structure_graph::vertex& u, const std::set<const structure_graph::vertex*>& A)
{
  using utilities::detail::contains;

  if (u.decoration != structure_graph::d_conjunction)
  {
    return true;
  }
  if (u.decoration != structure_graph::d_disjunction)
  {
    for (const structure_graph::vertex* v: u.successors)
    {
      if (!contains(A, v))
      {
        return false;
      }
    }
    return true;
  }
  return false;
}

// Computes the conjunctive attractor set, by extending A.
inline
void compute_attractor_set_conjunctive(std::set<const structure_graph::vertex*>& A)
{
  using utilities::detail::contains;
  typedef structure_graph::vertex vertex;

  // put all predecessors of elements in A in todo
  std::set<const vertex*> todo;
  for (const vertex* u: A)
  {
    for (const vertex* v: u->predecessors)
    {
      if (!contains(A, v))
      {
        todo.insert(v);
      }
    }
  }

  while (!todo.empty())
  {
    const vertex* u = *todo.begin();
    todo.erase(todo.begin());
    assert(!contains(A, u));

    if (is_attractor_conjunctive(*u, A))
    {
      A.insert(u);
      for (const vertex* v: u->predecessors)
      {
        if (!contains(A, v))
        {
          todo.insert(v);
        }
      }
    }
  }
}

// Computes the disjunctive attractor set, by extending A.
inline
void compute_attractor_set_disjunctive(std::set<const structure_graph::vertex*>& A)
{
  using utilities::detail::contains;
  typedef structure_graph::vertex vertex;

  // put all predecessors of elements in A in todo
  std::set<const vertex*> todo;
  for (const vertex* u: A)
  {
    for (const vertex* v: u->predecessors)
    {
      if (!contains(A, v))
      {
        todo.insert(v);
      }
    }
  }

  while (!todo.empty())
  {
    const vertex* u = *todo.begin();
    todo.erase(todo.begin());
    assert(!contains(A, u));

    if (is_attractor_disjunctive(*u, A))
    {
      A.insert(u);
      for (const vertex* v: u->predecessors)
      {
        if (!contains(A, v))
        {
          todo.insert(v);
        }
      }
    }
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H
