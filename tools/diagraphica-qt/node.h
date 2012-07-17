// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./node.h

#ifndef NODE_H
#define NODE_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <map>
#include <vector>

class Cluster;
class Edge;

class Node
{
  public:
    // -- constructors and destructors ------------------------------
    Node(size_t idx): index(idx), cluster(0) {}
    Node(size_t idx, const std::vector<double> &tpl): index(idx), tuple(tpl), cluster(0) {}

    // -- set functions ---------------------------------------------
    void swapTupleVal(size_t idx1, size_t idx2);
    void moveTupleVal(size_t idxFr, size_t idxTo);
    void moveTupleVals(std::map<size_t, size_t> &idcsFrTo);
    void addTupleVal(size_t idx, double val) { tuple.insert(tuple.begin() + idx, val); }
    void delTupleVal(size_t idx) { tuple.erase(tuple.begin() + idx); }
    void addInEdge(Edge* e) { inEdges.push_back(e); }
    void setInEdges(const std::vector<Edge *> &e) { inEdges = e; }
    void addOutEdge(Edge* e) { outEdges.push_back(e); }
    void setOutEdges(const std::vector<Edge *> &e) { outEdges = e; }
    void setCluster(Cluster* c) { cluster = c; }

    // -- get functions ---------------------------------------------
    size_t getIndex() const { return index; }
    size_t getSizeTuple() const { return tuple.size(); }
    double getTupleVal(const size_t& idx) const { return tuple[idx]; }
    size_t getSizeInEdges() const { return inEdges.size(); }
    Edge* getInEdge(const size_t& idx) const { return inEdges[idx]; }
    size_t getSizeOutEdges() const { return outEdges.size(); }
    Edge* getOutEdge(const size_t& idx) const { return outEdges[idx]; }
    Cluster* getCluster() const { return cluster; }

  protected:
    // -- data members ----------------------------------------------
    size_t index; // index in list of graph nodes
    std::vector< double >   tuple;
    std::vector< Edge* > inEdges;  // association
    std::vector< Edge* > outEdges; // association
    Cluster* cluster;
};

#endif

// -- end -----------------------------------------------------------
