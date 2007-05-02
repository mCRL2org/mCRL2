// --- attribute.h --------------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <algorithm>
#include <cstddef>
#include <string>
#include <map>
#include <vector>
using namespace std;
#include "value.h"

class Attribute
{
public:
    // -- constructors and destructor -------------------------------
    Attribute(
        const string &nam,
        const string &typ,
        const int &idx,
        const vector< string > &vals );
    Attribute( const Attribute &attr );
    virtual ~Attribute();

    // -- set functions ---------------------------------------------
    void setIndex( const int &idx );
    void setName( const string &nme );
    void clusterValues(
		const vector< int > &indices,
		const string &newValue );
    void swapValues(
        const int &idx1,
        const int &idx2 );
    void moveValue(
        const int &idxFr,
        const int &idxTo );
    void configValues(
        const vector< string > &curDomain,
        map< int, int  > &origToCurDomain );

    // -- get functions ---------------------------------------------
    int getIndex();
    string getName();
    string getType();
    int getSizeOrigValues();
    Value* getOrigValue( int idx );
    int getSizeCurValues();
    Value* getCurValue( int idx );
    int getSizeMap();
    Value* mapToValue( int idx );

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
    int    index;
    string name;
    string type;
    vector< Value* >  origValues; // original domain
    vector< Value* >  curValues;  // composition, current domain
    vector< Value** > curMap;     // composition, map to current values
};

#endif

// -- end -----------------------------------------------------------
