// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./cluster.h

#ifndef CLUSTER_H
#define CLUSTER_H

#include <cstddef>
#include <vector>
#include "attribute.h"
#include "node.h"

class Bundle;

class Cluster
{
public:
	// -- constructors and destructors ------------------------------
    Cluster();
    Cluster( const std::vector< int > &crd );
    Cluster( const Cluster &clst );
    virtual ~Cluster();

    // -- set functions ---------------------------------------------
    void setCoord( const std::vector< int > &crd  );
    void setIndex( const int &idx );
    void setParent( Cluster* p );
    void addChild( Cluster* c );
    void setChildren( const std::vector< Cluster* > &c );
    void addNode( Node* n );
    void setNodes( const std::vector< Node* > &n );

    void setAttribute( Attribute* attr );
    void setAttrValIdx( const int &idx );

    void addInBundle( Bundle* b );
    void setInBundles( const std::vector< Bundle* > b );
    void addOutBundle( Bundle* b );
    void setOutBundles( const std::vector< Bundle* > b );

    // -- get functions ---------------------------------------------
    int getSizeCoord();
    int getCoord( const int &idx );
    void getCoord( std::vector< int > &crd );
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
    void getSizeDescNodes( Cluster* curClst, int &sum );

    // -- data members ----------------------------------------------
    std::vector< int >      coord;
    int                index;
    Cluster*           parent;    // association
    std::vector< Cluster* > children;  // association
    std::vector< Node* >    nodes;     // association
    Attribute*         attribute; // association
    int                attrValIdx;
    std::vector< Bundle* >  inBundles;  // association
    std::vector< Bundle* >  outBundles; // association
};

#endif

// -- end -----------------------------------------------------------
