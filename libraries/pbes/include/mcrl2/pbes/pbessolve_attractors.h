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

template <typename StructureGraph, typename VertexSet>
void set_strategy_with_tau(const StructureGraph& G, typename StructureGraph::index_type u, const VertexSet& A, std::size_t alpha, strategy_vector& tau)
{
  if (G.decoration(u) == alpha)
  {
    for (auto v: G.successors(u))
    {
      if ((A.contains(v)))
      {
        set_strategy(G, u, v);
        tau[u] = v;
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

// Returns all predecessors of elements in A.
template <typename StructureGraph>
deque_vertex_set predecessors(const StructureGraph& G, const vertex_set& A)
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
  return todo;
}

// Inserts pred(u) \ A into todo
template <typename StructureGraph>
void insert_predecessors(const StructureGraph& G, structure_graph::index_type u, const vertex_set& A, deque_vertex_set& todo)
{
  for (auto v: G.predecessors(u))
  {
    if (!A.contains(v))
    {
      todo.insert(v);
    }
  }
}

// Computes an attractor set, by extending A.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph, bool SetStrategy = true>
vertex_set attr_default(const StructureGraph& G, vertex_set A, std::size_t alpha)
{
  deque_vertex_set todo = predecessors(G, A);

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
      insert_predecessors(G, u, A, todo);
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

// Computes an attractor set, by extending A.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph, bool SetStrategy = true>
std::pair<vertex_set, strategy_vector> attr_default_with_tau(const StructureGraph& G, vertex_set A, std::size_t alpha, strategy_vector tau)
{
  deque_vertex_set todo = predecessors(G, A);

  while (!todo.is_empty())
  {
    // N.B. Use a breadth first search, to minimize counter examples
    auto u = todo.pop_front();

    if (G.decoration(u) == alpha || includes_successors(G, u, A))
    {
      if (SetStrategy)
      {
        set_strategy_with_tau(G, u, A, alpha, tau);
      }
      A.insert(u);
      insert_predecessors(G, u, A, todo);
    }
  }

  return { A, tau };
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESSOLVE_ATTRACTORS_H
