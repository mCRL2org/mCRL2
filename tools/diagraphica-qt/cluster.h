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

#include <QtCore>
#include <QtGui>

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
    Cluster(const std::vector< size_t > &crd);
    Cluster(const Cluster& clst);
    virtual ~Cluster();

    // -- set functions ---------------------------------------------
    void setCoord(const std::vector< size_t > &crd);
    void setIndex(const size_t& idx);
    void setParent(Cluster* p);
    void addChild(Cluster* c);
    void setChildren(const std::vector< Cluster* > &c);
    void addNode(Node* n);
    void setNodes(const std::vector< Node* > &n);

    void setAttribute(Attribute* attr);
    void setAttrValIdx(const size_t& idx);

    void addInBundle(Bundle* b);
    void setInBundles(const std::vector< Bundle* > b);
    void addOutBundle(Bundle* b);
    void setOutBundles(const std::vector< Bundle* > b);

    // -- get functions ---------------------------------------------
    size_t getSizeCoord();
    size_t getCoord(const size_t& idx);
    void getCoord(std::vector< size_t > &crd);
    size_t getIndex();
    Cluster* getParent();
    size_t getSizeChildren();
    Cluster* getChild(const size_t& idx);
    size_t getSizeNodes();
    Node* getNode(const size_t& idx);
    size_t getSizeDescNodes();

    Attribute* getAttribute();
    size_t getAttrValIdx();

    size_t getSizeInBundles();
    Bundle* getInBundle(const size_t& idx);
    size_t getSizeOutBundles();
    Bundle* getOutBundle(const size_t& idx);

    // -- clear functions -------------------------------------------
    void clearParent();
    void clearChildren();
    void clearNodes();
    void clearAttribute();
    void clearInBundles();
    void clearOutBundles();

  protected:
    // -- utility functions -----------------------------------------
    void getSizeDescNodes(Cluster* curClst, size_t& sum);

    // -- data members ----------------------------------------------
    std::vector< size_t >      coord;
    size_t                index;
    Cluster*           parent;    // association
    std::vector< Cluster* > children;  // association
    std::vector< Node* >    nodes;     // association
    Attribute*         attribute; // association
    size_t                attrValIdx;
    std::vector< Bundle* >  inBundles;  // association
    std::vector< Bundle* >  outBundles; // association
};

#endif

// -- end -----------------------------------------------------------
