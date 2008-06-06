// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file decl.cpp
//
// Implements the decl datatype.

#include "decl.h"

using namespace grape::libgrape;

decl::decl( void )
{
  m_name = wxEmptyString;
  m_type = wxEmptyString;
}

decl::decl( const decl &p_decl )
{
  m_name = p_decl.m_name;
  m_type = p_decl.m_type;
}

decl::~decl( void )
{
}

wxString decl::get_name( void ) const
{
  return m_name;
}

void decl::set_name( const wxString &p_name )
{
  m_name = p_name;
}

wxString decl::get_type( void ) const
{
  return m_type;
}

void decl::set_type( const wxString &p_type )
{
  m_type = p_type;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_decl );
