// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_channel.cpp
//
// Defines GraPE events for channels

#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "dialogs/textdialog.h"

#include "event_channelcommunication.h"

#include "event_channel.h"
#include "event_property.h"

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
  m_visible.Empty();
  m_blocked.Empty();
  m_channel_communication.Empty();

  if ( m_normal )
  {
    // Create remove event for associated property, if applicable
    connection_property* prop = p_chan->get_property();
    if ( prop != 0 )
    {
      visible* vis_ptr = dynamic_cast<visible*> ( prop );
      if ( vis_ptr != 0 ) // cast succesful
      {
        grape_event_remove_visible* event = new grape_event_remove_visible( m_main_frame, vis_ptr, static_cast<architecture_diagram*> ( p_dia_ptr ) );
        m_visible.Add( event );
      }
      else
      {
        blocked* block_ptr = dynamic_cast<blocked*> ( prop );
        assert( block_ptr != 0 );
        grape_event_remove_blocked* event = new grape_event_remove_blocked( m_main_frame, block_ptr, static_cast<architecture_diagram*> ( p_dia_ptr ) );
        m_blocked.Add( event );
      }
    }

    // Create detach event for associated channel communication, if applicable.
    channel_communication* comm = p_chan->get_channel_communication();
    if ( comm != 0 )
    {
      grape_event_detach_channel_communication* event =new grape_event_detach_channel_communication( m_main_frame, comm, p_chan );
      m_channel_communication.Add( event );
    }
  }
}

grape_event_remove_channel::~grape_event_remove_channel( void )
{
  m_comments.Clear();
  m_visible.Clear();
  m_blocked.Clear();
  m_channel_communication.Clear();
}

bool grape_event_remove_channel::Do( void )
{
  // Perform remove event Do for visible and blocked.
  for ( unsigned int i = 0; i < m_visible.GetCount(); ++i )
  {
    grape_event_remove_visible event = m_visible.Item( i );
    event.Do();
  }
  for ( unsigned int i = 0; i < m_blocked.GetCount(); ++i )
  {
    grape_event_remove_blocked event = m_blocked.Item( i );
    event.Do();
  }
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

  // Perform remove event Undo for visible and blocked.
  for ( unsigned int i = 0; i < m_visible.GetCount(); ++i )
  {
    grape_event_remove_visible event = m_visible.Item( i );
    event.Undo();
  }
  for ( unsigned int i = 0; i < m_blocked.GetCount(); ++i )
  {
    grape_event_remove_blocked event = m_blocked.Item( i );
    event.Undo();
  }
  for ( unsigned int i = 0; i < m_channel_communication.GetCount(); ++i )
  {
    grape_event_detach_channel_communication event = m_channel_communication.Item( i );
    event.Undo();
  }

  finish_modification();
  return true;
}


grape_event_change_channel::grape_event_change_channel( grape_frame *p_main_frame, channel* p_channel )
: grape_event_base( p_main_frame, true, _T( "change channel name" ) )
{
  m_channel = p_channel->get_id();
  m_old_text = p_channel->get_name();

  grape_text_dlg dialog( _T("Change channel name"), _T("Give the new action for the channel."), m_old_text, false /* no multiline */ );

  m_pressed_ok = true;
  if ( !dialog.show_modal( m_new_text ) ) // Note Diana: show_modal returns an integer as far as I know, cannot be assigned to m_pressed_ok as boolean.
  {
    // user cancelled
    m_pressed_ok = false;
  }  
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
  channel_ptr->set_name( m_new_text );

  finish_modification();
  return true;
}

bool grape_event_change_channel::Undo( void )
{
  channel* channel_ptr = static_cast<channel*> ( find_object( m_channel, CHANNEL ) );
  channel_ptr->set_name( m_old_text );

  finish_modification();
  return true;
}


grape_event_detach_channel::grape_event_detach_channel( grape_frame *p_main_frame, channel* p_channel )
: grape_event_base( p_main_frame, false, _T( "detach channel" ) )
{
  m_channel = p_channel;
}

grape_event_detach_channel::~grape_event_detach_channel( void )
{
}

bool grape_event_detach_channel::Do( void )
{
  connection_property* prop_ptr = m_channel->get_property();
  if ( prop_ptr )
  {
    grape_event_detach_property* event = new grape_event_detach_property( m_main_frame, prop_ptr );
    m_main_frame->get_event_handler()->Submit( event, true );
    return true;
  }
  else
  {
    channel_communication* comm_ptr = m_channel->get_channel_communication();
    if ( comm_ptr )
    {
      grape_event_detach_channel_communication* event = new grape_event_detach_channel_communication( m_main_frame, comm_ptr, m_channel );
      m_main_frame->get_event_handler()->Submit( event, true );
      return true;
    }
  }
  return true;
}

bool grape_event_detach_channel::Undo( void )
{
  // cannot be undone
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_channel );
