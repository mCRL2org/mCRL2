// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "Graph.h"
#include <assert.h>
#include <stdlib.h>
#include <algorithm>
#include <utility>
#include <vector>

StaticGraph::StaticGraph()
    : successors_(NULL), predecessors_(NULL),
      successor_index_(NULL), predecessor_index_(NULL)
{
    reset(0, 0, EDGE_SUCCESSOR);
}

StaticGraph::~StaticGraph()
{
    delete[] successors_;
    delete[] predecessors_;
    delete[] successor_index_;
    delete[] predecessor_index_;
}

void StaticGraph::reset(verti V, edgei E, EdgeDirection edge_dir)
{
    V_ = V;
    E_ = E;
    edge_dir_ = edge_dir;

    delete[] successors_;
    delete[] predecessors_;
    delete[] successor_index_;
    delete[] predecessor_index_;

    if ((edge_dir & EDGE_SUCCESSOR))
    {
        successors_      = new verti[E];
        successor_index_ = new edgei[V + 1];
        for (verti v = 0; v <= V; ++v) successor_index_[v] = 0;
    }
    else
    {
        successors_      = NULL;
        successor_index_ = NULL;
    }

    if ((edge_dir_ & EDGE_PREDECESSOR))
    {
        predecessors_      = new verti[E];
        predecessor_index_ = new edgei[V + 1];
        for (verti v = 0; v <= V; ++v) predecessor_index_[v] = 0;
    }
    else
    {
        predecessors_      = NULL;
        predecessor_index_ = NULL;
    }
}

static bool edge_cmp_forward ( const std::pair<verti, verti> &a,
                               const std::pair<verti, verti> &b )
{
    return a.first < b.first || (a.first == b.first && a.second < b.second);
}

static bool edge_cmp_backward( const std::pair<verti, verti> &a,
                               const std::pair<verti, verti> &b )
{
    return a.second < b.second || (a.second == b.second && a.first < b.first);
}

void StaticGraph::make_random(verti V, unsigned out_deg, EdgeDirection edge_dir)
{
    /* Some assumptions on the RNG output range: */
    assert(RAND_MAX >= 2*out_deg);
    assert(RAND_MAX >= V);

    /* Create a random edge set, with at least one outgoing edge per node,
       and an average out-degree `out_deg`, without any duplicate edges (but
       possibly with self-edges). */
    edge_list edges;
    std::vector<verti> neighbours(V);
    for (verti i = 0; i < V; ++i) neighbours[i] = i;
    for (verti i = 0; i < V; ++i)
    {
        unsigned N = 1 + rand()%(2*out_deg - 1);

        for (unsigned n = 0; n < N && n < V; ++n)
        {
            std::swap(neighbours[n], neighbours[n + rand()%(V - n)]);
            edges.push_back(std::make_pair(i, neighbours[n]));
        }
    }

    /* Create graph from edge set */
    assign(edges, edge_dir);
}

void StaticGraph::assign(edge_list edges, EdgeDirection edge_dir)
{
    // Find number of vertices
    verti V = 0;
    for (edge_list::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        if (it->first  >= V) V = it->first  + 1;
        if (it->second >= V) V = it->second + 1;
    }

    // Count number of vertices
    edgei E = (edgei)edges.size();
    assert(E == edges.size());  /* detect integer overflow */

    /* Reallocate memory */
    reset(V, E, edge_dir);

    if (edge_dir_ & EDGE_SUCCESSOR)
    {
        /* Sort edges by predecessor first, successor second */
        if (!std::is_sorted(edges.begin(), edges.end(), std::ptr_fun(&edge_cmp_forward)))
        {
            std::sort(edges.begin(), edges.end(), edge_cmp_forward);
        }

        /* Create successor index */
        edgei pos = 0;
        for (verti v = 0; v < V; ++v)
        {
            while (pos < E && edges[pos].first < v) ++pos;
            successor_index_[v] = pos;
        }
        successor_index_[V] = E;

        /* Create successor list */
        for (edgei e = 0; e < E; ++e) successors_[e] = edges[e].second;
    }

    if (edge_dir_ & EDGE_PREDECESSOR)
    {
        /* Sort edges by successor first, predecessor second */
        std::sort(edges.begin(), edges.end(), edge_cmp_backward);

        /* Create predecessor index */
        edgei pos = 0;
        for (verti v = 0; v < V; ++v)
        {
            while (pos < E && edges[pos].second < v) ++pos;
            predecessor_index_[v] = pos;
        }
        predecessor_index_[V] = E;

        /* Create predecessor list */
        for (edgei e = 0; e < E; ++e) predecessors_[e] = edges[e].first;
    }
}


void StaticGraph::write_raw(std::ostream &os) const
{
    os.write((const char*)&V_, sizeof(V_));
    os.write((const char*)&E_, sizeof(E_));
    os.write((const char*)&edge_dir_, sizeof(edge_dir_));
    if (edge_dir_ & EDGE_SUCCESSOR)
    {
        os.write((const char*)successors_, sizeof(verti)*E_);
        os.write((const char*)successor_index_, sizeof(edgei)*(V_ + 1));
    }
    if (edge_dir_ & EDGE_PREDECESSOR)
    {
        os.write((const char*)predecessors_, sizeof(verti)*E_);
        os.write((const char*)predecessor_index_, sizeof(edgei)*(V_ + 1));
    }
}

void StaticGraph::read_raw(std::istream &is)
{
    verti V;
    edgei E;
    EdgeDirection edge_dir;

    is.read((char*)&V, sizeof(V));
    is.read((char*)&E, sizeof(E));
    is.read((char*)&edge_dir, sizeof(edge_dir));

    reset(V, E, edge_dir);

    if (edge_dir & EDGE_SUCCESSOR)
    {
        is.read((char*)successors_, sizeof(verti)*E_);
        is.read((char*)successor_index_, sizeof(edgei)*(V_ + 1));
    }
    if (edge_dir & EDGE_PREDECESSOR)
    {
        is.read((char*)predecessors_, sizeof(verti)*E_);
        is.read((char*)predecessor_index_, sizeof(edgei)*(V_ + 1));
    }
}

size_t StaticGraph::memory_use() const
{
    size_t res = 0;
    res += sizeof(edgei)*(V_ + 1);
    res += sizeof(verti)*E_;
    if (edge_dir_ == EDGE_BIDIRECTIONAL) res *= 2;
    return res;
}
