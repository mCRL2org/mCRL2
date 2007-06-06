// --- value.cpp ----------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "value.h"


// -- constructors and destructor -----------------------------------


// --------------------
Value::Value( 
    const int &idx,
    const string &val )
// --------------------
{
    index = idx;
    value = val;
}


// -----------------------------
Value::Value( const Value &val )
// -----------------------------
{
    index = val.index;
    value = val.value;
}


// ------------
Value::~Value()
// ------------
{}
	

// -- set functions -------------------------------------------------


// -----------------------------------
void Value::setIndex( const int &idx )
// -----------------------------------
{
    index = idx;
}


// --------------------------------------
void Value::setValue( const string &val )
// --------------------------------------
{
    value = val;
}
    

// -- get functions -------------------------------------------------


// ------------------
int Value::getIndex()
// ------------------
{
    return index;
}


// ---------------------
string Value::getValue()
// ---------------------
{
    return value;
}


// -- end -----------------------------------------------------------
