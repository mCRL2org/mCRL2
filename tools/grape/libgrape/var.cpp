// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file var.cpp
//
// Implements the var datatype.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include "var.h"

using namespace grape::libgrape;

var::var( void )
{
  m_var = wxEmptyString;
  m_name = wxEmptyString;
  m_type = wxEmptyString;
}

var::var( const var &p_var )
{
  m_var = p_var.m_var;
  m_name = p_var.m_name;
  m_type = p_var.m_type;
}

var::~var( void )
{
}

void var::set_var( const wxString &p_var )
{
  m_var = p_var;
}

wxString var::get_var( void ) const
{
  return m_var;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_var )

