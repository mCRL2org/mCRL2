// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file coordinate.cpp
//
// Implements the coordinate datatype.

#include "wx.hpp" // precompiled headers

#include <wx/wx.h>
#include "coordinate.h"

using namespace grape::libgrape;

bool coordinate::operator==( const coordinate &p_c )
{
  float tx, ty;
  tx = p_c.m_x - m_x;
  ty = p_c.m_y - m_y;

  return (tx < 0.001 && tx > -0.001) &&
         (ty < 0.001 && ty > -0.001);
}

coordinate & coordinate::operator=( const coordinate &p_c )
{
  m_x = p_c.m_x;
  m_y = p_c.m_y;
  return *this;
}

coordinate coordinate::operator-(const coordinate &p_c)
{
  coordinate result;
  result.m_x = m_x - p_c.m_x;
  result.m_y = m_y - p_c.m_y;
  return result;
}

coordinate coordinate::operator+(const coordinate &p_c)
{
  coordinate result;
  result.m_x = m_x + p_c.m_x;
  result.m_y = m_y + p_c.m_y;
  return result;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_coordinate )
