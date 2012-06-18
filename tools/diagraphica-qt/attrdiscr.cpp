// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./attrdiscr.cpp

#include "wx.hpp" // precompiled headers

#include "mcrl2/exception.h"
#include "attrdiscr.h"

// -- constructors and destructor -----------------------------------

using namespace std;

// -------------------------------
AttrDiscr::AttrDiscr(
  Mediator* m,
  const string& nam,
  const string& typ,
  const size_t& idx,
  const vector< string > &vals)
  : Attribute(
    m,
    nam,
    typ,
    idx)
// -------------------------------
{
  name   = nam;
  type   = typ;
  index  = idx;
  initValues(vals);
}


// ------------------------------------------
AttrDiscr::AttrDiscr(const AttrDiscr& attr)
  :Attribute(attr)
// ------------------------------------------
{
  {
    for (size_t i = 0; i < attr.origValues.size(); ++i)
    {
      origValues.push_back(new Value(*(attr.origValues[i])));
    }
  }

  {
    for (size_t i = 0; i < attr.curValues.size(); ++i)
    {
      curValues.push_back(new Value(*(attr.curValues[i])));
    }
  }

  Value** mapping;
  {
    for (size_t i = 0; i < attr.curMap.size(); ++i)
    {
      mapping  = new Value*;
      *mapping = curValues[(*attr.curMap[i])->getIndex() ];
      curMap.push_back(mapping);
    }
  }
  mapping = NULL;
}


// --------------------
AttrDiscr::~AttrDiscr()
// --------------------
{
  deleteOrigValues();
  deleteCurValues();
  deleteCurMap();
}


// -- set functions -------------------------------------------------


// ------------------------------
void AttrDiscr::clusterValues(
  const vector< int > &indices,
  const string& newValue)
// ------------------------------
{
  try
  {
    // variables
    vector< int > sorted;
    Value*        value;
    vector< vector< Value* >::iterator > toRemove;

    // sort indices in ascending order
    sorted = indices;
    sort(sorted.begin(), sorted.end());

    // ptr to new value
    value = new Value(NON_EXISTING , newValue);

    // update current map
    {
      for (size_t i = 0; i < curMap.size(); ++i)
      {
        for (size_t j = 0; j < sorted.size(); ++j)
        {
          if (*curMap[i] == curValues[sorted[j]])
          {
            *curMap[i] = value;
          }
        }
      }
    }

    // update current domain
    // get iterators to values to delete
    {
      for (size_t i = 0; i < sorted.size(); ++i)
      {
        toRemove.push_back(curValues.begin() + sorted[i]);
      }
    }

    // delete these values
    {
      for (size_t i = 0; i < toRemove.size(); ++i)
      {
        // delete object
        delete *(toRemove[i]);
      }
    }
    {
      for (size_t i = toRemove.size(); i > 0; --i)
      {
        // remove ptr from current domain
        curValues.erase(toRemove[i-1]);
      }
    }

    // update index
    value->setIndex(sorted[0]);
    // insert new value
    curValues.insert(curValues.begin() + sorted[0], value);

    // update value indices after new one
    {
      for (size_t i = sorted[0]; i < curValues.size(); ++i)
      {
        curValues[i]->setIndex(i);
      }
    }

    // -*-
    value = NULL;
  }
  catch (...)
  {
    throw mcrl2::runtime_error("Error clustering attribute domain.");
  }
}


// -----------------------
void AttrDiscr::moveValue(
  const size_t& idxFr,
  const size_t& idxTo)
// -----------------------
{
  try
  {
    Value* temp = curValues[idxFr];

    // 2 cases to consider
    if (idxFr < idxTo)
    {
      // move all values after idxFr 1 pos up
      for (size_t i = idxFr; i < idxTo; ++i)
      {
        curValues[i] = curValues[i+1];
        curValues[i]->setIndex(i);
      }
      // update idxTo
      curValues[idxTo] = temp;
      curValues[idxTo]->setIndex(idxTo);
    }
    else if (idxTo < idxFr)
    {
      // move all values before idxFr 1 pos down
      for (size_t i = idxFr; i > idxTo; --i)
      {
        curValues[i] = curValues[i-1];
        curValues[i]->setIndex(i);
      }
      // update idxTo
      curValues[idxTo] = temp;
      curValues[idxTo]->setIndex(idxTo);
    }

    temp = NULL;
  }
  catch (...)
  {
    throw mcrl2::runtime_error("Error moving attribute domain value.");
  }
}


// ------------------------------------
void AttrDiscr::configValues(
  const vector< string > &curDomain,
  map< size_t , size_t  > &origToCurDomain)
