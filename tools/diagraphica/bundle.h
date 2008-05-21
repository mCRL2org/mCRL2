// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./bundle.h

#ifndef BUNDLE_H
#define BUNDLE_H

#include <cstddef>
#include <map>
#include <vector>
using namespace std;
#include "edge.h"
#include "cluster.h"

//class Cluster;

class Bundle
{
public:
	// -- constructors and destructors ------------------------------
    Bundle();
    Bundle( const int &idx );
    Bundle( const Bundle &bdl );
    virtual ~Bundle();
	
    // -- set functions ---------------------------------------------
    void setIndex( const int &idx );
    void setParent( Bundle* p );
    void addChild( Bundle* c );
    void setInCluster( Cluster* in );
    void setOutCluster( Cluster* out );
    void addEdge( Edge* e );
    void setEdges( const vector< Edge* > &e );
    void updateLabel(
        const string &lbl,
        const string &status );
    
    // -- get functions ---------------------------------------------
    int getIndex();
    Bundle* getParent();
    int getSizeChildren();
    Bundle* getChild( const size_t &idx );
    Cluster* getInCluster();
    Cluster* getOutCluster();
    int getSizeEdges();
    Edge* getEdge( const size_t &idx );
    void getLabels( vector< string > &lbls );
    void getLabels( 
        vector< string > &lbls,
        vector< string > &status );
    void getLabels( 
        string &separator,
        string &lbls );
    
    // -- clear functions -------------------------------------------
    void clearParent();
    void clearChildren();
    void clearInCluster();
    void clearOutCluster();
    void clearEdges();

protected:
    // -- data members ----------------------------------------------
	int index;
    Bundle* parent;
    vector< Bundle* > children; // association
    Cluster* inCluster;         // association
	Cluster* outCluster;        // association
    vector< Edge* > edges;      // association
    map< string, string > labels;
};

#endif

// -- end -----------------------------------------------------------
