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

#include "mcrl2/pbes/pbesinst_lazy.h"
#include "mcrl2/pbes/pbessolve_attractors.h"
#include "mcrl2/pbes/simple_structure_graph.h"
#include "mcrl2/pbes/structure_graph_builder.h"





namespace mcrl2::pbes_system::detail {

inline
bool find_loop(const simple_structure_graph& G,
               const vertex_set& U,
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

  if (U.contains(w))
  {
    visited[w] = false;
    if (w_.decoration == structure_graph::d_none || w_.decoration == p % 2)
    {
      for (structure_graph::index_type u: w_.successors)
      {
        if (u == v || find_loop(G, U, v, u, p, visited))
        {
          global_strategy<simple_structure_graph>(G).set_strategy(w, u == v ? v : u);
          visited[w] = true;
          mCRL2log(log::debug) << "       case 1: found a loop starting in " << v << " with current vertex w = " << w << std::endl;
          return true;
        }
      }
    }
    else
    {
      return false;
    }
  }
  return false;
}

template <class Container>
inline void find_loops(const simple_structure_graph& G,
                       const Container& discovered,
                       const pbesinst_lazy_todo& todo, std::array<vertex_set, 2>& S,
                       std::array<strategy_vector, 2>& tau, std::size_t iteration_count,
                       const detail::structure_graph_builder& graph_builder
                      )
{
  mCRL2log(log::debug) << "Apply find loops (iteration " << iteration_count << ") to graph:\n" << G << std::endl;

  // count the number of insertions in the sets S[0] and S[1]
  std::size_t insertion_count = 0;

  std::size_t n = S[0].extent();

  // compute todo_
  boost::dynamic_bitset<> todo_(n);
  /* for (const propositional_variable_instantiation& X: todo.all_elements())  range::join does not seem to work.
                                                                               Hence split in todo.elements() and todo.irrelevant_elements() below. 
  {
    structure_graph::index_type u = graph_builder.find_vertex(X);
    todo_[u] = true;
  } */

  for (const propositional_variable_instantiation& X: todo.elements())
  {
    structure_graph::index_type u = graph_builder.find_vertex(X);
    todo_[u] = true;
  }

  for (const propositional_variable_instantiation& X: todo.irrelevant_elements())
  {
    structure_graph::index_type u = graph_builder.find_vertex(X);
    todo_[u] = true;
  }

  // compute done
  vertex_set done(n);
  for (const propositional_variable_instantiation& X: discovered)
  {
    structure_graph::index_type u = graph_builder.find_vertex(X);
    if (!todo_[u])
    {
      done.insert(u);
    }
  }

  std::unordered_map<structure_graph::index_type, bool> visited;
  bool b0 = false;
  bool b1 = false;

  for (structure_graph::index_type u: done.vertices())
  {
    const auto& u_ = G.find_vertex(u);
    assert(u_.rank != data::undefined_index());

    mCRL2log(log::debug) << "--- choose u = " << u << std::endl;
    auto i = visited.find(u);
    if (i != visited.end())
    {
      visited[u] = false;
    }
    bool b = find_loop(G, done, u, u, u_.rank, visited);
    visited[u] = b;
    if (b)
    {
      if (u_.rank % 2 == 0)
      {
        S[0].insert(u);
        b0 = true;
        insertion_count++;
        mCRL2log(log::debug) << "Find loops: insert vertex " << u << " in S[0]" << std::endl;
      }
      else
      {
        S[1].insert(u);
        b1 = true;
        insertion_count++;
        mCRL2log(log::debug) << "Find loops: insert vertex " << u << " in S[1]" << std::endl;
      }
    }
    if (b0)
    {
      S[0] = attr_default_with_tau(G, S[0], 0, tau);
    }
    if (b1)
    {
      S[1] = attr_default_with_tau(G, S[1], 1, tau);
    }
  }

  mCRL2log(log::debug) << "Find loops: (iteration " << iteration_count << ") inserted " << insertion_count << " vertices." << std::endl;
}

} // namespace mcrl2::pbes_system::detail





#endif // MCRL2_PBES_PBESINST_FIND_LOOPS_H
