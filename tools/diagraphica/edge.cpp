//  Author(s): A.j. (Hannes) pretorius
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./edge.cpp

// --- edge.cpp -----------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "edge.h"


// -- constructors and destructor -----------------------------------


// --------------------
Edge::Edge( 
    const int &idx,
    const string &lbl )
// --------------------
{
    index   = idx;
    label   = lbl;
	inNode  = NULL;
	outNode = NULL;
    bundle  = NULL;
}


// ----------
Edge::~Edge()
// ----------
{
	clearInNode();
    clearOutNode();
    clearBundle();
}


// -- set functions -------------------------------------------------


// -----------------------------
void Edge::setInNode( Node* in )
// -----------------------------
{
	inNode = in;
}


// -------------------------------
void Edge::setOutNode( Node* out )
// -------------------------------
{
	outNode = out;
}


// ------------------------------
void Edge::setBundle( Bundle* b )
// ------------------------------
{
    bundle = b;
}


// -- get functions -------------------------------------------------


// -----------------
int Edge::getIndex()
// -----------------
{
    return index;
}


// --------------------
string Edge::getLabel()
// --------------------
{
    return label;
}


// --------------------
Node* Edge::getInNode()
// --------------------
{
	return inNode;
}


// ---------------------
Node* Edge::getOutNode()
// ---------------------
{
	return outNode;
}


// ----------------------
Bundle* Edge::getBundle()
// ----------------------
{
    return bundle;
}


// -- clear functions -----------------------------------------------


// ---------------------
void Edge::clearInNode()
// ---------------------
{
    inNode = NULL;
}


// ----------------------
void Edge::clearOutNode()
// ----------------------
{
    outNode = NULL;
}


// ---------------------
void Edge::clearBundle()
// ---------------------
{
    bundle = NULL;
}


// -- end -----------------------------------------------------------
