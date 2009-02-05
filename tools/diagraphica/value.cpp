// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./value.cpp

#include "wx.hpp" // precompiled headers

#include "value.h"

using namespace std;

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
