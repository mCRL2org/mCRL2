// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_PG_SCC_H
#define MCRL2_PG_SCC_H

#include "mcrl2/pg/Graph.h"
#include "mcrl2/pg/SCC_impl.h"

#include <vector>


/*! \ingroup ParityGameData

    Decomposition into strongly-connected components using Tarjan's algorithm.

    Decomposes the static graph into strongly connected components. Components
    are found in reverse topological order (i.e. if component j is found after
    component i, there is no path from a node in i to a node in j).

    For each component found, the callback functor is called with as arguments
    a list of vertex indices (`const verti[]`) and the size of the component
    (`std::size_t`).  The callback should return an integer: zero to continue
    enumerating components, or non-zero to abort.

    @return the last value returned by a call to callback
*/

template<class Callback>
int decompose_graph(const StaticGraph &graph, Callback &callback)
{
    return SCC<Callback>(graph, callback).run();
}

/*! \ingroup ParityGameData

    A utility class to collect strongly connected components in a graph when
    used as the callback functor in the SCC class defined above.

    Note that it's usually more efficient to handle SCCs directly as they are
    found, rather than collect them explicitly, but this class may be easier to
    use in situation were performance is not critical.

    Usage example:
    \code
    SCCs sccs;
    decompose_graph(*this, sccs);
    std::cout << sccs.size() << " SCCs found!" << std::endl;
    \endcode
*/
class SCCs
{
    std::vector<std::vector<verti> > sccs;

public:
    //! Clear the list of collected SCCs.
    void clear() { sccs.clear(); }

    //! @return the number of collected SCCs.
    std::size_t size() const { return sccs.size(); }

    //! @return the i-th SCC as vector of vertex indices.
    std::vector<verti> &operator[](std::size_t i) { return sccs[i]; }

    //! @return the i-th SCC as const vector of vertex indices.
    const std::vector<verti> &operator[](std::size_t i) const { return sccs[i]; }

    /*! Add a strongly connected component to the list.
        @param scc an array of length `size` giving indices of the vertices 
        @param size the size of the component
        @return zero */
    int operator()(const verti scc[], std::size_t size)
    {
        sccs.resize(sccs.size() + 1);
        sccs.back().assign(&scc[0], &scc[size]); 
        return 0;
    }
};

#endif /* ndef MCRL2_PG_SCC_H */
