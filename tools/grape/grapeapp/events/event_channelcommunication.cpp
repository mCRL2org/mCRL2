// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_channelcommunication.cpp
//
// Defines GraPE events for channel communications

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "dialogs/channelcommunicationdialog.h"
#include "../visuals/visualchannel_communication.h"

#include "event_channelcommunication.h"

using namespace grape::grapeapp;

grape_event_add_channel_communication::grape_event_add_channel_communication( grape_frame *p_main_frame, coordinate &p_coord, channel* p_chan_1, channel* p_chan_2 )
: grape_event_base( p_main_frame, true, _T( "add channel communication" ) )
{
  m_c_comm = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_chan_1 = p_chan_1->get_id();
  m_chan_2 = p_chan_2->get_id();
  m_channel_type_1 = p_chan_1->get_channel_type();
  m_channel_type_2 = p_chan_2->get_channel_type();
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( m_main_frame->get_glcanvas()->get_diagram() );
  assert( dia_ptr != 0 );// The diagram has to exist and be of the specified type, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();
}

grape_event_add_channel_communication::~grape_event_add_channel_communication( void )
{
}

bool grape_event_add_channel_communication::Do( void )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Has to be the case or the event wouldn't have been generated.

  channel* chan_1 = dynamic_cast<channel*> ( find_object( m_chan_1, CHANNEL, dia_ptr->get_id() ) );
  channel* chan_2 = dynamic_cast<channel*> ( find_object( m_chan_2, CHANNEL, dia_ptr->get_id() ) );
  assert( ( chan_1 != 0 ) && ( chan_2 != 0 ) );


  dia_ptr->add_channel_communication( m_c_comm, m_coord, chan_1, chan_2 );

  finish_modification();
  return true;
}

bool grape_event_add_channel_communication::Undo( void )
{
  // find the diagram the channel communication was added to
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the channel communication that is to be removed
  channel_communication* comm_ptr = static_cast<channel_communication*> ( find_object( m_c_comm, CHANNEL_COMMUNICATION, dia_ptr->get_id() ) );
  dia_ptr->remove_channel_communication( comm_ptr );

  // Find the channels that were connected to the channelcommunication and restore the properties
  channel* chan_1 = dynamic_cast<channel*> ( find_object( m_chan_1, CHANNEL, dia_ptr->get_id() ) );
  channel* chan_2 = dynamic_cast<channel*> ( find_object( m_chan_2, CHANNEL, dia_ptr->get_id() ) );
  assert( ( chan_1 != 0 ) && ( chan_2 != 0 ) );
  chan_1->set_channel_type(m_channel_type_1);
  chan_2->set_channel_type(m_channel_type_2);

  finish_modification();
  return true;
}

grape_event_remove_channel_communication::grape_event_remove_channel_communication(  grape_frame *p_main_frame, channel_communication *p_c_comm, architecture_diagram* p_arch_dia_ptr, bool p_normal  )
: grape_event_base( p_main_frame, true, _T( "remove channel" ) )
{
  m_c_comm = p_c_comm->get_id();
  m_coordinate = p_c_comm->get_coordinate();
  m_width = p_c_comm->get_width();
  m_height = p_c_comm->get_height();
  m_rename = p_c_comm->get_rename_to();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_c_comm->count_comment(); ++i )
  {
    comment* comm_ptr = p_c_comm->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_arch_dia_ptr->get_id();
  m_channels.Empty();
  m_channel_communication_type = p_c_comm->get_channel_communication_type();

  // Remember the channels the channel communication was attached to.
  for ( unsigned int i = 0; i < p_c_comm->count_channel(); ++i )
  {
    channel* chan_ptr = p_c_comm->get_attached_channel( i );
    m_channels.Add( chan_ptr->get_id() );
  }
  assert( m_channels.GetCount() >= 2 ); // Channel communication should have two or more channels, else it cannot exist.
}

grape_event_remove_channel_communication::~grape_event_remove_channel_communication( void )
{
  m_comments.Clear();
  m_channels.Clear();
}

