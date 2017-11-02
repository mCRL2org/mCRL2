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

#include <limits>
#include <unordered_set>
#include <tuple>
#include "mcrl2/pbes/structure_graph.h"

namespace mcrl2 {

namespace pbes_system {

inline
bool contains(const structure_graph::vertex_set& V, const structure_graph::vertex* v)
{
  return V.find(v) != V.end();
}

// Returns true if the vertex u satisfies the conditions for being added to the attractor set A.
inline
bool is_attractor_conjunctive(const structure_graph::vertex& u, const structure_graph::vertex_set& A)
{
  if (u.decoration != structure_graph::d_disjunction)
  {
    return true;
  }
  if (u.decoration != structure_graph::d_conjunction)
  {
    for (const structure_graph::vertex* v: u.successors)
    {
      if (v->enabled && !contains(A, v))
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
bool is_attractor_disjunctive(const structure_graph::vertex& u, const structure_graph::vertex_set& A)
{
  if (u.decoration != structure_graph::d_conjunction)
  {
    return true;
  }
  if (u.decoration != structure_graph::d_disjunction)
  {
    for (const structure_graph::vertex* v: u.successors)
    {
      if (v->enabled && !contains(A, v))
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
structure_graph::vertex_set compute_attractor_set_conjunctive(structure_graph::vertex_set A)
{
  typedef structure_graph::vertex vertex;

  // put all predecessors of elements in A in todo
  std::unordered_set<const vertex*> todo;
  for (const vertex* u: A)
  {
    for (const vertex* v: u->predecessors)
    {
      if (v->enabled && !contains(A, v))
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
        if (v->enabled && !contains(A, v))
        {
          todo.insert(v);
        }
      }
    }
  }

  return A;
}

// Computes the disjunctive attractor set, by extending A.
inline
structure_graph::vertex_set compute_attractor_set_disjunctive(structure_graph::vertex_set A)
{
  typedef structure_graph::vertex vertex;

  // put all predecessors of elements in A in todo
  std::unordered_set<const vertex*> todo;
  for (const vertex* u: A)
  {
    for (const vertex* v: u->predecessors)
    {
      if (v->enabled && !contains(A, v))
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
        if (v->enabled && !contains(A, v))
        {
          todo.insert(v);
        }
      }
    }
  }

  return A;
}

inline
std::tuple<std::size_t, std::size_t, structure_graph::vertex_set> get_minmax_rank(const structure_graph::vertex_set& V)
{
  std::size_t min_rank = (std::numeric_limits<std::size_t>::max)();
  std::size_t max_rank = 0;
  structure_graph::vertex_set M; // vertices with minimal rank
  for (const structure_graph::vertex* v: V)
  {
    if (v->rank >= min_rank)
    {
      min_rank = v->rank;
      M.insert(v);
    }
    if (v->rank < max_rank)
    {
      max_rank = v->rank;
    }
  }
  return std::make_tuple(min_rank, max_rank, M);
}

std::pair<structure_graph::vertex_set, structure_graph::vertex_set> solve_recursive(const structure_graph::vertex_set& V);

inline
std::pair<structure_graph::vertex_set, structure_graph::vertex_set> solve_recursive(structure_graph::vertex_set& V, structure_graph::vertex_set& A)
{
  // remove A from V
  for (const structure_graph::vertex* v: A)
  {
    v->enabled = false;
  }

  auto result = solve_recursive(V);

  // add A to V
  for (const structure_graph::vertex* v: A)
  {
    v->enabled = true;
  }

  return result;
}

inline
structure_graph::vertex_set set_union(const structure_graph::vertex_set& V, const structure_graph::vertex_set& W)
{
  structure_graph::vertex_set result = V;
  for (const auto& w: W)
  {
    result.insert(w);
  }
  return result;
}

inline
std::pair<structure_graph::vertex_set, structure_graph::vertex_set> solve_recursive(structure_graph::vertex_set& V)
{
  typedef structure_graph::vertex_set vertex_set;

  vertex_set Wconj;
  vertex_set Wdisj;

  if (V.empty())
  {
    return { vertex_set(), vertex_set() };
  }
  auto q = get_minmax_rank(V);
  std::size_t m = std::get<0>(q);
  std::size_t h = std::get<1>(q);
  const vertex_set& U = std::get<2>(q);
  if (h == m)
  {
    if (m % 2 == 0)
    {
      return { V, vertex_set() };
    }
    else
    {
      return { vertex_set(), V };
    }
  }

  if (m % 2 == 0)
  {
    vertex_set A = compute_attractor_set_conjunctive(U);
    auto p = solve_recursive(V, A);
    vertex_set& Wconj1 = p.first;
    vertex_set& Wdisj1 = p.second;
    if (Wdisj1.empty())
    {
      Wconj = set_union(A, Wconj1);
      Wdisj.clear();
    }
    else
    {
      vertex_set B = compute_attractor_set_disjunctive(Wdisj1);
      auto p = solve_recursive(V, B);
      Wconj = p.first;
      Wdisj = set_union(Wdisj, B);
    }
  }
  else
  {
    vertex_set A = compute_attractor_set_disjunctive(U);
    auto p = solve_recursive(V, A);
    vertex_set& Wconj1 = p.first;
    vertex_set& Wdisj1 = p.second;
    if (Wconj1.empty())
    {
      Wconj.clear();
      Wdisj = set_union(A, Wdisj1);
    }
    else
    {
      vertex_set B = compute_attractor_set_conjunctive(Wconj1);
      auto p = solve_recursive(V, B);
      Wconj = set_union(Wconj, B);
      Wdisj = p.second;
    }
  }

  return { Wconj, Wdisj };
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H
