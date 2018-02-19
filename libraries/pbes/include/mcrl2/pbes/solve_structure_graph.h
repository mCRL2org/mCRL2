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
#include <sstream>
#include <unordered_set>
#include <tuple>
#include "mcrl2/core/detail/print_utility.h"
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

      // set strategy
      if (u->decoration == structure_graph::d_conjunction)
      {
        for (const vertex* w: u->successors)
        {
          if (contains(A, w))
          {
            mCRL2log(log::verbose) << "set strategy for node " << w->formula << std::endl;
            u->strategy = w;
            break;
          }
        }
        if (u->strategy == 0)
        {
          mCRL2log(log::verbose) << "Error: no strategy for node " << u << std::endl;
        }
      }

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

      // set strategy
      if (u->decoration == structure_graph::d_disjunction)
      {
        for (const vertex* w: u->successors)
        {
          if (contains(A, w))
          {
            mCRL2log(log::verbose) << "set strategy for node " << w->formula << std::endl;
            u->strategy = w;
            break;
          }
        }
        if (u->strategy == 0)
        {
          mCRL2log(log::verbose) << "Error: no strategy for node " << u << std::endl;
        }
      }

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
  typedef structure_graph::vertex_set vertex_set;
  typedef structure_graph::vertex vertex;

  std::size_t min_rank = (std::numeric_limits<std::size_t>::max)();
  std::size_t max_rank = 0;
  vertex_set M; // vertices with minimal rank

  for (const vertex* v: V)
  {
    if (!v->enabled)
    {
      continue;
    }
    if (v->rank <= min_rank)
    {
      if (v->rank < min_rank)
      {
        M.clear();
        min_rank = v->rank;
      }
      M.insert(v);
    }
    if (v->rank > max_rank)
    {
      max_rank = v->rank;
    }
  }
  auto result = std::make_tuple(min_rank, max_rank, M);
  return result;
}

std::pair<structure_graph::vertex_set, structure_graph::vertex_set> solve_recursive(structure_graph::vertex_set& V);

