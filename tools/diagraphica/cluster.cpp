// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./cluster.cpp

#include "wx.hpp" // precompiled headers

#include "cluster.h"

// -- constructors and destructors ----------------------------------

using namespace std;

// ---------------
Cluster::Cluster()
// ---------------
{
    index      = -1;
    parent     = NULL;
    attribute  = NULL;
    attrValIdx = -1;
}


// -----------------------------------------
Cluster::Cluster( const vector< int > &crd )
// -----------------------------------------
{
    coord      = crd;
    index      = -1;
    parent     = NULL;
    attribute  = NULL;
    attrValIdx = -1;
}


// ------------------------------------
Cluster::Cluster( const Cluster &clst )
// ------------------------------------
{
    coord 	   = clst.coord;
    index      = clst.index;
    parent     = clst.parent;
    children   = clst.children;
    nodes      = clst.nodes;
    attribute  = clst.attribute;
    attrValIdx = clst.attrValIdx;
    inBundles  = clst.inBundles;
    outBundles = clst.outBundles;
}


// ----------------
Cluster::~Cluster()
// ----------------
{
    clearParent();
    clearChildren();
    clearNodes();
    clearAttribute();
    clearInBundles();
    clearOutBundles();
}


// -- set functions -------------------------------------------------


// ----------------------------------------------
void Cluster::setCoord( const vector< int> &crd )
// ----------------------------------------------
{
    coord = crd;
}


// -------------------------------------
void Cluster::setIndex( const int &idx )
// -------------------------------------
{
    index = idx;
}


// ----------------------------------
void Cluster::setParent( Cluster* p )
// ----------------------------------
{
    parent = p;
}


// ---------------------------------
void Cluster::addChild( Cluster* c )
// ---------------------------------
{
    children.push_back( c );
}


// -----------------------------------------------------
void Cluster::setChildren( const vector< Cluster* > &c )
// -----------------------------------------------------
{
    clearChildren();
    children = c;
}


// -----------------------------
void Cluster::addNode( Node* n )
// -----------------------------
{
    nodes.push_back( n );
}


// -----------------------------------------------
void Cluster::setNodes( const vector< Node* > &n )
// -----------------------------------------------
{
    clearNodes();
    nodes = n;
}


// ------------------------------------------
void Cluster::setAttribute( Attribute* attr )
// ------------------------------------------
{
    attribute = attr;
}


// ------------------------------------------
void Cluster::setAttrValIdx( const int &idx )
// ------------------------------------------
{
    attrValIdx = idx;
}


// -----------------------------------
void Cluster::addInBundle( Bundle* b )
// -----------------------------------
{
    inBundles.push_back( b );
}


// ----------------------------------------------------
void Cluster::setInBundles( const vector< Bundle* > b )
// ----------------------------------------------------
{
    clearInBundles();
    inBundles = b;
}


// ------------------------------------
void Cluster::addOutBundle( Bundle* b )
// ------------------------------------
{
    outBundles.push_back( b );
}


// -----------------------------------------------------
void Cluster::setOutBundles( const vector< Bundle* > b )
// -----------------------------------------------------
{
    clearOutBundles();
    outBundles = b;
}


// -- get functions -------------------------------------------------


// ------------------------
int Cluster::getSizeCoord()
// ------------------------
{
    return coord.size();
}


// ------------------------------------
int Cluster::getCoord( const int &idx )
// ------------------------------------
{
    int result = -1;
    if ( 0 <= idx && static_cast <size_t> (idx) < coord.size() )
        result = coord[idx];
    return result;
}


// -----------------------------------------
void Cluster::getCoord( vector< int > &crd )
// -----------------------------------------
{
    crd = coord;
}


// --------------------
int Cluster::getIndex()
// --------------------
{
    return index;
}


// --------------------------
Cluster* Cluster::getParent()
// --------------------------
{
    return parent;
}


// ---------------------------
int Cluster::getSizeChildren()
// ---------------------------
{
    return children.size();
}


// -----------------------------------------
Cluster* Cluster::getChild( const int &idx )
// -----------------------------------------
{
    if ( 0 <= idx && static_cast <size_t> (idx) < children.size() )
        return children[idx];
    else
        throw new string( "Error retrieving cluster child." );
}


// ------------------------
int Cluster::getSizeNodes()
// ------------------------
{
    if (this != NULL)
      {return nodes.size();}
    else
      {return 0;}
}


// -------------------------------------
Node* Cluster::getNode( const int &idx )
// -------------------------------------
{
    if ( 0 <= idx && static_cast <size_t> (idx) < nodes.size() )
        return nodes[idx];
    else
        throw new string( "Error retrieving cluster node." );
}


// ----------------------------
int Cluster::getSizeDescNodes()
// ----------------------------
{
    int sum = 0;
    getSizeDescNodes( this, sum );
    return sum;
}


// -------------------------------
Attribute* Cluster::getAttribute()
// -------------------------------
{
    return attribute;
}


// -------------------------
int Cluster::getAttrValIdx()
// -------------------------
{
    return attrValIdx;
}


// ----------------------------
int Cluster::getSizeInBundles()
// ----------------------------
{
    return inBundles.size();
}


// -------------------------------------------
Bundle* Cluster::getInBundle( const int &idx )
// -------------------------------------------
{
	if ( 0 <= idx && static_cast <size_t> (idx) < inBundles.size() )
		return inBundles[idx];
	else
        throw new string( "Error retrieving cluster incoming bundle." );
}


// -----------------------------
int Cluster::getSizeOutBundles()
// -----------------------------
{
    return outBundles.size();
}


// --------------------------------------------
Bundle* Cluster::getOutBundle( const int &idx )
// --------------------------------------------
{
	if ( 0 <= idx && static_cast <size_t> (idx) < outBundles.size() )
		return outBundles[idx];
	else
        throw new string( "Error retrieving cluster outgoing bundle." );
}


// -- clear functions -----------------------------------------------


// ------------------------
void Cluster::clearParent()
// ------------------------
{
    parent = NULL;
}


// --------------------------
void Cluster::clearChildren()
// --------------------------
{
    for ( size_t i = 0; i < children.size(); ++i )
        children[i] = NULL;
    children.clear();
}


// -----------------------
void Cluster::clearNodes()
// -----------------------
{
    for ( size_t i = 0; i < nodes.size(); ++i )
        nodes[i] = NULL;
    nodes.clear();
}


// ---------------------------
void Cluster::clearAttribute()
// ---------------------------
{
    attribute  = NULL;
    attrValIdx = -1;
}


// ---------------------------
void Cluster::clearInBundles()
// ---------------------------
{
	for ( size_t i = 0; i < inBundles.size(); ++i )
		inBundles[i] = NULL;
    inBundles.clear();
}


// ----------------------------
void Cluster::clearOutBundles()
// ----------------------------
{
	for ( size_t i = 0; i < outBundles.size(); ++i )
		outBundles[i] = NULL;
    outBundles.clear();
}


// -- utility functions ---------------------------------------------


// ----------------------------
void Cluster::getSizeDescNodes(
    Cluster* curClst,
    int &sum )
// ----------------------------
{
    if ( curClst->getSizeChildren() == 0 )
        sum += curClst->getSizeNodes();
    else
    {
        for ( int i = 0; i < curClst->getSizeChildren(); ++i )
            getSizeDescNodes( curClst->getChild( i ), sum );
    }
}



// -- end -----------------------------------------------------------
