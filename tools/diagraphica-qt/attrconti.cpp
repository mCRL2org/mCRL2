// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./attrconti.cpp

#include "wx.hpp" // precompiled headers

#include "attrconti.h"

// -- constructors and destructor -----------------------------------

using namespace std;

// -----------------------
AttrConti::AttrConti(
  Mediator* m,
  const string& nam,
  const string& typ,
  const size_t& idx,
  const double& lwrBnd,
  const double& uprBnd)
  : Attribute(
    m,
    nam,
    typ,
    idx)
// -----------------------
{
  lowerBound = lwrBnd;
  upperBound = uprBnd;
}


// ------------------------------------------
AttrConti::AttrConti(const AttrConti& attr)
  :Attribute(attr)
// ------------------------------------------
{
  for (size_t i = 0; i < attr.curValues.size(); ++i)
  {
    curValues.push_back(new Value(*attr.curValues[i]));
  }

  map< double, Value* >::const_iterator it;
  size_t idx;
  Value* val;
  for (it = attr.curMap.begin(); it != attr.curMap.end(); ++it)
  {
    idx = it->second->getIndex();
    val = curValues[idx];
    curMap.insert(pair< double, Value* >(it->first, val));
  }
  val = NULL;

  lowerBound = attr.lowerBound;
  upperBound = attr.upperBound;
}


// --------------------
AttrConti::~AttrConti()
// --------------------
{
  deleteCurValues();
  deleteCurMap();
}


// -- set functions -------------------------------------------------


// ------------------------------
void AttrConti::clusterValues(
  const vector< int > &indices,
  const string& newValue)
