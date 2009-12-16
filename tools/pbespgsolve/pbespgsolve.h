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

#ifndef MCRL2_PBES_PBESPGSOLVE_H
#define MCRL2_PBES_PBESPGSOLVE_H

#ifndef WITH_MCRL2
#define WITH_MCRL2
#endif

#include <iostream>
#include "mcrl2/core/algorithm.h"
#include "ParityGame.h"
#include "SmallProgressMeasures.h"
#include "PredecessorLiftingStrategy.h"
#include "RecursiveSolver.h"
#include "ComponentSolver.h"

namespace mcrl2 {

  namespace pbes_system {

    enum pbespg_solver_type
    {
      spm_solver,
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
      bool verify_solution;

      pbespgsolve_options()
      : solver_type(spm_solver),
      use_scc_decomposition(true),
      verify_solution(true)
      {
      }
    };

    class pbespgsolve_algorithm : public core::algorithm
    {
    protected:
      std::auto_ptr<LiftingStrategyFactory> lift_strat_factory;
      std::auto_ptr<ParityGameSolverFactory> subsolver_factory;
      std::auto_ptr<ParityGameSolverFactory> solver_factory;
      pbespgsolve_options m_options;

    public:

      pbespgsolve_algorithm(unsigned int log_level, pbespgsolve_options options = pbespgsolve_options())
      : core::algorithm(log_level), m_options(options)
      {
        if (options.solver_type == spm_solver)
        {
          // Create a lifting strategy factory:
          lift_strat_factory.reset(
              new PredecessorLiftingStrategyFactory);

          // Create a SPM solver factory:
          solver_factory.reset(
              new SmallProgressMeasuresFactory(*lift_strat_factory));
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
          subsolver_factory = solver_factory;
          solver_factory.reset(
              new ComponentSolverFactory(*subsolver_factory));
        }
      }

      template <typename Container>
      bool run(pbes<Container>& p)
      {
        // Generate the game from a PBES:
        verti goal_v;
        ParityGame pg;
        pg.read_pbes(p, &goal_v); // N.B. mCRL2 could raise an exception here

        // Create a solver:
        std::auto_ptr<ParityGameSolver> solver(solver_factory->create(pg));

        // Solve the game:
        ParityGame::Strategy solution = solver->solve();
        if (solution.empty())
        {
          throw mcrl2::runtime_error("pbespgsolve: solving failed!\n");
        }

        // Optional: verify the solution
        if (m_options.verify_solution && !pg.verify(solution))
        {
          throw mcrl2::runtime_error("pbespgsolve: verification of the solution failed!\n");
        }

        // Print winner:
        if (pg.winner(solution, goal_v) == ParityGame::PLAYER_EVEN)
          std::clog << "Property holds.\n";
        else
          std::clog << "Property DOES NOT hold.\n";

        return true;
      }
    };

  } // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESPGSOLVE_H
