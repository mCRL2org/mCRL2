// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/pg/PredecessorLiftingStrategy.h"
#include "mcrl2/pg/RecursiveSolver.h"
#include "mcrl2/pg/ComponentSolver.h"

int main(int argc, char *argv[])
{
  // you get these values from where-ever:
  bool use_spm = true;    // use SPM solver?
  bool use_rec = false;   // use recursive solver?
  bool use_scc = true;    // use SCC decomposition?
  bool verify  = true;    // verify solution?

  if (argc != 2)
  {
    printf("usage: %s <pbes>\n", argv[0]);
    return 0;
  }

  std::shared_ptr<LiftingStrategyFactory> lift_strat_factory;
  std::shared_ptr<ParityGameSolverFactory> subsolver_factory;
  std::shared_ptr<ParityGameSolverFactory> solver_factory;

  if (use_spm)
  {
    // Create a lifting strategy factory:
    lift_strat_factory.reset(
        new PredecessorLiftingStrategyFactory );

    // Create a SPM solver factory:
    solver_factory.reset(
        new SmallProgressMeasuresSolverFactory(lift_strat_factory, 2, true) );
  }
  else if (use_rec)
  {
    // Create a recursive solver factory:
    solver_factory.reset( new RecursiveSolverFactory );
  }
  else
  {
    printf("no solver chosen!\n");
    return 1;
  }

  if (use_scc)
  {
    // Wrap solver factory into a component solver factory:
    subsolver_factory = solver_factory;
    solver_factory.reset(
        new ComponentSolverFactory(*subsolver_factory) );
  }

  // Generate the game from a PBES:
  verti goal_v;
  ParityGame pg;
  std::string path = argv[1];
  if (path == "-")
  {
    path = "";
  }
  pg.read_pbes(path, &goal_v);  // N.B. mCRL2 could raise an exception here

  // Create a solver:
  std::shared_ptr<ParityGameSolver> solver(solver_factory->create(pg));

  // Solve the game:
  ParityGame::Strategy solution = solver->solve();
  if (solution.empty())
  {
    printf("solving failed!\n");
    return 1;
  }

  // Optional: verify the solution
  if (verify && !pg.verify(solution, nullptr))
  {
    printf("verification failed!\n");
    return 1;
  }

  // Print winner:
  if (pg.winner(solution, goal_v) == PLAYER_EVEN)
  {
    printf("Property holds.\n");
  }
  else
  {
    printf("Property DOES NOT hold.\n");
  }

  return 0;
}
