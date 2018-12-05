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

#include "mcrl2/pbes/pbessolve_vertex_set.h"
#include "mcrl2/pbes/simple_structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Computes an attractor set, by extending A. Only predecessors in U are considered with a rank of at least j.
// alpha = 0: disjunctive
// alpha = 1: conjunctive
// StructureGraph is either structure_graph or simple_structure_graph
template <typename StructureGraph>
vertex_set compute_attractor_set_min_rank(const StructureGraph& G, vertex_set A, std::size_t alpha, const vertex_set& U, std::size_t j)
{
  // put all predecessors of elements in A in todo
  deque_vertex_set todo(G.all_vertices().size());
  for (auto v: A.vertices())
  {
    for (auto u: G.predecessors(v))
    {
      if (U.contains(u) && G.rank(u) >= j && !A.contains(u))
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
      if (G.decoration(u) == alpha)
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
        if (U.contains(v) && (G.rank(v) >= j || (G.rank(u) == data::undefined_index() && G.decoration(u) <= 1)) && !A.contains(v))
        {
          todo.insert(v);
        }
      }
    }
  }

  return A;
}

void insert_in_rank_map(std::map<std::size_t, vertex_set>& U_rank_map, structure_graph::index_type u, std::size_t j, std::size_t n)
{
  auto i = U_rank_map.find(j);
  if (i == U_rank_map.end())
  {
    i = U_rank_map.insert({ j, vertex_set(n) }).first;
  }
  i->second.insert(u);
}

void fatal_attractors(const simple_structure_graph& G,
                      vertex_set& S0,
                      vertex_set& S1,
                      std::size_t iteration_count,
                      const detail::structure_graph_builder& graph_builder
)
{
  mCRL2log(log::debug) << "Apply fatal attractors (iteration " << iteration_count << ") to graph:\n" << G << std::endl;

  // count the number of insertions in the sets S0 and S1
  std::size_t insertion_count = 0;
  std::size_t n = graph_builder.m_vertices.size();

  // compute V
  vertex_set V(n);
  for (structure_graph::index_type u = 0; u < n; u++)
  {
    V.insert(u);
  }

  // compute U_j_map, such that U_j_map[j] = U_j
  std::map<std::size_t, vertex_set> U_j_map;
  for (structure_graph::index_type u: V.vertices())
  {
    std::size_t j = G.rank(u);
    if (j == data::undefined_index())
    {
      continue;
    }
    auto alpha = j % 2;
    if ((alpha == 0 && G.decoration(u) == structure_graph::decoration_type::d_false) || (alpha == 1 && G.decoration(u) == structure_graph::decoration_type::d_true))
    {
      continue;
    }
    insert_in_rank_map(U_j_map, u, j, n);
  }

  detail::log_vertex_set(V, "V");
  detail::log_vertex_set(S0, "S0");
  detail::log_vertex_set(S1, "S1");

  for (auto& p: U_j_map)
  {
    std::size_t j = p.first;
    auto alpha = j % 2;
    vertex_set& S_alpha = alpha == 0 ? S0 : S1;
    vertex_set& S_one_minus_alpha = alpha == 0 ? S1 : S0;
    vertex_set& U_j = p.second;
    U_j = set_minus(U_j, S_one_minus_alpha);
    detail::log_vertex_set(U_j, "U_" + std::to_string(j));
    vertex_set U = set_union(U_j, S_alpha);
    vertex_set X = detail::compute_attractor_set_min_rank(G, U, alpha, V, j);
    vertex_set Y = set_minus(V, compute_attractor_set(G, set_minus(V, X), 1 - alpha));

    while (X != Y)
    {
      detail::log_vertex_set(X, "X");
      detail::log_vertex_set(Y, "Y");
      X = detail::compute_attractor_set_min_rank(G, set_intersection(U, Y), alpha, V, j);
      Y = set_minus(Y, compute_attractor_set(G, set_minus(Y, X), 1 - alpha));
    }
    detail::log_vertex_set(X, "X (final value)");

    for (structure_graph::index_type x: X.vertices())
    {
      insertion_count++;
      S_alpha.insert(x);
      mCRL2log(log::debug) << "Fatal attractors: insert vertex " << x << " in S" << alpha << std::endl;
    }
  }
  S0 = compute_attractor_set(G, S0, 0);
  S1 = compute_attractor_set(G, S1, 1);
  mCRL2log(log::debug) << "Fatal attractors: (iteration " << iteration_count << ") inserted " << insertion_count << " vertices." << std::endl;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_FATAL_ATTRACTORS_H
