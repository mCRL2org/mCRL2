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
  m_text = wxEmptyString;
  m_is_valid = false;
}

label::label( const label &p_label )
{
  m_text = p_label.m_text;
  m_actions = p_label.m_actions;
  m_declarations = p_label.m_declarations;
  m_timestamp = p_label.m_timestamp;
  m_variable_updates = p_label.m_variable_updates;
  m_condition = p_label.m_condition;
  m_is_valid = p_label.m_is_valid;
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

bool label::set_text( const wxString &p_text )
{
  m_text = p_text;
  process_text();
  return m_is_valid;
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
    result += action.get_name();
    list_of_dataexpression params = action.get_parameters();
    int count2 = params.GetCount();
    if ( count2 > 0 )
    {
      result += _T( "(" );
      for ( int j = 0; j < count2; ++j )
      {
        result += params.Item( j ).get_expression();
        if ( j < count2 - 1 )
        {
          result += _T( "," );
        }
      }
      result += _T( ")" );
    }
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

void label::process_text( void )
{
  m_is_valid = false;

  wxString text = m_text;
  int pos = 0;

  if(text.Find(_T(".")) != wxNOT_FOUND)
  {
    // remove variable declarations
    m_declarations.Empty();
    pos = text.Find(_T("."));
    wxString local_decls = text.Mid(0, pos);
    text = text.Mid(pos+1);

    // check
    wxStringTokenizer tkv(local_decls, _T(","));
    while(tkv.HasMoreTokens())
    {
      wxString decl_text = tkv.GetNextToken();
      wxStringTokenizer tkvd( decl_text, _T(":") );
      if ( tkvd.CountTokens() != 2 || decl_text.IsEmpty() )
      {
        // not valid
        return;
      }
      wxString local_decl_name = tkvd.GetNextToken();
      local_decl_name.Trim( true );
      local_decl_name.Trim( false );
      wxStringTokenizer tks( local_decl_name );
      if ( tks.CountTokens() != 1 || local_decl_name.IsEmpty() )
      {
        // not valid
        return;
      }
      wxString local_decl_type = tkvd.GetNextToken();
      local_decl_type.Trim( true );
      local_decl_type.Trim( false );
      tks.SetString( local_decl_type );
      if ( tks.CountTokens() != 1 || local_decl_type.IsEmpty() )
      {
        // not valid
        return;
      }
      // save declaration
      decl local_decl;
      local_decl.set_name( local_decl_name );
      local_decl.set_type( local_decl_type );
      m_declarations.Add( local_decl );
    }
  }
  if(text.Find(_T("[")) != wxNOT_FOUND)
  {
    // remove condition
    wxString condition_text = text.Mid(text.Find(_T("["))+1);
    pos = condition_text.Find(_T("]"));
    if(pos == wxNOT_FOUND)
    {
      // ERROR: invalid syntax
      return;
    }
    m_condition.set_expression(condition_text.Mid(0, pos));
    wxString tcondition = condition_text.Mid(0, pos);
    text = text.Mid(text.Find(_T("]"))+1);

    // check
    tcondition.Trim(true); tcondition.Trim(false);
    if(tcondition.IsEmpty())
    {
      // not valid
      return;
    }
  }
  if(text.Find(_T("@")) != wxNOT_FOUND)
  {
    // remove timestamp
    wxString timestamp_text = text.Mid(text.Find(_T("@"))+1);
    pos = timestamp_text.Find(_T("/"));
    if(pos == wxNOT_FOUND)
    {
      // niets aan de hand
    }
    m_timestamp.set_expression(timestamp_text.Mid(0, pos));
    wxString timest = timestamp_text.Mid(0, pos);
    text = text.Mid(0,text.Find(_T("@")))+text.Mid(text.Find(_T("/")));

    // check
    timest.Trim(true); timest.Trim(false);
    if(timest.IsEmpty())
    {
      // not valid
      return;
    }
  }
  if(text.Find(_T("/")) != wxNOT_FOUND)
  {
    // remove variable updates
    pos = text.Find(_T("/"));
    wxString var_updates = text.Mid(pos+1);
    text = text.Mid(0, pos);
    m_variable_updates.Empty();
    while(var_updates.Len() > 0)
    {
      varupdate var_update;
      wxString curr_update = var_updates;
      if(curr_update.Find(_T(",")) != wxNOT_FOUND)
      {
        int upd_pos = curr_update.Find(_T(","));
        wxString var_upd = curr_update.Mid(0, upd_pos);
        var_upd.Trim(true); var_upd.Trim(false);
        if(var_upd.IsEmpty())
        {
          // not valid
          return;
        }
        var_update.set_varupdate(var_upd);
        wxString upd_var = var_update.get_lhs();
        upd_var.Trim(true); upd_var.Trim(false);
        if(upd_var.IsEmpty())
        {
          // not valid
          return;
        }
        wxString upd_val = var_update.get_rhs();
        upd_val.Trim(true); upd_val.Trim(false);
        if(upd_val.IsEmpty())
        {
          // not valid
          return;
        }
        m_variable_updates.Add(var_update);
        var_updates = var_updates.Mid(upd_pos+1);
      }
      else
      {
        curr_update.Trim(true); curr_update.Trim(false);
        if(curr_update.IsEmpty())
        {
          // not valid
          return;
        }
        var_update.set_varupdate(curr_update);
        wxString upd_var = var_update.get_lhs();
        upd_var.Trim(true); upd_var.Trim(false);
        if(upd_var.IsEmpty())
        {
          // not valid
          return;
        }
        wxString upd_val = var_update.get_rhs();
        upd_val.Trim(true); upd_val.Trim(false);
        if(upd_val.IsEmpty())
        {
          // not valid
          return;
        }
        m_variable_updates.Add(var_update);
        var_updates = wxEmptyString;
      }
    }
  }

  // save multiaction
  m_actions.Empty();
  wxString multiaction = text;

  // check
  wxStringTokenizer tkma(multiaction, _T("|"));
  while(tkma.HasMoreTokens())
  {
    wxString action_text = tkma.GetNextToken();
    action action;
    int pos1 = action_text.Find( _T("(") );
    int pos2 = action_text.Find( ')', true );
    if( ( pos1 != wxNOT_FOUND ) && ( pos2 != wxNOT_FOUND ) )
    {
      // remove action
      wxString action_name = action_text.Mid(0, pos1);
      action_name.Trim(true);
      action_name.Trim(false);
      action.set_name( action_name );
      wxString action_params_text = action_text.Mid(pos1+1, pos2-pos1-1);
      wxStringTokenizer tka(action_params_text, _T(","));
      if (tka.HasMoreTokens())
      {
        list_of_dataexpression action_params;
        while(tka.HasMoreTokens())
        {
          wxString action_param_text = tka.GetNextToken();
          if(action_param_text.IsEmpty())
          {
            // not valid
            return;
          }
          dataexpression action_param;
          action_param.set_expression(action_param_text);
          action_params.Add(action_param);
        }
        action.set_parameters( action_params );
      }
    }
    else
    {
      action.set_name(action_text);
    }
    m_actions.Add(action);
  }

  m_is_valid = true;
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

bool label::has_valid_text() const
{
  return m_is_valid;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_label );
