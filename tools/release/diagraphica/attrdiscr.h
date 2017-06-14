// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./attrdiscr.h

#ifndef ATTRDISCR_H
#define ATTRDISCR_H

#include <QtCore>
#include <QtGui>

#include <algorithm>
#include <cstddef>
#include <string>
#include <map>
#include <vector>
#include "attribute.h"
#include "value.h"

class AttrDiscr : public Attribute
{
  public:
    // -- constructors and destructor -------------------------------
    AttrDiscr(
      QString name,
      QString type,
      const std::size_t& idx,
      const std::vector< std::string > &vals);
    AttrDiscr(const AttrDiscr& attr);
    virtual ~AttrDiscr();

    // -- set functions ---------------------------------------------
    void clusterValues(
      const std::vector< int > &indices,
      const std::string& newValue);
    void moveValue(
      const std::size_t& idxFr,
      const std::size_t& idxTo);
    void configValues(
      const std::vector< std::string > &curDomain,
      std::map< std::size_t, std::size_t  > &origToCurDomain);

    std::size_t getSizeOrigValues();
    Value* getOrigValue(std::size_t idx);
    std::size_t getSizeCurValues();
    Value* getCurValue(std::size_t idx);
    std::size_t getSizeMap();
    Value* mapToValue(double key);

    // -- clear functions -------------------------------------------
    void clearClusters();

  protected:
    // -- private utility functions ---------------------------------
    void initValues(const std::vector< std::string > &vals);
    void resetCurValues();
    void deleteOrigValues();
    void deleteCurValues();
    void deleteCurMap();

    // -- data members ----------------------------------------------
    std::vector< Value* >  origValues; // original domain
    std::vector< Value* >  curValues;  // composition, current domain
    std::vector< Value** > curMap;     // composition, std::map to current values

};

#endif

// -- end -----------------------------------------------------------