bool grape_event_remove_channel_communication::Do( void )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  channel_communication* comm_ptr = static_cast<channel_communication*> ( find_object( m_c_comm, CHANNEL_COMMUNICATION, dia_ptr->get_id() ) );

  if ( comm_ptr )
  {
    visualchannel_communication* vis_comm_ptr = static_cast<visualchannel_communication*> (m_main_frame->get_glcanvas()->get_visual_object( comm_ptr ) );
    unsigned int communication_selected = vis_comm_ptr->get_communication_selected();
    
    // if there is no valid selected communication
    if ( (communication_selected == -1) || (communication_selected > static_cast<unsigned>(comm_ptr->count_channel())) || (comm_ptr->count_channel() == 2) ) 
    {
      // remove the entire channel communication
      dia_ptr->remove_channel_communication( comm_ptr );
    } else {
      // remove the selected channel
      channel* chan = comm_ptr->get_attached_channel( communication_selected );
      comm_ptr->detach_channel( chan );
    }

    finish_modification();
    return true;
  }

  return false;
}

bool grape_event_remove_channel_communication::Undo( void )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );

  channel* chan_1 = static_cast<channel*> ( find_object( m_channels.Item( 0 ), CHANNEL, dia_ptr->get_id() ) );
  channel* chan_2 = static_cast<channel*> ( find_object( m_channels.Item( 1 ), CHANNEL, dia_ptr->get_id() ) );

  if ( !chan_1 || !chan_2 )
  {
    // not all channels have revived yet, try again later.
    return false;
  } 
  
  channel_communication* comm_ptr = static_cast<channel_communication*> ( find_object( m_c_comm, CHANNEL_COMMUNICATION, dia_ptr->get_id() ) );

  if ( comm_ptr == 0)
  {
    // Recreate channel communication
    channel_communication* new_comm = dia_ptr->add_channel_communication( m_c_comm, m_coordinate, chan_1, chan_2 );

    // Reattach channels
    for ( unsigned int i = 2; i < m_channels.GetCount(); ++i )
    {
      channel* chan =  dynamic_cast<channel*> ( find_object( m_channels.Item( i ), CHANNEL, dia_ptr->get_id() ) );
      dia_ptr->attach_channel_communication_to_channel( new_comm, chan );
    }

    new_comm->set_rename_to( m_rename );
    new_comm->set_channel_communication_type( m_channel_communication_type );
  } else {
    // Reattach channel
    for ( unsigned int i = 0; i < m_channels.GetCount(); ++i )
    {
      channel* chan =  dynamic_cast<channel*> ( find_object( m_channels.Item( i ), CHANNEL, dia_ptr->get_id() ) );
      // if the channel communication is not yet connected with chan
      if (!comm_ptr->has_channel(chan))
      {
        // make a connection
        dia_ptr->attach_channel_communication_to_channel( comm_ptr, chan );
      }
    }
  }
  finish_modification();
  return true;
}

grape_event_attach_channel_communication::grape_event_attach_channel_communication(  grape_frame *p_main_frame, channel_communication* p_channel_communication, channel* p_channel )
: grape_event_base( p_main_frame, true, _T( "attach channel communication to channel" ) )
{
  m_channel_communication = p_channel_communication->get_id();
  m_channel = p_channel->get_id();
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  m_diagram = dia_ptr->get_id();

  assert( ( p_channel->get_diagram() == dia_ptr ) && ( p_channel_communication->get_diagram() == dia_ptr ) );
}

grape_event_attach_channel_communication::~grape_event_attach_channel_communication(  void  )
{
}

bool grape_event_attach_channel_communication::Do(  void  )
{
  architecture_diagram* dia_ptr = static_cast<architecture_diagram*> ( find_diagram( m_diagram ) );
  channel_communication* comm_ptr = static_cast<channel_communication*> ( find_object( m_channel_communication ) );
  channel* chan_ptr = static_cast<channel*> ( find_object( m_channel ) );

  dia_ptr->attach_channel_communication_to_channel( comm_ptr, chan_ptr );

  finish_modification();
  return true;
}

bool grape_event_attach_channel_communication::Undo(  void  )
{
  architecture_diagram* dia_ptr = static_cast<architecture_diagram*> ( find_diagram( m_diagram ) );
  channel_communication* comm_ptr = static_cast<channel_communication*> ( find_object( m_channel_communication ) );
  channel* chan_ptr = static_cast<channel*> ( find_object( m_channel ) );

  dia_ptr->detach_channel_from_channel_communication( chan_ptr, comm_ptr );

  finish_modification();
  return true;
}


