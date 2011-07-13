// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef CONCURRENT_RECURSIVE_SOLVER_H_INCLUDED
#define CONCURRENT_RECURSIVE_SOLVER_H_INCLUDED

#include "RecursiveSolver.h"

/*! Parity game solver implementing Zielonka's recursive algorithm. */
class ConcurrentRecursiveSolver : public ParityGameSolver, public virtual Logger
{
public:
    ConcurrentRecursiveSolver(const ParityGame &game);
    ~ConcurrentRecursiveSolver();

    ParityGame::Strategy solve();

private:
    /*! Solves a subgame recursively, or returns false if solving is aborted. */
    bool solve(ParityGame &game, Substrategy &strat);
};

class ConcurrentRecursiveSolverFactory : public ParityGameSolverFactory
{
    ParityGameSolver *create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size );
};

#endif /* ndef RECURSIVE_SOLVER_H_INCLUDED */
