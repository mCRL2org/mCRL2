// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "ParityGame.h"
#include <algorithm>
#include <deque>
#include <map>
#include <stdlib.h>
#include <assert.h>

ParityGame::ParityGame()
    : d_(0), vertex_(NULL), cardinality_(NULL)
{
}

ParityGame::~ParityGame()
{
    delete[] vertex_;
    delete[] cardinality_;
}

void ParityGame::clear()
{
    delete[] vertex_;
    delete[] cardinality_;

    d_ = 0;
    graph_.clear();
    vertex_ = NULL;
    cardinality_ = NULL;
}

void ParityGame::assign(const ParityGame &game)
{
    if (&game == this) return;

    graph_.assign(game.graph_);
    verti V = graph_.V();
    reset(V, game.d_);
    std::copy(game.vertex_, game.vertex_ + V, vertex_);
    recalculate_cardinalities(V);
}

void ParityGame::reset(verti V, size_t d)
{
    delete[] vertex_;
    delete[] cardinality_;

    d_ = d;
    vertex_ = new ParityGameVertex[V];
    cardinality_ = new verti[d_];
}

void ParityGame::recalculate_cardinalities(verti num_vertices)
{
    std::fill(cardinality_, cardinality_ + d_, 0);
    for (verti v = 0; v < num_vertices; ++v)
    {
        cardinality_[vertex_[v].priority] += 1;
    }
}

void ParityGame::make_random( verti V, unsigned out_deg,
                              StaticGraph::EdgeDirection edge_dir, int d )
{
    graph_.make_random(V, out_deg, edge_dir);
    reset(V, d);
    for (verti v = 0; v < V; ++v)
    {
        vertex_[v].player   = (rand()%2 == 0) ? PLAYER_EVEN : PLAYER_ODD;
        vertex_[v].priority = rand()%d;
    }
    recalculate_cardinalities(V);
}

void ParityGame::make_dual()
{
    // For each vertex, invert player and increase priority by one
    for (verti v = 0; v < graph_.V(); ++v)
    {
        vertex_[v].player   = (Player)vertex_[v].player ^ 1;
        vertex_[v].priority = vertex_[v].priority + 1;
    }

    // Update priority counts (move each on space to the right)
    verti *new_cardinality = new verti[d_ + 1];
    new_cardinality[0] = 0;
    std::copy(cardinality_, cardinality_ + d_, new_cardinality + 1);
    delete[] cardinality_;
    cardinality_ = new_cardinality;
    d_ = d_ + 1;

    // Try to compress priorities
    compress_priorities();
}

void ParityGame::shuffle(const std::vector<verti> &perm)
{
    // N.B. maximum priority and priorities cardinalities remain unchanged.

    /* NOTE: shuffling could probably be done more efficiently (in-place?)
             if performance becomes an issue. */

    // Create new edge list
    StaticGraph::edge_list edges;
    for (verti v = 0; v < graph_.V(); ++v)
    {
        for ( StaticGraph::const_iterator it = graph_.succ_begin(v);
              it != graph_.succ_end(v); ++it )
        {
            verti w = *it;
            edges.push_back(std::make_pair(perm[v], perm[w]));
        }
    }
    graph_.assign(edges, graph_.edge_dir());

    // Create new vertex info
    ParityGameVertex *new_vertex = new ParityGameVertex[graph_.V()];
    for (verti v = 0; v < graph_.V(); ++v) new_vertex[perm[v]] = vertex_[v];
    delete vertex_;
    vertex_ = new_vertex;
}

