// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./bundle.cpp

#include "mcrl2/utilities/exception.h"
#include "bundle.h"

// -- constructors and destructors ----------------------------------

Bundle::Bundle()
{
  index = NON_EXISTING;
  parent = nullptr;
  inCluster = nullptr;
  outCluster = nullptr;
}


Bundle::Bundle(const std::size_t& idx)
{
  index      = idx;
  parent     = nullptr;
  inCluster  = nullptr;
  outCluster = nullptr;
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

  labels.insert(std::pair< std::string, std::string >(e->getLabel(), "MAY"));
}


void Bundle::setEdges(const std::vector< Edge* > &e)
{
  clearEdges();
  edges = e;

  labels.clear();
  for (auto & edge : edges)
  {
    labels.insert(std::pair< std::string, std::string >(edge->getLabel(), "MAY"));
  }
}


// -- get functions -------------------------------------------------


Bundle* Bundle::getChild(const std::size_t& idx)
{
  if (idx >= children.size())
    return nullptr;
  return children[idx];
}


Edge* Bundle::getEdge(const std::size_t& idx)
{
  if (idx >= edges.size())
    throw mcrl2::runtime_error("Error retrieving bundle edge.");
  return edges[idx];
}


void Bundle::getLabels(std::vector< std::string > &lbls)
{
  lbls.clear();

  std::map< std::string, std::string >::iterator it;
  for (it = labels.begin(); it != labels.end(); ++it)
  {
    lbls.push_back(it->first);
  }

}


void Bundle::getLabels(
  std::vector< std::string > &lbls,
  std::vector< std::string > &status)
{
  lbls.clear();
  status.clear();

  std::map< std::string, std::string >::iterator it;
  for (it = labels.begin(); it != labels.end(); ++it)
  {
    lbls.push_back(it->first);
    status.push_back(it->second);
  }
}


void Bundle::getLabels(
  std::string& separator,
  std::string& lbls)
{
  lbls = "";

  std::size_t size  = labels.size();
  std::size_t count = 0;

  std::map< std::string, std::string >::iterator it;
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
  for (auto & i : children)
  {
    i = nullptr;
  }
  children.clear();
}


void Bundle::clearEdges()
{
  for (auto & edge : edges)
  {
    edge = nullptr;
  }
  edges.clear();

  labels.clear();
}


// -- end -----------------------------------------------------------
