// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef CONCURRENT_RECURSIVE_SOLVER_H_INCLUDED
#define CONCURRENT_RECURSIVE_SOLVER_H_INCLUDED

#include "RecursiveSolver.h"

//! Concurrent implementation of Zielonka's recursive algorithm.
class ConcurrentRecursiveSolver : public ParityGameSolver, public virtual Logger
{
public:
    ConcurrentRecursiveSolver(const ParityGame &game);
    ~ConcurrentRecursiveSolver();

    ParityGame::Strategy solve();

private:
    //! Solves a subgame recursively, or returns false if solving is aborted.
    bool solve(ParityGame &game, Substrategy &strat);
};

//! Factory class for ConcurrentRecursiveSolver instances.
class ConcurrentRecursiveSolverFactory : public ParityGameSolverFactory
{
    //! Return a new ConcurrentRecursiveSolver instance.
    ParityGameSolver *create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size );
};

#endif /* ndef RECURSIVE_SOLVER_H_INCLUDED */
