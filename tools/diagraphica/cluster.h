//  Author(s): A.J. (Hannes) Pretorius
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./cluster.h

// --- cluster.h ----------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef CLUSTER_H
#define CLUSTER_H

#include <cstddef>
#include <vector>
using namespace std;
#include "attribute.h"
#include "node.h"

class Bundle;

class Cluster
{
public:
	// -- constructors and destructors ------------------------------
    Cluster();
    Cluster( const vector< int > &crd );
    Cluster( const Cluster &clst );
    virtual ~Cluster();
	
    // -- set functions ---------------------------------------------
    void setCoord( const vector< int > &crd  );
    void setIndex( const int &idx );
    void setParent( Cluster* p );
    void addChild( Cluster* c );
    void setChildren( const vector< Cluster* > &c );
    void addNode( Node* n );
    void setNodes( const vector< Node* > &n );

    void setAttribute( Attribute* attr );
    void setAttrValIdx( const int &idx );

    void addInBundle( Bundle* b );
    void setInBundles( const vector< Bundle* > b );
    void addOutBundle( Bundle* b );
    void setOutBundles( const vector< Bundle* > b );
    
    // -- get functions ---------------------------------------------
    int getSizeCoord();
    int getCoord( const int &idx );
    void getCoord( vector< int > &crd );
    int getIndex();
    Cluster* getParent();
    int getSizeChildren();
    Cluster* getChild( const int &idx );
    int getSizeNodes();
    Node* getNode( const int &idx );
    int getSizeDescNodes();
    
    Attribute* getAttribute();
    int getAttrValIdx();

    int getSizeInBundles();
    Bundle* getInBundle( const int &idx );
    int getSizeOutBundles();
    Bundle* getOutBundle( const int &idx );
    
    // -- clear functions -------------------------------------------
    void clearParent();
    void clearChildren();
    void clearNodes();
    void clearAttribute();
    void clearInBundles();
    void clearOutBundles();

protected:
    // -- utility functions -----------------------------------------
    void getSizeDescNodes(
        Cluster* curClst,
        int &sum );

    // -- data members ----------------------------------------------
    vector< int >      coord;
    int                index;
    Cluster*           parent;    // association
    vector< Cluster* > children;  // association
    vector< Node* >    nodes;     // association
    Attribute*         attribute; // association
    int                attrValIdx; 
    vector< Bundle* >  inBundles;  // association
	vector< Bundle* >  outBundles; // association
};

#endif

// -- end -----------------------------------------------------------
