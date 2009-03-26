// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_channel.cpp
//
// Defines GraPE events for channels

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "dialogs/channeldialog.h"

#include "event_channelcommunication.h"
#include "event_channel.h"

// Channel size. 0.05 was too large, 0.025 too small
const float DEFAULT_CHANNEL_WIDTH = 0.037f;
const float DEFAULT_CHANNEL_HEIGHT = 0.037f;

using namespace grape::grapeapp;

grape_event_add_channel::grape_event_add_channel( grape_frame *p_main_frame, coordinate &p_coord, compound_reference* p_ref )
: grape_event_base( p_main_frame, true, _T( "add channel" ) )
{
  m_chan = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_ref = p_ref->get_id();
  m_def_chan_width = DEFAULT_CHANNEL_WIDTH;
  m_def_chan_height = DEFAULT_CHANNEL_HEIGHT;

  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( m_main_frame->get_glcanvas()->get_diagram() );
  assert( dia_ptr != 0 );// The diagram has to exist and be of the specified type, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();
}

grape_event_add_channel::~grape_event_add_channel( void )
{
}

bool grape_event_add_channel::Do( void )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Has to be the case or the event wouldn't have been generated.

  compound_reference* ref = dynamic_cast<compound_reference*> ( find_object( m_ref ) );
  assert( ref != 0 );
  dia_ptr->add_channel( m_chan, m_coord, m_def_chan_width, m_def_chan_height, ref );

  finish_modification();
  return true;
}

bool grape_event_add_channel::Undo( void )
{
  // find the diagram the channel was added to
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the state that is to be removed
  channel* chan_ptr = static_cast<channel*> ( find_object( m_chan, CHANNEL, dia_ptr->get_id() ) );
  dia_ptr->remove_channel( chan_ptr );

  finish_modification();
  return true;
}

grape_event_remove_channel::grape_event_remove_channel( grape_frame *p_main_frame, channel* p_chan, diagram* p_dia_ptr, bool p_normal )
: grape_event_base( p_main_frame, true, _T( "remove channel" ) )
{
  m_chan = p_chan->get_id();
  m_normal = p_normal;
  m_name = p_chan->get_name();
  m_rename = p_chan->get_rename_to();
  m_coordinate = p_chan->get_coordinate();
  m_ref = p_chan->get_reference()->get_id();
  m_width = p_chan->get_width();
  m_height = p_chan->get_height();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_chan->count_comment(); ++i )
  {
    comment* comm_ptr = p_chan->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_dia_ptr->get_id();
  m_channel_type = p_chan->get_channel_type();
  m_channel_communication.Empty();

  if ( m_normal )
  {
    // Create detach event for associated channel communications, if applicable.
    arr_channel_communication_ptr* chan_comm_ptr = p_chan->get_channel_communications();
    for ( unsigned int j = 0; j < chan_comm_ptr->GetCount(); ++j )
    {
      channel_communication* comm = chan_comm_ptr->Item( j );
      if ( comm != 0 )
      {
        grape_event_detach_channel_communication* event =new grape_event_detach_channel_communication( m_main_frame, comm, p_chan );
        m_channel_communication.Add( event );
      }
    }
  }
}

grape_event_remove_channel::~grape_event_remove_channel( void )
{
  m_comments.Clear();
  m_channel_communication.Clear();
}

bool grape_event_remove_channel::Do( void )
{
  // Perform remove event Do for channel communications.
  for ( unsigned int i = 0; i < m_channel_communication.GetCount(); ++i )
  {
    grape_event_detach_channel_communication event = m_channel_communication.Item( i );
    event.Do();
  }

  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  channel* chan_ptr = dynamic_cast<channel*> ( find_object( m_chan, CHANNEL, dia_ptr->get_id() ) );
  assert( chan_ptr != 0 );

  dia_ptr->remove_channel( chan_ptr );

  finish_modification();
  return true;
}

bool grape_event_remove_channel::Undo( void )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  compound_reference* ref = dynamic_cast<compound_reference*> ( find_object( m_ref ) );
  assert( ref != 0 );
  channel* new_chan = dia_ptr->add_channel( m_chan, m_coordinate, m_width, m_height, ref );
  new_chan->set_name( m_name );
  new_chan->set_rename_to( m_rename );
  new_chan->set_channel_type( m_channel_type );

  // Perform remove event Undo for channel communications.
  for ( unsigned int i = 0; i < m_channel_communication.GetCount(); ++i )
  {
    grape_event_detach_channel_communication event = m_channel_communication.Item( i );
    event.Undo();
  }

  finish_modification();
  return true;
}


grape_event_change_channel::grape_event_change_channel( grape_frame *p_main_frame, channel* p_channel )
: grape_event_base( p_main_frame, true, _T( "change channel properties" ) )
{ 
  m_channel = p_channel->get_id();  
  
  m_old_channel = *p_channel;
  
  grape_channel_dlg dialog( m_old_channel );

  m_pressed_ok = dialog.show_modal( m_new_channel );  
}

grape_event_change_channel::~grape_event_change_channel( void )
{
}

bool grape_event_change_channel::Do( void )
{
  if ( !m_pressed_ok )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  channel* channel_ptr = static_cast<channel*> ( find_object( m_channel, CHANNEL ) );
  channel_ptr->set_name( m_new_channel.get_name() );
  channel_ptr->set_rename_to( m_new_channel.get_rename_to() );
  channel_ptr->set_channel_type( m_new_channel.get_channel_type() );
 
  finish_modification();
  return true;
}

bool grape_event_change_channel::Undo( void )
{
  channel* channel_ptr = static_cast<channel*> ( find_object( m_channel, CHANNEL ) );
  channel_ptr->set_name( m_old_channel.get_name() );
  channel_ptr->set_rename_to( m_old_channel.get_rename_to() );
  channel_ptr->set_channel_type( m_old_channel.get_channel_type() );
   
  finish_modification();
  return true;
}


grape_event_detach_channel::grape_event_detach_channel( grape_frame *p_main_frame, channel* p_channel )
: grape_event_base( p_main_frame, true, _T( "detach channel from channel communication" ) )
{
  m_channel = p_channel;

  arr_channel_communication_ptr* comms = m_channel->get_channel_communications();
  for ( unsigned int i = 0; i < comms->GetCount(); ++i )
  {
    channel_communication* comm_ptr = comms->Item( i );
    if ( comm_ptr != 0 )
    {
      grape_event_detach_channel_communication* event = new grape_event_detach_channel_communication( m_main_frame, comm_ptr, m_channel );
      m_channel_communication.Add( event );
    }
  }
}

grape_event_detach_channel::~grape_event_detach_channel( void )
{
  m_channel_communication.Clear();
}

bool grape_event_detach_channel::Do( void )
{
  // Perform remove event Do for channel communications.
  for ( unsigned int i = 0; i < m_channel_communication.GetCount(); ++i )
  {
    grape_event_detach_channel_communication event = m_channel_communication.Item( i );
    event.Do();
  }
  return true;
}

bool grape_event_detach_channel::Undo( void )
{
  // Perform remove event Undo for channel communications.
  for ( unsigned int i = 0; i < m_channel_communication.GetCount(); ++i )
  {
    grape_event_detach_channel_communication event = m_channel_communication.Item( i );
    event.Undo();
  }
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_channel );
