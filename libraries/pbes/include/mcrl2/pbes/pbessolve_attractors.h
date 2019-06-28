// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbessolve_attractors.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESSOLVE_ATTRACTORS_H
#define MCRL2_PBES_PBESSOLVE_ATTRACTORS_H

#include "mcrl2/pbes/pbessolve_vertex_set.h"

namespace mcrl2 {

namespace pbes_system {

template <typename StructureGraph>
void set_strategy(const StructureGraph& G, typename StructureGraph::index_type u, const vertex_set& A, std::size_t alpha)
{
  if (G.decoration(u) == alpha)
  {
    for (auto v: G.successors(u))
    {
      if ((A.contains(v)))
      {
        mCRL2log(log::debug) << "set strategy for node " << u << " to " << v << std::endl;
        G.find_vertex(u).strategy = v;
        break;
      }
    }
    if (G.strategy(u) == structure_graph::undefined_vertex)
    {
      mCRL2log(log::debug) << "Error: no strategy for node " << u << std::endl;
    }
  }
}

// Returns true if succ(u) \subseteq A
template <typename StructureGraph>
bool includes_successors(const StructureGraph& G, typename StructureGraph::index_type u, const vertex_set& A)
{
  for (auto v: G.successors(u))
  {
    if (!A.contains(v))
    {
      return false;
    }
  }
  return true;
}

// Returns true if succ(u) \subseteq (A \cup S)
template <typename StructureGraph>
bool includes_successors(const StructureGraph& G, typename StructureGraph::index_type u, const vertex_set& A, const vertex_set& S)
{
  for (auto v: G.successors(u))
  {
    if (!A.contains(v) && !S.contains(v))
    {
      return false;
    }
  }
  return true;
}

// Returns true if succ(u) \cap A == \emptyset
template <typename StructureGraph>
bool has_empty_intersection(const StructureGraph& G, typename StructureGraph::index_type u, const vertex_set& A)
{
  for (auto v: G.successors(u))
  {
    if (A.contains(v))
    {
      return false;
    }
  }
  return true;
}

// Computes an attractor set, by extending A.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph>
vertex_set attr_default(const StructureGraph& G, vertex_set A, std::size_t alpha)
{
  // put all predecessors of elements in A in todo
  deque_vertex_set todo(G.all_vertices().size());
  for (auto u: A.vertices())
  {
    for (auto v: G.predecessors(u))
    {
      if (!A.contains(v))
      {
        todo.insert(v);
      }
    }
  }

  while (!todo.is_empty())
  {
    // N.B. Use a breadth first search, to minimize counter examples
    auto u = todo.pop_front();

    if (G.decoration(u) == alpha || includes_successors(G, u, A))
    {
      set_strategy(G, u, A, alpha);

      A.insert(u);

      for (auto v: G.predecessors(u))
      {
        if (!A.contains(v))
        {
          todo.insert(v);
        }
      }
    }
  }

  return A;
}

// Computes an attractor set, by extending S.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph>
vertex_set attr_cheap(const StructureGraph& G, vertex_set S, typename StructureGraph::index_type v, std::size_t alpha)
{
  std::size_t N = G.all_vertices().size();

  deque_vertex_set todo(N);
  for (auto u: G.predecessors(v))
  {
    if (!S.contains(u))
    {
      todo.insert(u);
    }
  }

  vertex_set A(N);
  A.insert(v);

  while (!todo.is_empty())
  {
    // N.B. Use a breadth first search, to minimize counter examples
    auto u = todo.pop_front();

    if ((G.decoration(u) == alpha && has_empty_intersection(G, u, A)) || (G.decoration(u) != alpha && includes_successors(G, u, A, S)))
    {
      set_strategy(G, u, A, alpha);

      A.insert(u);
      for (auto w: G.predecessors(u))
      {
        if (!A.contains(w) && !S.contains(w))
        {
          todo.insert(w);
        }
      }
    }
  }

  for (auto u: A.vertices())
  {
    S.insert(u);
  }
  return S;
}

// Computes an attractor set, by extending A.
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph>
vertex_set attr_simple(const StructureGraph& G, vertex_set A)
{
  // put all predecessors of elements in A in todo
  deque_vertex_set todo(G.all_vertices().size());
  for (auto u: A.vertices())
  {
    for (auto v: G.predecessors(u))
    {
      if (!A.contains(v))
      {
        todo.insert(v);
      }
    }
  }

  while (!todo.is_empty())
  {
    auto u = todo.pop_front();

    if (includes_successors(G, u, A))
    {
      A.insert(u);
      for (auto v: G.predecessors(u))
      {
        if (!A.contains(v))
        {
          todo.insert(v);
        }
      }
    }
  }
  return A;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESSOLVE_ATTRACTORS_H
