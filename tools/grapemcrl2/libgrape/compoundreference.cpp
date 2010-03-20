// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file compoundreference.cpp
//
// Implements the compound_reference class.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include "compoundreference.h"

using namespace grape::libgrape;

compound_reference::compound_reference( void )
: object( NONE )
{
  // shouldn't be called
}

compound_reference::compound_reference( object_type p_type )
: object( p_type )
{
  assert( p_type == ARCHITECTURE_REFERENCE || p_type == PROCESS_REFERENCE );
  m_name = wxEmptyString;
  m_current_reference = false;
  m_has_channel.Empty();
}

compound_reference::compound_reference( const compound_reference &p_compound_ref )
: object( p_compound_ref )
{
  m_name = p_compound_ref.m_name;
  m_current_reference = p_compound_ref.m_current_reference;
  m_has_channel = p_compound_ref.m_has_channel;
}

compound_reference::~compound_reference( void )
{
  // Remove all references to this object.
  for ( unsigned int i = 0; i < m_has_channel.GetCount(); ++i )
  {
    channel* channel_ptr = m_has_channel.Item(i);
    channel_ptr->detach_reference();
  }

  // Free all used resources.
  m_has_channel.Clear();
}

wxString compound_reference::get_name(void) const
{
  return m_name;
}

void compound_reference::set_name( const wxString &p_name)
{
  m_name = p_name;
}

void compound_reference::attach_channel( channel* p_channel )
{
  m_has_channel.Add( p_channel );
}

void compound_reference::detach_channel( channel* p_channel )
{
  int i = m_has_channel.Index( p_channel, false );
  if ( i != wxNOT_FOUND )
  {
    m_has_channel.Remove( p_channel );
  }

}

channel* compound_reference::get_channel( int i )
{
  return m_has_channel.Item( i );
}

unsigned int compound_reference::count_channel( void )
{
  return m_has_channel.GetCount();
}


