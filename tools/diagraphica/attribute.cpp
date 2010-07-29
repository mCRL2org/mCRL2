// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./attribute.cpp

#include "wx.hpp" // precompiled headers

#include "attribute.h"

// -- constructors and destructor -----------------------------------

using namespace std;

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
    const vector< int > & /*indices*/,
    const string & /*newValue*/ )
// ------------------------------
{}


// -----------------------
void Attribute::moveValue(
    const size_t &/*idxFr*/,
    const size_t &/*idxTo*/ )
// ------------------------
{}


// ------------------------------------
void Attribute::configValues(
    const vector< string > &/*curDomain*/,
    map< int, int  > &/*origToCurDomain*/ )
// ------------------------------------
{}


// --------------------------------------------------------
void Attribute::classifyEqualIntervals( const int &/*number*/ )
// --------------------------------------------------------
{}


// ---------------------------------------------------
void Attribute::classifyQuantiles( const int &/*number*/ )
// ---------------------------------------------------
{}


// ---------------------------------------------------------------
void Attribute::classifyMeanStandardDeviation( const int &/*number*/ )
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
Value* Attribute::getOrigValue( int /*idx*/ )
// --------------------------------------
{
    return NULL;
}


// -------------------------------------
Value* Attribute::getCurValue( int /*idx*/ )
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
