// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef DELOOP_SOLVER_H_INCLUDED
#define DELOOP_SOLVER_H_INCLUDED

#include <deque>
#include <string>
#include <vector>
#include "mcrl2/utilities/logger.h"
#include "SmallProgressMeasures.h"
#include "DenseSet.h"
#include "SCC.h"

/*! A solver that identifies vertices with loops which are won by their owner
    (i.e. when the owner corresponds with the parity of the vertex priority)
    and removes their attractor sets from the game to obtain a loop-less reduced
    game that is then solved with a new solver.

    This REQUIRES that the game graph has been preprocessed with
    SmallProgressMeasures::preprocess_game()!

    Compared to the DecycleSolver, this preprocessor is faster but weaker.

    \see DecycleSolver
*/
class DeloopSolver : public ParityGameSolver
{
public:
    DeloopSolver( const ParityGame &game, ParityGameSolverFactory &pgsf,
                  const verti *vmap, verti vmap_size );
    ~DeloopSolver();

    ParityGame::Strategy solve();

private:
    // SCC callback
    int operator()(const verti *vertices, size_t num_vertices);
    friend class SCC<DeloopSolver>;

protected:
    ParityGameSolverFactory &pgsf_;       //!< Solver factory to use
    const verti             *vmap_;       //!< Current vertex map
    const verti             vmap_size_;   //!< Size of vertex map
};

//! A factory class for DeloopSolver instances.
class DeloopSolverFactory : public ParityGameSolverFactory
{
public:
    DeloopSolverFactory(ParityGameSolverFactory &pgsf)
        : pgsf_(pgsf) { pgsf_.ref(); }
    ~DeloopSolverFactory() { pgsf_.deref(); }

    //! Create a new DeloopSolver instance.
    ParityGameSolver *create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size );

protected:
    ParityGameSolverFactory &pgsf_;     //!< Factory used to create subsolvers
};

#endif /* ndef DELOOP_SOLVER_H_INCLUDED */
