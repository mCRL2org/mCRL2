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
  m_detached_comm_1.Empty();  
  m_detached_comm_2.Empty();  

  // This code is for detaching already connected channel communications. Which doesn't allow or-communications.
  // To allow or communications: comment the code below
  arr_channel_communication_ptr* comm_ptr_list_1 = p_chan_1->get_channel_communications();
  for (unsigned int i = 0; i < comm_ptr_list_1->GetCount(); ++i)
  {
    channel_communication* comm_ptr_1 = comm_ptr_list_1->Item(i);  
    if ( comm_ptr_1 )  
    {  
      grape_event_detach_channel_communication* event = new grape_event_detach_channel_communication( m_main_frame, comm_ptr_1, p_chan_1 );
      m_detached_comm_1.Add( event );
    }
  }  
  arr_channel_communication_ptr* comm_ptr_list_2 = p_chan_2->get_channel_communications();
  for (unsigned int i = 0; i < comm_ptr_list_2->GetCount(); ++i)
  {
    channel_communication* comm_ptr_2 = comm_ptr_list_2->Item(i);
    if ( comm_ptr_2 )  
    {
      bool found = false;
      for (unsigned int j = 0; j < comm_ptr_list_1->GetCount(); ++j)
      {
        if (comm_ptr_2 == comm_ptr_list_1->Item(j) && comm_ptr_2->count_channel() == 2)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        grape_event_detach_channel_communication* event = new grape_event_detach_channel_communication( m_main_frame, comm_ptr_2, p_chan_2 );
        m_detached_comm_2.Add( event );
      }
    }
  }  
  // end detaching
}

grape_event_add_channel_communication::~grape_event_add_channel_communication( void )
{
  m_detached_comm_1.Clear();
  m_detached_comm_2.Clear();
}

