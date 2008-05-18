// Author(s): A.j. (Hannes) pretorius
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./value.cpp


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
