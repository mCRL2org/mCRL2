// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sortexpression.cpp
//
// Implements the sortexpression datatype.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include "sortexpression.h"

using namespace grape::libgrape;

sortexpression::sortexpression( void )
{
  m_expression = wxEmptyString;
}

sortexpression::sortexpression( const sortexpression &p_sortexpression )
{
  m_expression = p_sortexpression.m_expression;
}

sortexpression::~sortexpression( void )
{
}

void sortexpression::set_expression( const wxString &p_expression )
{
  m_expression = p_expression;
}

wxString sortexpression::get_expression( void ) const
{
  return m_expression;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( list_of_sortexpression )

