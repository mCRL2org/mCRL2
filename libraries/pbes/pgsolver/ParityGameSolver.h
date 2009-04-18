// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef PARITY_GAME_SOLVER
#define PARITY_GAME_SOLVER

#include "ParityGame.h"

class ParityGameSolver
{
public:
    ParityGameSolver(const ParityGame &game) : game_(game), aborted_(false) { };
    virtual ~ParityGameSolver() { };

    /*! Solve the game. */
    virtual bool solve() = 0;

    /*! After the game has been solved, this function returns the winner of
        the parity game when starting from vertex i. */
    virtual ParityGame::Player winner(verti v) const = 0;

    /*! Returns an estimation of the peak memory use for this solver. */
    virtual size_t memory_use() const = 0;

    /*! Returns the parity game for this solver instance. */
    const ParityGame &game() const { return game_; }

    /*! Abort the solver. */
    const void abort() { aborted_ = true; }

    /*! Has the solver been aborted? */
    const bool aborted() { return aborted_; }

protected:
    const ParityGame &game_;
    volatile bool aborted_;
};

#endif /* ndef PARITY_GAME_SOLVER */
