// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>

#include "compatibility.h"

// Note: these should be unsigned types; some algorithms depend on it!
typedef compat_uint64_t verti;    //!< type used to number vertices
typedef compat_uint64_t edgei;    //!< type used to number edges

#define NO_VERTEX ((verti)-1)

class StaticGraph;

//! A not-completely-standard-compliant class to iterate over a graph's edges.
class EdgeIterator
{
public:
    EdgeIterator() { }
    EdgeIterator(const EdgeIterator &ei) : g(ei.g), v(ei.v), e(ei.e) { }
    inline EdgeIterator &operator=(const EdgeIterator &ei);
    inline std::pair<verti, verti> operator*();
    inline std::pair<verti, verti> operator++();
    inline std::pair<verti, verti> operator++(int);

    typedef edgei Distance;
    Distance operator-(const EdgeIterator &ei) { return e - ei.e; }

protected:
    EdgeIterator(const StaticGraph *g, verti v, edgei e) : g(g), v(v), e(e) { }

private:
    const StaticGraph *g;  //! underlying graph whose edges are being iterated
    verti v;               //! current vertex index
    edgei e;               //! current edge index

    friend class StaticGraph;
};


/*! A static graph consists of V vertices (numbered from 0 to V, exclusive)
    and E edges, and can store either edge successors, predecessors, or both. */
class StaticGraph
{
public:
    /*! Iterator used to iterate over predecessor/successor vertices. */
    typedef const verti *const_iterator;

    /*! Iterator used to iterate over edges. */
    typedef EdgeIterator const_edge_iterator;

    /*! A list of edges */
    typedef std::vector<std::pair<verti, verti> > edge_list;

    /*! Specifies which parts of the edges are stored.
        When storing successors only, the functions succ_begin() and succ_end()
        can be used to iterate over the successors of a given vertex.
        Conversely, when storing predecessors, pred_begin() and pred_end()
        can be used to iterate over predecessors.
        When storing bidirectional edges, both pairs of functions can be used,
        but this requires more memory. */
    enum EdgeDirection
    {
        EDGE_NONE           = 0,  /* for internal use only! */
        EDGE_SUCCESSOR      = 1,
        EDGE_PREDECESSOR    = 2,
        EDGE_BIDIRECTIONAL  = 3
    };

    StaticGraph();          /*!< Construct an empty static graph. */
    ~StaticGraph();         /*!< Destroy the static graph. */

    /*! Reset to an empty graph. */
    void clear();

    /*! Generate a random graph with `V` vertices and an average out-degree of
        `out_deg` (minimum out degree is 1). This replaces any old data stored.
        \param V        number of vertices to generate
        \param out_deg  desired average out degree (at least 1)
        \param edge_dir which parts of edges to store */
    void make_random(verti V, unsigned out_deg, EdgeDirection edge_dir);

    /*! Reset the graph to a copy of `graph'. */
    void assign(const StaticGraph &graph);

    /*! Reset the graph based on the given edge structure. */
    void assign(edge_list edges, EdgeDirection edge_dir);

    /*! Reset the graph to the subgraph induced by the given vertex set: */
    template<class ForwardIterator>
    void make_subgraph( const StaticGraph &graph,
                        ForwardIterator vertices_begin,
                        ForwardIterator vertices_end );

    /*! Removes the given edges from the graph. The contents of the edge list
        may be reordered by this function! */
    void remove_edges(edge_list &edges);

    /*! Write raw graph data to output stream */
    void write_raw(std::ostream &os) const;

    /*! Read raw graph data from input stream */
    void read_raw(std::istream &is);

    /*! Returns the memory used to store the graph data. */
    size_t memory_use() const;

    /*! Swaps the contents of this graph with another one. */
    void swap(StaticGraph &g);

    /*! Returns whether the graph is empty. */
    bool empty() const { return V_ == 0; }

    /*! Returns the number of vertices in the graph. */
    verti V() const { return V_; }

