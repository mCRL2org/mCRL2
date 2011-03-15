// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./node.cpp

#include "wx.hpp" // precompiled headers

#include "mcrl2/exception.h"
#include "node.h"

#include <string>

// -- constructors and destructors ----------------------------------

using namespace std;

// -------------------------
Node::Node(const size_t& idx)
// -------------------------
{
  index   = idx;
  cluster = NULL;
}


// ---------------------------
Node::Node(
  const size_t& idx,
  const vector< double > &tpl)
// ---------------------------
{
  index   = idx;
  tuple   = tpl;
  cluster = NULL;
}


// ----------
Node::~Node()
// ----------
{
  clearInEdges();
  clearOutEdges();
}


// -- set functions -------------------------------------------------


// ---------------------
void Node::swapTupleVal(
  const size_t& idx1,
  const size_t& idx2)
// ---------------------
{
  if (idx1 < tuple.size() &&
      idx2 < tuple.size())
  {
    double temp = tuple[idx1];
    tuple[idx1] = tuple[idx2];
    tuple[idx2] = temp;
  }
  else
  {
    throw mcrl2::runtime_error("Error swapping node tuple values.");
  }
}


// ---------------------
void Node::moveTupleVal(
  const size_t& idxFr,
  const size_t& idxTo)
// ---------------------
{
  if (idxFr < tuple.size() &&
      idxTo < tuple.size())
  {
    double temp = tuple[idxFr];

    // 2 cases to consider
    if (idxFr < idxTo)
    {
      // move all values after idxFr 1 pos up
      for (size_t i = idxFr; i < idxTo; ++i)
      {
        tuple[i] = tuple[i+1];
      }
      // update idxTo
      tuple[idxTo] = temp;
    }
    else if (idxTo < idxFr)
    {
      // move all values before idxFr 1 pos down
      for (size_t i = idxFr; i > idxTo; --i)
      {
        tuple[i] = tuple[i-1];
      }
      // update idxTo
      tuple[idxTo] = temp;
    }
  }
  else
  {
    throw mcrl2::runtime_error("Error moving node tuple value.");
  }
}


// --------------------------------------------------
void Node::moveTupleVals(map< size_t , size_t > &idcsFrTo)
// --------------------------------------------------
{
  if (idcsFrTo.size() == tuple.size())
  {
    try
    {
      vector< double > tupleNew;

      // init new tuple
      {
        for (size_t i = 0; i < idcsFrTo.size(); ++i)
        {
          tupleNew.push_back(-1);
        }
      }

      // update new tuple
      {
        for (size_t i = 0; i < idcsFrTo.size(); ++i)
        {
          tupleNew[ idcsFrTo[i] ] = tuple[i];
        }
      }

      // set tuple to new list of attributes
      tuple.clear();
      tuple = tupleNew;
      tupleNew.clear();
    }
    catch (...)
    {
      throw mcrl2::runtime_error("Error moving node tuple values.");
    }
  }
  else
  {
    throw mcrl2::runtime_error("Error moving node tuple values.");
  }
}


// --------------------
void Node::addTupleVal(
  const size_t& idx,
  const double& val)
// --------------------
{
  tuple.insert(
    tuple.begin() + idx,
    val);
}


// -------------------------------------
void Node::delTupleVal(const size_t& idx)
// -------------------------------------
{
  tuple.erase(tuple.begin() + idx);
}


// -----------------------------
void Node::addInEdge(Edge* in)
// -----------------------------
{
  inEdges.push_back(in);
}


// ---------------------------------------------
void Node::setInEdges(const vector< Edge* > e)
// ---------------------------------------------
{
  clearInEdges();
  inEdges = e;
}


// -------------------------------
void Node::addOutEdge(Edge* out)
// -------------------------------
{
  outEdges.push_back(out);
}


// ----------------------------------------------
void Node::setOutEdges(const vector< Edge* > e)
// ----------------------------------------------
{
  clearOutEdges();
  outEdges = e;
}


// --------------------------------
void Node::setCluster(Cluster* c)
// --------------------------------
{
  cluster = c;
}


// -- get functions -------------------------------------------------


// -----------------
size_t Node::getIndex()
// -----------------
{
  return index;
}


// ---------------------
size_t Node::getSizeTuple()
// ---------------------
{
  return tuple.size();
}


// ------------------------------------
double Node::getTupleVal(const size_t& idx)
// ------------------------------------
{
  if (idx < tuple.size())
  {
    return tuple[idx];
  }
  else
  {
    throw mcrl2::runtime_error("Error retrieving node tuple value.");
  }
}


// -----------------------
size_t Node::getSizeInEdges()
// -----------------------
{
  return inEdges.size();
}


// ------------------------------------
Edge* Node::getInEdge(const size_t& idx)
// ------------------------------------
{
  if (idx < inEdges.size())
  {
    return inEdges[idx];
  }
  else
  {
    throw mcrl2::runtime_error("Error retrieving node incoming edge.");
  }
}


// ------------------------
size_t Node::getSizeOutEdges()
// ------------------------
{
  return outEdges.size();
}


// -------------------------------------
Edge* Node::getOutEdge(const size_t& idx)
// -------------------------------------
{
  if (idx < outEdges.size())
  {
    return outEdges[idx];
  }
  else
  {
    throw mcrl2::runtime_error("Error retrieving node outgoing edge.");
  }
}


// ------------------------
Cluster* Node::getCluster()
// ------------------------
{
  return cluster;
}


// -- clear functions -----------------------------------------------


// ----------------------
void Node::clearInEdges()
// ----------------------
{
  for (size_t i = 0; i < inEdges.size(); ++i)
  {
    inEdges[i] = NULL;
  }
  inEdges.clear();
}


// -----------------------
void Node::clearOutEdges()
// -----------------------
{
  for (size_t i = 0; i < outEdges.size(); ++i)
  {
    outEdges[i] = NULL;
  }
  outEdges.clear();
}


// ----------------------
void Node::clearCluster()
// ----------------------
{
  cluster = NULL;
}


// -- private utility functions -------------------------------------


// -- end -----------------------------------------------------------
