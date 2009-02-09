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

#include <cstddef>
#include <map>
#include <vector>

class Cluster;
class Edge;

class Node
{
public:
	// -- constructors and destructors ------------------------------
    Node( const int &idx );
    Node(
        const int &idx,
        const std::vector< double > &tpl );
	virtual ~Node();

    // -- set functions ---------------------------------------------
    void swapTupleVal(
        const size_t &idx1,
        const size_t &idx2 );
    void moveTupleVal(
        const size_t &idxFr,
        const size_t &idxTo );
    void moveTupleVals( std::map< int, int > &idcsFrTo );
    void addTupleVal(
        const int &idx,
        const double &val );
    void delTupleVal( const int &idx );
    void addInEdge( Edge* e );
    void setInEdges( const std::vector< Edge* > e );
    void addOutEdge( Edge* e );
    void setOutEdges( const std::vector< Edge* > e );
    void setCluster( Cluster* c );

    // -- get functions ---------------------------------------------
    int getIndex();
    int getSizeTuple();
    double getTupleVal( const size_t &idx );
    int getSizeInEdges();
    Edge* getInEdge( const size_t &idx );
    int getSizeOutEdges();
    Edge* getOutEdge( const size_t &idx );
	Cluster* getCluster();

    // -- clear functions -------------------------------------------
    void clearInEdges();
    void clearOutEdges();
    void clearCluster();

protected:
    // -- data members ----------------------------------------------
    int index; // index in list of graph nodes
    std::vector< double >   tuple;
    std::vector< Edge* > inEdges;  // association
	std::vector< Edge* > outEdges; // association
    Cluster* cluster;
};

#endif

// -- end -----------------------------------------------------------
