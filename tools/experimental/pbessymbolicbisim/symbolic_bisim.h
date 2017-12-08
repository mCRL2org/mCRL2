// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_bisim.h


#ifndef MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H
#define MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H

#include <string>
#include <ctime>
#include <chrono>

#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/pg/ParityGame.h"
#include "mcrl2/pg/ParityGameSolver.h"
#include "mcrl2/pg/PredecessorLiftingStrategy.h"
#include "mcrl2/pg/SmallProgressMeasures.h"

#include "ppg_parser.h"
#include "partition.h"
#ifndef DBM_PACKAGE_AVAILABLE
  #define THIN       "0"
  #define BOLD       "1"
  #define GREEN(S)  "\033[" S ";32m"
  #define YELLOW(S) "\033[" S ";33m"
  #define RED(S)    "\033[" S ";31m"
  #define NORMAL    "\033[0;0m"
#endif

namespace mcrl2
{
namespace data
{

using namespace mcrl2::log;

class symbolic_bisim_algorithm
{
  typedef rewriter::substitution_type substitution_t;
  typedef std::pair< pbes_system::propositional_variable, data_expression > block_t;
  typedef pbes_system::detail::ppg_summand summand_type_t;
  typedef pbes_system::detail::ppg_equation equation_type_t;
  typedef pbes_system::detail::ppg_pbes pbes_type_t;

protected:
  rewriter rewr;
  rewriter proving_rewr;
  pbes_system::detail::ppg_pbes m_spec;
  dependency_graph_partition m_partition;

  const ParityGame::Strategy compute_pg_solution(const ParityGame& pg)
  {
    std::unique_ptr<ParityGameSolverFactory> solver_factory;
    solver_factory.reset(new SmallProgressMeasuresSolverFactory
              (new PredecessorLiftingStrategyFactory, 2, false));
    std::unique_ptr<ParityGameSolver> solver(solver_factory->create(pg));

    ParityGame::Strategy solution = solver->solve();
    if(solution.empty())
    {
      throw mcrl2::runtime_error("Solving of parity game failed.");
    }
    return solution;
  }

  const std::set< verti > compute_subpartition_from_strategy(const ParityGame& pg, const ParityGame::Strategy& solution)
  {
    std::set< verti > proof_graph_nodes;
    proof_graph_nodes.insert(0);
    std::queue< verti > open_set;
    open_set.push(0);

    // Explore the graph according to the strategy
    while(!open_set.empty())
    {
      const verti& v = open_set.front();
      open_set.pop();
      if(solution[v] != NO_VERTEX)
      {
        // Follow the edge of the strategy
        if(proof_graph_nodes.find(solution[v]) == proof_graph_nodes.end())
        {
          // Node has not been seen before
          open_set.push(solution[v]);
          proof_graph_nodes.insert(solution[v]);
        }
      }
      else
      {
        // This node belongs to the losing player
        // so we follow all edges
        for(StaticGraph::const_iterator succ = pg.graph().succ_begin(v); succ != pg.graph().succ_end(v); succ++)
        {
          if(proof_graph_nodes.find(*succ) == proof_graph_nodes.end())
          {
            // Node has not been seen before
            open_set.push(*succ);
            proof_graph_nodes.insert(*succ);
          }
        }
      }
    }

    // The initial node always has number 0
    // This is guaranteed by dependency_graph_parition::get_reachable_pg()
    std::cout << "Found a " << (pg.winner(solution, 0) == PLAYER_EVEN ? "positive" : "negative") << " proof graph." << std::endl;
    std::cout << "Proof graph contains nodes ";
    for(const verti& v: proof_graph_nodes)
    {
      std::cout << v << ", ";
    }
    std::cout << std::endl;
    return proof_graph_nodes;
  }

public:
  symbolic_bisim_algorithm(const pbes_system::pbes& spec, const data_expression& inv, const rewrite_strategy& st = jitty)
    : rewr(merge_data_specifications(spec.data(),simplifier::norm_rules_spec()),st)
#ifdef MCRL2_JITTYC_AVAILABLE
    , proving_rewr(spec.data(), st == jitty ? jitty_prover : jitty_compiling_prover)
#else
    , proving_rewr(spec.data(), jitty_prover)
#endif
    , m_spec(pbes_system::detail::ppg_pbes(spec).simplify(rewr))
    , m_partition(m_spec,rewr,proving_rewr)
  {}

  void run()
  {
    mCRL2log(mcrl2::log::verbose) << "Running symbolic bisimulation..." << std::endl;
    const std::chrono::time_point<std::chrono::high_resolution_clock> t_start = 
      std::chrono::high_resolution_clock::now();

    std::cout << m_spec << std::endl;

    // while the (sub) partition is stable, we have to keep
    // refining and searching for proof graphs
    std::size_t num_iterations = 0;
    double total_pg_time = 0.0;
    while(!m_partition.refine_n_steps(3))
    {
      const std::chrono::time_point<std::chrono::high_resolution_clock> t_start_pg_solver = 
        std::chrono::high_resolution_clock::now();

      ParityGame pg;
      m_partition.get_reachable_pg(pg);

      const ParityGame::Strategy& solution = compute_pg_solution(pg);
      pg.write_debug(solution, std::cout);

      m_partition.set_proof(compute_subpartition_from_strategy(pg, solution));

      num_iterations++;
      std::cout << "End of iteration " << num_iterations << std::endl;
      total_pg_time += std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start_pg_solver).count();
    }
    std::cout << "Partition refinement completed in " << 
        std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start).count() << 
        " seconds.\n" << 
        "Time spent on PG solving: " << total_pg_time << " seconds" << std::endl;

    m_partition.save_bes();
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H