// ------------------------------
{
  // variables
  bool          valid;
  vector< int > sorted;
  Value*        value;
  vector< vector< Value* >::iterator > toRemove;

  // sort indices in ascending order
  sorted = indices;
  sort(sorted.begin(), sorted.end());

  // make sure all indices are within bounds
  valid = true;
  for (size_t i = 0; i < indices.size(); ++i)
  {
    if (indices[i] < 0 || indices[i] > (int) curValues.size())
    {
      valid = false;
      break;
    }
  }

  if (valid == true)
  {
    // ptr to new value
    value = new Value(NON_EXISTING , newValue);

    // update current map
    map< double, Value* >::iterator it;

    for (it = curMap.begin(); it != curMap.end(); ++it)
    {
      for (size_t j = 0; j < sorted.size(); ++j)
      {
        if (it->second == curValues[sorted[j]])
        {
          it->second = value;
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
      for (int i = sorted[0]; i < static_cast<int>(curValues.size()); ++i)
      {
        curValues[i]->setIndex(i);
      }
    }

    value = NULL;
  }
}


// -----------------------
void AttrConti::moveValue(
  const size_t& idxFr,
  const size_t& idxTo)
// -----------------------
{
  if ((idxFr < curValues.size()) &&
      (idxTo < curValues.size()))
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
}


// --------------------------------------------------------
void AttrConti::classifyEqualIntervals(const size_t& number)
// --------------------------------------------------------
{
  if (number == 0)
  {
    // remove partitions
    deleteCurValues();
    deleteCurMap();
  }
  else if (number > 0 && (upperBound-lowerBound) > 0)
  {
    vector< double > values;
    vector< string > legend;
    map< double, size_t > valuesToLegend;

    // calc classification
    mediator->getAttrValues(this->getIndex(), values);
    Utils::classEqualIntervals(
      number,
      values,
      legend,
      valuesToLegend);

    // delete previous values and mapping
    deleteCurValues();
    deleteCurMap();

    // init new values
    Value* value;
    for (size_t i = 0; i < legend.size(); ++i)
    {
      value = new Value(i, legend[i]);
      curValues.push_back(value);
    }
    value = NULL;

    // init new mapping
    map< double, size_t >::iterator it;
    for (it = valuesToLegend.begin(); it != valuesToLegend.end(); ++it)
    {
      curMap.insert(pair< double, Value* >(it->first, curValues[it->second]));
    }
  }
}


// ---------------------------------------------------
void AttrConti::classifyQuantiles(const size_t& number)
// ---------------------------------------------------
{
  if (number == 0)
  {
    // remove partitions
    deleteCurValues();
    deleteCurMap();
  }
  else if (number > 0 && (upperBound-lowerBound) > 0)
  {
    //vector< double > values;
    set< double > values;
    vector< string > legend;
    map< double, size_t > valuesToLegend;

    // calc classification
    mediator->getAttrValues(this->getIndex(), values);
    Utils::classifyQuantiles(
      number,
      values,
      legend,
      valuesToLegend);

    // delete previous values and mapping
    deleteCurValues();
    deleteCurMap();

    // init new values
    Value* value;
    for (size_t i = 0; i < legend.size(); ++i)
    {
      value = new Value(i, legend[i]);
      curValues.push_back(value);
    }
    value = NULL;

    // init new mapping
    map< double, size_t >::iterator it;
    for (it = valuesToLegend.begin(); it != valuesToLegend.end(); ++it)
    {
      curMap.insert(pair< double, Value* >(it->first, curValues[it->second]));
    }
  }
}


// ---------------------------------------------------------------
void AttrConti::classifyMeanStandardDeviation(const size_t& number)
// ---------------------------------------------------------------
{
  if (number == 0)
  {
    // remove partitions
    deleteCurValues();
    deleteCurMap();
  }
  else if (number > 0 && (upperBound-lowerBound) > 0)
  {
    vector< double > values;
    vector< string > legend;
    map< double, size_t > valuesToLegend;

    // calc classification
    mediator->getAttrValues(this->getIndex(), values);
    Utils::classifyMeanStandardDeviation(
      number,
      values,
      legend,
      valuesToLegend);

    // delete previous values and mapping
    deleteCurValues();
    deleteCurMap();

    // init new values
    Value* value;
    for (size_t i = 0; i < legend.size(); ++i)
    {
      value = new Value(i, legend[i]);
      curValues.push_back(value);
    }
    value = NULL;

    // init new mapping
    map< double, size_t >::iterator it;
    for (it = valuesToLegend.begin(); it != valuesToLegend.end(); ++it)
    {
      curMap.insert(pair< double, Value* >(it->first, curValues[it->second]));
    }
    /*
            // -*- //
            *mediator << "Mean: ";
            *mediator << Utils::dblToStr( Utils::mean( values ) );
            *mediator << "\n";
            *mediator << "Std dev: ";
            *mediator << Utils::dblToStr( Utils::stdDev( values ) );
            *mediator << "\n";
            *mediator << Utils::intToStr( legend.size() );
            *mediator << "\n---\n";
            for ( int i = 0; i < legend.size(); ++i )
            {
                *mediator << legend[i];
                *mediator << "\n";
            }
            *mediator << Utils::intToStr( valuesToLegend.size() );
            *mediator << "\n---\n";
            map< double, int >::iterator it;
            for ( it = valuesToLegend.begin(); it != valuesToLegend.end(); ++it )
            {
                *mediator << Utils::dblToStr( it->first );
                *mediator << " -> ";
                *mediator << Utils::intToStr( it->second );
                *mediator << "\n";
            }
            *mediator << "\n";
    */
  }
}


// -----------------------------------
void AttrConti::removeClassification()
// -----------------------------------
{
  // remove partitions
  deleteCurValues();
  deleteCurMap();
}


// --------------------------------------------------
void AttrConti::setLowerBound(const double& lwrBnd)
// --------------------------------------------------
{
  lowerBound = lwrBnd;
}


// --------------------------------------------------
void AttrConti::setUpperBound(const double& uprBnd)
// --------------------------------------------------
{
  upperBound = uprBnd;
}


// -- get functions -------------------------------------------------


// ------------------------
int AttrConti::getAttrType()
// -------------------------
{
  return ATTR_TYPE_CONTI;
}


// ------------------------------
double AttrConti::getLowerBound()
// ------------------------------
{
  return lowerBound;
}


// ------------------------------
double AttrConti::getUpperBound()
// ------------------------------
{
  return upperBound;
}


// --------------------------------
void AttrConti::getRangeOrigValues(
  double& lwrBnd,
  double& uprBnd)
// --------------------------------
{
  lwrBnd = lowerBound;
  uprBnd = upperBound;
}


// ------------------------------
size_t AttrConti::getSizeCurValues()
// ------------------------------
{
  return curValues.size();
}


// -------------------------------------
Value* AttrConti::getCurValue(size_t idx)
// -------------------------------------
{
  Value* result = NULL;
  if (idx != NON_EXISTING && idx < curValues.size())
  {
    result = curValues[idx];
  }
  return result;
}


// ------------------------
size_t AttrConti::getSizeMap()
// ------------------------
{
  return curMap.size();
}


// ---------------------------------------
Value* AttrConti::mapToValue(double key)
// ---------------------------------------
{
  Value* result = NULL;

  map< double, Value* >::iterator it;
  it = curMap.find(key);
  if (it != curMap.end())
  {
    result = it->second;
  }

  return result;
}


// -- clear functions -----------------------------------------------


// ----------------------------
void AttrConti::clearClusters()
// ----------------------------
{}


// -- private utility functions -------------------------------------


// ------------------------------
void AttrConti::deleteCurValues()
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
void AttrConti::deleteCurMap()
// ---------------------------
{
  map< double, Value* >::iterator it;
  for (it = curMap.begin(); it != curMap.end(); ++it)
  {
    it->second = NULL;
  }
  curMap.clear();
}


// -- end -----------------------------------------------------------
