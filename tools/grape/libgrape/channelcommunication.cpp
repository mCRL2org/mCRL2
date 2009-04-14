// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file channelcommunication.cpp
//
// Implements the channel_communication class.

#include "channelcommunication.h"

using namespace grape::libgrape;

channel_communication::channel_communication( void )
: object( CHANNEL_COMMUNICATION )
{
  m_communication.Empty();
  set_channel_communication_type(VISIBLE_CHANNEL_COMMUNICATION);
}

channel_communication::channel_communication( channel* p_channel_1, channel* p_channel_2 )
: object( CHANNEL_COMMUNICATION )
{
  m_communication.Empty();
  set_channel_communication_type(VISIBLE_CHANNEL_COMMUNICATION);
  p_channel_1->set_channel_type(HIDDEN_CHANNEL);
  p_channel_2->set_channel_type(HIDDEN_CHANNEL);
  
  communication* comm_1 = new communication( p_channel_1 );
  communication* comm_2 = new communication( p_channel_2 );
  m_communication.Add( comm_1 );
  m_communication.Add( comm_2 );
}


channel_communication::channel_communication( const channel_communication &p_channel_comm )
: object( p_channel_comm )
{
  m_communication = p_channel_comm.m_communication;
  m_rename_to = p_channel_comm.m_rename_to;
  set_channel_communication_type(p_channel_comm.get_channel_communication_type());
}

channel_communication::~channel_communication( void )
{
  // Remove all references to the channel communication
  for ( unsigned int i = 0; i < m_communication.GetCount(); ++i )
  {
    communication &comm = m_communication.Item ( i );
    channel* channel_ptr = comm.get_channel();
    channel_ptr->detach_channel_communication(this);
  }

  m_communication.Clear();
}

wxString channel_communication::get_rename_to(void) const
{
  return m_rename_to;
}

void channel_communication::set_rename_to( const wxString &p_rename_to)
{
  m_rename_to = p_rename_to;
}

void channel_communication::attach_channel( channel* p_channel )
{
  communication* comm = new communication( p_channel );
  m_communication.Add( comm );
}

void channel_communication::detach_channel( channel* p_channel )
{
  // for-loop declarations
  int count = m_communication.GetCount();
  for ( int i = 0; i < count; ++i )
  {
    communication &comm = m_communication.Item( i );
    if ( comm.get_channel() == p_channel )
    {
      communication* del_comm = m_communication.Detach( i );
      delete del_comm;
      break;
    } // end if
  } // end for
}

unsigned int channel_communication::count_channel( void )
{
  return m_communication.GetCount();
}

channel* channel_communication::get_attached_channel( int p_i )
{
  communication &comm = m_communication.Item( p_i );
  return comm.get_channel();
}

arr_communication* channel_communication::get_communications( void )
{
  return &m_communication;
}

channel_communication_type channel_communication::get_channel_communication_type( void ) const
{
  return m_channel_communication_type;
}

void channel_communication::set_channel_communication_type( const channel_communication_type &p_channel_communication_type )
{
  m_channel_communication_type = p_channel_communication_type;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_channel_communication );
