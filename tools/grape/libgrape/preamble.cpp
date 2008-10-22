// Author(s): VitaminB100
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

decl_init::decl_init(void) : decl()
{
}

decl_init::decl_init(const decl_init &p_decl_init) : decl(p_decl_init)
{
  m_value = p_decl_init.m_value;
}

decl_init::~decl_init(void)
{
}

wxString decl_init::get_value(void) const
{
  return m_value.get_expression();
}

void decl_init::set_value(const wxString &p_value)
{
  m_value.set_expression(p_value);
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_decl_init );

preamble::preamble( void )
{
  m_parameter_declarations = wxEmptyString;
  m_local_variable_declarations = wxEmptyString;
  m_parameter_declarations_list.Empty();
  m_local_variable_declarations_list.Empty();
}

preamble::preamble( const preamble &p_preamble )
{
  m_parameter_declarations = p_preamble.m_parameter_declarations;
  m_local_variable_declarations = p_preamble.m_parameter_declarations;
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
  for ( uint i = 0; i < m_parameter_declarations_list.GetCount(); ++i )
  {
    decl parameter = m_parameter_declarations_list.Item( i );
    result += parameter.get_name() + _T(": ") + parameter.get_type() + _T(";\n");
  }
  return result;
}

void preamble::set_parameter_declarations( const wxString &p_parameter_declarations )
{
  m_parameter_declarations = p_parameter_declarations;
}

bool preamble::check_parameter_declarations_syntax( void )
{
  m_parameter_declarations_list.Empty();

  if( !m_parameter_declarations.IsEmpty() )
  {
    wxStringTokenizer tkp( m_parameter_declarations, _T(","), wxTOKEN_RET_EMPTY_ALL );
    decl p_decl;
    while( tkp.HasMoreTokens() )
    {
      wxString token = tkp.GetNextToken();

      // process parameter declaration
      wxStringTokenizer tkt( token, _T(":") );
      if ( tkt.CountTokens() != 2 || token.IsEmpty() )
      {
        return false;
      }
      wxString param = tkt.GetNextToken();
      param.Trim( true );
      param.Trim( false );
      wxStringTokenizer tks( param );
      if ( tks.CountTokens() != 1 || param.IsEmpty() )
      {
        return false;
      }
      wxString param_type = tkt.GetNextToken();
      param_type.Trim( true );
      param_type.Trim( false );
      tks.SetString( param_type );
      if (tks.CountTokens() != 1 || param_type.IsEmpty() )
      {
        return false;
      }
      p_decl.set_name( param );
      p_decl.set_type( param_type );
      m_parameter_declarations_list.Add( p_decl );
    }
  }
  return true;
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
  for ( uint i = 0; i < m_local_variable_declarations_list.GetCount(); ++i )
  {
    decl_init local_variable = m_local_variable_declarations_list.Item( i );
    result += local_variable.get_name() + _T(": ") + local_variable.get_type() + _T(" = ") + local_variable.get_value() + _T(";\n");
  }
  return result;
}

void preamble::set_local_variable_declarations( const wxString &p_local_variable_declarations )
{
  m_local_variable_declarations = p_local_variable_declarations;
}

bool preamble::check_local_variable_declarations_syntax( void )
{

  m_local_variable_declarations_list.Empty();

  if( !m_local_variable_declarations.IsEmpty() )
  {
    wxStringTokenizer tkp( m_local_variable_declarations, _T(","), wxTOKEN_RET_EMPTY_ALL );
    decl_init p_decl;
    while(tkp.HasMoreTokens())
    {
      wxString token = tkp.GetNextToken();

      // process local variable declaration
      wxStringTokenizer tkt( token, _T(":") );
      if ( tkt.CountTokens() != 2 || token.IsEmpty() )
      {
        return false;
      }
      wxString var = tkt.GetNextToken();
      var.Trim( true );
      var.Trim( false );
      wxStringTokenizer tks( var );
      if ( tks.CountTokens() != 1 || var.IsEmpty() )
      {
        return false;
      }
      wxString var_rest = tkt.GetNextToken();
      wxStringTokenizer tkr( var_rest, _T("=") );
      if ( tkr.CountTokens() != 2 || var_rest.IsEmpty() )
      {
        return false;
      }
      wxString var_type = tkr.GetNextToken();
      var_type.Trim( true );
      var_type.Trim( false );
      tks.SetString( var_type );
      if ( tks.CountTokens() != 1 || var_type.IsEmpty() )
      {
        return false;
      }
      wxString var_val = tkr.GetNextToken();
      var_val.Trim( true );
      var_val.Trim( false );
      tks.SetString( var_val );
      if ( tks.CountTokens() != 1 || var_val.IsEmpty() )
      {
        return false;
      }
      p_decl.set_name( var );
      p_decl.set_type( var_type );
      p_decl.set_value( var_val );
      m_local_variable_declarations_list.Add( p_decl );
    }
  }
  return true;
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

