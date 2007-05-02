// --- attribute.cpp ------------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "attribute.h"


// -- constructors and destructor -------------------------------


// -------------------------------
Attribute::Attribute(
    const string &nam,
    const string &typ,
    const int &idx,
    const vector< string > &vals )
// -------------------------------
{
    name   = nam;
    type   = typ;
    index  = idx;
    initValues( vals );
}


// ------------------------------------------
Attribute::Attribute( const Attribute &attr )
// ------------------------------------------
{
    index = attr.index;
    name  = attr.name;
    type  = attr.type;
    
    {
    for ( int i = 0; i < attr.origValues.size(); ++i )
        origValues.push_back( new Value( *(attr.origValues[i]) ) );
    }

    {
    for ( int i = 0; i < attr.curValues.size(); ++i )
        curValues.push_back( new Value( *(attr.curValues[i]) ) );
    }

    Value** mapping;
    {
    for ( int i = 0; i < attr.curMap.size(); ++i )
    { 
        mapping  = new Value*;
        *mapping = curValues[ ( *attr.curMap[i] )->getIndex() ]; 
        curMap.push_back( mapping );
    }
    }
    mapping = NULL;
}


// --------------------
Attribute::~Attribute()
// --------------------
{
    deleteOrigValues();
    deleteCurValues();
    deleteCurMap();
}


// -- set functions ---------------------------------------------


// ---------------------------------------
void Attribute::setIndex( const int &idx )
// ---------------------------------------
{
    index = idx;
}


// -----------------------------------------
void Attribute::setName( const string &nme )
// -----------------------------------------
{
    name = nme;
}


// ------------------------------
void Attribute::clusterValues(
    const vector< int > &indices,
	const string &newValue )
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
        sort( sorted.begin(), sorted.end() );

        // ptr to new value
        value = new Value( -1, newValue );
    
        // update current map
        {
        for ( int i = 0; i < curMap.size(); ++i )
        {
            for ( int j = 0; j < sorted.size(); ++j )
            {
                if ( *curMap[i] == curValues[sorted[j]] )
                    *curMap[i] = value;
            }
        }
        }

        // update current domain
        // get iterators to values to delete
        {
        for ( int i = 0; i < sorted.size(); ++i )
        {
            toRemove.push_back( curValues.begin() + sorted[i] );
        }
        }
        // delete these values
        {
        for ( int i = 0; i < toRemove.size(); ++i )
        {
            // subtract i fr toRemove[i] because
            // i objects & iterators have been deleted
            // delete object
            delete *(toRemove[i]-i);
            // remove ptr from current domain
            curValues.erase( toRemove[i]-i );
        }
        }
        
        // update index
        value->setIndex( sorted[0] );
        // insert new value
        curValues.insert( curValues.begin() + sorted[0], value );

        // update value indices after new one
        {
        for ( int i = sorted[0]; i < curValues.size(); ++i )
            curValues[i]->setIndex( i );
        }
    }
    catch( ... )
    {
        throw new string( "Error clustering attribute domain." );
    }
}


// ------------------------
void Attribute::swapValues(
    const int &idx1,
    const int &idx2 )
// ------------------------
{
    try
    {
        // swap values
        Value* temp     = curValues[idx1];
        curValues[idx1] = curValues[idx2];
        curValues[idx2] = temp;
        temp = NULL;

        // update value indices
        curValues[idx1]->setIndex( idx1 );
        curValues[idx2]->setIndex( idx2 );
    }
    catch( ... )
    {
        throw new string( "Error swapping attribute domain values." );
    }
}


// -----------------------
void Attribute::moveValue(
    const int &idxFr,
    const int &idxTo )
// -----------------------
{
    try
    {
        Value* temp = curValues[idxFr];
        
        // 2 cases to consider
        if ( idxFr < idxTo )
        {
            // move all values after idxFr 1 pos up
            for ( int i = idxFr; i < idxTo; ++i )
            {
                curValues[i] = curValues[i+1];
                curValues[i]->setIndex( i );
            }
            // update idxTo
            curValues[idxTo] = temp;
            curValues[idxTo]->setIndex( idxTo );
        }
        else if ( idxTo < idxFr )
        {
            // move all values before idxFr 1 pos down
            for ( int i = idxFr; i > idxTo; --i )
            {
                curValues[i] = curValues[i-1];
                curValues[i]->setIndex( i );
            }
            // update idxTo
            curValues[idxTo] = temp;
            curValues[idxTo]->setIndex( idxTo );
        }
        
        temp = NULL;
    }
    catch ( ... )
    {
        throw new string( "Error moving attribute domain value." );
    }
}


// ------------------------------------
void Attribute::configValues(
    const vector< string > &curDomain,
    map< int, int  > &origToCurDomain )
