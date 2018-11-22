// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_find_loops.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_FIND_LOOPS_H
#define MCRL2_PBES_PBESINST_FIND_LOOPS_H

#include "mcrl2/pbes/simple_structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
bool find_loop(const simple_structure_graph& G,
               structure_graph::index_type v,
               structure_graph::index_type w,
               std::size_t p,
               std::unordered_map<structure_graph::index_type, bool>& visited
)
{
  const auto& w_ = G.find_vertex(w);
  if (w_.decoration == structure_graph::d_true || w_.decoration == structure_graph::d_false)
  {
    return false;
  }
  if (w_.rank != data::undefined_index() && w_.rank != p)
  {
    return false;
  }
  auto i = visited.find(w);
  if (i != visited.end())
  {
    return i->second;
  }

  if (w_.decoration == structure_graph::d_none || w_.decoration == p % 2)
  {
    visited[w] = false;
    for (structure_graph::index_type u: w_.successors)
    {
      if (u == v || find_loop(G, v, u, p, visited))
      {
        visited[w] = true;
        mCRL2log(log::verbose) << "       case 1: found a loop starting in " << v << " with current vertex w = " << w << std::endl;
        return true;
      }
    }
    if (visited[w])
    {
      mCRL2log(log::verbose) << "       case 2: found a loop starting in " << v << " with current vertex w = " << w << std::endl;
    }
    return visited[w];
  }
  else
  {
    visited[w] = true;
    bool has_successors = false;
    for (structure_graph::index_type u: w_.successors)
    {
      has_successors = true;
      if (u != v && !find_loop(G, v, u, p, visited))
      {
        visited[w] = false;
        return false;
      }
    }
    if (!has_successors)
    {
      visited[w] = false;
    }
    if (visited[w])
    {
      mCRL2log(log::verbose) << "       case 3: found a loop starting in " << v << " with current vertex w = " << w << std::endl;
    }
    return visited[w];
  }
  return false;
}

void find_loops(const simple_structure_graph& G,
                const std::unordered_set<propositional_variable_instantiation>& discovered,
                vertex_set& S0,
                vertex_set& S1,
                std::size_t iteration_count,
                const detail::structure_graph_builder& graph_builder
)
{
  mCRL2log(log::verbose) << "Apply find loops (iteration " << iteration_count << ") to graph:\n" << G << std::endl;

  // count the number of insertions in the sets S0 and S1
  std::size_t insertion_count = 0;

  std::unordered_map<structure_graph::index_type, bool> visited;
  for (const propositional_variable_instantiation& X: discovered)
  {
    structure_graph::index_type u = graph_builder.find_vertex(X);
    const auto& u_ = G.find_vertex(u);
    if (u_.rank == data::undefined_index())
    {
      continue;
    }
    mCRL2log(log::verbose) << "--- choose u = " << u << std::endl;
    auto i = visited.find(u);
    if (i != visited.end())
    {
      visited[u] = false;
    }
    bool b = detail::find_loop(G, u, u, u_.rank, visited);
    visited[u] = b;
    if (b)
    {
      if (u_.rank % 2 == 1)
      {
        S1.insert(u);
        insertion_count++;
        mCRL2log(log::verbose) << "Find loops: insert vertex " << u << " in S1" << std::endl;
      }
      else
      {
        S0.insert(u);
        insertion_count++;
        mCRL2log(log::verbose) << "Find loops: insert vertex " << u << " in S0" << std::endl;
      }
    }
  }

  mCRL2log(log::verbose) << "Find loops: (iteration " << iteration_count << ") inserted " << insertion_count << " vertices." << std::endl;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_FIND_LOOPS_H
