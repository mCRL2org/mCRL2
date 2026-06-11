// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./attrdiscr.cpp

#include "mcrl2/utilities/exception.h"
#include "attrdiscr.h"

// -- constructors and destructor -----------------------------------

AttrDiscr::AttrDiscr(
  QString name,
  QString type,
  const std::size_t& idx,
  const std::vector< std::string >& vals)
  : Attribute(name, type, idx)
{
  initValues(vals);
}


AttrDiscr::AttrDiscr(const AttrDiscr& attr)
  :Attribute(attr)
{
  {
    for (auto origValue : attr.origValues)
    {
      origValues.push_back(new Value(*origValue));
    }
  }

  {
    for (auto curValue : attr.curValues)
    {
      curValues.push_back(new Value(*curValue));
    }
  }

  Value** mapping;
  {
    for (auto i : attr.curMap)
    {
      mapping  = new Value*;
      *mapping = curValues[(*i)->getIndex() ];
      curMap.push_back(mapping);
    }
  }
  mapping = nullptr;
}


AttrDiscr::~AttrDiscr()
{
  deleteOrigValues();
  deleteCurValues();
  deleteCurMap();
}


// -- set functions -------------------------------------------------


void AttrDiscr::clusterValues(
  const std::vector< int >& indices,
  const std::string& newValue)
{
  try
  {
    // variables
    std::vector< int > sorted;
    Value*        value;
    std::vector< std::vector< Value* >::iterator > toRemove;

    // sort indices in ascending order
    sorted = indices;
    sort(sorted.begin(), sorted.end());

    // ptr to new value
    value = new Value(NON_EXISTING, newValue);

    // update current map
    {
      for (auto & i : curMap)
      {
        for (int j : sorted)
        {
          if (*i == curValues[j])
          {
            *i = value;
          }
        }
      }
    }

    // update current domain
    // get iterators to values to delete
    {
      for (int i : sorted)
      {
        toRemove.push_back(curValues.begin() + i);
      }
    }

    // delete these values
    {
      for (auto & i : toRemove)
      {
        // delete object
        delete *i;
      }
    }
    {
      for (std::size_t i = toRemove.size(); i > 0; --i)
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
      for (std::size_t i = sorted[0]; i < curValues.size(); ++i)
      {
        curValues[i]->setIndex(i);
      }
    }

    // -*-
    value = nullptr;
  }
  catch (...)
  {
    throw mcrl2::runtime_error("Error clustering attribute domain.");
  }

  emit changed();
}


void AttrDiscr::moveValue(
  const std::size_t& idxFr,
  const std::size_t& idxTo)
{
  try
  {
    Value* temp = curValues[idxFr];

    // 2 cases to consider
    if (idxFr < idxTo)
    {
      // move all values after idxFr 1 pos up
      for (std::size_t i = idxFr; i < idxTo; ++i)
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
      for (std::size_t i = idxFr; i > idxTo; --i)
      {
        curValues[i] = curValues[i-1];
        curValues[i]->setIndex(i);
      }
      // update idxTo
      curValues[idxTo] = temp;
      curValues[idxTo]->setIndex(idxTo);
    }

    temp = nullptr;
  }
  catch (...)
  {
    throw mcrl2::runtime_error("Error moving attribute domain value.");
  }

  emit changed();
}


void AttrDiscr::configValues(
  const std::vector< std::string >& curDomain,
  std::map< std::size_t , std::size_t  >& origToCurDomain)
{
  try
  {
    // clear current values
    {
      for (auto & curValue : curValues)
      {
        delete curValue;
        curValue = nullptr;
      }
    }
    curValues.clear();

    // clear current map
    curMap.clear();

    // re-init current values
    {
      for (std::size_t i = 0; i < curDomain.size(); ++i)
      {
        curValues.push_back(new Value(i, curDomain[i]));
      }
    }

    // re-init mapping to current values
    Value** mapping;
    {
      for (unsigned long i : origToCurDomain)
      {
        mapping  = new Value*;
        *mapping = curValues[ i ];
        curMap.push_back(mapping);
      }
    }
    mapping = nullptr;
  }
  catch (const mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string("Error configuring attribute domain values.\n") + std::string(e.what()));
  }

  emit changed();
}


// -- get functions -------------------------------------------------


std::size_t AttrDiscr::getSizeOrigValues()
{
  return origValues.size();
}


Value* AttrDiscr::getOrigValue(std::size_t idx)
{
  if (idx != NON_EXISTING && idx < origValues.size())
  {
    return origValues[idx];
  }
  else
    throw mcrl2::runtime_error(
      "Error retrieving attribute value from original domain.");
}


std::size_t AttrDiscr::getSizeCurValues()
{
  return curValues.size();
}


Value* AttrDiscr::getCurValue(std::size_t idx)
{
  if (idx != NON_EXISTING && idx < curValues.size())
  {
    return curValues[idx];
  }
  else
    throw mcrl2::runtime_error(
      "Error retrieving attribute value from current domain.");
}


std::size_t AttrDiscr::getSizeMap()
{
  return curMap.size();
}


Value* AttrDiscr::mapToValue(double key)
{
  std::size_t idx = static_cast <std::size_t>(key);
  if (idx < curMap.size())
  {
    return *curMap[idx];
  }
  else
    throw mcrl2::runtime_error(
      "Error mapping index to current attribute value.");
}


// -- clear functions -----------------------------------------------


void AttrDiscr::clearClusters()
{
  resetCurValues();
  emit changed();
}


// -- private utility functions -------------------------------------


void AttrDiscr::initValues(const std::vector< std::string >& vals)
{
  Value*  value0  = nullptr;
  Value*  value1  = nullptr;
  Value** mapping = nullptr;

  // init orig & current domain, current map
  for (std::size_t i = 0; i < vals.size(); ++i)
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

  value0  = nullptr;
  value1  = nullptr;
  mapping = nullptr;
}


void AttrDiscr::resetCurValues()
{
  Value* value    = nullptr;
  Value** mapping = nullptr;

  // clear current domain & mapping
  deleteCurValues();
  deleteCurMap();

  // reset current domain to original & update mapping
  for (auto & origValue : origValues)
  {
    // call copy constructor
    value = new Value(
      origValue->getIndex(),
      origValue->getValue());

    // init domain
    curValues.push_back(value);

    // init mapping: pointer to pointer
    mapping  = new Value*;
    *mapping = value;
    curMap.push_back(mapping);
  }

  value   = nullptr;
  mapping = nullptr;
}


void AttrDiscr::deleteOrigValues()
{
  for (auto & origValue : origValues)
  {
    delete origValue;
    origValue = nullptr;
  }
  origValues.clear();
}


void AttrDiscr::deleteCurValues()
{
  for (auto & curValue : curValues)
  {
    delete curValue;
    curValue = nullptr;
  }
  curValues.clear();
}


void AttrDiscr::deleteCurMap()
{
  for (auto & i : curMap)
  {
    delete i;
    i = nullptr;
  }
  curMap.clear();
}


// -- end -----------------------------------------------------------
