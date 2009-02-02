// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file channel.cpp
//
// Implements the Channel class.

#include "channel.h"
#include "compoundreference.h"
#include "channelcommunication.h"

using namespace grape::libgrape;

channel::channel( void )
: connection( CHANNEL )
{
  m_has_channel = 0;
  init();
}

channel::channel( compound_reference &p_has_channel )
: connection( CHANNEL )
{
  m_has_channel = &p_has_channel;
  init();
}

void channel::init()
{
  m_name = wxEmptyString;
  m_enabled_channel = false;
  m_channel_communication = 0;
}

channel::channel( const channel &p_channel )
: connection( p_channel )
{
  m_name= p_channel.m_name;
  m_enabled_channel = p_channel.m_enabled_channel;
  m_has_channel = p_channel.m_has_channel;
  m_channel_communication = p_channel.m_channel_communication;
}

channel::~channel(void)
{
  // Remove all references to the channel.
  if ( m_has_channel != 0 )
  {
    m_has_channel->detach_channel( this );
  }
  if ( m_channel_communication != 0 )
  {
    m_channel_communication->detach_channel( this );
  }
}

bool channel::operator==( const channel &p_channel )
{
  return m_name == p_channel.m_name &&
  m_enabled_channel == p_channel.m_enabled_channel &&
  &m_has_channel == &(p_channel.m_has_channel) &&
  &m_channel_communication == &(p_channel.m_channel_communication);
}

wxString channel::get_name(void) const
{
  return m_name;
}

void channel::set_name( const wxString &p_name)
{
  m_name = p_name;
}

void channel::attach_reference( compound_reference* p_reference )
{
  m_has_channel = p_reference;
}

void channel::detach_reference( void )
{
  m_has_channel = 0;
}

compound_reference* channel::get_reference( void )
{
  return m_has_channel;
}

void channel::attach_channel_communication( channel_communication* p_comm )
{
  m_channel_communication = p_comm;
}

void channel::detach_channel_communication( void )
{
  m_channel_communication = 0;
}

channel_communication* channel::get_channel_communication( void )
{
  return m_channel_communication;
}

bool channel::get_enabled( void ) const
{
  return m_enabled_channel;
}

void channel::set_enabled( bool p_enabled )
{
  m_enabled_channel = p_enabled;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_channel );
