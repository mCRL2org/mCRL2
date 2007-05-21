// --- attribute.cpp ------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "attribute.h"



// -- constructors and destructor -----------------------------------


// ------------------
Attribute::Attribute(
    Mediator* m,
    const string &nam,
    const string &typ,
    const int &idx )
    : Colleague( m )
// ------------------
{
    name  = nam;
    type  = typ;
    index = idx;
}


// ------------------------------------------
Attribute::Attribute( const Attribute &attr )
    :Colleague( attr )
// ------------------------------------------
{
    index = attr.index;
    name  = attr.name;
    type  = attr.type;
}


// --------------------
Attribute::~Attribute()
// --------------------
{}


// -- set functions -------------------------------------------------


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


// -----------------------------------------
void Attribute::setType( const string &typ )
// -----------------------------------------
{
    type = typ;
}


// ------------------------------
void Attribute::clusterValues(
    const vector< int > &indices,
    const string &newValue )
// ------------------------------
{}


// -----------------------
void Attribute::moveValue(
    const int &idxFr,
    const int &idxTo )
// ------------------------
{}


// ------------------------------------
void Attribute::configValues(
    const vector< string > &curDomain,
    map< int, int  > &origToCurDomain )
// ------------------------------------
{}


// --------------------------------------------------------
void Attribute::classifyEqualIntervals( const int &number )
// --------------------------------------------------------
{}


// ---------------------------------------------------
void Attribute::classifyQuantiles( const int &number )
// ---------------------------------------------------
{}


// ---------------------------------------------------------------
void Attribute::classifyMeanStandardDeviation( const int &number )
// ---------------------------------------------------------------
{}


// -----------------------------------
void Attribute::removeClassification()
// -----------------------------------
{}


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
    return 0;
}


// --------------------------------------
Value* Attribute::getOrigValue( int idx )
// --------------------------------------
{
    return NULL;
}


// -------------------------------------
Value* Attribute::getCurValue( int idx )
// -------------------------------------
{
    return NULL;
}


// ------------------------------
double Attribute::getLowerBound()
// ------------------------------
{
    return 0.0;
}


// ------------------------------
double Attribute::getUpperBound()
// ------------------------------
{
    return 0.0;
}


// -------------------------------
void Attribute::getRangeOrigValues(
    double &lwrBnd,
    double &uprBnd )
// ------------------------------
{
    lwrBnd = uprBnd = 0.0;
}


// -- end -----------------------------------------------------------
