// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file datatypespecification.cpp
//
// Implements the datatype_specification datatype.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include "datatypespecification.h"

using namespace grape::libgrape;

datatype_specification::datatype_specification( void )
{
  m_dataexpressions = _T( "" );
  m_dataexpression_list.Empty();
}

datatype_specification::datatype_specification( const datatype_specification &p_datatype_specification )
{
  m_dataexpressions = p_datatype_specification.m_dataexpressions;
  m_dataexpression_list = p_datatype_specification.m_dataexpression_list;
}

datatype_specification::~datatype_specification( void )
{
  m_dataexpression_list.Clear();
}

wxString datatype_specification::get_declarations ( void ) const
{
  return m_dataexpressions;  
}

void datatype_specification::set_declarations( const wxString &p_declarations )
{
  m_dataexpressions = p_declarations;
}

list_of_dataexpression datatype_specification::get_declaration_list( void ) const
{
  return m_dataexpression_list;
}

unsigned int datatype_specification::get_count_dataexpression( void )
{
  return m_dataexpression_list.GetCount();
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_datatype_specification )
