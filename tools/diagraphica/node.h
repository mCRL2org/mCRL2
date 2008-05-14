//  Author(s): A.J. (Hannes) Pretorius
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./node.h

// --- node.h -------------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef NODE_H
#define NODE_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>
using namespace std;

class Cluster;
class Edge;

class Node
{
public:
	// -- constructors and destructors ------------------------------
    Node( const int &idx );
    Node( 
        const int &idx,
        const vector< double > &tpl );
	virtual ~Node();
	
    // -- set functions ---------------------------------------------
    void swapTupleVal( 
        const size_t &idx1,
        const size_t &idx2 );
    void moveTupleVal( 
        const size_t &idxFr,
        const size_t &idxTo );
    void moveTupleVals( map< int, int > &idcsFrTo );
    void addTupleVal( 
        const int &idx,
        const double &val );
    void delTupleVal( const int &idx );
    void addInEdge( Edge* e );
    void setInEdges( const vector< Edge* > e );
    void addOutEdge( Edge* e );
    void setOutEdges( const vector< Edge* > e );
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
    vector< double >   tuple;
    vector< Edge* > inEdges;  // association
	vector< Edge* > outEdges; // association
    Cluster* cluster;
};

#endif

// -- end -----------------------------------------------------------
