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

#include <algorithm>
#include <cstddef>
#include <string>
#include <map>
#include <vector>
using namespace std;
#include "colleague.h"
#include "value.h"

class Attribute : public Colleague
{
public:
    // -- constructors and destructor -------------------------------
    Attribute(
        Mediator* m,
        const string &nam,
        const string &typ,
        const int &idx );
    Attribute( const Attribute &attr );
    virtual ~Attribute();

    // -- set functions ---------------------------------------------
    void setIndex( const int &idx );
    void setName( const string &nme );
    void setType( const string &typ );

    virtual void clusterValues(
        const vector< int > &indices,
        const string &newValue );
    virtual void moveValue(
        const int &idxFr,
        const int &idxTo );
    
    // functions overridden by AttrDiscr
    virtual void configValues(
        const vector< string > &curDomain,
        map< int, int  > &origToCurDomain );

    // functions overridden by AttrConti
    virtual void classifyEqualIntervals( const int &number );
    virtual void classifyQuantiles( const int &number );
    virtual void classifyMeanStandardDeviation( const int &number );
    virtual void removeClassification();

    // -- get functions ---------------------------------------------
    int getIndex();
    string getName();
    string getType();
    virtual int getAttrType() = 0;

    // functions overridden by AttrDiscr
    virtual int getSizeOrigValues();
    virtual Value* getOrigValue( int idx );
    virtual Value* getCurValue( int idx );

    // functions overridden by AttrConti
    virtual double getLowerBound();
    virtual double getUpperBound();
    virtual void getRangeOrigValues(
        double &lwrBnd,
        double &uprBnd );

    virtual int getSizeCurValues() = 0;
    virtual Value* mapToValue( double key ) = 0;

    // -- clear functions -------------------------------------------
    virtual void clearClusters() = 0;

// -- public constants ------------------------------------------
    enum
    {
        ATTR_TYPE_CONTI,
        ATTR_TYPE_DISCR,
        PART_METH_EQUAL_INTERVALS,
        PART_METH_QUANTILES,
        PART_METH_MEAN_STANDARD_DEVIATION,
    };

protected:
    // -- private utility functions ---------------------------------
    virtual void deleteCurValues() = 0;
    virtual void deleteCurMap() = 0;

    // -- data members ----------------------------------------------
    int    index;
    string name;
    string type;
};

#endif

// -- end -----------------------------------------------------------
