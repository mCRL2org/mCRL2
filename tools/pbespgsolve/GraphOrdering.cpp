#include "GraphOrdering.h"
#include "assert.h"
#include <queue>
#include <stack>

static int sign(int i)
{
    if (i < 0) return -1;
    if (i > 0) return +1;
    return 0;
}

edgei count_ordered_edges(const StaticGraph &g, int dir)
{
    dir = sign(dir);
    edgei res = 0;
    for (verti v = 0; v < g.V(); ++v)
    {
        for ( StaticGraph::const_iterator it = g.succ_begin(v);
              it != g.succ_end(v); ++it )
        {
            res += (sign(*it - v) == dir);
        }
    }
    return res;
}

void get_bfs_order(const StaticGraph &graph, std::vector<verti> &perm)
{
    assert(perm.empty());
    perm.resize(graph.V(), (verti)-1);

    std::queue<verti> queue;
    verti new_v = 0;
    for (verti root = 0; root < graph.V(); ++root)
    {
        if (perm[root] != (verti)-1) continue;
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
                if (perm[w] == (verti)-1)
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
    assert(perm.empty());
    perm.resize(graph.V(), (verti)-1);

    std::stack<std::pair<verti, StaticGraph::const_iterator> > stack;
    verti new_v = 0;
    for (verti root = 0; root < graph.V(); ++root)
    {
        if (perm[root] != (verti)-1) continue;
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
                if (perm[w] == (verti)-1)
                {
                    perm[w] = new_v++;
                    stack.push(std::make_pair(w, graph.succ_begin(w)));
                }
            }
        }
    }
    assert(new_v == graph.V());
}
