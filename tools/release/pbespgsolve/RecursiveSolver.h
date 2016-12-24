// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef RECURSIVE_SOLVER_H_INCLUDED
#define RECURSIVE_SOLVER_H_INCLUDED

#include "mcrl2/utilities/logger.h"
#include "ParityGameSolver.h"

/*! Provides a view of a strategy corresponding to a subset of the vertex set.
    Note that elements of the substrategy can be written to, and the underlying
    global strategy is then updated accordingly, transparently mapping local
    to global vertex indices. */
class Substrategy
{
private:
    friend class Reference;

    class Reference
    {
    public:
        Reference(Substrategy &s, verti v) : substrat_(s), v_(s.global(v)) { }

        Reference &operator=(verti w)
        {
            if (w != NO_VERTEX) w = substrat_.global(w);
            substrat_.strategy_[v_] = w;
            return *this;
        }

    private:
        Substrategy &substrat_;
        verti v_;
    };

public:
    //! Constructs a strategy for all the vertices in a global strategy.
    Substrategy(ParityGame::Strategy &strategy)
        : strategy_(strategy)
    {
    }

    //! Constructs a substrategy from an existing (sub)strategy and vertex map.
    Substrategy(const Substrategy &substrat, std::vector<verti> vmap)
        : strategy_(substrat.strategy_)
    {
        global_.resize(vmap.size());
        for (size_t i = 0; i < global_.size(); ++i)
        {
            global_[i] = substrat.global(vmap[i]);
        }
    }

    //! Swaps this substrategy ovbect with another.
    void swap(Substrategy &other)
    {
        strategy_.swap(other.strategy_);
        global_.swap(other.global_);
    }

    //! Returns a write-only reference to the strategy for vertex `v`.
    Reference operator[](verti v)
    {
        return Reference(*this, v);
    }

    //! Returns the winner for vertex `v` assuming it is controlled by `p`.
    ParityGame::Player winner(verti v, ParityGame::Player p)
    {
        if (strategy_[global(v)] == NO_VERTEX) p = ParityGame::Player(1 - p);
        return p;
    }

    //! Maps local to global vertex index
    verti global(verti v) const
    {
        return global_.empty() ? v : global_[v];
    }

private:
    //! Reference to the global strategy
    ParityGame::Strategy &strategy_;

    //! Mapping from local to global vertex indices, or empty for identity.
    std::vector<verti> global_;
};

/*! Returns the first inversion of parity, i.e. the least priority `p` such that
    some vertices exist with priorities p and q, where q < p and q%2 != p%2.
    If there are no inversions, game.d() is returned instead. */
int first_inversion(const ParityGame &game);


/*! Parity game solver implementing Zielonka's recursive algorithm. */
class RecursiveSolver : public ParityGameSolver
{
public:
    RecursiveSolver(const ParityGame &game);
    ~RecursiveSolver();

    ParityGame::Strategy solve();

private:
    /*! Solves a subgame recursively, or returns false if solving is aborted. */
    bool solve(ParityGame &game, Substrategy &strat);
};

//! Factory object for RecursiveSolver instances.
class RecursiveSolverFactory : public ParityGameSolverFactory
{
    //! Returns a new ResuriveSolver instance.
    ParityGameSolver *create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size );
};

#endif /* ndef RECURSIVE_SOLVER_H_INCLUDED */
