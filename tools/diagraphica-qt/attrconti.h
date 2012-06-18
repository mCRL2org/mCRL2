// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./attrconti.h

#ifndef ATTRCONTI_H
#define ATTRCONTI_H

#include <algorithm>
#include <cstddef>
#include <string>
#include <map>
#include <vector>
#include "attribute.h"
#include "value.h"

class AttrConti : public Attribute
{
  public:
    // -- constructors and destructor -------------------------------
    AttrConti(
      Mediator* m,
      const std::string& nam,
      const std::string& typ,
      const size_t& idx,
      const double& lwrBnd,
      const double& uprBnd);
    AttrConti(const AttrConti& attr);
    virtual ~AttrConti();

    // -- set functions ---------------------------------------------

    void clusterValues(
      const std::vector< int > &indices,
      const std::string& newValue);
    void moveValue(
      const size_t& idxFr,
      const size_t& idxTo);

    void classifyEqualIntervals(const size_t& number);
    void classifyQuantiles(const size_t& number);
    void classifyMeanStandardDeviation(const size_t& number);
    void removeClassification();

    void setLowerBound(const double& lwrBnd);
    void setUpperBound(const double& uprBnd);

    // -- get functions ---------------------------------------------
    int getAttrType();

    double getLowerBound();
    double getUpperBound();
    void getRangeOrigValues(
      double& lwrBnd,
      double& uprBnd);

    size_t getSizeCurValues();
    Value* getCurValue(size_t idx);
    size_t getSizeMap();
    Value* mapToValue(double key);

    // -- clear functions -------------------------------------------
    void clearClusters();

  protected:
    // -- private utility functions ---------------------------------
    void deleteCurValues();
    void deleteCurMap();

    // -- data members ----------------------------------------------
    std::vector< Value* >  curValues;  // composition, current domain
    std::map< double, Value* > curMap; // composition, std::map to current values

    double lowerBound;
    double upperBound;
};

#endif

// -- end -----------------------------------------------------------
