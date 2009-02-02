// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file blocked.cpp
//
// Implements the blocked class.

#include "blocked.h"
#include "connection.h"

using namespace grape::libgrape;

blocked::blocked( void )
: connection_property( BLOCKED )
{
}

blocked::blocked( const blocked &p_blocked )
: connection_property( p_blocked )
{
}

blocked::~blocked( void )
{
  if ( m_has_property != 0 )
  {
    // Remove references to the property.
    m_has_property->detach_property();
  }

  // Clear used space
  m_breakpoints.Clear();
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_blocked );
