// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file action.cpp
//
// Implements the action datatype.

#include "action.h"

using namespace grape::libgrape;

action::action( void )
{
  m_name = wxEmptyString;
  m_parameters.Empty();
}

action::action( const action &p_action )
{
  m_name = p_action.m_name;
  m_parameters = p_action.m_parameters;
}

action::~action(void)
{
  m_parameters.Clear();
}

wxString action::get_name( void )
{
  return m_name;
}

void action::set_name( const wxString &p_name )
{
  m_name = p_name;
}

list_of_dataexpression action::get_parameters( void )
{
  return m_parameters;
}

void action::set_parameters( list_of_dataexpression &p_parameters)
{
  m_parameters = p_parameters;
}


// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_action );
