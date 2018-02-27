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
#include <regex>
#include <sstream>
#include <unordered_set>
#include <tuple>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/join.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/structure_graph.h"

#include "mcrl2/lps/print.h"

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
      // set strategy
      if (u->decoration != structure_graph::d_disjunction)
      {
        for (const vertex* w: u->successors)
        {
          if (contains(A, w))
          {
            mCRL2log(log::debug) << "set strategy for node " << u->formula << " to " << w->formula << std::endl;
            u->strategy = w;
            break;
          }
        }
        if (u->strategy == 0)
        {
          mCRL2log(log::debug) << "Error: no strategy for node " << u << std::endl;
        }
      }

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
      // set strategy
      if (u->decoration != structure_graph::d_conjunction)
      {
        for (const vertex* w: u->successors)
        {
          if (contains(A, w))
          {
            mCRL2log(log::debug) << "set strategy for node " << u->formula << " to " << w->formula << std::endl;
            u->strategy = w;
            break;
          }
        }
        if (u->strategy == 0)
        {
          mCRL2log(log::debug) << "Error: no strategy for node " << u << std::endl;
        }
      }

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

inline
void log_vertex_set(const structure_graph::vertex_set& V, const std::string& name)
{
  mCRL2log(log::debug) << "--- " << name << " ---" << std::endl;
  for (const structure_graph::vertex* v: V)
  {
    if (v->enabled)
    {
      mCRL2log(log::debug) << "  " << *v << std::endl;
    }
  }
  mCRL2log(log::debug) << "\n";
}

