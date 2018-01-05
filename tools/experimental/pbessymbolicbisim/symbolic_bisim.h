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
#include "mcrl2/pg/SCC.h"
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
  std::size_t m_num_refine_steps;

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
    mCRL2log(log::verbose) << "Found a " << (pg.winner(solution, 0) == PLAYER_EVEN ? "positive" : "negative") << " proof graph." << std::endl;
    mCRL2log(log::verbose) << "Proof graph contains nodes ";
    for(const verti& v: proof_graph_nodes)
    {
      mCRL2log(log::verbose) << v << ", ";
    }
    mCRL2log(log::verbose) << std::endl;
    return proof_graph_nodes;
  }

  bool contains_one_parity(const ParityGame& pg, const std::set<verti>& scc)
  {
    assert(!scc.empty());
    player_t p((player_t)(signed char)(pg.priority(*scc.begin()) % 2));
    for(const verti& v: scc)
    {
      if((player_t)(signed char)(pg.priority(v) % 2) != p)
      {
        return false;
      }
    }
    return true;
  }

  bool transition_exists(const StaticGraph& graph, const std::set<verti>& src, const std::set<verti> dest)
  {
    for(const verti& v: src)
    {
      for(StaticGraph::const_iterator succ = graph.succ_begin(v); succ != graph.succ_end(v); ++succ)
      {
        if(dest.find(*succ) != dest.end())
        {
          return true;
        }
      }
    }
    return false;
  }

  void compute_sink_subgraphs(const ParityGame& pg)
  {
    class scc_decomposition
    {
      std::vector<std::set<verti> > sccs;
      std::vector<bool> m_is_sink;
      std::vector<player_t> m_parity;

    public:
      void set_sink(std::size_t i, player_t parity)
      {
        m_is_sink[i] = true;
        m_parity[i] = parity;
      }

      player_t parity(std::size_t i)
      {
        if(!m_is_sink[i])
        {
          mcrl2::runtime_error("Parity is not defined for this SCC");
        }
        return m_parity[i];
      }

      bool is_sink(const std::size_t& i) { return m_is_sink[i]; }

      //! @return the number of collected SCCs.
      std::size_t size() const { return sccs.size(); }

      //! @return the i-th SCC as vector of vertex indices.
      std::set<verti> &operator[](std::size_t i) { return sccs[i]; }

      //! @return the i-th SCC as const vector of vertex indices.
      const std::set<verti> &operator[](std::size_t i) const { return sccs[i]; }

      /*! Add a strongly connected component to the list.
          @param scc an array of length `size` giving indices of the vertices 
          @param size the size of the component
          @return zero */
      int operator()(const verti scc[], std::size_t size)
      {
          std::set<verti> new_scc;
          new_scc.insert(&scc[0], &scc[size]);
          sccs.push_back(new_scc);
          m_is_sink.push_back(false);
          m_parity.push_back(PLAYER_EVEN);
          return 0;
      }
    };

    scc_decomposition sccs;
    decompose_graph(pg.graph(), sccs);
    mCRL2log(log::verbose) << "Found " << sccs.size() << " SCCs." << std::endl;

    std::set<verti> result;
    for(std::size_t i = 0; i < sccs.size(); ++i)
    {
      // First, check whether only one parity is present
      // in this SCC
      if(!contains_one_parity(pg, sccs[i]))
      {
        continue;
      }

      player_t p((player_t)(signed char)(pg.priority(*sccs[i].begin()) % 2));
      // Second check whether all SCCs that can be
      // reached from this SCC have the same parity.
      // Since SCCs are sorted in reverse topological
      // order, we only have to check for SCCs with smaller
      // indices.
      bool all_succ_are_sink = true;
      player_t all_succ_player(p);
      bool succ_player_set = false;
      for(std::size_t j = 0; j < i; ++j)
      {
        if(transition_exists(pg.graph(), sccs[i], sccs[j]))
        {
          if(!succ_player_set)
          {
            all_succ_player = sccs.parity(i);
            succ_player_set = true;
          }
          if(!sccs.is_sink(j) || sccs.parity(j) != all_succ_player)
          {
            all_succ_are_sink = false;
            break;
          }
        }
      }
      if(all_succ_are_sink && (p == all_succ_player || !transition_exists(pg.graph(), sccs[i], sccs[i])))
      {
        sccs.set_sink(i, all_succ_player);
        result.insert(sccs[i].begin(), sccs[i].end());
      }
    }

    mCRL2log(log::verbose) << "Sink vertices are ";
    for(const verti& v: result)
    {
      mCRL2log(log::verbose) << v << ", ";
    }
    mCRL2log(log::verbose) << std::endl;
  }

  rewriter make_rewriter(const pbes_system::pbes& spec, const data_specification& dataspec, const rewrite_strategy& st)
  {
    std::set<function_symbol> used_functions = pbes_system::find_function_symbols(spec);
    used_functions.insert(sort_bool::not_());
    if(used_functions.find(sort_real::creal()) != used_functions.end())
    {
      // Specification contains real numbers, so we should add all relevant
      // function symbols
      used_functions.insert(less(sort_real::real_()));
      used_functions.insert(less_equal(sort_real::real_()));
      used_functions.insert(greater(sort_real::real_()));
      used_functions.insert(greater_equal(sort_real::real_()));
      used_functions.insert(equal_to(sort_real::real_()));
      for(const function_symbol& f: sort_real::real_generate_functions_code())
      {
        used_functions.insert(f);
      }
    }
    return rewriter(dataspec, used_data_equation_selector(dataspec, used_functions, spec.global_variables()), st);
  }

