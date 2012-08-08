// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./dof.cpp

#include "dof.h"

using namespace std;

// -- constructors and destructor -----------------------------------


DOF::DOF(
  const int& idx,
  const string& lbl)
{
  index = idx;
  label = lbl;
  /*
  min   = 0.0;
  max   = 0.0;
  */
  values.push_back(0.0);   // init min
  values.push_back(0.0);   // init max
  dir   = 1;
  attr  = 0;
  textStatus = ID_TEXT_NONE;
}


DOF::DOF(const DOF& dof)
// Copy constructor.
{
  index = dof.index;    // index in attribute
  label = dof.label;
  /*
  min   = dof.min;
  max   = dof.max;
  */
  values = dof.values;
  dir   = dof.dir;
  attr  = dof.attr;
  textStatus = dof.textStatus;
}


DOF::~DOF()
{
  // association
  attr = 0;
}


// -- set functions -------------------------------------------------


void DOF::setIndex(const int& idx)
{
  index = idx;
}


void DOF::setLabel(const string& lbl)
{
  label = lbl;
}


void DOF::setMin(const double& m)
{
  //min = m;
  values[0] = m;
}


void DOF::setMax(const double& m)
{
  //max = m;
  values[values.size()-1] = m;
}


void DOF::setMinMax(const double& mn, const double& mx)
{
  /*
  min = mn;
  max = mx;
  */
  values[0] = mn;
  values[values.size()-1] = mx;
}


void DOF::setValue(
  const size_t& idx,
  const double& val)
{
  if (idx != NON_EXISTING && idx < values.size())
  {
    values[idx] = val;
  }
}


void DOF::addValue(const double& val)
{
  values.push_back(val);
}


void DOF::clearValue(const size_t& idx)
{
  if (values.size() > 2 &&
      (idx != NON_EXISTING && static_cast <size_t>(idx) < values.size()))
  {
    values.erase(values.begin() + idx);
  }
}


void DOF::setDir(const int& dr)
{
  dir = dr;
}


void DOF::setAttribute(Attribute* a)
{
  attr = a;
}


void DOF::setTextStatus(const int& status)
{
  if (status == ID_TEXT_NONE ||
      status == ID_TEXT_ALL  ||
      status == ID_TEXT_ATTR ||
      status == ID_TEXT_VAL)
  {
    textStatus =  status;
  }
  else
  {
    textStatus = ID_TEXT_NONE;
  }
}


// -- get functions -------------------------------------------------


size_t DOF::getIndex()
{
  return index;
}


string DOF::getLabel()
{
  return label;
}


double DOF::getMin()
{
  //return min;
  return values[0];
}


double DOF::getMax()
{
  //return max;
  return values[values.size()-1];
}


size_t DOF::getSizeValues()
{
  return values.size();
}


double DOF::getValue(const size_t& idx)
{
  double result = -1;
  if (idx != NON_EXISTING && static_cast <size_t>(idx) < values.size())
  {
    result = values[idx];
  }
  return result;
}


void DOF::getValues(vector< double > &vals)
{
  vals = values;
}


int DOF::getDir()
{
  return dir;
}


Attribute* DOF::getAttribute()
{
  return attr;
}


int DOF::getTextStatus()
{
  return textStatus;
}


// -- end -----------------------------------------------------------
