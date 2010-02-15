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
#include "Abortable.h"
#include <vector>

/*! Merges a substrategy into a main strategy, overwriting the existing strategy
    for all vertices with indices in vertex_map. */
void merge_strategies( std::vector<verti> &strategy,
                       const std::vector<verti> &substrat,
                       const std::vector<verti> &vertex_map );


/*! Abstract base class for parity game solvers: classes that encapsulate
    algorithms to compute the winning set and optimal strategies in a game. */
class ParityGameSolver : public Abortable
{
public:
    ParityGameSolver(const ParityGame &game)
        : game_(game), max_memory_size(0) { };
    virtual ~ParityGameSolver() { };

    /*! Solve the game and return the strategies for both players. */
    virtual ParityGame::Strategy solve() = 0;

    /*! Returns an estimation of the peak memory use for this solver. */
    size_t memory_use() const { return max_memory_size; }

    /*! Returns the parity game for this solver instance. */
    const ParityGame &game() const { return game_; }

protected:
    void update_memory_use(size_t current_size) {
        if (current_size > max_memory_size) max_memory_size = current_size;
    }

protected:
    const ParityGame    &game_;     //!< Game being solved
    size_t              max_memory_size;    //!< Max. amount of memory used
};

/*! Abstract base class for parity game solver factories. */
class ParityGameSolverFactory
{
public:
    virtual ~ParityGameSolverFactory() { };

    /*! Create a parity game solver for the given game.
        \param vertex_map maps vertex indices from the given subgame to the
            main game. (This allows the SPM solver to correctly collect per-
            vertex lifting statistics even if the game is decomposed first.)
        \param vertex_map_size number of vertices mapped */
    virtual ParityGameSolver *create( const ParityGame &game,
        const verti *vertex_map = NULL, verti vertex_map_size = 0 ) = 0;
};

#endif /* ndef PARITY_GAME_SOLVER */