grape_event_detach_channel_communication::grape_event_detach_channel_communication(  grape_frame *p_main_frame, channel_communication* p_channel_communication, channel* p_channel )
: grape_event_base( p_main_frame, true, _T( "detach channel communication" ) )
{
  m_channel_communication = p_channel_communication->get_id();
  m_channel = p_channel->get_id();
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  m_diagram = dia_ptr->get_id();

  m_remove_event = 0;
  if ( p_channel_communication->count_channel() <= 2 )
  {
    m_remove_event = new grape_event_remove_channel_communication( m_main_frame, p_channel_communication, static_cast<architecture_diagram*> ( dia_ptr ) );
  }
}

grape_event_detach_channel_communication::~grape_event_detach_channel_communication(  void  )
{
}

bool grape_event_detach_channel_communication::Do(  void  )
{
  architecture_diagram* dia_ptr = static_cast<architecture_diagram*> ( find_diagram( m_diagram ) );
  channel_communication* comm_ptr = static_cast<channel_communication*> ( find_object( m_channel_communication, CHANNEL_COMMUNICATION, dia_ptr->get_id() ) );
  if ( m_remove_event ) // make a choice between deleting the channel communication
  {
    m_remove_event->Do();
  }
  else // or just detaching one channel
  {
    channel* chan_ptr = static_cast<channel*> ( find_object( m_channel, CHANNEL, dia_ptr->get_id() ) );
    dia_ptr->detach_channel_from_channel_communication( chan_ptr, comm_ptr );
  }

  comm_ptr = static_cast<channel_communication*> ( find_object( m_channel_communication, CHANNEL_COMMUNICATION, dia_ptr->get_id() ) );
  if ( comm_ptr )
  {
    assert( comm_ptr->count_channel() >= 2 ); // the communication should have two or more channels, or it should have been deleted
  }

  finish_modification();
  return true;
}

bool grape_event_detach_channel_communication::Undo(  void  )
{
  if ( m_remove_event ) // if the channel communication was deleted, put it back
  {
    m_remove_event->Undo();
  }
  else // it should exist, re-attach the channel
  {
    architecture_diagram* dia_ptr = static_cast<architecture_diagram*> ( find_diagram( m_diagram ) );
    channel_communication* comm_ptr = static_cast<channel_communication*> ( find_object( m_channel_communication, CHANNEL_COMMUNICATION, dia_ptr->get_id() ) );
    // the channel communication isn't there yet, try it again later on when reviving another channel
    if ( !comm_ptr )
    {
      return false;
    }

    channel* chan_ptr = static_cast<channel*> ( find_object( m_channel, CHANNEL, dia_ptr->get_id() ) );
    dia_ptr->attach_channel_communication_to_channel( comm_ptr, chan_ptr );
  }

  finish_modification();
  return true;
}


grape_event_change_channel_communication::grape_event_change_channel_communication( grape_frame *p_main_frame, channel_communication* p_channel_communication )
: grape_event_base( p_main_frame, true, _T( "change channel communication properties" ) )
{ 
  m_channel_communication = p_channel_communication->get_id();  
  
  m_old_channel_communication = *p_channel_communication;
  
  grape_channel_communication_dlg dialog( m_old_channel_communication );

  m_pressed_ok = dialog.show_modal( m_new_channel_communication );  
}

grape_event_change_channel_communication::~grape_event_change_channel_communication( void )
{
}

bool grape_event_change_channel_communication::Do( void )
{
  if ( !m_pressed_ok )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  channel_communication* channel_communication_ptr = static_cast<channel_communication*> ( find_object( m_channel_communication, CHANNEL_COMMUNICATION ) );
  channel_communication_ptr->set_rename_to( m_new_channel_communication.get_rename_to() );
  channel_communication_ptr->set_channel_communication_type( m_new_channel_communication.get_channel_communication_type() );
 
  finish_modification();
  return true;
}

bool grape_event_change_channel_communication::Undo( void )
{
  channel_communication* channel_communication_ptr = static_cast<channel_communication*> ( find_object( m_channel_communication, CHANNEL_COMMUNICATION ) );
  channel_communication_ptr->set_rename_to( m_old_channel_communication.get_rename_to() );
  channel_communication_ptr->set_channel_communication_type( m_old_channel_communication.get_channel_communication_type() );
   
  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_channel_communication );
WX_DEFINE_OBJARRAY( arr_event_detach_channel_communication );
