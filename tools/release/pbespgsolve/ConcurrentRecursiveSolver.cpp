// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// N.B. This is copied pretty much verbatim from RecursiveSolver!
//      Should be integrated in a nice way, later.

#include "DenseSet.h"
#include "ConcurrentRecursiveSolver.h"
#include "attractor.h"
#include <set>
#include <cassert>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_queue.h>

static std::vector<verti> concurrent_get_complement(
    const std::vector< tbb::atomic<char> > &vertices )
{
    const verti V = (verti)vertices.size();
    std::vector<verti> res;
    for (verti v = 0; v < V; ++v)
    {
        if (!vertices[v]) res.push_back(v);
    }
    return res;
}

static bool is_subset_of(
    StaticGraph::const_iterator it, StaticGraph::const_iterator end,
    const std::vector< tbb::atomic<char> > &vertices)
{
    for (; it != end; ++it) if (!vertices[*it]) return false;
    return true;
}

void concurrent_make_attractor_set(
    const ParityGame &game, ParityGame::Player player,
    std::vector< tbb::atomic<char> > &vertices,
    tbb::concurrent_queue<verti> &todo,
    Substrategy &strategy )
{
    const StaticGraph &graph = game.graph();

    #pragma omp parallel
    for (verti w; todo.try_pop(w); )
    {
        // Check all predecessors v of w:
        for (StaticGraph::const_iterator it = graph.pred_begin(w);
            it != graph.pred_end(w); ++it)
        {
            const verti v = *it;

            // Skip predecessors that are already in the attractor set:
            if (vertices[v]) continue;

            if (game.player(v) == player)
            {
                // Store strategy for player-controlled vertex:
                strategy[v] = w;
            }
            else  // opponent controls vertex
            if (is_subset_of(graph.succ_begin(v), graph.succ_end(v), vertices))
            {
                // Store strategy for opponent-controlled vertex:
                strategy[v] = NO_VERTEX;
            }
            else
            {
                continue;  // not in the attractor set yet!
            }

            // Add vertex v to the attractor set:
            if (vertices[v].compare_and_swap(1, 0) == 0) todo.push(v);
        }
    }
}

ConcurrentRecursiveSolver::ConcurrentRecursiveSolver(const ParityGame &game)
    : ParityGameSolver(game)
{
    assert(sizeof(tbb::atomic<char>) == 1);
}

ConcurrentRecursiveSolver::~ConcurrentRecursiveSolver()
{
}

ParityGame::Strategy ConcurrentRecursiveSolver::solve()
{
    ParityGame game;
    game.assign(game_);
    ParityGame::Strategy strategy(game.graph().V(), NO_VERTEX);
    Substrategy substrat(strategy);
    if (!solve(game, substrat)) strategy.clear();
    return strategy;
}

bool ConcurrentRecursiveSolver::solve(ParityGame &game, Substrategy &strat)
{
    if (aborted()) return false;

    int prio;
    while ((prio = first_inversion(game)) < game.d())
    {
        const StaticGraph &graph = game.graph();
        const verti V = graph.V();
        std::vector<verti> unsolved;

        // Compute attractor set of minimum priority vertices:
        {
            ParityGame::Player player = (ParityGame::Player)((prio - 1)%2);
            std::vector< tbb::atomic<char> > min_prio_attr(V);
            tbb::concurrent_queue<verti> min_prio_attr_queue;
            for (verti v = 0; v < V; ++v)
            {
                if (game.priority(v) < prio)
                {
                    min_prio_attr[v] = 1;
                    min_prio_attr_queue.push(v);
                }
            }
            assert(!min_prio_attr_queue.empty());
            concurrent_make_attractor_set( game, player, min_prio_attr,
                                           min_prio_attr_queue, strat );
            concurrent_get_complement(min_prio_attr).swap(unsolved);
            if (unsolved.empty()) break;
        }

        // Solve vertices not in the minimum priority attractor set:
        {
            ParityGame subgame;
            subgame.make_subgame_threads(game, &unsolved[0], (verti)unsolved.size(), true);
            Substrategy substrat(strat, unsolved);
            if (!solve(subgame, substrat)) return false;

            // Compute attractor set of all vertices won by the opponent:
            ParityGame::Player opponent = (ParityGame::Player)(prio%2);
            std::vector<  tbb::atomic<char> > lost_attr(V);
            tbb::concurrent_queue<verti> lost_attr_queue;
            for ( std::vector<verti>::const_iterator it = unsolved.begin();
                  it != unsolved.end(); ++it )
            {
                if (strat.winner(*it, game.player(*it)) == opponent)
                {
                    lost_attr[*it] = 1;
                    lost_attr_queue.push(*it);
                }
            }
            if (lost_attr_queue.empty()) break;
            concurrent_make_attractor_set( game, opponent, lost_attr,
                                           lost_attr_queue, strat );
            concurrent_get_complement(lost_attr).swap(unsolved);
        }

        // Repeat with subgame of which vertices won by odd have been removed:
        {
            ParityGame subgame;
            subgame.make_subgame_threads(game, &unsolved[0], (verti)unsolved.size(), true);
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
            if (game.player(v) == game.priority(v)%2)
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

ParityGameSolver *ConcurrentRecursiveSolverFactory::create(
    const ParityGame &game, const verti *vertex_map, verti vertex_map_size )
{
    (void)vertex_map;       // unused
    (void)vertex_map_size;  // unused

    return new ConcurrentRecursiveSolver(game);
}
