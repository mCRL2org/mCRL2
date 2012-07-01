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

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <map>
#include <vector>
#include <string>
#include "edge.h"
#include "cluster.h"

//class Cluster;

class Bundle
{
  public:
    // -- constructors and destructors ------------------------------
    Bundle();
    Bundle(const size_t& idx);
    Bundle(const Bundle& bdl);
    virtual ~Bundle();


    // -- set functions ---------------------------------------------
    void setIndex(const size_t& idx) { index = idx; }
    void setParent(Bundle* p) { parent = p; }
    void addChild(Bundle* c) { children.push_back(c); }
    void setInCluster(Cluster* in) { inCluster = in; }
    void setOutCluster(Cluster* out) { outCluster = out; }
    void addEdge(Edge* e);
    void setEdges(const std::vector< Edge* > &e);
    void updateLabel(const std::string& lbl, const std::string& status) { labels[lbl] = status; }


    // -- get functions ---------------------------------------------
    size_t getIndex() { return index; }
    Bundle* getParent() { return parent; }
    size_t getSizeChildren() { return children.size(); }
    Bundle* getChild(const size_t& idx);
    Cluster* getInCluster() { return inCluster; }
    Cluster* getOutCluster() { return outCluster; }
    size_t getSizeEdges() { return edges.size(); }
    Edge* getEdge(const size_t& idx);
    void getLabels(std::vector< std::string > &lbls);
    void getLabels(
      std::vector< std::string > &lbls,
      std::vector< std::string > &status);
    void getLabels(
      std::string& separator,
      std::string& lbls);

    // -- clear functions -------------------------------------------
    void clearParent() { parent = NULL; }
    void clearChildren();
    void clearInCluster() { inCluster = NULL; }
    void clearOutCluster() { outCluster = NULL; }
    void clearEdges();

  protected:
    // -- data members ----------------------------------------------
    size_t index;
    Bundle* parent;
    std::vector< Bundle* > children; // association
    Cluster* inCluster;         // association
    Cluster* outCluster;        // association
    std::vector< Edge* > edges;      // association
    std::map< std::string, std::string > labels;
};

#endif

// -- end -----------------------------------------------------------
