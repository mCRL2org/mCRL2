#ifndef SCC_H_INCLUDED
#define SCC_H_INCLUDED

#include "Graph.h"

#include "SCC_impl.h"

/* Decomposes the static graph into strongly connected components. Components
   are found in reverse topological order (i.e. if component j is found after
   component i, there is no path from a node in i to a node in j).

   For each node found, the callback is called with as arguments a list of
   vertex indices (const verti *) and the size of the component (size_t).
   The callback should return an integer; if zero, enumeration of components
   continues, otherwise, decompose_graph() returns with the value returned by
   the callback.
*/

template<class Callback>
int decompose_graph(const StaticGraph &graph, Callback &callback)
{
    return SCC<Callback>(graph, callback).run();
}

#endif /* ndef SCC_H_INCLUDED */
