// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file label.cpp
//
// Implements the label datatype.

#include <wx/tokenzr.h>

#include "label.h"

using namespace grape::libgrape;

label::label( void )
{
}

label::label( const label &p_label )
{
  m_actions = p_label.m_actions;
  m_declarations = p_label.m_declarations;
  m_timestamp = p_label.m_timestamp;
  m_variable_updates = p_label.m_variable_updates;
  m_condition = p_label.m_condition;
}

label::~label( void )
{
  m_variable_updates.Clear();
}

wxString label::get_text( void ) const
{
  wxString result;
  result = get_declarations_text();
  if ( !get_condition().IsEmpty() )
  {
    result += _T( "[" ) + get_condition() + _T( "]" );
  }
  result += get_actions_text();
  if ( !get_timestamp().IsEmpty() )
  {
    result += _T( "@" ) +  get_timestamp();
  }
  result += get_variable_updates_text();

  return result;
}

list_of_action &label::get_actions( void )
{
  return m_actions;
}

wxString label::get_actions_text( void ) const
{
  wxString result = wxEmptyString;
  int count = m_actions.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    action action = m_actions.Item( i );
    result += action.get_text();
    if ( i < count - 1 )
    {
      result += _T( "|" );
    }
  }
  return result;
}

list_of_decl &label::get_declarations( void )
{
  return m_declarations;
}

wxString label::get_declarations_text( void ) const
{
  wxString result = wxEmptyString;
  int count = m_declarations.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    decl declaration = m_declarations.Item( i );
    result += declaration.get_name() + _T( ":" ) + declaration.get_type();
    if ( i < count - 1 )
    {
      result += _T( "," );
    }
  }
  if ( count > 0 )
  {
    result += _T( "." );
  }
  return result;
}

wxString label::get_timestamp( void ) const
{
  return m_timestamp.get_expression();
}

list_of_varupdate &label::get_variable_updates( void )
{
  return m_variable_updates;
}

wxString label::get_variable_updates_text( void ) const
{
  wxString result = wxEmptyString;
  int count = m_variable_updates.GetCount();
  if ( count > 0 )
  {
    result += _T( "/" );
    for ( int i = 0; i < count; ++i )
    {
      varupdate var_upd = m_variable_updates.Item( i );
      result += var_upd.get_lhs() + _T( ":=" ) + var_upd.get_rhs();
      if ( i < count - 1 )
      {
        result += _T( "," );
      }
    }
  }
  return result;
}

wxString label::get_condition( void ) const
{
  return m_condition.get_expression();
}

void label::set_declarations( const list_of_decl &p_declarations )
{
  m_declarations = p_declarations;
}

void label::set_condition( const wxString &p_condition )
{
  m_condition.set_expression( p_condition );
}

void label::set_actions( const list_of_action &p_actions )
{
  m_actions = p_actions;
}

void label::set_timestamp( const wxString &p_timestamp )
{
  m_timestamp.set_expression( p_timestamp );
}

void label::set_variable_updates( const list_of_varupdate &p_variable_updates )
{
  m_variable_updates = p_variable_updates;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_label );
