// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "GraphOrdering.h"
#include <cassert>
#include <queue>
#include <stack>

edgei count_ordered_edges(const StaticGraph &g, int dir)
{
    edgei res = 0;
    for (verti v = 0; v < g.V(); ++v)
    {
        for ( StaticGraph::const_iterator it = g.succ_begin(v);
              it != g.succ_end(v); ++it )
        {
            res += (dir < 0) ? (*it < v) :
                   (dir > 0) ? (*it > v) : (*it == v);
        }
    }
    return res;
}

void get_bfs_order(const StaticGraph &graph, std::vector<verti> &perm)
{
    perm.assign(graph.V(), NO_VERTEX);

    std::queue<verti> queue;
    verti new_v = 0;
    for (verti root = 0; root < graph.V(); ++root)
    {
        if (perm[root] != NO_VERTEX) continue;
        perm[root] = new_v++;
        queue.push(root);
        while (!queue.empty())
        {
            verti v = queue.front();
            queue.pop();
            StaticGraph::const_iterator it = graph.succ_begin(v);
            while (it != graph.succ_end(v))
            {
                verti w = *it++;
                if (perm[w] == NO_VERTEX)
                {
                    perm[w] = new_v++;
                    queue.push(w);
                }
            }
        }
    }
    assert(new_v == graph.V());
}

void get_dfs_order(const StaticGraph &graph, std::vector<verti> &perm)
{
    perm.assign(graph.V(), NO_VERTEX);

    std::stack<std::pair<verti, StaticGraph::const_iterator> > stack;
    verti new_v = 0;
    for (verti root = 0; root < graph.V(); ++root)
    {
        if (perm[root] != NO_VERTEX) continue;
        perm[root] = new_v++;
        stack.push(std::make_pair(root, graph.succ_begin(root)));
        while (!stack.empty())
        {
            verti v = stack.top().first;
            StaticGraph::const_iterator &it = stack.top().second;
            if (it == graph.succ_end(v))
            {
                stack.pop();
            }
            else
            {
                verti w = *it++;
                if (perm[w] == NO_VERTEX)
                {
                    perm[w] = new_v++;
                    stack.push(std::make_pair(w, graph.succ_begin(w)));
                }
            }
        }
    }
    assert(new_v == graph.V());
}
