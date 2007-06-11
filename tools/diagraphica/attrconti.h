// --- attrconti.h --------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef ATTRCONTI_H
#define ATTRCONTI_H

#include <algorithm>
#include <cstddef>
#include <string>
#include <map>
#include <vector>
using namespace std;
#include "attribute.h"
#include "value.h"

class AttrConti : public Attribute
{
public:
    // -- constructors and destructor -------------------------------
    AttrConti(
        Mediator* m,
        const string &nam,
        const string &typ,
        const int &idx,
        const double &lwrBnd,
        const double &uprBnd );
    AttrConti( const AttrConti &attr );
    virtual ~AttrConti();

    // -- set functions ---------------------------------------------
    
    void clusterValues(
        const vector< int > &indices,
        const string &newValue );
    void moveValue(
        const size_t &idxFr,
        const size_t &idxTo );

    void classifyEqualIntervals( const int &number );
    void classifyQuantiles( const int &number );
    void classifyMeanStandardDeviation( const int &number );
    void removeClassification();

    void setLowerBound( const double &lwrBnd );
    void setUpperBound( const double &uprBnd );

    // -- get functions ---------------------------------------------
    int getAttrType();

    double getLowerBound();
    double getUpperBound();
    void getRangeOrigValues(
        double &lwrBnd,
        double &uprBnd );

    int getSizeCurValues();
    Value* getCurValue( size_t idx );
    int getSizeMap();
    Value* mapToValue( double key );

    // -- clear functions -------------------------------------------
    void clearClusters();

protected:
    // -- private utility functions ---------------------------------
    void deleteCurValues();
    void deleteCurMap();

    // -- data members ----------------------------------------------
    vector< Value* >  curValues;  // composition, current domain
    map< double, Value* > curMap; // composition, map to current values

    double lowerBound;
    double upperBound;
};

#endif

// -- end -----------------------------------------------------------
