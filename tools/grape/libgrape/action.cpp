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
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/struct.h"        // ATerm building blocks.

using namespace mcrl2::core;
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

wxString action::get_text( void )
{
  wxString result = wxEmptyString;
  result += m_name;
  int count = m_parameters.GetCount();
  if ( count > 0 )
  {
    result += _T( "(" );
    for ( int j = 0; j < count; ++j )
    {
      result += m_parameters.Item( j ).get_expression();
      if ( j < count - 1 )
      {
        result += _T( "," );
      }
    }
    result += _T( ")" );
  }
  return result;
}

wxString action::get_name( void )
{
  return m_name;
}

void action::set_name( const wxString &p_name )
{
  m_name = p_name;
  m_name.Trim(true); m_name.Trim(false);
}

list_of_dataexpression action::get_parameters( void )
{
  return m_parameters;
}

void action::set_parameters( list_of_dataexpression p_parameters)
{
  m_parameters = p_parameters;
}

void action::set_parameters_text( ATermList p_parameters )
{   
  m_parameters.Clear();

  for (ATermList a_args = p_parameters; !ATisEmpty(a_args); a_args = ATgetNext(a_args))
  {
    dataexpression dataexpression;
    ATermAppl p = ATAgetFirst(a_args);
    std::string a_param = PrintPart_CXX(ATerm(p));
    dataexpression.set_expression(wxString(a_param.c_str(), wxConvLocal));
    m_parameters.Add(dataexpression);
  }
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_action );
