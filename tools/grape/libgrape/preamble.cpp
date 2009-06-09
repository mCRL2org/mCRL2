// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file preamble.cpp
//
// Implements the preamble datatype.

#include "preamble.h"
#include <wx/tokenzr.h>

using namespace grape::libgrape;

preamble::preamble( void )
{
  m_parameter_declarations_list.Empty();
  m_local_variable_declarations_list.Empty();
  m_width = 0;
  m_height = 0;
}

preamble::preamble( const preamble &p_preamble )
{
  m_parameter_declarations_list = p_preamble.m_parameter_declarations_list;
  m_local_variable_declarations_list = p_preamble.m_local_variable_declarations_list;
  m_width = p_preamble.m_width;
  m_height = p_preamble.m_height;
}

preamble::~preamble( void )
{
  m_parameter_declarations_list.Clear();
  m_local_variable_declarations_list.Clear();
}

wxString preamble::get_parameter_declarations( void ) const
{
  wxString result = wxEmptyString;
  for ( unsigned int i = 0; i < m_parameter_declarations_list.GetCount(); ++i )
  {
    decl parameter = m_parameter_declarations_list.Item( i );
    result += parameter.get_name() + _T(":") + parameter.get_type() + _T(";\n");
  }
  return result;
}

bool preamble::set_parameter_declarations( const wxString &p_parameter_declarations )
{
  bool valid = true;
  m_parameter_declarations_list.Empty();
  wxStringTokenizer tkw( p_parameter_declarations, _T(";") );
  while ( tkw.HasMoreTokens() )
  {
    decl param_decl;
    wxString token = tkw.GetNextToken();
    valid &= param_decl.set_decl( token );
    if ( valid )
    {
      m_parameter_declarations_list.Add( param_decl );
    }
  }
  return valid;
}

list_of_decl preamble::get_parameter_declarations_list( void ) const
{
  return m_parameter_declarations_list;
}

void preamble::set_parameter_declarations_list( const list_of_decl &p_parameter_declarations )
{
  m_parameter_declarations_list = p_parameter_declarations;
}

wxString preamble::get_local_variable_declarations( void ) const
{
  wxString result = wxEmptyString;
  for ( unsigned int i = 0; i < m_local_variable_declarations_list.GetCount(); ++i )
  {
    decl_init local_variable = m_local_variable_declarations_list.Item( i );
    result += local_variable.get_name() + _T(":") + local_variable.get_type() + _T("=") + local_variable.get_value() + _T(";\n");
  }
  return result;
}

bool preamble::set_local_variable_declarations( const wxString &p_local_variable_declarations )
{
  bool valid = true;
  m_local_variable_declarations_list.Empty();
  wxStringTokenizer tkw( p_local_variable_declarations, _T(";") );
  while ( tkw.HasMoreTokens() )
  {
    decl_init var_decl_init;
    wxString token = tkw.GetNextToken();
    valid &= var_decl_init.set_decl_init( token );
    if ( valid )
    {
      m_local_variable_declarations_list.Add( var_decl_init );
    }
  }
  return valid;
}

list_of_decl_init preamble::get_local_variable_declarations_list( void ) const
{
  return m_local_variable_declarations_list;
}

void preamble::set_local_variable_declarations_list( const list_of_decl_init &p_local_variable_declarations )
{
  m_local_variable_declarations_list = p_local_variable_declarations;
}

float preamble::get_width( void ) const
{
  return m_width;
}

void preamble::set_width( float p_width )
{
  m_width = p_width;
}

float preamble::get_height( void ) const
{
  return m_height;
}

void preamble::set_height( float p_height )
{
  m_height = p_height;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_preamble );
