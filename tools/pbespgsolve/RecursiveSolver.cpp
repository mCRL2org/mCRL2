// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "DenseSet.h"
#include "RecursiveSolver.h"
#include "attractor.h"
#include <set>
#include <assert.h>

size_t first_inversion(const ParityGame &game)
{
    size_t d = game.d();
    size_t q = 0;
    while (q < d && game.cardinality(q) == 0) ++q;
    size_t p = q + 1;
    while (p < d && game.cardinality(p) == 0) p += 2;
    if (p > d) p = d;
    return p;
}

RecursiveSolver::RecursiveSolver(const ParityGame &game)
    : ParityGameSolver(game)
{
}

RecursiveSolver::~RecursiveSolver()
{
}

ParityGame::Strategy RecursiveSolver::solve()
{
    ParityGame game;
    game.assign(game_);
    ParityGame::Strategy strategy(game.graph().V(), NO_VERTEX);
    Substrategy substrat(strategy);
    if (!solve(game, substrat)) strategy.clear();
    return strategy;
}

/* Implementation note: the recursive solver might use either a DenseSet or
   a std::set to store vertex sets (which are passed to make_attractor_set).
   The former is faster when the size of these sets is large, but requires O(V)
   time and memory to initialize, which is costly when these sets are small.

   It seems that the benefit of faster lookups during attractor set computation
   usually tips the balance in favor of the DenseSet.

   Note that hash sets cannot readily be used because get_complement() expects
   iterators to produce the set contents in-order.
*/

bool RecursiveSolver::solve(ParityGame &game, Substrategy &strat)
{
    if (aborted()) return false;

    size_t prio;
    while ((prio = first_inversion(game)) < game.d())
    {
        mCRL2log(mcrl2::log::debug) << "prio=" << prio << std::endl;

        const StaticGraph &graph = game.graph();
        const verti V = graph.V();
        std::vector<verti> unsolved;

        // Compute attractor set of minimum priority vertices:
        {
            ParityGame::Player player = (ParityGame::Player)((prio - 1)%2);
            //std::set<verti> min_prio_attr;
            DenseSet<verti> min_prio_attr(0, V);
            for (verti v = 0; v < V; ++v)
            {
                if (game.priority(v) < prio) min_prio_attr.insert(v);
            }
            mCRL2log(mcrl2::log::debug) << "|min_prio|=" << min_prio_attr.size() << std::endl;
            assert(!min_prio_attr.empty());
            make_attractor_set(game, player, min_prio_attr, strat);
            mCRL2log(mcrl2::log::debug) << "|min_prio_attr|=" << min_prio_attr.size() << std::endl;
            if (min_prio_attr.size() == V) break;
            get_complement(V, min_prio_attr.begin(), min_prio_attr.end())
                .swap(unsolved);
        }

        // Solve vertices not in the minimum priority attractor set:
        {
            ParityGame subgame;
            subgame.make_subgame(game, unsolved.begin(), unsolved.end());
            Substrategy substrat(strat, unsolved);
            if (!solve(subgame, substrat)) return false;

            // Compute attractor set of all vertices won by the opponent:
            ParityGame::Player opponent = (ParityGame::Player)(prio%2);
            //std::set<verti> lost_attr;
            DenseSet<verti> lost_attr(0, V);
            for ( std::vector<verti>::const_iterator it = unsolved.begin();
                  it != unsolved.end(); ++it )
            {
                if (strat.winner(*it, game.player(*it)) == opponent)
                {
                    lost_attr.insert(*it);
                }
            }
            mCRL2log(mcrl2::log::debug) << "|lost|=" << lost_attr.size() << std::endl;
            if (lost_attr.empty()) break;
            make_attractor_set(game, opponent, lost_attr, strat);
            mCRL2log(mcrl2::log::debug) << "|lost_attr|=" << lost_attr.size() << std::endl;
            get_complement(V, lost_attr.begin(), lost_attr.end())
                .swap(unsolved);
        }

        // Repeat with subgame of which vertices won by odd have been removed:
        {
            ParityGame subgame;
            subgame.make_subgame(game, unsolved.begin(), unsolved.end());
            Substrategy substrat(strat, unsolved);
            strat.swap(substrat);
            game.swap(subgame);
        }
    }

    // If we get here, then the opponent's winning set was empty; the strategy
    // for most vertices has already been initialized, except for those with
    // minimum priority. Since the whole game is won by the current player, it
    // suffices to pick an arbitrary successor for these vertices:
    const StaticGraph &graph = game.graph();
    const verti V = graph.V();
    for (verti v = 0; v < V; ++v)
    {
        if (game.priority(v) < prio)
        {
            if (game.player(v) == static_cast<int>(game.priority(v)%2))
            {
                strat[v] = *graph.succ_begin(v);
            }
            else
            {
                strat[v] = NO_VERTEX;
            }
        }
    }
    return true;
}

ParityGameSolver *RecursiveSolverFactory::create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size )
{
    (void)vertex_map;       // unused
    (void)vertex_map_size;  // unused

    return new RecursiveSolver(game);
}
