// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./graph.h

#ifndef GRAPH_H
#define GRAPH_H

#include <cstddef>
#include <map>
#include <vector>
#include <string>
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
    void setFileName( const std::string &fn );
/*
    void addAttribute(
        const std::string &name,
        const std::string &type,
        const int &idx,
        const std::vector< std::string > &vals );
    void addAttribute(
        const std::string &name,
        const std::string &type,
        const int &idx,
        const double &lwrBnd,
        const double &uprBnd );
*/
    void addAttrDiscr(
        const std::string &name,
        const std::string &type,
        const int &idx,
        const std::vector< std::string > &vals );
    void addAttrConti(
        const std::string &name,
        const std::string &type,
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
        std::map< int, int > &idcsFrTo,
        std::map< int, std::vector< std::string > > &attrCurDomains,
        std::map< int, std::map< int, int  > > &attrOrigToCurDomains );
    void duplAttributes( const std::vector< int > &idcs );
    void deleteAttribute( const int &idx );

    void addNode( const std::vector< double > &tpl );
    void addEdge(
        const std::string &lbl,
        const int &inNodeIdx,
        const int &outNodeIdx );

    void initGraph();

    // -- get functions  --------------------------------------------
    std::string getFileName();
    int getSizeAttributes();
    Attribute* getAttribute( const size_t &idx );
    Attribute* getAttribute( const std::string &name );
    int getSizeNodes();
    Node* getNode( const size_t &idx );
    int getSizeEdges();
    Edge* getEdge( const size_t &idx );
    Cluster* getRoot();
    Cluster* getCluster( const std::vector< int > coord );
    Cluster* getLeaf( const size_t &idx );
    int getSizeLeaves();
    Bundle* getBundle( const size_t &idx );
    int getSizeBundles();

    // -- calculation functions -------------------------------------
    void calcAttrDistr(
        const int &attrIdx,
        std::vector< int > &distr );
    void calcAttrDistr(
        Cluster* clust,
        const int &attrIdx,
        std::vector< int > &distr );

    void calcAttrCorrl(
        const int &attrIdx1,
        const int &attrIdx2,
        std::vector< std::vector< int > > &corrlMap,
        std::vector< std::vector< int > > &number );
    void calcAttrCorrl(
        Cluster* clust,
        const int &attrIdx1,
        const int &attrIdx2,
        std::vector< std::vector< int > > &corrlMap,
        std::vector< std::vector< int > > &number );

    void calcAttrCombn(
        const std::vector< int > &attrIndcs,
        std::vector< std::vector< int > > &combs,
        std::vector< int > &number );
    void calcAttrCombn(
        Cluster* clust,
        const std::vector< int > &attrIndcs,
        std::vector< std::vector< int > > &combs,
        std::vector< int > &number );
    void calcAttrCombn(
        const std::vector< int > &attrIndcs,
        std::vector< std::vector< int > > &combs );
    void calcAttrCombn(
        Cluster* clust,
        const std::vector< int > &attrIndcs,
        std::vector< std::vector< int > > &combs );
    void calcAttrCombn(
        Cluster* clust,
        const std::vector< int > &attrIndcs,
        std::vector< std::vector< Node* > > &combs );
    void calcAttrCombn(
        Cluster* clust,
        const std::vector< Attribute* > &attrs,
        std::vector< Cluster* > &combs );

    bool hasMultAttrCombns(
        Cluster* clust,
        const std::vector< int > &attrIndcs );

    // -- cluster & bundle functions --------------------------------
    void clustNodesOnAttr( const std::vector< int > &attrIdcs );
    void clearSubClusters( const std::vector< int > &coord );

    int sumNodesInCluster( const std::vector< int > &coord );
    void sumNodesInCluster(
        Cluster* clust,
        int &total );
    void getDescNodesInCluster(
        const std::vector< int > &coord,
        std::vector< Node* > &nodes );
    void getDescNodesInCluster(
        Cluster* clust,
        std::vector< Node* > &nodes );
    int calcMaxNumCombns( const std::vector< int > &attrIdcs );

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
        std::vector< int > attrIdcs,
        int &progress );
    void clustClusterOnAttr(
        const std::vector< int > coord,
        const int &attrIdx );
    void clustClusterOnAttr(
        Cluster* clust,
        const int &attrIdx );
    void clearSubClusters( Cluster* clust );

    void updateLeaves();
    void updateLeaves( Cluster* clust );
    //void updateLeaves( std::vector< Cluster* > &clusts );
    void clearLeaves();
    void deleteClusters();

    void printClusters();
    void printClusters( std::vector< Cluster* > &clusts );

    void updateBundles( int &progress );
    void updateBundles();
    void deleteBundles();
    void printBundles();

    // -- data members ----------------------------------------------
    std::string               fileName;   // file name
    std::vector< Attribute* > attributes; // attributes
    std::vector< Node* >      nodes;      // composition
	std::vector< Edge* >      edges;      // composition
    Cluster*             root;       // composition
    std::vector< Cluster* >   leaves;     // association
    std::vector< Bundle* >    bundles;    // composition

    // -- constants -------------------------------------------------
    static int PROGRESS_INTERV_HINT;
};

#endif

// -- end -----------------------------------------------------------
