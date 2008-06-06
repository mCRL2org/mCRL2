// Author(s): VitaminB100
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
  m_varupdate = wxEmptyString;
  m_dataexpression = 0;
  m_var = 0;
}

varupdate::varupdate( const varupdate &p_varupdate )
{
  m_varupdate = p_varupdate.m_varupdate;
  m_dataexpression = p_varupdate.m_dataexpression;
  m_var = p_varupdate.m_var;
}

varupdate::~varupdate( void )
{
}

wxString varupdate::get_varupdate( void ) const
{
  return m_varupdate;
}

void varupdate::set_varupdate( const wxString &p_varupdate )
{
  m_varupdate = p_varupdate;
}

var * varupdate::get_lhs( void )
{
  m_var = new var;

  if(m_varupdate.Find(_T(":")) != wxNOT_FOUND)
  {
    int pos = m_varupdate.Find(_T(":"));
    wxString variable = m_varupdate.Mid(0, pos);
    variable.Trim(true);
    variable.Trim(false);
    m_var->set_var(variable);
  }

  return m_var;
}

dataexpression * varupdate::get_rhs( void )
{
  m_dataexpression = new dataexpression;

  if(m_varupdate.Find(_T("=")) != wxNOT_FOUND)
  {
    int pos = m_varupdate.Find(_T("="));
    wxString upd = m_varupdate.Mid(pos+1);
    upd.Trim(true);
    upd.Trim(false);
    m_dataexpression->set_expression(upd);
  }

  return m_dataexpression;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_varupdate );

