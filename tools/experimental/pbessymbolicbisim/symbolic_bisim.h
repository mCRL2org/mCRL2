// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_bisim.h


#ifndef MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H
#define MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H

#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/pbes/solve_structure_graph.h"

#include "partition.h"

namespace mcrl2::data
{

class symbolic_bisim_algorithm
{
  using substitution_t = rewriter::substitution_type;
  using block_t = std::pair<pbes_system::propositional_variable, data_expression>;
  using sg_index_t = pbes_system::structure_graph::index_type;

protected:
  rewriter rewr;
  rewriter proving_rewr;
  pbes_system::detail::ppg_pbes m_spec;
  pbes_system::structure_graph m_structure_graph;
  dependency_graph_partition m_partition;
  std::size_t m_num_refine_steps;

  rewriter make_rewriter(const pbes_system::pbes& spec, const data_specification& dataspec, const rewrite_strategy& st)
  {
    std::set<function_symbol> used_functions = pbes_system::find_function_symbols(spec);
    used_functions.insert(sort_bool::not_());
    if(used_functions.find(sort_real::creal()) != used_functions.end())
    {
      // Specification contains real numbers, so we should add all relevant
      // function symbols since the simplifiers might introduce funtions
      // symbols that are not present in the specificaton.
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
    return rewriter(dataspec, used_data_equation_selector(dataspec, used_functions, spec.global_variables(), false), st);
  }

public:
  symbolic_bisim_algorithm(const pbes_system::pbes& spec, const std::size_t& refine_steps,
    const rewrite_strategy& st = jitty, const simplifier_mode& mode = simplify_auto,
    bool fine_initial = false, bool early_termination = true, bool randomize = false)
    : rewr(make_rewriter(spec, merge_data_specifications(spec.data(),simplifier::norm_rules_spec()),st))
#ifdef MCRL2_JITTYC_AVAILABLE
    , proving_rewr(make_rewriter(spec, spec.data(), st == jitty ? jitty_prover : jitty_compiling_prover))
#else
    , proving_rewr(make_rewriter(spec, spec.data(), jitty_prover))
#endif
    , m_spec(pbes_system::detail::ppg_pbes(spec).simplify(rewr))
    , m_partition(m_spec, rewr, proving_rewr, mode, m_structure_graph, fine_initial, early_termination, randomize)
    , m_num_refine_steps(refine_steps)
  {}

  void run()
  {
    mCRL2log(mcrl2::log::verbose) << "Running symbolic bisimulation..." << std::endl;
    const std::chrono::time_point<std::chrono::high_resolution_clock> t_start =
      std::chrono::high_resolution_clock::now();

    mCRL2log(log::verbose) << m_spec << std::endl;

    bool latest_solution = false;
    if(m_num_refine_steps != 0)
    {
      // while the (sub) partition is stable, we have to keep
      // refining and searching for proof graphs
      std::size_t num_iterations = 0;
      pbes_system::solve_structure_graph_algorithm sg_solver(false, false);
      do
      {
        latest_solution = sg_solver.solve(m_structure_graph);
        std::set<sg_index_t> proof_graph = extract_minimal_structure_graph(m_structure_graph, m_structure_graph.initial_vertex(), latest_solution);
        if(mCRL2logEnabled(log::verbose))
        {
          mCRL2log(log::verbose) << m_structure_graph;
          mCRL2log(log::verbose) << "initial state: " << m_structure_graph.initial_vertex() << std::endl;
          mCRL2log(log::verbose) << "solution " << (latest_solution ? "true" : "false") << std::endl;
          mCRL2log(log::verbose) << "Proof graph contains nodes ";
          for(const sg_index_t& v: proof_graph)
          {
            mCRL2log(log::verbose) << v << ", ";
          }
          mCRL2log(log::verbose) << std::endl;
        }
        m_partition.set_proof(proof_graph);

        num_iterations++;
        mCRL2log(log::status) << "End of iteration " << num_iterations << ", " << (latest_solution ? "positive" : "negative")
         << " proof graph has size " << proof_graph.size()
         << ", total amount of blocks " << (m_partition.size()) << "\n";
      } while(!m_partition.refine_n_steps(m_num_refine_steps, latest_solution));
      mCRL2log(log::info) << "Partition refinement completed in " <<
          std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start).count() <<
          " seconds." << std::endl;

    }
    else
    {
      m_partition.refine_n_steps(0, false);

      latest_solution = pbes_system::solve_structure_graph(m_structure_graph);

      mCRL2log(log::status) << "Amount of blocks " << m_partition.size() << "\n";
      mCRL2log(log::info) << "Partition refinement completed in " <<
          std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t_start).count() <<
          " seconds." << std::endl;
    }

    // m_partition.save_bes();
    std::cout << (latest_solution ? "true" : "false") << std::endl;
  }
};

} // namespace mcrl2::data

#endif // MCRL2_PBESSYMBOLICBISIM_SYMBOLIC_BISIM_H
