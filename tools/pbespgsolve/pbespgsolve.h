// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tools/pbespgsolve/pbespgsolve.h
/// \brief add your file description here.

#ifndef MCRL2_TOOLS_PBESPGSOLVE_H
#define MCRL2_TOOLS_PBESPGSOLVE_H

#ifndef WITH_MCRL2
#define WITH_MCRL2
#endif

#include <iostream>
#include "mcrl2/utilities/execution_timer.h"
#include "ParityGame.h"
#include "SmallProgressMeasures.h"
#include "PredecessorLiftingStrategy.h"
#include "RecursiveSolver.h"
#include "ComponentSolver.h"
#include "DecycleSolver.h"
#include "DeloopSolver.h"

namespace mcrl2
{

namespace pbes_system
{

enum pbespg_solver_type
{
  spm_solver,
  alternative_spm_solver,
  recursive_solver
};

/// \return A string representation of the edge direction.

inline
std::string print(StaticGraph::EdgeDirection edge_direction)
{
  if (edge_direction == StaticGraph::EDGE_SUCCESSOR)
  {
    return "successor";
  }
  else if (edge_direction == StaticGraph::EDGE_PREDECESSOR)
  {
    return "predecessor";
  }
  else if (edge_direction == StaticGraph::EDGE_BIDIRECTIONAL)
  {
    return "bidirectional";
  }
  return "unknown edge direction";
}

/// \return A string representation of the solver type.

inline
std::string print(pbespg_solver_type solver_type)
{
  if (solver_type == spm_solver)
  {
    return "spm_solver";
  }
  else if (solver_type == alternative_spm_solver)
  {
    return "alternative_spm_solver";
  }
  else if (solver_type == recursive_solver)
  {
    return "recursive_solver";
  }
  return "unknown solver";
}

struct pbespgsolve_options
{
  pbespg_solver_type solver_type;
  bool use_scc_decomposition;
  bool use_decycle_solver;
  bool use_deloop_solver;
  bool verify_solution;
  data::rewriter::strategy rewrite_strategy;

  pbespgsolve_options()
    : solver_type(spm_solver),
      use_scc_decomposition(true),
      use_decycle_solver(false),
      use_deloop_solver(true),
      verify_solution(true),
      rewrite_strategy(data::rewriter::jitty)
  {
  }
};

class pbespgsolve_algorithm
{
  protected:
    std::auto_ptr<ParityGameSolverFactory> solver_factory;
    mcrl2::utilities::execution_timer& m_timer;
    pbespgsolve_options m_options;

  public:

    pbespgsolve_algorithm(mcrl2::utilities::execution_timer& timing, pbespgsolve_options options = pbespgsolve_options())
      : m_timer(timing),
        m_options(options)
    {
      if (options.solver_type == spm_solver || options.solver_type == alternative_spm_solver)
      {
        bool alternative_solver = (options.solver_type == alternative_spm_solver);

        // Create a SPM solver factory:
        solver_factory.reset(
          new SmallProgressMeasuresSolverFactory
                (new PredecessorLiftingStrategyFactory, alternative_solver)
        );
      }
      else if (options.solver_type == recursive_solver)
      {
        // Create a recursive solver factory:
        solver_factory.reset(new RecursiveSolverFactory);
      }
      else
      {
        throw mcrl2::runtime_error("pbespgsolve: unknown solver type");
      }

      if (options.use_scc_decomposition)
      {
        // Wrap solver factory into a component solver factory:
        solver_factory.reset(
          new ComponentSolverFactory(*solver_factory.release()));
      }

      if (options.use_decycle_solver && options.use_deloop_solver)
      {
        throw mcrl2::runtime_error("pbespgsolve: cannot use self-loop removal and cycle removal simultaneously");
      }

      if (options.use_decycle_solver)
      {
        solver_factory.reset(
          new DecycleSolverFactory(*solver_factory.release()));
      }

      if (options.use_deloop_solver)
      {
        solver_factory.reset(
          new DeloopSolverFactory(*solver_factory.release()));
      }
    }

    template <typename Container>
    bool run(pbes<Container>& p)
    {
      m_timer.start("initialization");
      mCRL2log(log::verbose) << "Generating parity game..."  << std::endl;
      // Generate the game from a PBES:
      verti goal_v;
      ParityGame pg;

      pg.assign_pbes(p, &goal_v, StaticGraph::EDGE_BIDIRECTIONAL, m_options.rewrite_strategy); // N.B. mCRL2 could raise an exception here

      mCRL2log(log::verbose) << "Solving..." << std::endl;

      // Create a solver:
      std::auto_ptr<ParityGameSolver> solver(solver_factory->create(pg));
      m_timer.finish("initialization");

      // Solve the game:
      m_timer.start("solving");
      ParityGame::Strategy solution = solver->solve();
      m_timer.finish("solving");

      if (solution.empty())
      {
        throw mcrl2::runtime_error("pbespgsolve: solving failed!\n");
      }

      verti error_vertex;

      // Optional: verify the solution
      if (m_options.verify_solution && !pg.verify(solution, &error_vertex))
      {
        throw mcrl2::runtime_error("pbespgsolve: verification of the solution failed!\n");
      }

      return pg.winner(solution, goal_v) == ParityGame::PLAYER_EVEN;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_TOOLS_PBESPGSOLVE_H
