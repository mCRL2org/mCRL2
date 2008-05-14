//  Author(s): A.j. (Hannes) pretorius
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./graph.h

// --- graph.h ------------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef GRAPH_H
#define GRAPH_H

#include <cstddef>
#include <map>
#include <set>
#include <vector>
using namespace std;
#include "attribute.h"
#include "attrconti.h"
#include "attrdiscr.h"
#include "bundle.h"
#include "cluster.h"
#include "colleague.h"
#include "edge.h"
#include "node.h"

class Mediator;

class Graph : public Colleague
{
public:
	// -- constructors and destructors ------------------------------
    Graph( Mediator* m );
	virtual ~Graph();

    // -- set functions ---------------------------------------------
    void setFileName( const string &fn );
/*
    void addAttribute(
        const string &name,
        const string &type,
        const int &idx,
        const vector< string > &vals );
    void addAttribute(
        const string &name,
        const string &type,
        const int &idx,
        const double &lwrBnd,
        const double &uprBnd );
*/
    void addAttrDiscr(
        const string &name,
        const string &type,
        const int &idx,
        const vector< string > &vals );
    void addAttrConti(
        const string &name,
        const string &type,
        const int &idx,
        const double &lwrBnd,
        const double &uprBnd );

    void swapAttributes(
        const size_t &idx1,
        const size_t &idx2 );
    void moveAttribute(
        const size_t &idxFr,
        const size_t &idxTo );
    void configAttributes( 
        map< int, int > &idcsFrTo,
        map< int, vector< string > > &attrCurDomains,
        map< int, map< int, int  > > &attrOrigToCurDomains );
    void duplAttributes( const vector< int > &idcs );
    void deleteAttribute( const int &idx );
    
    void addNode( const vector< double > &tpl );
    void addEdge(
        const string &lbl,
        const int &inNodeIdx,
        const int &outNodeIdx );
    
    void initGraph();

    // -- get functions  --------------------------------------------
    string getFileName();
    int getSizeAttributes();
    Attribute* getAttribute( const size_t &idx );
    Attribute* getAttribute( const string &name );
    int getSizeNodes();
    Node* getNode( const size_t &idx );
    int getSizeEdges();
    Edge* getEdge( const size_t &idx );
    Cluster* getRoot();
    Cluster* getCluster( const vector< int > coord );
    Cluster* getLeaf( const size_t &idx );
    int getSizeLeaves();
    Bundle* getBundle( const size_t &idx );
    int getSizeBundles();
    
    // -- calculation functions -------------------------------------
    void calcAttrDistr( 
        const int &attrIdx,
        vector< int > &distr );
    void calcAttrDistr(
        Cluster* clust,
        const int &attrIdx,
        vector< int > &distr );
    
    void calcAttrCorrl(
        const int &attrIdx1,
        const int &attrIdx2,
        vector< vector< int > > &corrlMap,
        vector< vector< int > > &number );
    void calcAttrCorrl(
        Cluster* clust,
        const int &attrIdx1,
        const int &attrIdx2,
        vector< vector< int > > &corrlMap,
        vector< vector< int > > &number );
    
    void calcAttrCombn(
        const vector< int > &attrIndcs,
        vector< vector< int > > &combs,
        vector< int > &number );
    void calcAttrCombn(
        Cluster* clust,
        const vector< int > &attrIndcs,
        vector< vector< int > > &combs,
        vector< int > &number );
    void calcAttrCombn(
        const vector< int > &attrIndcs,
        vector< vector< int > > &combs );
    void calcAttrCombn(
        Cluster* clust,
        const vector< int > &attrIndcs,
        vector< vector< int > > &combs );
    void calcAttrCombn(
        Cluster* clust,
        const vector< int > &attrIndcs,
        vector< vector< Node* > > &combs );
    void calcAttrCombn(
        Cluster* clust,
        const vector< Attribute* > &attrs,
        vector< Cluster* > &combs );
    
    bool hasMultAttrCombns(
        Cluster* clust,
        const vector< int > &attrIndcs );
    
    // -- cluster & bundle functions --------------------------------
    void clustNodesOnAttr( const vector< int > &attrIdcs );
    void clearSubClusters( const vector< int > &coord );
    
    int sumNodesInCluster( const vector< int > &coord );
    void sumNodesInCluster( 
        Cluster* clust,
        int &total );
    void getDescNodesInCluster( 
        const vector< int > &coord,
        vector< Node* > &nodes );
    void getDescNodesInCluster( 
        Cluster* clust,
        vector< Node* > &nodes );
    int calcMaxNumCombns( const vector< int > &attrIdcs );

protected:
    // -- private utility functions ---------------------------------
    void deleteAttributes();
    void addNode( Node* n );
    void deleteNodes();
    void addEdge( Edge* e );
    void deleteEdges();
    
    void initRoot();
    
    void clustNodesOnAttr(
        Cluster* clust,
        vector< int > attrIdcs,
        int &progress );
    void clustClusterOnAttr(
        const vector< int > coord,
        const int &attrIdx );
    void clustClusterOnAttr(
        Cluster* clust,
        const int &attrIdx );
    void clearSubClusters( Cluster* clust );
    
    void updateLeaves();
    void updateLeaves( Cluster* clust );
    //void updateLeaves( vector< Cluster* > &clusts );
    void clearLeaves();
    void deleteClusters();
    
    void printClusters();
    void printClusters( vector< Cluster* > &clusts );

    void updateBundles( int &progress );
    void updateBundles();
    void deleteBundles();
    void printBundles();

    // -- data members ----------------------------------------------
    string               fileName;   // file name
    vector< Attribute* > attributes; // attributes
    vector< Node* >      nodes;      // composition
	vector< Edge* >      edges;      // composition
    Cluster*             root;       // composition
    vector< Cluster* >   leaves;     // association
    vector< Bundle* >    bundles;    // composition

    // -- constants -------------------------------------------------
    static int PROGRESS_INTERV_HINT;
};

#endif

// -- end -----------------------------------------------------------