public:
  symbolic_bisim_algorithm(const pbes_system::pbes& spec, const std::size_t& refine_steps, const rewrite_strategy& st = jitty)
    : rewr(make_rewriter(spec, merge_data_specifications(spec.data(),simplifier::norm_rules_spec()),st))
#ifdef MCRL2_JITTYC_AVAILABLE
    , proving_rewr(make_rewriter(spec, spec.data(), st == jitty ? jitty_prover : jitty_compiling_prover))
#else
    , proving_rewr(make_rewriter(spec, spec.data(), jitty_prover))
#endif
    , m_spec(pbes_system::detail::ppg_pbes(spec).simplify(rewr))
    , m_partition(m_spec,rewr,proving_rewr)
    , m_num_refine_steps(refine_steps)
  {}

  void run()
  {
    mCRL2log(mcrl2::log::verbose) << "Running symbolic bisimulation..." << std::endl;
    const std::chrono::time_point<std::chrono::high_resolution_clock> t_start = 
      std::chrono::high_resolution_clock::now();

    mCRL2log(log::verbose) << m_spec << std::endl;

    // while the (sub) partition is stable, we have to keep
    // refining and searching for proof graphs
    std::size_t num_iterations = 0;
    double total_pg_time = 0.0;
    player_t latest_winner;
    do
    {
      const std::chrono::time_point<std::chrono::high_resolution_clock> t_start_pg_solver = 
        std::chrono::high_resolution_clock::now();

      ParityGame pg;
      m_partition.get_reachable_pg(pg);

      const ParityGame::Strategy& solution = compute_pg_solution(pg);
      if(mCRL2logEnabled(log::verbose))
      {
        pg.write_debug(solution, mcrl2::log::mcrl2_logger().get(log::verbose));
      }
      // compute_sink_subgraphs(pg);

      std::set<verti> proof_graph = compute_subpartition_from_strategy(pg, solution);
      m_partition.set_proof(proof_graph);

      num_iterations++;
      latest_winner = pg.winner(solution,0);
      mCRL2log(log::status) << "End of iteration " << num_iterations << ", " << (latest_winner == PLAYER_EVEN ? "positive" : "negative")
       << " proof graph has size " << proof_graph.size() 
       << ", total amount of blocks " << (m_partition.get_proof_blocks().size() + m_partition.get_other_blocks().size()) << "\n";
      total_pg_time += std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start_pg_solver).count();
    } while(!m_partition.refine_n_steps(m_num_refine_steps));
    mCRL2log(log::info) << "Partition refinement completed in " << 
        std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start).count() << 
        " seconds.\n" << 
        "Time spent on PG solving: " << total_pg_time << " seconds" << std::endl;

    // m_partition.save_bes();
    std::cout << (latest_winner == PLAYER_EVEN ? "true" : "false") << std::endl;
  }
};

} // namespace data
} // namespace mcrl2


#endif // MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H