ParityGame::Player ParityGame::compress_priorities( const verti cardinality[],
                                                    bool preserve_parity )
{
    if (cardinality == 0) cardinality = cardinality_;

    // Quickly check if we have anything to compress first:
    if ( empty() || std::find( cardinality + preserve_parity,
                               cardinality + d_, 0 ) == cardinality + d_ )
    {
        return (d_ == 0) ? PLAYER_NONE : PLAYER_EVEN;
    }

    // Find out how to map old priorities to new priorities
    std::vector<size_t> prio_map(d_, std::numeric_limits<size_t>::max());
    size_t first_prio = 0, last_prio = 0;
    if (!preserve_parity)
    {
        // Find lowest priority in use:
        while (cardinality[first_prio] == 0) ++first_prio;
        assert(first_prio < d_);  // fails only if cardinality count is invalid!
    }
    bool swap_players = first_prio%2 != 0;
    prio_map[first_prio] = last_prio;
    for (size_t p = first_prio + 1; p < d_; ++p)
    {
        if (cardinality[p] == 0) continue;  // remove priority p
        bool last_prio_odd = last_prio%2 != 1;
        bool p_odd = p%2 != 1;
        if ((last_prio_odd != p_odd) != swap_players) ++last_prio;
        prio_map[p] = last_prio;
    }

    // Update priority limit and cardinality counts
    size_t new_d = last_prio + 1;
    verti *new_cardinality = new verti[new_d];
    std::fill(new_cardinality, new_cardinality + new_d, 0);
    for (size_t p = 0; p < d_; ++p)
    {
      if (prio_map[p] != std::numeric_limits<size_t>::max())
      {
        new_cardinality[prio_map[p]] += cardinality_[p];
      }
    }
    delete[] cardinality_;
    cardinality_ = new_cardinality;
    d_ = new_d;

    // Remap priorities and players of all vertices
    for (verti v = 0; v < graph_.V(); ++v)
    {
        assert(prio_map[vertex_[v].priority] != std::numeric_limits<size_t>::max());
        vertex_[v].priority = prio_map[vertex_[v].priority];
        if (swap_players) vertex_[v].player = Player(1 - vertex_[v].player);
    }

    return swap_players ? PLAYER_ODD : PLAYER_EVEN;
}

size_t ParityGame::propagate_priority( verti v, StaticGraph::const_iterator it,
                                             StaticGraph::const_iterator end )
{
    size_t p = priority(v), q = 0;
    for ( ; it != end; ++it)
    {
        verti w = *it;
        size_t r = priority(w);
        if (r >= p) return 0;
        if (r > q) q = r;
    }
    vertex_[v].priority = q;
    --cardinality_[p];
    ++cardinality_[q];
    return p - q;
}

/* N.B. this method is designed to be reasonably fast and use little memory
    in the common case that few priorities can be propagated, which is why the
    algorithm starts with a first pass looking for vertices which can be
    updated, rather than putting them all in the initial queue, which would be
    simpler but require more memory up-front. */
size_t ParityGame::propagate_priorities()
{
    size_t res = 0;
    std::deque<verti> todo;

    // Make an initial pass to look for updatable vertices:
    for (verti v = 0; v < graph_.V(); ++v)
    {
        if (priority(v) > 0)
        {
            size_t change = propagate_priority(v, graph_.succ_begin(v),
                                               graph_.succ_end(v) )
                       + propagate_priority(v, graph_.pred_begin(v),
                                               graph_.pred_end(v) );
            if (change > 0) {
                res += change;
                todo.push_back(v);
            }
        }
    }

    // Check neighbours of updated vertices again:
    while (!todo.empty())
    {
        verti w = todo.front();
        size_t p = priority(w);
        todo.pop_front();

        // Perform backwards propagation on predecessors:
        for ( StaticGraph::const_iterator it = graph_.pred_begin(w);
              it != graph_.pred_end(w); ++it )
        {
            verti v = *it;
            if (priority(v) > p)
            {
                size_t change = propagate_priority(v, graph_.succ_begin(v),
                                                   graph_.succ_end(v) );
                if (change > 0) {
                    res += change;
                    todo.push_back(v);
                }
            }
        }

        // Perform forwards propagation on successors:
        for ( StaticGraph::const_iterator it = graph_.succ_begin(w);
              it != graph_.succ_end(w); ++it )
        {
            verti v = *it;
            if (priority(v) > p)
            {
                size_t change = propagate_priority(v, graph_.pred_begin(v),
                                                   graph_.pred_end(v) );
                if (change > 0) {
                    res += change;
                    todo.push_back(v);
                }
            }
        }
    }

    return res;
}

size_t ParityGame::memory_use() const
{
    size_t res = graph_.memory_use();
    res += sizeof(ParityGameVertex)*graph_.V();     // vertex info
    res += sizeof(verti)*d_;                        // priority frequencies
    return res;
}

bool ParityGame::proper() const
{
    for (verti v = 0; v < graph_.V(); ++v)
    {
        if (graph_.succ_begin(v) == graph_.succ_end(v)) return false;
    }
    return true;
}

void ParityGame::swap(ParityGame &pg)
{
    std::swap(d_, pg.d_);
    std::swap(graph_, pg.graph_);
    std::swap(vertex_, pg.vertex_);
    std::swap(cardinality_, pg.cardinality_);
}
