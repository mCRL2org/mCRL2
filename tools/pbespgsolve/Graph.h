// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
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

typedef uint32_t verti;    /*!< type used to number vertices */
typedef uint32_t edgei;    /*!< type used to number edges */

#define NO_VERTEX ((verti)-1)

/*! A static graph consists of V vertices (numbered from 0 to V, exclusive)
    and E edges, and can store either edge successors, predecessors, or both. */
class StaticGraph
{
public:
    /*! Iterator used to iterate over predecessor/successor vertices. */
    typedef const verti *const_iterator;

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
        EDGE_SUCCESSOR      = 1,
        EDGE_PREDECESSOR    = 2,
        EDGE_BIDIRECTIONAL  = 3
    };

    StaticGraph();          /*!< Construct an empty static graph. */
    ~StaticGraph();         /*!< Destroy the static graph. */

    /*! Generate a random graph with `V` vertices and an average out-degree of
        `out_deg` (minimum out degree is 1). This replaces any old data stored.
        \param V        number of vertices to generate
        \param out_deg  desired average out degree (at least 1)
        \param edge_dir which parts of edges to store */
    void make_random(verti V, unsigned out_deg, EdgeDirection edge_dir);

    /*! Reset the graph based on the given edge structure. */
    void assign(edge_list edges, EdgeDirection edge_dir);

    /*! Write raw graph data to output stream */
    void write_raw(std::ostream &os) const;

    /*! Read raw graph data from input stream */
    void read_raw(std::istream &is);

    /*! Returns the memory used to store the graph data. */
    size_t memory_use() const;

    verti V() const { return V_; }  /*!< Return number of vertices in the graph */
    edgei E() const { return E_; }  /*!< Return number of edges in the graph */

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

protected:
    /*! Frees allocated memory and then reallocates memory to store a graph
        with `V` vertices and `E` edges. */
    void reset(verti V, edgei E, EdgeDirection edge_dir);

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
};

#endif /* ndef GRAPH_H_INCLUDED */
