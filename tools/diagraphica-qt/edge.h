// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./edge.h

#ifndef EDGE_H
#define EDGE_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <string>

class Bundle;
class Node;

class Edge
{
  public:
    // -- constructors and destructor -------------------------------
    Edge(
      const size_t& idx,
      const std::string& lbl);
    virtual ~Edge();

    // -- set functions ---------------------------------------------
    void setInNode(Node* in) { inNode = in; }
    void setOutNode(Node* out) { outNode = out; }
    void setBundle(Bundle* b) { bundle = b; }

    // -- get functions ---------------------------------------------
    size_t getIndex() { return index; }
    std::string getLabel() { return label; }
    Node* getInNode() { return inNode; }
    Node* getOutNode() { return outNode; }
    Bundle* getBundle() { return bundle; }

    // -- clear functions -------------------------------------------
    void clearInNode() { inNode = NULL; }
    void clearOutNode() { outNode = NULL; }
    void clearBundle() { bundle = NULL; }

  protected:
    // -- data members ----------------------------------------------
    size_t index;      // index in list of graph edges
    std::string label;   // "action label"
    Node* inNode;   // association
    Node* outNode;  // association
    Bundle* bundle; // association
};

#endif

// -- end -----------------------------------------------------------