// find a successor of u in U, or a random one if no successor in U exists
inline
const structure_graph::vertex* find_successor(const structure_graph::vertex* u, const structure_graph::vertex_set& U)
{
  if (u->successors.empty())
  {
    throw mcrl2::runtime_error("no successor found!");
  }

  typedef structure_graph::vertex vertex;
  const vertex* result = nullptr;
  for (const vertex* v: u->successors)
  {
    if (v->enabled)
    {
      result =  v;
    }
    if (contains(U, v))
    {
      result = v;
      break;
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

  log_vertex_set(V, "solve_recursive input");

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
        if (u->decoration == structure_graph::d_disjunction)
        {
          auto v = find_successor(u, U);
          if (v)
          {
            u->strategy = v;
            mCRL2log(log::debug) << "set initial strategy for node " << u->formula << " to " << u->strategy->formula << std::endl;
          }
        }
      }
    }
    else
    {
      for (const vertex* u: U)
      {
        if (u->decoration == structure_graph::d_conjunction)
        {
          auto v = find_successor(u, U);
          if (v)
          {
            u->strategy = v;
            mCRL2log(log::debug) << "set initial strategy for node " << u->formula << " to " << u->strategy->formula << std::endl;
          }
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

  mCRL2log(log::debug) << "\n--- solve_recursive_extended ---" << std::endl;
  log_vertex_set(V, "V");
  log_vertex_set(Vconj, "Vconj");
  log_vertex_set(Vdisj, "Vdisj");

  // extend Vconj and Vdisj
  if (!Vconj.empty())
  {
    Vconj = compute_attractor_set_conjunctive(Vconj);
  }
  if (!Vdisj.empty())
  {
    Vdisj = compute_attractor_set_disjunctive(Vdisj);
  }
  log_vertex_set(Vconj, "Vconj after extension");
  log_vertex_set(Vdisj, "Vdisj after extension");

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
void check_solve_recursive_solution(structure_graph::vertex_set& Wconj, structure_graph::vertex_set& Wdisj)
{
  typedef structure_graph::vertex_set vertex_set;
  typedef structure_graph::vertex vertex;

  vertex_set Wconj1;
  vertex_set Wdisj1;

  mCRL2log(log::debug) << "\n--- check_solve_recursive_solution ---" << std::endl;
  log_vertex_set(Wconj, "Wconj");
  log_vertex_set(Wdisj, "Wdisj");

  for (const vertex* u: Wconj)
  {
    u->enabled = true;
  }
  for (const vertex* u: Wdisj)
  {
    u->enabled = false;
  }
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
    log_vertex_set(Wconj1, "Wconj1");
    log_vertex_set(Wdisj1, "Wdisj1");
    throw mcrl2::runtime_error("check_solve_recursive_solution failed!");
  }

  for (const vertex* u: Wconj)
  {
    u->enabled = false;
  }
  for (const vertex* u: Wdisj)
  {
    u->enabled = true;
  }
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
    log_vertex_set(Wconj1, "Wconj1");
    log_vertex_set(Wdisj1, "Wdisj1");
    throw mcrl2::runtime_error("check_solve_recursive_solution failed!");
  }
}

inline
bool solve_structure_graph(const structure_graph& G, bool check_strategy = false)
{
  typedef structure_graph::vertex_set vertex_set;
  typedef structure_graph::vertex vertex;

  mCRL2log(log::verbose) << "Solving parity game..." << std::endl;
  structure_graph::vertex_set V = G.vertices();
  log_vertex_set(V, "structure graph");
  vertex_set Wconj;
  vertex_set Wdisj;
  std::tie(Wconj, Wdisj) = solve_recursive_extended(V);

  const vertex& init = G.initial_vertex();
  mCRL2log(log::verbose) << "vertices corresponding to true " << pp(Wdisj) << std::endl;
  mCRL2log(log::verbose) << "vertices corresponding to false " << pp(Wconj) << std::endl;

  if (check_strategy)
  {
    check_solve_recursive_solution(Wconj, Wdisj);
  }

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

inline
structure_graph::vertex_set find_counter_example_nodes(const structure_graph::vertex_set& V, const structure_graph::vertex& init, bool is_disjunctive)
{
  typedef structure_graph::vertex_set vertex_set;
  typedef structure_graph::vertex vertex;

  vertex_set todo = { &init };
  vertex_set done;
  while (!todo.empty())
  {
    const vertex* u = *todo.begin();
    todo.erase(todo.begin());
    done.insert(u);
    if ((is_disjunctive && u->decoration == structure_graph::d_disjunction) || (!is_disjunctive && u->decoration == structure_graph::d_conjunction))
    {
      // explore only the strategy edge
      const vertex* v = u->strategy;
      if (!contains(done, v))
      {
        todo.insert(v);
      }
    }
    else
    {
      // explore all outgoing edges
      for (const vertex* v: u->successors)
      {
        if (!contains(done, v))
        {
          todo.insert(v);
        }
      }
    }
  }
  return done;
}

inline
lps::specification create_counter_example_lps(const structure_graph::vertex_set& V, const lps::specification& lpsspec, const pbes& p, const pbes_equation_index& p_index)
{
  typedef structure_graph::vertex vertex;

  lps::specification result = lpsspec;
  result.process().action_summands().clear();
  result.process().deadlock_summands().clear();
  auto& action_summands = result.process().action_summands();
  std::regex re("Z(neg|pos)_(\\d+)_.*");
  std::size_t n = lpsspec.process().process_parameters().size();

  for (const vertex* v: V)
  {
    const propositional_variable_instantiation& Z = atermpp::down_cast<propositional_variable_instantiation>(v->formula);
    std::string Zname = Z.name();
    std::smatch m;
    if (std::regex_match(Zname, m, re))
    {
      std::size_t summand_index = std::stoul(m[2]);
      lps::action_summand summand = lpsspec.process().action_summands()[summand_index];

      std::size_t equation_index = p_index.index(Z.name());
      const pbes_equation& eqn = p.equations()[equation_index];
      const data::variable_list& d = eqn.variable().parameters();
      data::variable_vector d1(d.begin(), d.end());

      const data::data_expression_list& e = Z.parameters();
      data::data_expression_vector e1(e.begin(), e.end());

      data::data_expression_vector condition;
      data::assignment_vector next_state_assignments;
      std::size_t m = d.size() - 2 * n;

      for (std::size_t i = 0; i < n; i++)
      {
        condition.push_back(data::equal_to(d[i], e[i]));
        next_state_assignments.push_back(data::assignment(d[i], e[n + m + i]));
      }

      process::action_vector actions;
      std::size_t index = 0;
      for (const process::action& a: summand.multi_action().actions())
      {
        process::action a1(a.label(), data::data_expression_list(&e1[n + index], &e1[n + index + a.arguments().size()]));
        actions.push_back(a1);
        index = index + a.arguments().size();
      }

      summand.condition() = data::join_and(condition.begin(), condition.end());
      summand.multi_action().actions() = process::action_list(actions.begin(), actions.end());
      summand.assignments() = data::assignment_list(next_state_assignments.begin(), next_state_assignments.end());

      action_summands.push_back(summand);
    }
  }
  return result;
}

inline
structure_graph::vertex_set filter_counter_example_nodes(const structure_graph::vertex_set& V)
{
  typedef structure_graph::vertex_set vertex_set;
  typedef structure_graph::vertex vertex;

  vertex_set result;
  std::regex re("Z(neg|pos)_(\\d+)_.*");

  for (const vertex* v: V)
  {
    if (!is_propositional_variable_instantiation(v->formula))
    {
      continue;
    }
    const propositional_variable_instantiation& Z = atermpp::down_cast<propositional_variable_instantiation>(v->formula);
    std::string Zname = Z.name();
    std::smatch m;
    if (std::regex_match(Zname, m, re))
    {
      result.insert(v);
    }
  }
  return result;
}

/// \param lpsspec The original LPS that was used to create the PBES.
inline
std::pair<bool, lps::specification> solve_structure_graph_with_counter_example(const structure_graph& G, const lps::specification& lpsspec, const pbes& p, const pbes_equation_index& p_index)
{
  typedef structure_graph::vertex_set vertex_set;
  typedef structure_graph::vertex vertex;

  mCRL2log(log::verbose) << "Solving parity game..." << std::endl;
  structure_graph::vertex_set V = G.vertices();
  vertex_set Wconj;
  vertex_set Wdisj;
  std::tie(Wconj, Wdisj) = solve_recursive_extended(V);
  const vertex& init = G.initial_vertex();

  mCRL2log(log::debug) << "Wdisj = " << pp(Wdisj) << std::endl;
  mCRL2log(log::debug) << "Wconj = " << pp(Wconj) << std::endl;
  if (contains(Wdisj, &init))
  {
    mCRL2log(log::verbose) << "Extracting witness..." << std::endl;
    vertex_set W = find_counter_example_nodes(Wdisj, init, true);
    mCRL2log(log::debug) << "Counter example nodes in Wdisj (contains init) = " << pp(filter_counter_example_nodes(W)) << std::endl;
    return { true, create_counter_example_lps(W, lpsspec, p, p_index) };
  }
  else if (contains(Wconj, &init))
  {
    mCRL2log(log::verbose) << "Extracting counter example..." << std::endl;
    vertex_set W = find_counter_example_nodes(Wconj, init, false);
    mCRL2log(log::debug) << "Counter example nodes in Wconj (contains init) = " << pp(filter_counter_example_nodes(W)) << std::endl;
    return { false, create_counter_example_lps(W, lpsspec, p, p_index) };
  }
  throw mcrl2::runtime_error("No solution found in solve_structure_graph!");
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SOLVE_STRUCTURE_GRAPH_H
