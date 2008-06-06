// Author(s): VitaminB100
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
  m_conditions = p_label.m_conditions;
  m_is_valid = p_label.m_is_valid;
}

label::~label( void )
{
  m_variable_updates.Clear();
}

wxString label::get_text( void )
{
  return m_text;
}

bool label::set_text( const wxString &p_text )
{
  m_text = p_text;
  process_text();
  return m_is_valid;
}

dataexpression &label::get_actions( void )
{
  return m_actions;
}

dataexpression &label::get_declarations( void )
{
  return m_declarations;
}

dataexpression &label::get_timestamp( void )
{
  return m_timestamp;
}

list_of_varupdate &label::get_variable_updates( void )
{
  return m_variable_updates;
}

dataexpression &label::get_conditions( void )
{
  return m_conditions;
}

void label::process_text( void )
{
  m_is_valid = false;

  wxString text = m_text;
  int pos = 0;

  if(text.Find(_T(".")) != wxNOT_FOUND)
  {
    // remove variable declarations
    pos = text.Find(_T("."));
    m_declarations.set_expression(text.Mid(0, pos));
    wxString local_decls = m_declarations.get_expression();
    text = text.Mid(pos+1);

    // check
    wxStringTokenizer tkv(local_decls, _T(","));
    while(tkv.HasMoreTokens())
    {
      wxString ldecl = tkv.GetNextToken();
      wxStringTokenizer tkvd(ldecl, _T(":"));
      if(tkvd.CountTokens() != 2 || ldecl.IsEmpty())
      {
        // not valid
        return;
      }
      wxString decl = tkvd.GetNextToken();
      decl.Trim(true); decl.Trim(false);
      wxString decl_type = tkvd.GetNextToken();
      decl_type.Trim(true); decl_type.Trim(false);
      if(decl.IsEmpty() || decl_type.IsEmpty())
      {
        // not valid
        return;
      }
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
    m_conditions.set_expression(condition_text.Mid(0, pos));
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
  if(text.Find(_T("/")) != wxNOT_FOUND)
  {
    // remove variable updates
    pos = text.Find(_T("/"));
    wxString var_updates = text.Mid(pos+1);
    text = text.Mid(0, pos);
    m_variable_updates.Empty();
    while(var_updates.Len() > 0)
    {
      varupdate::varupdate var_update;
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
        wxString upd_var = var_update.get_lhs()->get_var();
        upd_var.Trim(true); upd_var.Trim(false);
        if(upd_var.IsEmpty())
        {
          // not valid
          return;
        }
        wxString upd_val = var_update.get_rhs()->get_expression();
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
        wxString upd_var = var_update.get_lhs()->get_var();
        upd_var.Trim(true); upd_var.Trim(false);
        if(upd_var.IsEmpty())
        {
          // not valid
          return;
        }
        wxString upd_val = var_update.get_rhs()->get_expression();
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
  if(text.Find(_T("@")) != wxNOT_FOUND)
  {
    // remove timestamp
    pos = text.Find(_T("@"));
    wxString timest = text.Mid(pos+1);
    timest.Trim(true); timest.Trim(false);
    if(timest.IsEmpty())
    {
      // not valid
      return;
    }
    wxStringTokenizer tkt(timest, _T(" "));
    if(tkt.CountTokens() > 1)
    {
      // not valid
      return;
    }
    m_timestamp.set_expression(timest);
    text = text.Mid(0, pos);
  }

  // save multiaction
  m_actions.set_expression(text);

  m_is_valid = true;
}

bool label::has_valid_text() const
{
  return m_is_valid;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_label );
