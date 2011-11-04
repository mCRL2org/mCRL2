// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file label.cpp
//
// Implements the label datatype.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include <wx/tokenzr.h>

#include <sstream>
#include "mcrl2/core/parse.h"
#include "mcrl2/lps/action_parse.h"
#include "label.h"
#include "action.h"
#include "dataexpression.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace grape::libgrape;
using namespace std;

label::label(void)
{
}

label::label(const label& p_label)
{
  m_actions = p_label.m_actions;
  m_declarations = p_label.m_declarations;
  m_timestamp = p_label.m_timestamp;
  m_variable_updates = p_label.m_variable_updates;
  m_condition = p_label.m_condition;
}

label::~label(void)
{
  m_variable_updates.Clear();
}

wxString label::get_text(void) const
{
  wxString result = _T("");
  if (!get_declarations_text().IsEmpty())
  {
    result += get_declarations_text() + _T(".");
  }
  if (!get_condition().IsEmpty())
  {
    result += _T("[") + get_condition() + _T("]");
  }
  result += get_actions_text();
  if (!get_timestamp().IsEmpty())
  {
    result += _T("@") + get_timestamp();
  }
  if (!get_variable_updates_text().IsEmpty())
  {
    result += _T("/") + get_variable_updates_text();
  }
  return result;
}

list_of_action& label::get_actions(void)
{
  return m_actions;
}

void label::set_actions_text(const wxString& p_actions)
{
  m_actions.Clear();

  std::string s(p_actions.mb_str());
  atermpp::aterm_appl a_parsed_multi_action;
  try
  {
    a_parsed_multi_action = mcrl2::lps::detail::multi_action_to_aterm(mcrl2::lps::parse_multi_action_new(s));
  }
  catch (...)
  {
    // skip
  }

  if (a_parsed_multi_action)
  {
    // get list of ParamId's from MultAct
    atermpp::aterm_list al(a_parsed_multi_action.argument(0));
    // loop through list of ParamId's
    for (atermpp::aterm_list::const_iterator i = al.begin(); i != al.end(); ++i)
    {
      action action;
      action.set_name(wxString(mcrl2::core::pp(atermpp::aterm_appl(*i).argument(0)).c_str(), wxConvLocal));
      action.set_parameters_text(atermpp::aterm_appl(*i).argument(1));
      m_actions.Add(action);
    }
  }
}

wxString label::get_actions_text(void) const
{
  wxString result = wxEmptyString;
  size_t count = m_actions.GetCount();
  for (size_t i = 0; i < count; ++i)
  {
    action action = m_actions.Item(i);
    result += action.get_text();
    if (i < count - 1)
    {
      result += _T("|");
    }
  }
  return result;
}

list_of_decl& label::get_declarations(void)
{
  return m_declarations;
}

wxString label::get_declarations_text(void) const
{
  wxString result = wxEmptyString;
  size_t count = m_declarations.GetCount();
  for (size_t i = 0; i < count; ++i)
  {
    decl declaration = m_declarations.Item(i);
    result += declaration.get_name() + _T(":") + declaration.get_type();
    if (i < count - 1)
    {
      result += _T(",");
    }
  }
  return result;
}

wxString label::get_timestamp(void) const
{
  return m_timestamp.get_expression();
}

list_of_varupdate& label::get_variable_updates(void)
{
  return m_variable_updates;
}

wxString label::get_variable_updates_text(void) const
{
  wxString result = wxEmptyString;
  size_t count = m_variable_updates.GetCount();
  if (count > 0)
  {
    for (size_t i = 0; i < count; ++i)
    {
      result += m_variable_updates.Item(i).get_varupdate();
      if (i < count - 1)
      {
        result += _T(",");
      }
    }
  }
  return result;
}

wxString label::get_condition(void) const
{
  return m_condition.get_expression();
}

void label::set_declarations(const list_of_decl& p_declarations)
{
  m_declarations = p_declarations;
}

void label::set_condition(const wxString& p_condition)
{
  m_condition.set_expression(p_condition);
}

void label::set_actions(const list_of_action& p_actions)
{
  m_actions = p_actions;
}

void label::set_timestamp(const wxString& p_timestamp)
{
  m_timestamp.set_expression(p_timestamp);
}

void label::set_variable_updates(const list_of_varupdate& p_variable_updates)
{
  m_variable_updates = p_variable_updates;
}

void label::set_variable_updates_text(wxString p_variable_updates)
{
  wxString text = p_variable_updates;
  text.Trim(true);
  text.Trim(false);
  wxString lhs, rhs, rlhs, delimiter;
  ssize_t index = -1, index2 = -1;
  ssize_t loop = 0;
  // loop until we parsed all variable updates
  m_variable_updates.Clear();
  while (!text.IsEmpty())
  {
    index = text.Find(_T(":="));
    if (index == wxNOT_FOUND && loop == 0)
    {
      return;
    }
    if (index == wxNOT_FOUND && loop != 0)
    {
      index = text.Len();
    }
    if (loop == 0)
    {
      lhs = text.Mid(0, index);
    }
    else
    {
      rlhs = text.Mid(0, index);
      delimiter = _T(",");
      index2 = rlhs.Find(delimiter.GetChar(0), true);
      if (index2 == wxNOT_FOUND || text.Find(_T(":=")) == wxNOT_FOUND)
      {
        index2 = rlhs.Len();
      }
      rhs = rlhs.Mid(0, index2);
      lhs.Trim(true);
      lhs.Trim(false);
      rhs.Trim(true);
      rhs.Trim(false);
      if (!lhs.IsEmpty() && !rhs.IsEmpty())
      {
        varupdate varupdate;
        varupdate.set_lhs(lhs);
        varupdate.set_rhs(rhs);
        m_variable_updates.Add(varupdate);
      }
      lhs = rlhs.Mid(index2 + 1);
    }
    text = text.Mid(index + 2);
    ++loop;
  }
}

void label::set_declarations_text(wxString p_declarations)
{
  wxString text = p_declarations;
  text.Trim(true);
  text.Trim(false);
  wxString sub_text;
  ssize_t index = -1;

  // loop until we parsed all declarations
  m_declarations.Clear();
  while (!text.IsEmpty())
  {
    index = text.First(_T(","));
    if (index == -1)
    {
      index = text.Len();
    }

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
WX_DEFINE_OBJARRAY(list_of_label)
