// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./attribute.cpp

#include "attribute.h"

// -- constructors and destructor -----------------------------------

using namespace std;

Attribute::Attribute(
  QString name,
  QString type,
  const size_t& idx)
{
  m_name  = name;
  m_type  = type;
  index = idx;
}


Attribute::Attribute(const Attribute& attr)
{
  index = attr.index;
  m_name  = attr.m_name;
  m_type  = attr.m_type;
}


Attribute::~Attribute()
{}


// -- set functions -------------------------------------------------


void Attribute::setIndex(const size_t& idx)
{
  index = idx;
}


void Attribute::setName(QString name)
{
  m_name = name;
  emit renamed();
}


void Attribute::setType(QString type)
{
  m_type = type;
}


void Attribute::clusterValues(
  const vector< int > & /*indices*/,
  const string& /*newValue*/)
{}


void Attribute::moveValue(
  const size_t& /*idxFr*/,
  const size_t& /*idxTo*/)
{}


void Attribute::configValues(
  const vector< string > &/*curDomain*/,
  map< size_t, size_t  > &/*origToCurDomain*/)
{}


// -- get functions -------------------------------------------------


size_t Attribute::getIndex()
{
  return index;
}


QString Attribute::name()
{
  return m_name;
}


QString Attribute::type()
{
  return m_type;
}


size_t Attribute::getSizeOrigValues()
{
  return 0;
}


Value* Attribute::getOrigValue(size_t /*idx*/)
{
  return 0;
}


Value* Attribute::getCurValue(size_t /*idx*/)
{
  return 0;
}


// -- end -----------------------------------------------------------
