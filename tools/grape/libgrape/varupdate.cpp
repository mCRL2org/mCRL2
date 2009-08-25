// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file varupdate.cpp
//
// Implements the varupdate datatype.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include <wx/tokenzr.h>

#include <sstream>
#include "mcrl2/core/parse.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/identifier_string.h"

#include "varupdate.h"

using namespace mcrl2::core;
using namespace grape::libgrape;
using namespace std;

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

  istringstream r(string(variable_update_lhs.mb_str()).c_str());
  ATermAppl a_parsed_identifier = mcrl2::core::parse_identifier(r);
  if ( a_parsed_identifier )
  {
    string a_name = identifier_string(a_parsed_identifier);
    set_lhs( wxString(a_name.c_str(), wxConvLocal) );

    istringstream s(string(variable_update_rhs.mb_str()).c_str());
    ATermAppl a_parsed_data_expr = mcrl2::core::parse_data_expr(s);
    if ( a_parsed_data_expr )
    {
      string a_value = PrintPart_CXX(ATerm(a_parsed_data_expr));
      set_rhs( wxString(a_value.c_str(), wxConvLocal) );
      return true;
    }
  }
  return false;
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
WX_DEFINE_OBJARRAY( list_of_varupdate )

