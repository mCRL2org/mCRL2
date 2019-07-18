// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_partial_solve.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_PARTIAL_SOLVE_H
#define MCRL2_PBES_PBESINST_PARTIAL_SOLVE_H

#include "mcrl2/pbes/pbesinst_lazy.h"
#include "mcrl2/pbes/pbessolve_vertex_set.h"
#include "mcrl2/pbes/simple_structure_graph.h"
#include "mcrl2/pbes/solve_structure_graph.h"
#include "mcrl2/pbes/structure_graph_builder.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
void partial_solve(structure_graph& G,
                   const pbesinst_lazy_todo& todo,
                   vertex_set& S0,
                   vertex_set& S1,
                   strategy_vector& tau0,
                   strategy_vector& tau1,
                   std::size_t iteration_count,
                   const detail::structure_graph_builder& graph_builder
                  )
{
  mCRL2log(log::debug) << "Apply partial solve (iteration " << iteration_count << ") to graph:\n" << G << std::endl;

  detail::log_vertex_set(S0, "S0 (start of partial solve)");
  detail::log_vertex_set(S1, "S1 (start of partial solve)");

  std::size_t N = G.extent();

  // The size of S0 and S1 may be bigger than N, because of the resize strategy in vertex_set.
  S0.truncate(N);
  S1.truncate(N);

  S0 = attr_default(G, S0, 0);
  S1 = attr_default(G, S1, 1);

  // compute Si_todo = Si \cup { v \in V | v.is_defined() }
  vertex_set S0_todo = S0;
  vertex_set S1_todo = S1;
  for (const propositional_variable_instantiation& X: todo.all_elements())
  {
    structure_graph::index_type u = graph_builder.find_vertex(X);
    S0_todo.insert(u);
    S1_todo.insert(u);
  }

  bool check_strategy = false;
  bool use_toms_optimization = false;
  solve_structure_graph_algorithm algorithm(check_strategy, use_toms_optimization);

  vertex_set W[2] = { vertex_set(N), vertex_set(N) };
  std::tie(W[0], W[1]) = algorithm.solve_recursive(G, set_union(S1, attr_default_no_strategy(G, S0_todo, 0)));
  std::tie(S1, tau1) = attr_default_with_tau(G, set_union(S1, W[1]), 1, tau1);
  std::tie(W[0], W[1]) = algorithm.solve_recursive(G, set_union(S0, attr_default_no_strategy(G, S1_todo, 1)));
  std::tie(S0, tau0) = attr_default_with_tau(G, set_union(S0, W[0]), 0, tau0);

  mCRL2log(log::debug) << "Result of partial solve\n" << G << std::endl;
  mCRL2log(log::debug) << "S0 = " << S0 << std::endl;
  mCRL2log(log::debug) << "S1 = " << S1 << std::endl;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_PARTIAL_SOLVE_H