inline
std::pair<structure_graph::vertex_set, structure_graph::vertex_set> solve_recursive(structure_graph::vertex_set& V, structure_graph::vertex_set& A)
{
  // remove A from V
  std::vector<const structure_graph::vertex*> changed;
  for (const structure_graph::vertex* v: A)
  {
    if (v->enabled)
    {
      v->enabled = false;
      changed.push_back(v);
    }
  }

  auto result = solve_recursive(V);

  std::string VminusA = pp(V, false);

  // add A to V
  for (const structure_graph::vertex* v: changed)
  {
    v->enabled = true;
  }

  mCRL2log(log::debug) << "<solve_recursive> V = " << pp(V, false) << " A = " << pp(A) << " V - A = " << VminusA << " Wconj = " << pp(result.first) << " Wdisj = " << pp(result.second) << std::endl;
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
structure_graph::vertex_set remove_disabled_vertices(const structure_graph::vertex_set& V)
{
  structure_graph::vertex_set result;
  for (const auto& v: V)
  {
    if (v->enabled)
    {
      result.insert(v);
    }
  }
  return result;
}

// pre: V does not contain nodes with decoration true or false.
inline
std::pair<structure_graph::vertex_set, structure_graph::vertex_set> solve_recursive(structure_graph::vertex_set& V)
{
  typedef structure_graph::vertex vertex;
  typedef structure_graph::vertex_set vertex_set;

  vertex_set Wconj;
  vertex_set Wdisj;
  vertex_set Wconj1;
  vertex_set Wdisj1;

  if (is_empty(V))
  {
    Wconj = vertex_set();
    Wdisj = vertex_set();
  }
  else
  {
    auto q = get_minmax_rank(V);
    std::size_t m = std::get<0>(q);
    std::size_t h = std::get<1>(q);
    const vertex_set& U = std::get<2>(q);
    mCRL2log(log::debug) << "U = " << pp(U) << std::endl;

    // set strategy
    if (m % 2 == 0)
    {
      for (const vertex* u: U)
      {
        if (u->decoration == structure_graph::d_disjunction && !u->successors.empty())
        {
          mCRL2log(log::verbose) << "set initial strategy for node " << u->formula << std::endl;
          u->strategy = *(u->successors.begin());
        }
      }
    }
    else
    {
      for (const vertex* u: U)
      {
        if (u->decoration == structure_graph::d_conjunction && !u->successors.empty())
        {
          mCRL2log(log::verbose) << "set initial strategy for node " << u->formula << std::endl;
          u->strategy = *(u->successors.begin());
        }
      }
    }

    if (h == m)
    {
      if (m % 2 == 0)
      {
        Wconj = vertex_set();
        Wdisj = remove_disabled_vertices(V);
      }
      else
      {
        Wconj = remove_disabled_vertices(V);
        Wdisj = vertex_set();
      }
    }
    else if (m % 2 != 0)
    {
      vertex_set A = compute_attractor_set_conjunctive(U);
      mCRL2log(log::debug) << "A = " << pp(A) << std::endl;
      std::tie(Wconj1, Wdisj1) = solve_recursive(V, A);
      if (is_empty(Wdisj1))
      {
        Wconj = set_union(A, Wconj1);
        Wdisj.clear();
      }
      else
      {
        vertex_set B = compute_attractor_set_disjunctive(Wdisj1);
        mCRL2log(log::debug) << "B = " << pp(B) << std::endl;
        std::tie(Wconj, Wdisj) = solve_recursive(V, B);
        Wdisj = set_union(Wdisj, B);
      }
    }
    else
    {
      vertex_set A = compute_attractor_set_disjunctive(U);
      mCRL2log(log::debug) << "A = " << pp(A) << std::endl;
      std::tie(Wconj1, Wdisj1) = solve_recursive(V, A);
      if (is_empty(Wconj1))
      {
        Wconj.clear();
        Wdisj = set_union(A, Wdisj1);
      }
      else
      {
        vertex_set B = compute_attractor_set_conjunctive(Wconj1);
        mCRL2log(log::debug) << "B = " << pp(B) << std::endl;
        std::tie(Wconj, Wdisj) = solve_recursive(V, B);
        Wconj = set_union(Wconj, B);
      }
    }
  }

  return { Wconj, Wdisj };
}

// Handles nodes with decoration true or false.
inline
std::pair<structure_graph::vertex_set, structure_graph::vertex_set> solve_recursive_extended(structure_graph::vertex_set& V)
{
  typedef structure_graph::vertex_set vertex_set;
  typedef structure_graph::vertex vertex;

  vertex_set Vconj;
  vertex_set Vdisj;

  // find vertices Vconj with decoration false and Vdisj with decoration true
  for (const vertex* v: V)
  {
    if (v->decoration == structure_graph::d_false)
    {
      Vconj.insert(v);
    }
    else if (v->decoration == structure_graph::d_true)
    {
      Vdisj.insert(v);
    }
  }

  // extend Vconj and Vdisj
  if (!Vconj.empty())
  {
    Vconj = compute_attractor_set_conjunctive(Vconj);
  }
  if (!Vdisj.empty())
  {
    Vdisj = compute_attractor_set_disjunctive(Vdisj);
  }

  // default case
  if (Vconj.empty() && Vdisj.empty())
  {
    return solve_recursive(V);
  }
  else
  {
    vertex_set Wconj;
    vertex_set Wdisj;
    vertex_set Vunion = set_union(Vconj, Vdisj);
    std::tie(Wconj, Wdisj) = solve_recursive(V, Vunion);
    return std::make_pair(set_union(Wconj, Vconj), set_union(Wdisj, Vdisj));
  }
}

inline
void log_vertex_set(const structure_graph::vertex_set& V, const std::string& name)
{
  mCRL2log(log::verbose) << "--- " << name << " ---" << std::endl;
  for (const structure_graph::vertex* v: V)
  {
    if (v->enabled)
    {
      mCRL2log(log::verbose) << *v << std::endl;
    }
  }
}

inline
void check_solve_recursive_solution(structure_graph::vertex_set& Wconj, structure_graph::vertex_set& Wdisj)
{
  typedef structure_graph::vertex_set vertex_set;
  typedef structure_graph::vertex vertex;

  vertex_set Wconj1;
  vertex_set Wdisj1;

  mCRL2log(log::verbose) << "--- check_solve_recursive_solution ---" << std::endl;
  log_vertex_set(Wconj, "Wconj");
  for (const vertex* u: Wconj)
  {
    if (u->decoration == structure_graph::d_conjunction)
    {
      u->decoration = structure_graph::d_none;

      for (const vertex* v: u->successors)
      {
        v->remove_predecessor(*u);
      }
      u->successors.clear();

      // add the edge (u, u.strategy)
      if (u->strategy == 0)
      {
        std::cout << "no strategy for node " << *u << std::endl;
      }
      assert(u->strategy != 0);
      u->successors.push_back(u->strategy);
      u->strategy->predecessors.push_back(u);
    }
  }
  log_vertex_set(Wconj, "Wconj after removal of edges");
  std::tie(Wconj1, Wdisj1) = solve_recursive_extended(Wconj);
  if (!Wdisj1.empty() || Wconj1 != Wconj)
  {
    throw mcrl2::runtime_error("check_solve_recursive_solution failed!");
  }

  log_vertex_set(Wdisj, "Wdisj");
  for (const vertex* u: Wdisj)
  {
    if (u->decoration == structure_graph::d_disjunction)
    {
      u->decoration = structure_graph::d_none;

      for (const vertex* v: u->successors)
      {
        v->remove_predecessor(*u);
      }
      u->successors.clear();

      // add the edge (u, u.strategy)
      if (u->strategy == 0)
      {
        std::cout << "no strategy for node " << *u << std::endl;
      }
      assert(u->strategy != 0);
      u->successors.push_back(u->strategy);
      u->strategy->predecessors.push_back(u);
    }
  }
  log_vertex_set(Wdisj, "Wdisj after removal of edges");
  std::tie(Wconj1, Wdisj1) = solve_recursive_extended(Wdisj);
  if (!Wconj1.empty() || Wdisj1 != Wdisj)
  {
    throw mcrl2::runtime_error("check_solve_recursive_solution failed!");
  }
}

inline
bool solve_structure_graph(const structure_graph& G)
{
  typedef structure_graph::vertex_set vertex_set;
  typedef structure_graph::vertex vertex;

  mCRL2log(log::verbose) << "--- structure graph ----\n" << std::endl;
  structure_graph::vertex_set V = G.vertices();
  for (const structure_graph::vertex* v: V)
  {
    mCRL2log(log::verbose) << *v << std::endl;
  }
  vertex_set Wconj;
  vertex_set Wdisj;
  std::tie(Wconj, Wdisj) = solve_recursive_extended(V);

  const vertex& init = G.initial_vertex();
  mCRL2log(log::verbose) << "vertices corresponding to true " << pp(Wdisj) << std::endl;
  mCRL2log(log::verbose) << "vertices corresponding to false " << pp(Wconj) << std::endl;

  // check_solve_recursive_solution(Wconj, Wdisj);

  if (contains(Wdisj, &init))
  {
    return true;
  }
  else if (contains(Wconj, &init))
  {
    return false;
  }
  throw mcrl2::runtime_error("No solution found in solve_structure_graph!");
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H
