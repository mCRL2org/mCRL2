// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/pg/attractor.h"

#include <queue>

template<class ForwardIterator, class SetT>
bool is_subset_of(ForwardIterator it, ForwardIterator end, const SetT &set)
{
    for (; it != end; ++it) if (!set.count(*it)) return false;
    return true;
}

template<class SetT, class StrategyT>
void make_attractor_set( const ParityGame &game, ParityGame::Player player,
                         SetT &vertices, StrategyT &strategy )
{
    std::deque<verti> todo(vertices.begin(), vertices.end());
    return make_attractor_set(game, player, vertices, todo, strategy);
}

template<class SetT, class DequeT, class StrategyT>
void make_attractor_set( const ParityGame &game, ParityGame::Player player,
    SetT &vertices, DequeT &todo, StrategyT &strategy )
{
    const StaticGraph &graph = game.graph();

    while (!todo.empty())
    {
        const verti w = todo.front();
        todo.pop_front();

        // Check all predecessors v of w:
        for (StaticGraph::const_iterator it = graph.pred_begin(w);
             it != graph.pred_end(w); ++it)
        {
            const verti v = *it;

            // Skip predecessors that are already in the attractor set:
            if (vertices.find(v) != vertices.end()) continue;

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
            vertices.insert(v);
            todo.push_back(v);
        }
    }
}

template<class SetT, class StrategyT>
void make_attractor_set_2( const ParityGame &game, ParityGame::Player player,
                           SetT &vertices, StrategyT &strategy )
{
    std::deque<verti> todo(vertices.begin(), vertices.end());
    return make_attractor_set_2(game, player, vertices, todo, strategy);
}

// Experimental, alternate implementation.
// Only uses predecessor edges but uses O(|V|) extra space and O(|E|) extra time.
// TODO: document!
template<class SetT, class DequeT, class StrategyT>
void make_attractor_set_2( const ParityGame &game, ParityGame::Player player,
    SetT &vertices, DequeT &todo, StrategyT &strategy )
{
    const StaticGraph &graph = game.graph();

    // Initialize liberties so that liberties[v] == outdegree of v
    std::vector<verti> liberties(graph.V(), 0);
    for (verti v = 0; v < graph.V(); ++v)
    {
        for (StaticGraph::const_iterator it = graph.pred_begin(v);
             it != graph.pred_end(v); ++it) ++liberties[*it];
    }

    // Mark initial set as included:
    for (typename SetT::const_iterator it = vertices.begin();
         it != vertices.end(); ++it)
    {
        liberties[*it] = 0;
    }

    // Process queue:
    while (!todo.empty())
    {
        const verti w = todo.front();
        todo.pop_front();

        // Check all predecessors v of w:
        for (StaticGraph::const_iterator it = graph.pred_begin(w);
             it != graph.pred_end(w); ++it)
        {
            const verti v = *it;

            // Skip predecessors that are already in the attractor set:
            if (liberties[v] == 0) continue;

            if (game.player(v) == player)
            {
                // Store strategy for player-controlled vertex:
                strategy[v] = w;
                liberties[v] = 0;
            }
            else  // opponent controls vertex
            if (--liberties[v] == 0)
            {
                // Store strategy for opponent-controlled vertex:
                strategy[v] = NO_VERTEX;
            }
            else
            {
                continue;  // not in the attractor set yet!
            }

            // Add vertex v to the attractor set:
            vertices.insert(v);
            todo.push_back(v);
        }
    }
}
