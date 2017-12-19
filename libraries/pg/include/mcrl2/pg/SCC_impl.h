// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Don't include this directly; include SCC.h instead!


#ifndef MCRL2_PG_SCC_IMPL_H
#define MCRL2_PG_SCC_IMPL_H

#include <algorithm>
#include <vector>
#include <utility>
#include <cassert>

#include "mcrl2/pg/Graph.h"


/*! Implements Tarjan's algorithm for finding strongly connected components in
    a directed graph.

    Visits each vertex and edge in the graph once, so it has a run-time
    complexity O(V + E). For each vertex, two items are stored: the vertex index
    (which denotes the order in which vertices are visited) and a lowest link
    index, which gives the lowest index of a vertex that is reachable from the
    current vertex.

    When a vertex has not yet been visited, its index is set to NO_VERTEX.
    Furthermore, the lowest link index is set to NO_VERTEX if the vertex is not
    part of the current component.

    Worst-case memory use: 5*sizeof(verti) + c.
*/
template<class Callback>
class SCC
{
public:
    SCC(const StaticGraph &graph, Callback &callback)
        : graph_(graph), callback_(callback)
    {
    }

    int run()
    {
        // Initialize data structures used in the algorithm
        next_index = 0;
        info.clear();
        info.insert( info.end(), graph_.V(),
                     std::make_pair(NO_VERTEX, NO_VERTEX) );
        stack.clear();

        // Process all vertices
        for (verti v = 0; v < graph_.V();++v)
        {
            if (info[v].first == NO_VERTEX)
            {
                assert(stack.empty());
                add(v);
                int res = dfs();
                if (res != 0) return res;
            }
        }
        assert(stack.empty());
        return 0;
    }

private:
    void add(verti v)
    {
        // Mark vertex as visited and part of the current component
        info[v].first = info[v].second = next_index++;
        component.push_back(v);

        // Add to stack to be processed in depth-first-search
        stack.push_back(std::make_pair(v, 0));
    }

    /* This implements depth-first-search using a stack, which is a bit more
       complicated but allows us to process arbitrarily large graphs limited
       by available heap space only (instead of being limited by the call
       stack size) as well as conserving some memory. */
    int dfs()
    {
        int res = 0;

        while (res == 0 && !stack.empty())
        {
            verti v = stack.back().first;
            StaticGraph::const_iterator edge_it =
                graph_.succ_begin(v) + stack.back().second++;

            if (edge_it != graph_.succ_end(v))
            {
                // Find next successor `w` of `v`
                verti w = *edge_it;

                if (info[w].first == NO_VERTEX)  // unvisited?
                {
                    add(w);
                }
                else
                if (info[w].second != NO_VERTEX)  // part of current component?
                {
                    /* Check if w's index is lower than v's lowest link, if so,
                       set it to be our lowest link index. */
                    info[v].second = std::min(info[v].second, info[w].first);
                }
            }
            else
            {
                // We're done with this vertex
                stack.pop_back();

                if (!stack.empty())
                {
                    /* Push my lower link index to parent vertex `u`, if it
                       is lower than the parent's current lower link index. */
                    int u = stack.back().first;
                    info[u].second = std::min(info[u].second, info[v].second);
                }

                // Check if v is the component's root (idx == lowest link idx)
                if (info[v].first == info[v].second)
                {
                    // Find v in the current component
                    std::vector<verti>::iterator it = component.end();
                    do {
                        assert(it != component.begin());
                        info[*--it].second = NO_VERTEX;  // mark as removed
                    } while (*it != v);

                    // Call callback functor to handle this component
                    res = callback_((const verti*)&*it, component.end() - it);

                    // Remove vertices from current component
                    component.erase(it, component.end());
                }
            }
        }

        return res;
    }

public:
    const StaticGraph &graph_;
    Callback &callback_;

private:
    //! Index of next vertex to be labelled by inorder traversal.
    verti next_index;

    //! Inorder index and lowest link index of each vertex.
    std::vector<std::pair<verti, verti> > info;

    //! Vertex indices of the current component.
    std::vector<verti> component;

    /*! The depth-first-search stack.

        Each entry consists of a vertex index and an index into its successor
        list.  When a new unvisited vertex `v` is discovered, a pair (`v`, 0)
        is appened at the end of the stack.  The top element is popped off the
        stack when its successor index points to the end of the successor list.
    */
    std::vector< std::pair< verti, verti > > stack;
};

#endif // MCRL2_PG_SCC_IMPL_H
