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

#include "mcrl2/utilities/exception.h"
#include "cluster.h"

// -- constructors and destructors ----------------------------------

using namespace std;

Cluster::Cluster()
{
  index      = NON_EXISTING;
  parent     = NULL;
  attribute  = NULL;
  attrValIdx = NON_EXISTING;
}


Cluster::Cluster(const vector< size_t > &crd)
{
  coord      = crd;
  index      = NON_EXISTING;
  parent     = NULL;
  attribute  = NULL;
  attrValIdx = NON_EXISTING;
}


Cluster::Cluster(const Cluster& clst)
{
  coord      = clst.coord;
  index      = clst.index;
  parent     = clst.parent;
  children   = clst.children;
  nodes      = clst.nodes;
  attribute  = clst.attribute;
  attrValIdx = clst.attrValIdx;
  inBundles  = clst.inBundles;
  outBundles = clst.outBundles;
}


Cluster::~Cluster()
{
  clearParent();
  clearChildren();
  clearNodes();
  clearAttribute();
  clearInBundles();
  clearOutBundles();
}


// -- get functions -------------------------------------------------


size_t Cluster::getCoord(const size_t& idx)
{
  size_t result = NON_EXISTING;
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < coord.size())
  {
    result = coord[idx];
  }
  return result;
}


Cluster* Cluster::getChild(const size_t& idx)
{
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < children.size())
  {
    return children[idx];
  }
  else
  {
    throw mcrl2::runtime_error("Error retrieving cluster child.");
  }
}


Node* Cluster::getNode(const size_t& idx)
{
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < nodes.size())
  {
    return nodes[idx];
  }
  else
  {
    throw mcrl2::runtime_error("Error retrieving cluster node.");
  }
}


size_t Cluster::getSizeDescNodes()
{
  size_t sum = 0;
  getSizeDescNodes(this, sum);
  return sum;
}


Bundle* Cluster::getInBundle(const size_t& idx)
{
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < inBundles.size())
  {
    return inBundles[idx];
  }
  else
  {
    throw mcrl2::runtime_error("Error retrieving cluster incoming bundle.");
  }
}


Bundle* Cluster::getOutBundle(const size_t& idx)
{
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < outBundles.size())
  {
    return outBundles[idx];
  }
  else
  {
    throw mcrl2::runtime_error("Error retrieving cluster outgoing bundle.");
  }
}


// -- clear functions -----------------------------------------------


void Cluster::clearChildren()
{
  for (size_t i = 0; i < children.size(); ++i)
  {
    children[i] = NULL;
  }
  children.clear();
}


void Cluster::clearNodes()
{
  for (size_t i = 0; i < nodes.size(); ++i)
  {
    nodes[i] = NULL;
  }
  nodes.clear();
}


void Cluster::clearAttribute()
{
  attribute  = NULL;
  attrValIdx = NON_EXISTING;
}


void Cluster::clearInBundles()
{
  for (size_t i = 0; i < inBundles.size(); ++i)
  {
    inBundles[i] = NULL;
  }
  inBundles.clear();
}


void Cluster::clearOutBundles()
{
  for (size_t i = 0; i < outBundles.size(); ++i)
  {
    outBundles[i] = NULL;
  }
  outBundles.clear();
}


// -- utility functions ---------------------------------------------


void Cluster::getSizeDescNodes(
  Cluster* curClst,
  size_t& sum)
{
  if (curClst->getSizeChildren() == 0)
  {
    sum += curClst->getSizeNodes();
  }
  else
  {
    for (size_t i = 0; i < curClst->getSizeChildren(); ++i)
    {
      getSizeDescNodes(curClst->getChild(i), sum);
    }
  }
}



// -- end -----------------------------------------------------------
