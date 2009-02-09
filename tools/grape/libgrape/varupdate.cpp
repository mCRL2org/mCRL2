// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file varupdate.cpp
//
// Implements the varupdate datatype.

#include <wx/tokenzr.h>

#include "varupdate.h"

using namespace grape::libgrape;

varupdate::varupdate( void )
{
}

varupdate::varupdate( const varupdate &p_varupdate )
{
  m_dataexpression = p_varupdate.m_dataexpression;
  m_var = p_varupdate.m_var;
}

varupdate::~varupdate( void )
{
}

wxString varupdate::get_varupdate( void ) const
{
  return get_lhs()+_T(":=")+get_rhs();
}

bool varupdate::set_varupdate( const wxString &p_varupdate )
{
  int pos = p_varupdate.Find( _T( ":=" ) );
  wxString variable_update_lhs = p_varupdate.Mid( 0, pos );
  wxString variable_update_rhs = p_varupdate.Mid( pos+2 );
  variable_update_lhs.Trim(true); variable_update_lhs.Trim(false);
  variable_update_rhs.Trim(true); variable_update_rhs.Trim(false);
  if ( variable_update_lhs.IsEmpty() || variable_update_rhs.IsEmpty() )
  {
    return false;
  }
  set_lhs( variable_update_lhs );
  set_rhs( variable_update_rhs );
  return true;
}

wxString varupdate::get_lhs( void ) const
{
  return m_var.get_var();
}

void varupdate::set_lhs( const wxString& p_var )
{
  m_var.set_var( p_var );
}

wxString varupdate::get_rhs( void ) const
{
  return m_dataexpression.get_expression();
}

void varupdate::set_rhs( const wxString& p_update )
{
  m_dataexpression.set_expression( p_update );
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_varupdate );

