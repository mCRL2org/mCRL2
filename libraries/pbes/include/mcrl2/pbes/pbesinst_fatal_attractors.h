// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_fatal_attractors.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_FATAL_ATTRACTORS_H
#define MCRL2_PBES_PBESINST_FATAL_ATTRACTORS_H

#include "mcrl2/pbes/simple_structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Computes an attractor set, by extending A. Only predecessors in U are considered with a rank of at least j.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph>
vertex_set compute_attractor_set_min_rank(const StructureGraph& G, vertex_set A, int alpha, const vertex_set& U, std::size_t j)
{
  // put all predecessors of elements in A in todo
  deque_vertex_set todo(G.all_vertices().size());
  for (auto v: A.vertices())
  {
    for (auto u: G.predecessors(v))
    {
      if (U.contains(u) && G.rank(u) == j && !A.contains(u))
      {
        todo.insert(u);
      }
    }
  }

  while (!todo.is_empty())
  {
    // N.B. Use a breadth first search, to minimize counter examples
    auto u = todo.pop_front();

    if (G.decoration(u) == alpha || includes_successors(G, u, A))
    {
      // set strategy
      if (G.decoration(u) != (1 - alpha))
      {
        for (auto w: G.successors(u))
        {
          if ((A.contains(w)))
          {
            mCRL2log(log::debug) << "set strategy for node " << u << " to " << w << std::endl;
            G.find_vertex(u).strategy = w;
            break;
          }
        }
        if (G.strategy(u) == structure_graph::undefined_vertex)
        {
          mCRL2log(log::debug) << "Error: no strategy for node " << u << std::endl;
        }
      }
      A.insert(u);

      for (auto v: G.predecessors(u))
      {
        if (U.contains(v) && G.rank(v) == j && !A.contains(v))
        {
          todo.insert(v);
        }
      }
    }
  }

  return A;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_FATAL_ATTRACTORS_H
