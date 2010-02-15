// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file communication.cpp
//
// Implements the communication association class.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include "communication.h"

using namespace grape::libgrape;

communication::communication( channel* p_channel )
{
  m_linetype = straight;
  m_breakpoints.Empty();
  m_channel = p_channel;
}

communication::communication( const communication &p_communication )
{
  m_linetype = p_communication.m_linetype;
  m_breakpoints = p_communication.m_breakpoints;
  m_channel = p_communication.m_channel;
}

communication::~communication( void )
{
  // There are no references to be removed, as no object maintains a list of communications that are attached to it.
  m_breakpoints.Clear();
}

channel* communication::get_channel( void )
{
  return m_channel;
}

void communication::add_breakpoint ( coordinate &p_coordinate, int p_place )
{
  m_breakpoints.Insert( p_coordinate, p_place, 1 );
}

void communication::move_breakpoint ( coordinate &p_coordinate, int p_place )
{
  m_breakpoints.RemoveAt( p_place, 1 );
  m_breakpoints.Insert( p_coordinate, p_place, 1 );
}

void communication::remove_breakpoint ( int p_place )
{
  m_breakpoints.RemoveAt( p_place, 1 );
}


void communication::set_linetype( linetype p_linetype )
{
  m_linetype = p_linetype;
}

linetype communication::get_linetype( void ) const
{
  return m_linetype;
}



// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_communication )

