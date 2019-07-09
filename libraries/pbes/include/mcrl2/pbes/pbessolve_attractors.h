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
void set_strategy(const StructureGraph& G, typename StructureGraph::index_type u, typename StructureGraph::index_type v)
{
  mCRL2log(log::debug) << "set strategy for node " << u << " to " << v << std::endl;
  G.find_vertex(u).strategy = v;
}

template <typename StructureGraph, typename VertexSet>
void set_strategy(const StructureGraph& G, typename StructureGraph::index_type u, const VertexSet& A, std::size_t alpha)
{
  if (G.decoration(u) == alpha)
  {
    for (auto v: G.successors(u))
    {
      if ((A.contains(v)))
      {
        set_strategy(G, u, v);
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
template <typename StructureGraph, typename VertexSet>
bool includes_successors(const StructureGraph& G, typename StructureGraph::index_type u, const VertexSet& A)
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

// Computes an attractor set, by extending A.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph, bool SetStrategy = true>
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
      if (SetStrategy)
      {
        set_strategy(G, u, A, alpha);
      }

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

// Variant of attr_default that does not set any strategies.
template <typename StructureGraph>
vertex_set attr_default_no_strategy(const StructureGraph& G, vertex_set A, std::size_t alpha)
{
  return attr_default<StructureGraph, false>(G, A, alpha);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESSOLVE_ATTRACTORS_H