bool grape_event_add_channel_communication::Do( void )
{
  // These 2 for loops are to perform the channel communication detach events for the channel communications that are already connected to the channels.
  // To allow or communications: comment the 2 for loops
  for (unsigned int i = 0; i < m_detached_comm_1.GetCount(); ++i)
  {
    grape_event_detach_channel_communication event = m_detached_comm_1[i];
    event.Do();
  }
  for (unsigned int i = 0; i < m_detached_comm_2.GetCount(); ++i)
  {
    grape_event_detach_channel_communication event = m_detached_comm_2[i];
    event.Do();
  }
  // end detaching
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

  // These 2 for loops are to perform the undo of the channel communication detach events for the channel communications that are already connected to the channels.
  // To allow or communications: comment the 2 for loops
  for (unsigned int i = 0; i < m_detached_comm_1.GetCount(); ++i)
  {
    grape_event_detach_channel_communication event = m_detached_comm_1[i];
    event.Undo();
  }
  for (unsigned int i = 0; i < m_detached_comm_2.GetCount(); ++i)
  {
    grape_event_detach_channel_communication event = m_detached_comm_2[i];
    event.Undo();
  }
  // end detaching

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
  m_name = p_c_comm->get_name_to();
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
  visualchannel_communication* vis_comm_ptr = static_cast<visualchannel_communication*> (m_main_frame->get_glcanvas()->get_visual_object( p_c_comm ) );
  m_communication_selected = vis_comm_ptr->get_communication_selected();
  if (m_communication_selected == -1)
  {
    m_communication_channel = -1;
  }
  else
  {
    channel* chan = p_c_comm->get_attached_channel( m_communication_selected );
    m_communication_channel = chan->get_id();
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
    // if there is no valid selected communication
    if ( (m_communication_selected == -1) || (m_communication_selected > static_cast<signed>(comm_ptr->count_channel())) || (comm_ptr->count_channel() == 2) ) 
    {
      // remove the entire channel communication
      dia_ptr->remove_channel_communication( comm_ptr );
    } else {
      // remove the selected channel
      channel* chan = dynamic_cast<channel*> ( find_object( m_communication_channel, CHANNEL, dia_ptr->get_id() ) );
      //channel* chan = comm_ptr->get_attached_channel( m_communication_selected );
      if (comm_ptr->has_channel(chan))
      {
        dia_ptr->detach_channel_from_channel_communication(chan, comm_ptr);
      }
      else
      {
        return false;
      }
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

    new_comm->set_name_to( m_name );
    new_comm->set_channel_communication_type( m_channel_communication_type );
  } else {
    // Reattach channel
    channel* chan = dynamic_cast<channel*> ( find_object( m_communication_channel, CHANNEL, dia_ptr->get_id() ) );
    if (chan != 0)
    {
      dia_ptr->attach_channel_communication_to_channel( comm_ptr, chan );
    }
/*  for ( unsigned int i = 0; i < m_channels.GetCount(); ++i )
    {
      channel* chan =  dynamic_cast<channel*> ( find_object( m_channels.Item( i ), CHANNEL, dia_ptr->get_id() ) );
      // if the channel communication is not yet connected with chan
      if (!comm_ptr->has_channel(chan))
      {
        // make a connection
        dia_ptr->attach_channel_communication_to_channel( comm_ptr, chan );
      }
    }
*/
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

  m_detached_comm.Empty();

  // This code is for detaching already connected channel communications. Which doesn't allow or-communications.
  // To allow or communications: comment the code below
  arr_channel_communication_ptr* comm_ptr_list = p_channel->get_channel_communications();
  for (unsigned int i = 0; i < comm_ptr_list->GetCount(); ++i)
  {
    channel_communication* comm_ptr = comm_ptr_list->Item(i);  
    if ( comm_ptr )  
    {  
      grape_event_detach_channel_communication* event = new grape_event_detach_channel_communication( m_main_frame, comm_ptr, p_channel );
      m_detached_comm.Add( event );
    }
  }  
  // end detaching

  assert( ( p_channel->get_diagram() == dia_ptr ) && ( p_channel_communication->get_diagram() == dia_ptr ) );
}

grape_event_attach_channel_communication::~grape_event_attach_channel_communication(  void  )
{
  m_detached_comm.Clear();
}

bool grape_event_attach_channel_communication::Do(  void  )
{
  // This code is for detaching already connected channel communications. Which doesn't allow or-communications.
  // To allow or communications: comment the code below
  for (unsigned int i = 0; i < m_detached_comm.GetCount(); ++i)
  {
    grape_event_detach_channel_communication event = m_detached_comm[i];
    event.Do();
  }
  // end detaching
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

  // This code is for detaching already connected channel communications. Which doesn't allow or-communications.
  // To allow or communications: comment the code below
  for (unsigned int i = 0; i < m_detached_comm.GetCount(); ++i)
  {
    grape_event_detach_channel_communication event = m_detached_comm[i];
    event.Undo();
  }
  // end detaching
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

//  m_remove_event = 0;
//  if ( p_channel_communication->count_channel() <= 2 )
//  {
    m_remove_event = new grape_event_remove_channel_communication( m_main_frame, p_channel_communication, static_cast<architecture_diagram*> ( dia_ptr ) );
//  }
}

grape_event_detach_channel_communication::~grape_event_detach_channel_communication(  void  )
{
}

bool grape_event_detach_channel_communication::Do(  void  )
{
  architecture_diagram* dia_ptr = static_cast<architecture_diagram*> ( find_diagram( m_diagram ) );
  channel_communication* comm_ptr = static_cast<channel_communication*> ( find_object( m_channel_communication, CHANNEL_COMMUNICATION, dia_ptr->get_id() ) );
//  if ( m_remove_event ) // make a choice between deleting the channel communication
  if (comm_ptr->count_channel() == 2)
  {
    m_remove_event->Do();
  }
  else // or just detaching one channel
  {
    m_remove_event = 0;
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
  channel_communication_ptr->set_name_to( m_new_channel_communication.get_name_to() );
  channel_communication_ptr->set_channel_communication_type( m_new_channel_communication.get_channel_communication_type() );
 
  finish_modification();
  return true;
}

bool grape_event_change_channel_communication::Undo( void )
{
  channel_communication* channel_communication_ptr = static_cast<channel_communication*> ( find_object( m_channel_communication, CHANNEL_COMMUNICATION ) );
  channel_communication_ptr->set_name_to( m_old_channel_communication.get_name_to() );
  channel_communication_ptr->set_channel_communication_type( m_old_channel_communication.get_channel_communication_type() );
   
  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_channel_communication );
WX_DEFINE_OBJARRAY( arr_event_detach_channel_communication );
