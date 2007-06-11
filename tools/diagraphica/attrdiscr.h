// --- attrdiscr.h --------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef ATTRDISCR_H
#define ATTRDISCR_H

#include <algorithm>
#include <cstddef>
#include <string>
#include <map>
#include <vector>
using namespace std;
#include "attribute.h"
#include "value.h"

class AttrDiscr : public Attribute
{
public:
    // -- constructors and destructor -------------------------------
    AttrDiscr(
        Mediator* m,
        const string &nam,
        const string &typ,
        const int &idx,
        const vector< string > &vals );
    AttrDiscr( const AttrDiscr &attr );
    virtual ~AttrDiscr();

    // -- set functions ---------------------------------------------
    void clusterValues(
        const vector< int > &indices,
        const string &newValue );
    void moveValue(
        const int &idxFr,
        const int &idxTo );
    void configValues(
        const vector< string > &curDomain,
        map< int, int  > &origToCurDomain );

    // -- get functions ---------------------------------------------
    int getAttrType();

    int getSizeOrigValues();
    Value* getOrigValue( size_t idx );
    int getSizeCurValues();
    Value* getCurValue( size_t idx );
    int getSizeMap();
    Value* mapToValue( double key );

    // -- clear functions -------------------------------------------
    void clearClusters();

protected:
    // -- private utility functions ---------------------------------
    void initValues( const vector< string > &vals );
    void resetCurValues();
    void deleteOrigValues();
    void deleteCurValues();
    void deleteCurMap();

    // -- data members ----------------------------------------------
    vector< Value* >  origValues; // original domain
    vector< Value* >  curValues;  // composition, current domain
    vector< Value** > curMap;     // composition, map to current values

};

#endif

// -- end -----------------------------------------------------------
