// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./bundle.cpp

#include "mcrl2/utilities/exception.h"
#include "bundle.h"

// -- constructors and destructors ----------------------------------

using namespace std;

Bundle::Bundle()
{
  index = NON_EXISTING;
  parent = 0;
  inCluster = 0;
  outCluster = 0;
}


Bundle::Bundle(const size_t& idx)
{
  index      = idx;
  parent     = 0;
  inCluster  = 0;
  outCluster = 0;
}


Bundle::Bundle(const Bundle& bdl)
{
  index      = bdl.index;
  parent     = bdl.parent;
  children   = bdl.children;
  inCluster  = bdl.inCluster;
  outCluster = bdl.outCluster;
  edges      = bdl.edges;
  labels     = bdl.labels;
}


Bundle::~Bundle()
{
  clearParent();
  clearChildren();
  clearInCluster();
  clearOutCluster();
  clearEdges();
}


// -- set functions -------------------------------------------------


void Bundle::addEdge(Edge* e)
{
  edges.push_back(e);

  labels.insert(pair< string, string >(e->getLabel(), "MAY"));
}


void Bundle::setEdges(const vector< Edge* > &e)
{
  clearEdges();
  edges = e;

  labels.clear();
  for (size_t i = 0; i < edges.size(); ++i)
  {
    labels.insert(pair< string, string >(edges[i]->getLabel(), "MAY"));
  }
}


// -- get functions -------------------------------------------------


Bundle* Bundle::getChild(const size_t& idx)
{
  if (idx >= children.size())
    return 0;
  return children[idx];
}


Edge* Bundle::getEdge(const size_t& idx)
{
  if (idx >= edges.size())
    throw mcrl2::runtime_error("Error retrieving bundle edge.");
  return edges[idx];
}


void Bundle::getLabels(vector< string > &lbls)
{
  lbls.clear();

  map< string, string >::iterator it;
  for (it = labels.begin(); it != labels.end(); ++it)
  {
    lbls.push_back(it->first);
  }

}


void Bundle::getLabels(
  vector< string > &lbls,
  vector< string > &status)
{
  lbls.clear();
  status.clear();

  map< string, string >::iterator it;
  for (it = labels.begin(); it != labels.end(); ++it)
  {
    lbls.push_back(it->first);
    status.push_back(it->second);
  }
}


void Bundle::getLabels(
  string& separator,
  string& lbls)
{
  lbls = "";

  size_t size  = labels.size();
  size_t count = 0;

  map< string, string >::iterator it;
  for (it = labels.begin(); it != labels.end(); ++it)
  {
    count += 1;
    lbls  += it->first;
    lbls  += "[";
    lbls  += it->second;
    lbls  += "]";

    if (count < size)
    {
      lbls += separator;
    }
  }
}


// -- clear functions -----------------------------------------------


void Bundle::clearChildren()
{
  for (size_t i = 0; i < children.size(); ++i)
  {
    children[i] = 0;
  }
  children.clear();
}


void Bundle::clearEdges()
{
  for (size_t i = 0; i < edges.size(); ++i)
  {
    edges[i] = 0;
  }
  edges.clear();

  labels.clear();
}


// -- end -----------------------------------------------------------
