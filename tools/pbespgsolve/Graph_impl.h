// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Don't include this directly; include Graph.h instead!

#include "DenseMap.h"
#include <algorithm>
#include <iterator>
#include <assert.h>

template<class It, class Cmp>
bool is_sorted(It i, It j, Cmp cmp)
{
    if (i == j) return true;
    for (;;)
    {
        It k = i;
        if (++k == j) return true;
        if (cmp(*k, *i)) return false; // *(i+1) > *i
        i = k;
    }
}

template<class ForwardIterator>
void StaticGraph::make_subgraph( const StaticGraph &graph,
                                 ForwardIterator vertices_begin,
                                 ForwardIterator vertices_end )
{
    // FIXME: determine which cut-off value works best:
    if (std::distance(vertices_begin, vertices_end) < graph.V()/static_cast<verti>(3))
    {
        HASH_MAP(verti, verti) map;
        return make_subgraph(graph, vertices_begin, vertices_end, map);
    }
    else
    {
        DenseMap<verti, verti> map(0, graph.V());
        return make_subgraph(graph, vertices_begin, vertices_end, map);
    }
}

template<class ForwardIterator, class VertexMapT>
void StaticGraph::make_subgraph( const StaticGraph &graph,
                                 ForwardIterator vertices_begin,
                                 ForwardIterator vertices_end,
                                 VertexMapT &vertex_map )
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
    reset(num_vertices, num_edges, graph.edge_dir());

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
            if (!::is_sorted(begin, end, std::less<verti>()))
            {
                std::sort(begin, end);
            }
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
            if (!::is_sorted(begin, end, std::less<verti>()))
            {
                std::sort(begin, end);
            }
        }
        assert(v == V_ && e == E_);
        predecessor_index_[v] = e;
    }
}