// ------------------------------------
{
  try
  {
    // clear current values
    {
      for (size_t i = 0; i < curValues.size(); ++i)
      {
        delete curValues[i];
        curValues[i] = NULL;
      }
    }
    curValues.clear();

    // clear current map
    curMap.clear();

    // re-init current values
    {
      for (size_t i = 0; i < curDomain.size(); ++i)
      {
        curValues.push_back(new Value(i, curDomain[i]));
      }
    }

    // re-init mapping to current values
    Value** mapping;
    {
      for (size_t i = 0; i < origToCurDomain.size(); ++i)
      {
        mapping  = new Value*;
        *mapping = curValues[ origToCurDomain[i] ];
        curMap.push_back(mapping);
      }
    }
    mapping = NULL;
  }
  catch (const mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(string("Error configuring attribute domain values.\n") + string(e.what()));
  }
}


// -- get functions -------------------------------------------------


// -------------------------
int AttrDiscr::getAttrType()
// -------------------------
{
  return ATTR_TYPE_DISCR;
}


// -------------------------------
size_t AttrDiscr::getSizeOrigValues()
// -------------------------------
{
  return origValues.size();
}


// --------------------------------------
Value* AttrDiscr::getOrigValue(size_t idx)
// --------------------------------------
{
  if (idx != NON_EXISTING && idx < origValues.size())
  {
    return origValues[idx];
  }
  else
    throw mcrl2::runtime_error(
      "Error retrieving attribute value from original domain.");
}


// ------------------------------
size_t AttrDiscr::getSizeCurValues()
// ------------------------------
{
  return curValues.size();
}


// -------------------------------------
Value* AttrDiscr::getCurValue(size_t idx)
// -------------------------------------
{
  if (idx != NON_EXISTING && idx < curValues.size())
  {
    return curValues[idx];
  }
  else
    throw mcrl2::runtime_error(
      "Error retrieving attribute value from current domain.");
}


// ------------------------
size_t AttrDiscr::getSizeMap()
// ------------------------
{
  return curMap.size();
}


// ---------------------------------------
Value* AttrDiscr::mapToValue(double key)
// ---------------------------------------
{
  size_t idx = static_cast <size_t>(key);
  if (idx < curMap.size())
  {
    return *curMap[idx];
  }
  else
    throw mcrl2::runtime_error(
      "Error mapping index to current attribute value.");
}


// -- clear functions -----------------------------------------------


// ----------------------------
void AttrDiscr::clearClusters()
// ----------------------------
{
  resetCurValues();
}


// -- private utility functions -------------------------------------


// -------------------------------------------------------
void AttrDiscr::initValues(const vector< string > &vals)
// -------------------------------------------------------
{
  Value*  value0  = NULL;
  Value*  value1  = NULL;
  Value** mapping = NULL;

  // init orig & current domain, current map
  for (size_t i = 0; i < vals.size(); ++i)
  {
    // init new values
    value0 = new Value(i, vals[i]);
    value1 = new Value(i, vals[i]);

    // init domains
    origValues.push_back(value0);
    curValues.push_back(value1);

    // init mapping: pointer to pointer
    mapping  = new Value*;
    *mapping = value1;
    curMap.push_back(mapping);
  }

  value0  = NULL;
  value1  = NULL;
  mapping = NULL;
}


// -----------------------------
void AttrDiscr::resetCurValues()
// -----------------------------
{
  Value* value    = NULL;
  Value** mapping = NULL;

  // clear current domain & mapping
  deleteCurValues();
  deleteCurMap();

  // reset current domain to original & update mapping
  for (size_t i = 0; i < origValues.size(); ++i)
  {
    // call copy constructor
    value = new Value(
      origValues[i]->getIndex(),
      origValues[i]->getValue());

    // init domain
    curValues.push_back(value);

    // init mapping: pointer to pointer
    mapping  = new Value*;
    *mapping = value;
    curMap.push_back(mapping);
  }

  value   = NULL;
  mapping = NULL;
}


// -------------------------------
void AttrDiscr::deleteOrigValues()
// -------------------------------
{
  for (size_t i = 0; i < origValues.size(); ++i)
  {
    delete origValues[i];
    origValues[i] = NULL;
  }
  origValues.clear();
}


// ------------------------------
void AttrDiscr::deleteCurValues()
// ------------------------------
{
  for (size_t i = 0; i < curValues.size(); ++i)
  {
    delete curValues[i];
    curValues[i] = NULL;
  }
  curValues.clear();
}


// ---------------------------
void AttrDiscr::deleteCurMap()
// ---------------------------
{
  for (size_t i = 0; i < curMap.size(); ++i)
  {
    delete curMap[i];
    curMap[i] = NULL;
  }
  curMap.clear();
}


// -- end -----------------------------------------------------------
