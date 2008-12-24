// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visible.cpp
//
// Implements the visible class.

#include "visible.h"
#include "connection.h"

using namespace grape::libgrape;

visible::visible(void)
: connection_property( VISIBLE )
{
  m_name = wxEmptyString;
}

visible::visible( const visible &p_visible )
: connection_property( p_visible )
{
  m_name = p_visible.m_name;
}

visible::~visible(void)
{
  if ( m_has_property != 0 )
  {
    // Remove references to the property.
    m_has_property->detach_property();
  }

  // Clear used space
  m_breakpoints.Clear();
}

wxString visible::get_name( void ) const
{
  return m_name;
}

void visible::set_name( const wxString &p_name )
{
  m_name = p_name;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_visible );