// ------------------------------------
{
    try
    {
        // clear current values
        {
        for ( int i = 0; i < curValues.size(); ++i )
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
        for ( int i = 0; i < curDomain.size(); ++i )
            curValues.push_back( new Value( i, curDomain[i] ) );
        }

        // re-init mapping to current values
        Value** mapping;
        {
        for ( int i = 0; i < origToCurDomain.size(); ++i )
        {
            mapping  = new Value*;
            *mapping = curValues[ origToCurDomain[i] ];
            curMap.push_back( mapping );
        }
        }
        mapping = NULL;
    }
    catch( const string* msg )
    {
        string* errMsg = new string( "Error configuring attribute domain values." );
        errMsg->append( "\n" );
        errMsg->append( *msg );

        delete msg;
        msg = NULL;
            
        throw errMsg;
    }
}


// -- get functions -------------------------------------------------


// ----------------------
int Attribute::getIndex()
// ----------------------
{
    return index;
}


// ------------------------
string Attribute::getName()
// ------------------------
{
    return name;
}


// ------------------------
string Attribute::getType()
// ------------------------
{
    return type;
}


// -------------------------------
int Attribute::getSizeOrigValues()
// -------------------------------
{
    return origValues.size();
}


// --------------------------------------
Value* Attribute::getOrigValue( int idx )
// --------------------------------------
{
    if ( 0 <= idx && idx < origValues.size() )
        return origValues[idx];
    else
        throw new string( 
            "Error retrieving attribute value from original domain." );
}


// ------------------------------
int Attribute::getSizeCurValues()
// ------------------------------
{
    return curValues.size();
}


// -------------------------------------
Value* Attribute::getCurValue( int idx )
// -------------------------------------
{
    if ( 0 <= idx && idx < curValues.size() )
        return curValues[idx];
    else
        throw new string( 
            "Error retrieving attribute value from current domain." );
}


// ------------------------
int Attribute::getSizeMap()
// ------------------------
{
    return curMap.size();
}


// ------------------------------------
Value* Attribute::mapToValue( int idx )
// ------------------------------------
{
    if ( 0 <= idx && idx < curMap.size() )
        return *curMap[idx];
    else
        throw new string( 
            "Error mapping index to current attribute value." );
}


// -- clear functions -----------------------------------------------


// ----------------------------
void Attribute::clearClusters()
// ----------------------------
{
    resetCurValues();
}


// -- private utility functions ---------------------------------


// -------------------------------------------------------
void Attribute::initValues( const vector< string > &vals )
// -------------------------------------------------------
{
    Value*  value0  = NULL;
    Value*  value1  = NULL;
    Value** mapping = NULL;
    
    // init orig & current domain, current map
    for ( int i = 0; i < vals.size(); ++i )
    {
        // init new values
        value0 = new Value( i, vals[i] );
        value1 = new Value( i, vals[i] );
        
        // init domains
        origValues.push_back( value0 );
        curValues.push_back( value1 );
        
        // init mapping: pointer to pointer
        mapping  = new Value*;
        *mapping = value1; 
        curMap.push_back( mapping );
    }
    
    value0  = NULL;
    value1  = NULL;
    mapping = NULL;
}


// -----------------------------
void Attribute::resetCurValues()
// -----------------------------
{
    Value* value    = NULL;
    Value** mapping = NULL;

    // clear current domain & mapping
    deleteCurValues();
    deleteCurMap();

    // reset current domain to original & update mapping
    for ( int i = 0; i < origValues.size(); ++i )
    {
        // call copy constructor
        value = new Value(
            origValues[i]->getIndex(),
            origValues[i]->getValue() );

        // init domain
        curValues.push_back( value );

        // init mapping: pointer to pointer
        mapping  = new Value*;
        *mapping = value;
        curMap.push_back( mapping );
    }

    value   = NULL;
    mapping = NULL;
}


// -------------------------------
void Attribute::deleteOrigValues()
// -------------------------------
{
    for ( int i = 0; i < origValues.size(); ++i )
    {
        delete origValues[i];
        origValues[i] = NULL;
    }
    origValues.clear();
}


// ------------------------------
void Attribute::deleteCurValues()
// ------------------------------
{
    for ( int i = 0; i < curValues.size(); ++i )
    {
        delete curValues[i];
        curValues[i] = NULL;
    }
    curValues.clear();
}


// ---------------------------
void Attribute::deleteCurMap()
// ---------------------------
{
    for ( int i = 0; i < curMap.size(); ++i )
    {
        delete curMap[i];
        curMap[i] = NULL;
    }
    curMap.clear();
}


// -- end -----------------------------------------------------------
