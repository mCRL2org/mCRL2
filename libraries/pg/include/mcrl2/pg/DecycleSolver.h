// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_PG_DECYCLE_SOLVER_H
#define MCRL2_PG_DECYCLE_SOLVER_H

#include "mcrl2/pg/SmallProgressMeasures.h"
#include "mcrl2/pg/DenseSet.h"
#include "mcrl2/pg/SCC.h"
#include "mcrl2/utilities/logger.h"

#include <deque>
#include <string>
#include <vector>

/*! A partial solver that efficiently solves cycles controlled by a single
    player.

    Specifically, it removes all i-dominated cycles controlled by player p for
    all values of i and p where i%2 == p, including the vertices in their
    attractor sets, and then calls a general solver to solve the remaining
    subgame.

    This is a generalization of the DeloopSolver, which is limited to detecting
    cycles of length 1. The only downside to using the DecycleSolver is that it
    is slower.

    \see DeloopSolver
*/
class DecycleSolver : public ParityGameSolver
{
public:
    DecycleSolver( const ParityGame &game, ParityGameSolverFactory &pgsf,
                   const verti *vmap, verti vmap_size );
    ~DecycleSolver();

    ParityGame::Strategy solve();

protected:
    ParityGameSolverFactory &pgsf_;       //!< Solver factory to use
    const verti             *vmap_;       //!< Current vertex map
    const verti             vmap_size_;   //!< Size of vertex map
};

//! A factory class for DecycleSolver instances.
class DecycleSolverFactory : public ParityGameSolverFactory
{
public:
    DecycleSolverFactory(ParityGameSolverFactory &pgsf)
        : pgsf_(pgsf) { pgsf_.ref(); }
    ~DecycleSolverFactory() { pgsf_.deref(); }

    //! Return a new DecycleSolver instance.
    ParityGameSolver *create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size );

protected:
    ParityGameSolverFactory &pgsf_;     //!< Factory used to create subsolvers
};

#endif /* ndef MCRL2_PG_DECYCLE_SOLVER_H */
