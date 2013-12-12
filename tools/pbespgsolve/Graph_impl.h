// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Don't include this directly; include Graph.h instead!

#include "DenseMap.h"
#include <algorithm>
#include <iterator>
#include <assert.h>

// Note: code below is not currently used and has not been properly tested.
#if 0
EdgeIterator &EdgeIterator::operator=(const EdgeIterator &ei)
{
    g = ei.g;
    v = ei.v;
    e = ei.e;
    return *this;
}

std::pair<verti, verti> EdgeIterator::operator*()
{
    return std::pair<verti, verti>(v, g->successors_[e]);
}

std::pair<verti, verti> EdgeIterator::operator++()
{
    if (++e < g->E_) while (g->successor_index_[v + 1] < e) ++v;
    return **this;
}

std::pair<verti, verti> EdgeIterator::operator++(int)
{
    std::pair<verti, verti> result = **this;
    ++*this;
    return result;
}
#endif

template<class ForwardIterator>
void StaticGraph::make_subgraph( const StaticGraph &graph,
                                 ForwardIterator vertices_begin,
                                 ForwardIterator vertices_end,
                                 bool proper,
                                 StaticGraph::EdgeDirection edge_dir )
{
    assert(vertices_begin <= vertices_end);

    // FIXME: determine which cut-off value works best:
    if (static_cast<verti>(std::distance(vertices_begin, vertices_end)) < graph.V()/3)
    {
        HASH_MAP(verti, verti) map;
        return make_subgraph(graph, vertices_begin,
                             vertices_end, map, proper, edge_dir);
    }
    else
    {
        DenseMap<verti, verti> map(0, graph.V());
        return make_subgraph(graph, vertices_begin, vertices_end,
                             map, proper, edge_dir);
    }
}

template<class ForwardIterator, class VertexMapT>
void StaticGraph::make_subgraph( const StaticGraph &graph,
                                 ForwardIterator vertices_begin,
                                 ForwardIterator vertices_end,
                                 VertexMapT &vertex_map,
                                 bool proper,
                                 EdgeDirection edge_dir )
{
    assert(this != &graph);

    verti num_vertices = 0;
    edgei num_edges = 0;

    // Create a map of old->new vertex indices, while counting vertices:
    for (ForwardIterator it = vertices_begin; it != vertices_end; ++it)
    {
        vertex_map[*it] = num_vertices++;
    }

    // Count number of new edges:
    for (ForwardIterator it = vertices_begin; it != vertices_end; ++it)
    {
        const_iterator a, b;
        if (graph.edge_dir() & EDGE_SUCCESSOR)
        {
            a = graph.succ_begin(*it);
            b = graph.succ_end(*it);
        }
        else
        {
            a = graph.pred_begin(*it);
            b = graph.pred_end(*it);
        }
        while (a != b) num_edges += (vertex_map.find(*a++) != vertex_map.end());
    }

    // Allocate memory:
    reset(num_vertices, num_edges, edge_dir ? edge_dir : graph.edge_dir());

    if (edge_dir_ & EDGE_SUCCESSOR)
    {
        // Assign new successors:
        verti v = 0;
        edgei e = 0;
        for (ForwardIterator it = vertices_begin; it != vertices_end; ++it)
        {
            successor_index_[v++] = e;
            verti *begin = &successors_[e];
            for (const_iterator succ_it  = graph.succ_begin(*it),
                                succ_end = graph.succ_end(*it);
                 succ_it != succ_end; ++succ_it)
            {
                typename VertexMapT::const_iterator it(vertex_map.find(*succ_it));
                if (it != vertex_map.end()) successors_[e++] = (*it).second;
            }
            verti *end = &successors_[e];
            if (!is_sorted(begin, end, std::less<verti>()))
            {
                std::sort(begin, end);
            }
            if (proper) assert(begin != end);  /* proper parity game graph */
        }
        assert(v == V_ && e == E_);
        successor_index_[v] = e;
    }

    if (edge_dir_ & EDGE_PREDECESSOR)
    {
        // Assign new predecessors:
        verti v = 0;
        edgei e = 0;
        for (ForwardIterator it = vertices_begin; it != vertices_end; ++it)
        {
            predecessor_index_[v++] = e;
            verti *begin = &predecessors_[e];
            for (const_iterator pred_it  = graph.pred_begin(*it),
                                pred_end = graph.pred_end(*it);
                 pred_it != pred_end; ++pred_it)
            {
                typename VertexMapT::const_iterator it(vertex_map.find(*pred_it));
                if (it != vertex_map.end()) predecessors_[e++] = it->second;
            }
            verti *end = &predecessors_[e];
            if (!is_sorted(begin, end, std::less<verti>()))
            {
                std::sort(begin, end);
            }
        }
        assert(v == V_ && e == E_);
        predecessor_index_[v] = e;
    }
}
