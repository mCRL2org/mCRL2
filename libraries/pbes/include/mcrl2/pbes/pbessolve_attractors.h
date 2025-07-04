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

namespace mcrl2::pbes_system {

// Does not set a strategy
struct no_strategy
{
  static void set_strategy(structure_graph::index_type /* u */, structure_graph::index_type /* v */)
  {}
};

// Puts strategy annotations in the strategy attributes of the nodes of graph G
template <typename StructureGraph>
struct global_strategy
{
  const StructureGraph& G;

  explicit global_strategy(const StructureGraph& G_)
   : G(G_)
  {}

  void set_strategy(structure_graph::index_type u, structure_graph::index_type v)
  {
    if (v == undefined_vertex())
    {
      mCRL2log(log::debug) << "Error: undefined strategy for node " << u << std::endl;
    }
    mCRL2log(log::debug) << "  set tau[" << u << "] = " << v << std::endl;
    G.find_vertex(u).strategy = v;
  }
};

// Puts strategy annotations in tau[alpha]
struct local_strategy
{
  std::array<strategy_vector, 2>& tau;
  std::size_t alpha;

  local_strategy(std::array<strategy_vector, 2>& tau_, std::size_t alpha_)
    : tau(tau_), alpha(alpha_)
  {}

  void set_strategy(structure_graph::index_type u, structure_graph::index_type v)
  {
    if (v == undefined_vertex())
    {
      mCRL2log(log::debug) << "Error: undefined strategy for node " << u << std::endl;
    }
    mCRL2log(log::debug) << "  set tau" << alpha << "[" << u << "] = " << v << std::endl;
    tau[alpha][u] = v;
  }
};

// Combination of global and local strategy
template <typename StructureGraph>
struct global_local_strategy
{
  global_strategy<StructureGraph> global;
  local_strategy local;

  global_local_strategy(const StructureGraph& G, std::array<strategy_vector, 2>& tau, std::size_t alpha)
   : global(G), local(tau, alpha)
  {}

  void set_strategy(structure_graph::index_type u, structure_graph::index_type v)
  {
    global.set_strategy(u, v);
    local.set_strategy(u, v);
  }
};

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

// Returns pred(A) \ A
template <typename StructureGraph>
deque_vertex_set exclusive_predecessors(const StructureGraph& G, const vertex_set& A)
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
// Strategy is either no_strategy, global_strategy, local_strategy or global_local_strategy
template <typename StructureGraph, typename Strategy>
vertex_set attr_default_generic(const StructureGraph& G, vertex_set A, std::size_t alpha, Strategy tau)
{
  deque_vertex_set todo = exclusive_predecessors(G, A);

  while (!todo.is_empty())
  {
    // N.B. Use a breadth first search, to minimize counter examples
    auto u = todo.pop_front();

    if (G.decoration(u) == alpha || includes_successors(G, u, A))
    {
      tau.set_strategy(u, find_successor_in(G, u, A));
      A.insert(u);
      insert_predecessors(G, u, A, todo);
    }
  }

  return A;
}

// Computes an attractor set, by extending A.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph>
vertex_set attr_default(const StructureGraph& G, vertex_set A, std::size_t alpha)
{
  return attr_default_generic(G, A, alpha, global_strategy<StructureGraph>(G));
}

// Variant of attr_default that does not set any strategies.
template <typename StructureGraph>
vertex_set attr_default_no_strategy(const StructureGraph& G, vertex_set A, std::size_t alpha)
{
  return attr_default_generic(G, A, alpha, no_strategy());
}

// Computes an attractor set, by extending A.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph>
vertex_set attr_default_with_tau(const StructureGraph& G, vertex_set A, std::size_t alpha, std::array<strategy_vector, 2>& tau)
{
  return attr_default_generic(G, A, alpha, global_local_strategy<StructureGraph>(G, tau, alpha));
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_PBESSOLVE_ATTRACTORS_H
