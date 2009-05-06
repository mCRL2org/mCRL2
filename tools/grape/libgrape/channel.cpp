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
: object( CHANNEL )
{
  m_has_channel = 0;
  init();
}

channel::channel( compound_reference &p_has_channel )
: object( CHANNEL )
{
  m_has_channel = &p_has_channel;
  init();
}

void channel::init()
{
  m_name = wxEmptyString;
  m_enabled_channel = false;
  m_channel_communication.Empty();
  m_channel_type = VISIBLE_CHANNEL;
}

channel::channel( const channel &p_channel )
: object( p_channel )
{
  m_name = p_channel.m_name;
  m_rename_to = p_channel.m_rename_to;
  m_enabled_channel = p_channel.m_enabled_channel;
  m_has_channel = p_channel.m_has_channel;
  m_channel_communication = p_channel.m_channel_communication;
  m_channel_type = p_channel.m_channel_type;
}

channel::~channel(void)
{
  // Remove all references to the channel.
  if ( m_has_channel != 0 )
  {
    m_has_channel->detach_channel( this );
  }

  for ( unsigned int i = 0; i < m_channel_communication.GetCount(); ++i )
  {
    channel_communication *comm = m_channel_communication.Item ( i );
    comm->detach_channel(this);
  }
  m_channel_communication.Clear();
}

bool channel::operator==( const channel &p_channel )
{
  return m_name == p_channel.m_name &&
  m_rename_to == p_channel.m_rename_to &&
  m_enabled_channel == p_channel.m_enabled_channel &&
  &m_has_channel == &(p_channel.m_has_channel) &&
  &m_channel_communication == &(p_channel.m_channel_communication) &&
  &m_channel_type == &(p_channel.m_channel_type);
}

wxString channel::get_name(void) const
{
  return m_name;
}

void channel::set_name( const wxString &p_name)
{
  m_name = p_name;
}

wxString channel::get_rename_to(void) const
{
  return m_rename_to;
}

void channel::set_rename_to( const wxString &p_rename_to)
{
  m_rename_to = p_rename_to;
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
  m_channel_communication.Add(p_comm);
  m_channel_type = HIDDEN_CHANNEL;
}

void channel::detach_channel_communication( channel_communication* p_comm )
{
  int i = m_channel_communication.Index( p_comm );
  if (i != wxNOT_FOUND)
  {
    m_channel_communication.Remove( p_comm );
    m_channel_communication.Shrink();
  }
  if (m_channel_communication.GetCount() == 0)
  {
    m_channel_type = VISIBLE_CHANNEL;
  }
}

arr_channel_communication_ptr* channel::get_channel_communications( void )
{
  return &m_channel_communication;
}

bool channel::get_enabled( void ) const
{
  return m_enabled_channel;
}

void channel::set_enabled( bool p_enabled )
{
  m_enabled_channel = p_enabled;
}
       
channel_type channel::get_channel_type( void ) const
{
  return m_channel_type;
}

void channel::set_channel_type( const channel_type &p_channel_type )
{
  m_channel_type = p_channel_type;
}
        
// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_channel );
