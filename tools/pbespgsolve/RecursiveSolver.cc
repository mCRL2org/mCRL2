// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "RecursiveSolver.h"
#include "attractor.h"
#include <set>
#include "assert.h"

template<class T>
static size_t memory_use(const std::vector<T> &v)
{
    return v.capacity()*sizeof(T);
}

/*! Returns the complement of a vertex set; i.e. an ordered list of all vertex
    indices under V, from which the contents of `vertices' have been removed. */
static std::vector<verti> get_complement( verti V,
                                          const std::set<verti> &vertices )
{
    std::vector<verti> res;
    res.reserve(V - vertices.size());
    std::set<verti>::const_iterator it = vertices.begin();
    for (verti v = 0; v < V; ++v)
    {
        if (it == vertices.end() || v < *it)
        {
            res.push_back(v);
        }
        else
        {
            assert(*it == v);
            ++it;
        }
    }
    assert(it == vertices.end());
    return res;
}

/*! Merges a substrategy into a main strategy, overwriting the existing strategy
    for all vertices with indices in vertex_map. */
void merge_strategies( std::vector<verti> &strategy,
                       const std::vector<verti> &substrat,
                       const std::vector<verti> &vertex_map )
{
    assert(substrat.size() == vertex_map.size());
    for (verti v = 0; v < (verti)vertex_map.size(); ++v)
    {
        strategy[vertex_map[v]] =
            (substrat[v] == NO_VERTEX) ? NO_VERTEX : vertex_map[substrat[v]];
    }
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
    return solve(game(), 0);
}

ParityGame::Strategy RecursiveSolver::solve(const ParityGame &game, int min_prio)
{
    assert(min_prio < game.d());

    const StaticGraph        &graph   = game.graph();
    const verti              V        = graph.V();
    const ParityGame::Player player   = (ParityGame::Player)(min_prio%2);
    const ParityGame::Player opponent = (ParityGame::Player)(1 - min_prio%2);

    if (aborted()) return ParityGame::Strategy();

    if (game.d() - min_prio <= 1)
    {
        // Only one priority left; construct trivial strategy.
        ParityGame::Strategy strategy(V, NO_VERTEX);
        for (verti v = 0; v < V; ++v)
        {
            if (game.player(v) == player) strategy[v] = *graph.succ_begin(v);
        }
        return strategy;
    }

    // Degenerate case: no vertices with this priority exist:
    if (game.cardinality(min_prio) == 0) return solve(game, min_prio + 1);

    ParityGame::Strategy strategy(V, NO_VERTEX);

    // Compute attractor set of minimum priority vertices:
    std::set<verti> min_prio_attr;
    for (verti v = 0; v < V; ++v)
    {
        assert(game.priority(v) >= min_prio);
        if (game.priority(v) == min_prio) min_prio_attr.insert(v);
    }
    assert(!min_prio_attr.empty());
    make_attractor_set(game, player, min_prio_attr, &strategy);

    // Compute attractor set of vertices lost to the opponent:
    std::set<verti> lost_attr;

    if (min_prio_attr.size() < V)
    {
        // Find unsolved vertices so far:
        std::vector<verti> unsolved = get_complement(V, min_prio_attr);
        min_prio_attr.clear();  // free no-longer used memory

        // Create subgame with unsolved vertices:
        ParityGame subgame;
        subgame.make_subgame(game, &unsolved[0], unsolved.size());
        ParityGame::Strategy substrat = solve(subgame, min_prio + 1);

        // Check if solving failed (or was aborted):
        if (substrat.size() != unsolved.size()) return ParityGame::Strategy();

        // Calculate current memory use:
        update_memory_use( subgame.memory_use() +
                           ::memory_use(unsolved) +
                           ::memory_use(substrat) );

        merge_strategies(strategy, substrat, unsolved);

        // Create attractor set of all vertices won by the opponent:
        for (verti v = 0; v < (verti)unsolved.size(); ++v)
        {
            if (subgame.winner(substrat, v) == opponent)
            {
                lost_attr.insert(unsolved[v]);
            }
        }
        make_attractor_set(game, opponent, lost_attr, &strategy);
    }

    if (lost_attr.empty())  // whole game won by current player!
    {
        // Pick an arbitrary edge for minimum-priority vertices:
        for (verti v = 0; v < V; ++v)
        {
            if (game.player(v) == player && strategy[v] == NO_VERTEX)
            {
                assert(game.priority(v) == min_prio);
                strategy[v] = *graph.succ_begin(v);
            }
        }
    }
    else  // opponent wins some vertices
    {
        // Construct subgame without vertices lost to opponent:
        std::vector<verti> unsolved = get_complement(V, lost_attr);
        ParityGame subgame;
        subgame.make_subgame(game, &unsolved[0], unsolved.size());
        ParityGame::Strategy substrat = solve(subgame, min_prio);

        // Check if solving failed (or was aborted):
        if (substrat.size() != unsolved.size()) return ParityGame::Strategy();

        // Calculate current memory use:
        update_memory_use( subgame.memory_use() +
                           ::memory_use(unsolved) +
                           ::memory_use(substrat) );

        merge_strategies(strategy, substrat, unsolved);
    }

    return strategy;
}

ParityGameSolver *RecursiveSolverFactory::create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size )
{
    (void)vertex_map;       // unused
    (void)vertex_map_size;  // unused

    return new RecursiveSolver(game);
}
