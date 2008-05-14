//  Author(s): A.j. (Hannes) pretorius
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./edge.h

// --- edge.h -------------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef EDGE_H
#define EDGE_H

#include <cstddef>
#include <string>
#include <vector>
using namespace std;

class Bundle;
class Node;

class Edge
{
public:
	// -- constructors and destructor -------------------------------
    Edge( 
        const int &idx,
        const string &lbl );
	virtual ~Edge();
	
    // -- set functions ---------------------------------------------
    void setInNode( Node* in );
    void setOutNode( Node* out );
    void setBundle( Bundle* b );
    
    // -- get functions ---------------------------------------------
    int getIndex();
    string getLabel();
    Node* getInNode();
    Node* getOutNode();
    Bundle* getBundle();

    // -- clear functions -------------------------------------------
    void clearInNode();
    void clearOutNode();
    void clearBundle();

protected:
    // -- data members ----------------------------------------------
	int index;      // index in list of graph edges
    string label;   // "action label"
    Node* inNode;   // association
	Node* outNode;  // association
    Bundle* bundle; // association
};

#endif

// -- end -----------------------------------------------------------
