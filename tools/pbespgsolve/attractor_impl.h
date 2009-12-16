// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "attractor.h"
#include <queue>                // checkme: needed?
#include <tr1/unordered_set>    // checkme: needed?

template<class SetT>
void make_attractor_set( const ParityGame &game, ParityGame::Player player,
                         SetT &vertices, ParityGame::Strategy *strategy )
{
    const StaticGraph &graph = game.graph();

    std::deque<verti> todo(vertices.begin(), vertices.end());
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
            if (vertices.find(v) != vertices.end()) goto skip_v;

            if (game.player(v) == player)
            {
                // Store strategy for player-controlled vertex:
                if (strategy) (*strategy)[v] = w;
            }
            else  // game.player(v) != player
            {
                // Can the opponent keep the token out of the attractor set?
                for (StaticGraph::const_iterator jt = graph.succ_begin(v);
                     jt != graph.succ_end(v); ++jt)
                {
                    if (vertices.find(*jt) == vertices.end()) goto skip_v;
                }

                // Store strategy for owner-controlled vertex:
                if (strategy) (*strategy)[v] = NO_VERTEX;
            }

            // Add vertex v to the attractor set:
            vertices.insert(v);
            todo.push_back(v);

        skip_v:
            continue;
        }
    }
}
