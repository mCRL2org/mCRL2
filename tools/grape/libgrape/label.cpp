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
#include "action.h"
#include "dataexpression.h"

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
  wxString result = _T("");
  if ( !get_declarations_text().IsEmpty() )
  {
    result += get_declarations_text() + _T( "." );  
  }
  if ( !get_condition().IsEmpty() )
  {
    result += _T( "[" ) + get_condition() + _T( "]" );
  }
  result += get_actions_text();
  if ( !get_timestamp().IsEmpty() )
  {
    result += _T( "@" ) + get_timestamp();
  }
  if ( !get_variable_updates_text().IsEmpty() )
  {
    result += _T( "/" ) + get_variable_updates_text();
  }
  return result;
}

list_of_action &label::get_actions( void )
{
  return m_actions;
}

void label::set_actions_text( const wxString &p_actions )
{ 
  wxString text = p_actions;
  text.Trim(true); text.Trim(false);
  wxString sub_text; 
  wxString text_out; 
  int brackets_cnt = 0;
  int tmp_index = -1;
  int index = -1;
  
  // loop until we parsed all actions
  m_actions.Clear(); 
  while (!text.IsEmpty())
  {
    index = text.First(_T("|"));
    if (index == -1) index = text.Len();
    
    action action;
      
    // check if there is an open bracket
    tmp_index = text.First(_T("("));
    if (tmp_index == -1 || tmp_index > index)
    { 
      tmp_index = text.Len();   
      action.set_name(text.SubString(0, index-1));
      m_actions.Add(action);
    
      sub_text = text.SubString(0, index-1);  
    } else
    { 
      action.set_name(text.SubString(0, tmp_index-1));
      
      sub_text = text.SubString(tmp_index+1, index-1);  
        
      // check if there is a close bracket on the same level
      brackets_cnt = 0;
      tmp_index = -1;
      int sub_text_length = sub_text.Len();
      while ((tmp_index+1) <= sub_text_length)
      {    
        tmp_index += 1;
        wxString c = sub_text.GetChar(tmp_index);
        if (c == _T("(")) brackets_cnt += 1;
        if (c == _T(")"))
        {
          if (brackets_cnt == 0) break;
          brackets_cnt -= 1;          
        }
      }
      sub_text = sub_text.SubString(0, tmp_index-1);  

      // parse all parameters
      action.set_parameters_text(sub_text);
      
      // add action into the action list
      m_actions.Add(action);
    }         
    text = text.SubString(index+1, text.Len());
  } 
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
    for ( int i = 0; i < count; ++i )
    {
      result += m_variable_updates.Item( i ).get_varupdate();
      if ( i < count - 1 ) result += _T( "," );
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

void label::set_variable_updates_text(wxString p_variable_updates )
{
  wxString text = p_variable_updates; 
  text.Trim(true); text.Trim(false);
  wxString sub_text; 
  wxString text_out; 
  int index = -1;
  
  // loop until we parsed all variable updates
  m_variable_updates.Clear(); 
  while (!text.IsEmpty())
  {
    index = text.First(_T(","));
    if (index == -1) index = text.Len();
    
    varupdate varupdate;
        
    sub_text = text.SubString(0, index-1);  

    // parse all varupdate
    varupdate.set_varupdate(sub_text);
      
    // add varupdate into the varupdate list
    m_variable_updates.Add(varupdate);
          
    text = text.SubString(index+1, text.Len());
  }   
}

void label::set_declarations_text( wxString p_declarations )
{
  wxString text = p_declarations; 
  text.Trim(true); text.Trim(false);
  wxString sub_text; 
  wxString text_out; 
  int index = -1;
  
  // loop until we parsed all declarations
  m_declarations.Clear(); 
  while (!text.IsEmpty())
  {
    index = text.First(_T(","));
    if (index == -1) index = text.Len();
    
    decl decl;
       
    sub_text = text.SubString(0, index-1);  

    // parse all declarations
    decl.set_decl(sub_text);
      
    // add varupdate into the declarations list
    m_declarations.Add(decl);
          
    text = text.SubString(index+1, text.Len());
  } 
}
        
// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_label );
