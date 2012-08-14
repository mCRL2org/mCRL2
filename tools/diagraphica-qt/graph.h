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

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <map>
#include <vector>
#include <string>
#include "attribute.h"
#include "attrdiscr.h"
#include "bundle.h"
#include "cluster.h"
#include "edge.h"
#include "node.h"

class Mediator;

class Graph : public QObject
{
  Q_OBJECT

  public:
    // -- constructors and destructors ------------------------------
    Graph();
    ~Graph();

    // -- set functions ---------------------------------------------
    void setFileName(QString filename);
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
      QString name,
      QString type,
      const size_t& idx,
      const std::vector< std::string > &vals);

    void moveAttribute(
      const size_t& idxFr,
      const size_t& idxTo);
    void configAttributes(
      std::map< size_t , size_t > &idcsFrTo,
      std::map< size_t, std::vector< std::string > > &attrCurDomains,
      std::map< size_t, std::map< size_t, size_t  > > &attrOrigToCurDomains);
    void duplAttributes(const std::vector< size_t > &idcs);
    void deleteAttribute(const size_t& idx);

    void addNode(const std::vector< double > &tpl);
    void addEdge(
      const std::string& lbl,
      const size_t& inNodeIdx,
      const size_t& outNodeIdx);

    void initGraph();

    // -- get functions  --------------------------------------------
    QString filename();
    size_t getSizeAttributes();
    Attribute* getAttribute(const size_t& idx);
    Attribute* getAttribute(QString name);
    size_t getSizeNodes();
    Node* getNode(const size_t& idx);
    size_t getSizeEdges();
    Edge* getEdge(const size_t& idx);
    Cluster* getRoot();
    Cluster* getCluster(const std::vector< size_t > coord);
    Cluster* getLeaf(const size_t& idx);
    size_t getSizeLeaves();
    Bundle* getBundle(const size_t& idx);
    size_t getSizeBundles();

    // -- calculation functions -------------------------------------
    void calcAttrDistr(
      const size_t& attrIdx,
      std::vector< size_t > &distr);

    void calcAttrCorrl(
      const size_t& attrIdx1,
      const size_t& attrIdx2,
      std::vector< std::vector< size_t > > &corrlMap,
      std::vector< std::vector< int > > &number);

    void calcAttrCombn(
      const std::vector< size_t > &attrIndcs,
      std::vector< std::vector< size_t > > &combs,
      std::vector< size_t > &number);
    void calcAttrCombn(
      Cluster* clust,
      const std::vector< size_t > &attrIndcs,
      std::vector< std::vector< size_t > > &combs,
      std::vector< size_t > &number);
    void calcAttrCombn(
      const std::vector< size_t > &attrIndcs,
      std::vector< std::vector< size_t > > &combs);
    void calcAttrCombn(
      Cluster* clust,
      const std::vector< size_t > &attrIndcs,
      std::vector< std::vector< size_t > > &combs);
    void calcAttrCombn(
      Cluster* clust,
      const std::vector< size_t > &attrIndcs,
      std::vector< std::vector< Node* > > &combs);
    void calcAttrCombn(
      Cluster* clust,
      const std::vector< Attribute* > &attrs,
      std::vector< Cluster* > &combs);

    bool hasMultAttrCombns(
      Cluster* clust,
      const std::vector< int > &attrIndcs);

    // -- cluster & bundle functions --------------------------------
    void clustNodesOnAttr(const std::vector< size_t > &attrIdcs);
    void clearSubClusters(const std::vector< size_t > &coord);

    size_t sumNodesInCluster(const std::vector< size_t > &coord);
    void sumNodesInCluster(
      Cluster* clust,
      size_t& total);
    void getDescNodesInCluster(
      const std::vector< size_t > &coord,
      std::vector< Node* > &nodes);
    void getDescNodesInCluster(
      Cluster* clust,
      std::vector< Node* > &nodes);
    size_t calcMaxNumCombns(const std::vector< size_t > &attrIdcs);

  signals:
    void startedClusteringNodes(int steps);
    void startedClusteringEdges(int steps);
    void progressedClustering(int steps);
    void clusteringChanged();

  protected:
    // -- private utility functions ---------------------------------
    void deleteAttributes();
    void addNode(Node* n);
    void deleteNodes();
    void addEdge(Edge* e);
    void deleteEdges();

    void initRoot();

    void clustNodesOnAttr(
      Cluster* clust,
      std::vector< size_t > attrIdcs,
      size_t& progress);
    void clustClusterOnAttr(
      const std::vector< size_t > coord,
      const size_t& attrIdx);
    void clustClusterOnAttr(
      Cluster* clust,
      const size_t& attrIdx);
    void clearSubClusters(Cluster* clust);

    void updateLeaves();
    void updateLeaves(Cluster* clust);
    //void updateLeaves( std::vector< Cluster* > &clusts );
    void clearLeaves();
    void deleteClusters();

    void updateBundles(size_t& progress);
    void updateBundles();
    void deleteBundles();

    // -- data members ----------------------------------------------
    QString                   m_filename; // file name
    std::vector< Attribute* > attributes; // attributes
    std::vector< Node* >      nodes;      // composition
    std::vector< Edge* >      edges;      // composition
    Cluster*                  root;       // composition
    std::vector< Cluster* >   leaves;     // association
    std::vector< Bundle* >    bundles;    // composition
};

#endif

// -- end -----------------------------------------------------------
