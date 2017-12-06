// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/pg/Graph.h"
#include "mcrl2/pg/SCC.h"
#include "mcrl2/pg/shuffle.h"
#include "mcrl2/utilities/logger.h"

#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <vector>


StaticGraph::StaticGraph()
    : successors_(NULL), predecessors_(NULL),
      successor_index_(NULL), predecessor_index_(NULL)
{
    reset(0, 0, EDGE_NONE);
}

StaticGraph::~StaticGraph()
{
    delete[] successors_;
    delete[] predecessors_;
    delete[] successor_index_;
    delete[] predecessor_index_;
}

void StaticGraph::clear()
{
    reset(0, 0, EDGE_NONE);
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

void StaticGraph::make_random_scc(edge_list &edges)
{
    SCCs sccs;
    decompose_graph(*this, sccs);

    /* If we happen to have a single SCC by luck, we are done too: */
    if (sccs.size() <= 1) return;

    /* Otherwise, identify `top` and `bottom` components: */
    std::vector<verti> index(V_, NO_VERTEX);
    for (verti i = 0; i < sccs.size(); ++i)
    {
        const std::vector<verti> scc;
        for (verti j = 0; j < scc.size(); ++j)
        {
            index[scc[j]] = i;
        }
    }
    std::vector<char> is_top(sccs.size(), 1),
                      is_bot(sccs.size(), 1);
    for (verti v = 0; v < V_; ++v)
    {
        for (const_iterator it = succ_begin(v); it != succ_end(v); ++it)
        {
            verti w = *it;
            if (index[v] != index[w])
            {
                is_bot[index[v]] = 0;
                is_top[index[w]] = 0;
            }
        }
    }

    /* Pick one vertex per SCC at random (excluding SCCs that are neither on top
       nor at the bottom of the hierarchy) and connect them in a cycle. */
    // FIXME: this creates more edges than strictly necessary!
    std::vector<verti> vertis;
    for (verti i = 0; i < sccs.size(); ++i)
    {
        if (is_top[i] || is_bot[i])
        {
            vertis.push_back(sccs[i][rand()%sccs[i].size()]);
        }
    }
    mCRL2log(mcrl2::log::debug) << "Connecting " << vertis.size()
                                << " of " << V_ << " vertices to create an SCC." << std::endl;
    shuffle_vector(vertis);
    for (verti i = 0; i < sccs.size(); ++i)
    {
        const verti v = vertis[i],
                    w = vertis[(i + 1)%sccs.size()];
        edges.push_back(std::make_pair(v, w));
    }
}

void StaticGraph::make_random(verti V, unsigned outdeg, EdgeDirection edge_dir,
                              bool scc)
{
    if (V < 2)
    {
        edge_list edges;
        assign(edges, edge_dir);
        return;
    }

    /* Some assumptions on the RNG output range: */
    assert(RAND_MAX >= 2*outdeg);
    assert(RAND_MAX >= V);

    /* Create a random edge set, with at least one outgoing edge per node,
       and an average outdegree `outdeg`, without any duplicate edges (but
       possibly with self-edges). */
    edge_list edges;
    std::vector<verti> neighbours(V);
    for (verti i = 0; i < V; ++i) neighbours[i] = i;
    for (verti i = 0; i < V; ++i)
    {
        unsigned N = 1 + rand()%(2*outdeg - 1);
        if (N >= V - 1) N = V - 1;
        for (unsigned n = 0; n < N; ++n)
        {
            std::swap(neighbours[n], neighbours[n + rand()%(V - n)]);
            if (neighbours[n] == i)  // don't generate loops
            {
                std::swap( neighbours[n],
                           neighbours[n + 1 + rand()%(V - n - 1)] );
            }
            edges.push_back(std::make_pair(i, neighbours[n]));
        }
    }

    /* Create graph from edge set */
    assign(edges, edge_dir);

    if (scc)
    {
        /* Turn graph into a single strongly connected component: */
        make_random_scc(edges);
        assign(edges, edge_dir);
#ifdef DEBUG
        /* Check the resulting graph: */
        SCCs new_sccs;
        decompose_graph(*this, new_sccs);
        assert(new_sccs.size() == 1);
#endif
    }
}

void StaticGraph::make_random_clustered( verti cluster_size, verti V,
        unsigned outdeg, EdgeDirection edge_dir)
{
    assert(cluster_size > 1);
    std::size_t clusters = V/cluster_size;
    if (clusters <= 1)
    {
        make_random(V, outdeg, edge_dir, true);
        return;
    }

    // Build `clusters` initial random graphs of cluster_size each:
    StaticGraph *subgraphs = new StaticGraph[clusters];
    for (std::size_t i = 0; i < clusters; ++i)
    {
        subgraphs[i].make_random( cluster_size + (i < V%cluster_size),
                                  outdeg, edge_dir, true );
    }

    // Iteratively merge clusters:
    for (int i = 0; clusters > 1; ++i)
    {
        mCRL2log(mcrl2::log::verbose) << "Generating clustered random game level "
                                      << i << "..." << std::endl;
        std::size_t next_clusters = (clusters + cluster_size - 1)/cluster_size;
        StaticGraph *next_subgraphs = new StaticGraph[next_clusters];
        std::vector<verti> offset(clusters, 0);
        for (std::size_t c = 0; c < next_clusters; ++c)
        {
            /* Combine clusters [i:j) into one: */
            std::size_t i = c*clusters/next_clusters,
                   j = (c + 1)*clusters/next_clusters;
            mCRL2log(mcrl2::log::debug) << "combining " << j-i << " subgraphs ("
                                        << i << " through " << j << " of "
                                        << clusters << ")" << std::endl;

            /* Calculate offsets to apply to vertex indices: */
            for (std::size_t k = i + 1; k < j; ++k)
            {
                offset[k] = offset[k - 1] + subgraphs[k - 1].V();
            }

            /* Build edge list of combined subgraphs: */
            edge_list edges;
            for (std::size_t k = i; k < j; ++k)
            {
                edge_list subedges = subgraphs[k].get_edges();
                for ( edge_list::const_iterator it = subedges.begin();
                      it != subedges.end(); ++it )
                {
                    edges.push_back(std::make_pair( it->first  + offset[k],
                                                    it->second + offset[k] ));
                }
            }

            /* Create parent graph and use its edges to connect subgraphs: */
            StaticGraph parent;
            parent.make_random(j - i, outdeg, edge_dir, true);
            edge_list paredges = parent.get_edges();
            for (std::size_t e = 0; e < paredges.size(); ++e)
            {
                verti v = paredges[e].first,
                      w = paredges[e].second;
                edges.push_back(std::make_pair(
                    offset[i + v] + rand()%subgraphs[i + v].V(),
                    offset[i + w] + rand()%subgraphs[i + w].V() ));
            }
            next_subgraphs[c].assign(edges, edge_dir);
        }
        delete[] subgraphs;
        subgraphs = next_subgraphs;
        clusters  = next_clusters;
    }
    assert(clusters == 1);
    swap(subgraphs[0]);
    delete[] subgraphs;
}

void StaticGraph::shuffle_vertices()
{
    std::vector<verti> perm(V_);
    for (verti i = 0; i < V_; ++i) perm[i] = i;
    shuffle_vector(perm);
    shuffle_vertices(perm);
}

void StaticGraph::shuffle_vertices(const std::vector<verti> &perm)
{
    edge_list edges = get_edges();
    for (edge_list::iterator it = edges.begin(); it != edges.end(); ++it)
    {
        it->first  = perm[it->first];
        it->second = perm[it->second];
    }
    assign(edges, edge_dir_);
}

void StaticGraph::assign(const StaticGraph &graph)
{
    if (&graph == this) return;

    reset(graph.V_, graph.E_, graph.edge_dir_);

    if (edge_dir_ & EDGE_SUCCESSOR)
    {
        std::copy(graph.successors_, graph.successors_ + E_, successors_);
        std::copy(graph.successor_index_, graph.successor_index_ + V_ + 1,
                  successor_index_);
    }
    if (edge_dir_ & EDGE_PREDECESSOR)
    {
        std::copy(graph.predecessors_, graph.predecessors_ + E_, predecessors_);
        std::copy(graph.predecessor_index_, graph.predecessor_index_ + V_ + 1,
                  predecessor_index_);
    }
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
        if (!std::is_sorted(edges.begin(), edges.end(), edge_cmp_forward))
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

void StaticGraph::remove_edges(StaticGraph::edge_list &edges)
{
    // Add end-of-list marker:
    edges.push_back(std::make_pair(V_, V_));

    if (edge_dir_ & EDGE_SUCCESSOR)
    {
        // Sort edges by predecessor first, successor second
        if (!std::is_sorted(edges.begin(), edges.end(), edge_cmp_forward))
        {
            std::sort(edges.begin(), edges.end(), edge_cmp_forward);
        }

        // Loop over existing edges and remove those listed in `edges`:
        StaticGraph::edge_list::const_iterator it = edges.begin();
        const verti *p = successors_;
        verti v = 0;
        edgei e = 0;
        while (v < V_)
        {
            if (p == successors_ + successor_index_[v + 1])
            {
                successor_index_[++v] = e;
                continue;
            }
            std::pair<verti, verti> edge(v, *p++);
            while (edge_cmp_forward(*it, edge)) ++it;
            if (*it == edge) ++it; else successors_[e++] = edge.second;
        }
    }

    if (edge_dir_ & EDGE_PREDECESSOR)
    {
        // Sort edges by successor first, predecessor second
        std::sort(edges.begin(), edges.end(), edge_cmp_backward);

        // Loop over existing edges and remove those listed in `edges`:
        StaticGraph::edge_list::const_iterator it = edges.begin();
        const verti *p = predecessors_;
        verti v = 0;
        edgei e = 0;
        while (v < V_)
        {
            if (p == predecessors_ + predecessor_index_[v + 1])
            {
                predecessor_index_[++v] = e;
                continue;
            }
            std::pair<verti, verti> edge(*p++, v);
            while (edge_cmp_backward(*it, edge)) ++it;
            if (*it == edge) ++it; else predecessors_[e++] = edge.first;
        }
    }

    // Remove end-of-list marker:
    edges.pop_back();

    // Update edge count
    if (edge_dir_ & EDGE_SUCCESSOR)
    {
        if (edge_dir_ & EDGE_PREDECESSOR)
        {
            assert(successor_index_[V_] == predecessor_index_[V_]);
        }
        E_ = successor_index_[V_];
    }
    else
    {
        assert(edge_dir_ & EDGE_PREDECESSOR);
        E_ = predecessor_index_[V_];
    }
}

StaticGraph::edge_list StaticGraph::get_edges() const
{
    assert(edge_dir_ & EDGE_SUCCESSOR);  // successor info required for now!

    edge_list result;
    result.reserve(E_);
    for (verti v = 0; v < V_; ++v)
    {
        edgei begin = successor_index_[v], end = successor_index_[v + 1];
        for (edgei i = begin; i < end; ++i)
        {
            verti w = successors_[i];
            result.push_back(std::make_pair(v, w));
        }
    }
    assert(result.size() == E_);
    return result;
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

void StaticGraph::swap(StaticGraph &g)
{
    if (this == &g) return;
    std::swap(V_, g.V_);
    std::swap(E_, g.E_);
    std::swap(successors_, g.successors_);
    std::swap(predecessors_, g.predecessors_);
    std::swap(successor_index_, g.successor_index_);
    std::swap(predecessor_index_, g.predecessor_index_);
    std::swap(edge_dir_, g.edge_dir_);
}

#ifdef WITH_THREADS
void StaticGraph::make_subgraph_threads( const StaticGraph &graph,
                                         const verti *verts,
                                         const verti num_vertices,
                                         bool proper,
                                         EdgeDirection edge_dir )
{
    assert(this != &graph);

    edgei num_edges = 0;

    // Create a map of old->new vertex indices:
    std::vector<verti> map(graph.V(), NO_VERTEX);
    #pragma omp parallel
    {
        #pragma omp for
        for (verti i = 0; i < num_vertices; ++i)
        {
            map[verts[i]] = i;
        }

        // Count number of new edges:
        #pragma omp for reduction(+:num_edges)
        for (verti i = 0; i < num_vertices; ++i)
        {
            const_iterator a = graph.succ_begin(verts[i]),
                        b = graph.succ_end(verts[i]);
            while (a != b) if (map[*a++] != NO_VERTEX) ++num_edges;
        }
    }

    // Allocate memory:
    reset(num_vertices, num_edges, edge_dir ? edge_dir : graph.edge_dir());

    //
    // TODO: parallellize rest of function!
    //

    if (edge_dir_ & EDGE_SUCCESSOR)
    {
        // Assign new successors:
        verti v = 0;
        edgei e = 0;
        for (verti i = 0; i < num_vertices; ++i)
        {
            const verti u = verts[i];
            successor_index_[v++] = e;
            verti *begin = &successors_[e];
            for (const_iterator succ_it  = graph.succ_begin(u),
                                succ_end = graph.succ_end(u);
                 succ_it != succ_end; ++succ_it)
            {
                verti w = map[*succ_it];
                if (w != NO_VERTEX) successors_[e++] = w;
            }
            verti *end = &successors_[e];
            if (!std::is_sorted(begin, end, std::less<verti>()))
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
        for (verti i = 0; i < num_vertices; ++i)
        {
            const verti u = verts[i];
            predecessor_index_[v++] = e;
            verti *begin = &predecessors_[e];
            for (const_iterator pred_it  = graph.pred_begin(u),
                                pred_end = graph.pred_end(u);
                 pred_it != pred_end; ++pred_it)
            {
                verti w = map[*pred_it];
                if (w != NO_VERTEX) predecessors_[e++] = w;
            }
            verti *end = &predecessors_[e];
            if (!std::is_sorted(begin, end, std::less<verti>()))
            {
                std::sort(begin, end);
            }
        }
        assert(v == V_ && e == E_);
        predecessor_index_[v] = e;
    }
}
#endif
