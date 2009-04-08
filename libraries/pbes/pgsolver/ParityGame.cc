#include "ParityGame.h"
#include <map>
#include <algorithm>
#include <stdlib.h>
#include <assert.h>
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

void ParityGame::reset(verti V, int d)
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

void ParityGame::make_subgame( const ParityGame &game,
                               const verti *vertices, verti num_vertices,
                               const Player *winners )
{
    const StaticGraph &graph = game.graph();
    reset(num_vertices + 2, game.d());

    // Create dummy vertex won by even
    const verti v_even = num_vertices + 0;
    vertex_[v_even].player   = PLAYER_EVEN;
    vertex_[v_even].priority = 0;

    // Create dummy vertex won by odd
    const verti v_odd  = num_vertices + 1;
    vertex_[v_odd].player   = PLAYER_ODD;
    vertex_[v_odd].priority = 1;

    // Create a map of old->new vertex indices
    // TODO: replace this with a hash map for better performance?
    std::map<verti, verti> vertex_map;
    for (verti n = 0; n < num_vertices; ++n)
    {
        vertex_[n] = game.vertex_[vertices[n]];
        vertex_map[vertices[n]] = n;
    }

    // Create new edge list
    StaticGraph::edge_list edges;
    for (verti v = 0; v < num_vertices; ++v)
    {
        for ( StaticGraph::const_iterator it = graph.succ_begin(vertices[v]);
              it != graph.succ_end(vertices[v]); ++it )
        {
            verti w;
            std::map<verti, verti>::const_iterator map_it = vertex_map.find(*it);
            if (map_it != vertex_map.end())
            {
                w = map_it->second;
            }
            else
            {
                Player winner = winners[*it];
                assert(winner == PLAYER_EVEN || winner == PLAYER_ODD);
                w = (winner == PLAYER_EVEN) ? v_even : v_odd;
            }
            edges.push_back(std::make_pair(v, w));
        }
    }
    edges.push_back(std::make_pair(v_even, v_even));
    edges.push_back(std::make_pair(v_odd,  v_odd));
    graph_.assign(edges, graph.edge_dir());
    recalculate_cardinalities(num_vertices + 2);
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

void ParityGame::compress_priorities()
{
    // Quickly check if we have anything to compress first:
    if (std::find(cardinality_ + 1, cardinality_ + d_, 0) == cardinality_ + d_)
    {
        return;
    }

    // Find out how to map old priorities to new priorities
    std::vector<int> prio_map(d_, -1);
    int last_prio = 0;
    prio_map[0] = last_prio;
    for (int p = 1; p < d_; ++p)
    {
        if (cardinality_[p] == 0) continue;  // remove priority p
        if (last_prio%2 != p%2) ++last_prio;
        prio_map[p] = last_prio;
    }

    // Remap priorities of all vertices
    for (verti v = 0; v < graph_.V(); ++v)
    {
        assert(prio_map[vertex_[v].priority] >= 0);
        vertex_[v].priority = prio_map[vertex_[v].priority];
    }

    // Update priority limit and cardinality counts
    int new_d = last_prio + 1;
    assert(new_d < d_);
    verti *new_cardinality = new verti[new_d];
    for (int p = 0; p < d_; ++p)
    {
        if (prio_map[p] >= 0)
        {
            new_cardinality[prio_map[p]] += cardinality_[p];
        }
    }
    delete[] cardinality_;
    cardinality_ = new_cardinality;
    d_ = new_d;
}

size_t ParityGame::memory_use() const
{
    size_t res = graph_.memory_use();
    res += sizeof(ParityGameVertex)*graph_.V();     // vertex info
    res += sizeof(verti)*d_;                        // priority frequencies
    return res;
}