    /*! Returns the number of edges in the graph. */
    edgei E() const { return E_; }

    /*! Return direction of edges stored. */
    EdgeDirection edge_dir() const { return edge_dir_; }

    /*! Returns an iterator pointing to the first successor of vertex `v`. */
    const_iterator succ_begin(verti v) const {
        return &successors_[successor_index_[v]];
    }

    /*! Returns an iterator pointing past the last successor of vertex `v`. */
    const_iterator succ_end(verti v) const {
        return &successors_[successor_index_[v + 1]];
    }

    /*! Returns an iterator pointing to the first predecessor of vertex `v`. */
    const_iterator pred_begin(verti v) const {
        return &predecessors_[predecessor_index_[v]];
    }

    /*! Returns an iterator pointing past the last predecessor of vertex `v`. */
    const_iterator pred_end(verti v) const {
        return &predecessors_[predecessor_index_[v + 1]];
    }

    /*! Returns whether `v' has a successor `w'. */
    bool has_succ(verti v, verti w) const {
        return std::binary_search(succ_begin(v), succ_end(v), w);
    }

    /*! Returns whether `w' has a predecessor `v'. */
    bool has_pred(verti w, verti v) const {
        return std::binary_search(pred_begin(w), pred_end(w), v);
    }

    /*! Returns the degree for vertex `v'. */
    edgei degree(verti v) const {
        return indegree(v) + outdegree(v);
    }

    /*! Returns the outdegree for vertex `v'. */
    edgei outdegree(verti v) const {
        return succ_end(v) - succ_begin(v);
    }

    /*! Returns the indegree for vertex `v'. */
    edgei indegree(verti v) const {
        return pred_end(v) - pred_begin(v);
    }

    /*! Returns an edge iterator starting at the given vertex.
        Currently, this requires the graph to store successors. */
    const_edge_iterator edges_begin(verti v = 0) const {
        return EdgeIterator(this, v, successor_index_[v]);
    }

    /*! Returns an edge iterator pointing to the end of the edge list.
        Currently, this requires the graph to store successors. */
    const_edge_iterator edges_end() const {
        return EdgeIterator(this, V_, E_);
    }

protected:
    /*! Frees allocated memory and then reallocates memory to store a graph
        with `V` vertices and `E` edges. */
    void reset(verti V, edgei E, EdgeDirection edge_dir);

    /*! Reset the graph to the subgraph induced by the given vertex set, using
        the given map data structure to create the vertex mapping. */
    template<class ForwardIterator, class VertexMapT>
    void make_subgraph( const StaticGraph &graph,
                        ForwardIterator vertices_begin,
                        ForwardIterator vertices_end,
                        VertexMapT &vertex_map );

private:
    explicit StaticGraph(const StaticGraph &graph);
    StaticGraph &operator=(const StaticGraph &graph);

private:
    verti V_;  /*!< number of vertices */
    edgei E_;  /*!< number of edges */

    /*! Successor/predecessor lists (of size E).
        If edges are pairs of nodes (i,j), then `successors` is the list of
        successors (j's) obtained after sorting edges by predecessors (i's),
        and vice versa for `predecessors`. */
    verti *successors_, *predecessors_;

    /*! Indices into the successor/predecessor lists (of size V + 1).
        successor_index[i] is the lowest index of an edge in `successors` with
        a predecessor >= i; successor_index[V] is E. */
    edgei *successor_index_, *predecessor_index_;

    /*! Direction of stored edges. */
    EdgeDirection edge_dir_;

private:
    /* This is a bit of a hack to allow the small progress measures code to
       do a preprocessing pass for nodes with self-loops. */
    friend class SmallProgressMeasures;

    friend class EdgeIterator;
};

inline
void swap(StaticGraph &a, StaticGraph &b)
{
    a.swap(b);
}

#include "Graph_impl.h"

#endif /* ndef GRAPH_H_INCLUDED */
