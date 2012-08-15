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


DOF::DOF(int index, QString label)
{
  m_index     = index;
  m_label     = label;
  m_values    << 0.0 << 0.0;
  m_direction = 1;
  m_attribute = 0;
}


DOF::DOF(const DOF& dof)
// Copy constructor.
{
  m_index     = dof.m_index;
  m_label     = dof.m_label;
  m_values    = dof.m_values;
  m_direction = dof.m_direction;
  m_attribute = dof.m_attribute;
}


// -- set functions -------------------------------------------------

void DOF::setValue(int index, double value)
{
  if (0 <= index && index < m_values.size())
  {
    m_values[index] = value;
  }
}

void DOF::removeValue(int index)
{
  if (m_values.size() > 2 && 0 <= index && index < m_values.size())
  {
    m_values.removeAt(index);
  }
}

// -- get functions -------------------------------------------------

double DOF::value(int index)
{
  if (0 <= index && index < m_values.size())
  {
    return m_values[index];
  }
  return 0;
}

// -- end -----------------------------------------------------------
