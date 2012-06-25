// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./attribute.h

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <QtCore>
#include <QtGui>

#include <algorithm>
#include <cstddef>
#include <string>
#include <map>
#include <vector>
#include "colleague.h"
#include "value.h"

class Attribute : public Colleague
{
  public:
    // -- constructors and destructor -------------------------------
    Attribute(
      Mediator* m,
      const std::string& nam,
      const std::string& typ,
      const size_t& idx);
    Attribute(const Attribute& attr);
    virtual ~Attribute();

    // -- set functions ---------------------------------------------
    void setIndex(const size_t& idx);
    void setName(const std::string& nme);
    void setType(const std::string& typ);

    virtual void clusterValues(
      const std::vector< int > &indices,
      const std::string& newValue);
    virtual void moveValue(
      const size_t& idxFr,
      const size_t& idxTo);

    // functions overridden by AttrDiscr
    virtual void configValues(
      const std::vector< std::string > &curDomain,
      std::map< size_t, size_t  > &origToCurDomain);

    // -- get functions ---------------------------------------------
    size_t getIndex();
    std::string getName();
    std::string getType();

    // functions overridden by AttrDiscr
    virtual size_t getSizeOrigValues();
    virtual Value* getOrigValue(size_t idx);
    virtual Value* getCurValue(size_t idx);

    virtual size_t getSizeCurValues() = 0;
    virtual Value* mapToValue(double key) = 0;

    // -- clear functions -------------------------------------------
    virtual void clearClusters() = 0;

  protected:
    // -- private utility functions ---------------------------------
    virtual void deleteCurValues() = 0;
    virtual void deleteCurMap() = 0;

    // -- data members ----------------------------------------------
    size_t    index;
    std::string name;
    std::string type;
};

#endif

// -- end -----------------------------------------------------------
