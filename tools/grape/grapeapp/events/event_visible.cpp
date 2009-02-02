// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_visible.cpp
//
// Defines GraPE events for visible property

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "dialogs/textdialog.h"

#include "event_blocked.h"
#include "event_channelcommunication.h"

#include "event_visible.h"

const float DEFAULT_VISIBLE_WIDTH = 0.05f;
const float DEFAULT_VISIBLE_HEIGHT = 0.05f;

using namespace grape::grapeapp;

grape_event_add_visible::grape_event_add_visible( grape_frame *p_main_frame, coordinate &p_coord, connection* p_conn )
: grape_event_base( p_main_frame, true, _T( "add visible" ) )
{
  m_vis = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_conn = p_conn->get_id();
  m_def_vis_width = DEFAULT_VISIBLE_WIDTH;
  m_def_vis_height = DEFAULT_VISIBLE_HEIGHT;

  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( m_main_frame->get_glcanvas()->get_diagram() );
  assert( dia_ptr != 0 );// The diagram has to exist and be of the specified type, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();

  m_removed_visible = 0;
  m_removed_blocked = 0;
  connection_property* prop_ptr = p_conn->get_property();
  if ( prop_ptr != 0 )
  {
    visible* vis_ptr = dynamic_cast<visible*> ( prop_ptr );
    if ( vis_ptr )
    {
      m_removed_visible = new grape_event_remove_visible( m_main_frame, vis_ptr, dia_ptr );
    }
    else
    {
      blocked* block_ptr = dynamic_cast<blocked*> ( prop_ptr );
      m_removed_blocked = new grape_event_remove_blocked( m_main_frame, block_ptr, dia_ptr );
    }
  }

  m_detached_chan_comm = 0;
  channel* chan_ptr = dynamic_cast<channel*> ( p_conn );
  if ( chan_ptr )
  {
    channel_communication* comm_ptr = chan_ptr->get_channel_communication();
    if ( comm_ptr ) m_detached_chan_comm = new grape_event_detach_channel_communication( m_main_frame, comm_ptr, chan_ptr );
  }
}

grape_event_add_visible::~grape_event_add_visible( void )
{
}

bool grape_event_add_visible::Do( void )
{
  if ( m_removed_visible )
  {
    m_removed_visible->Do();
  }
  if ( m_removed_blocked )
  {
    m_removed_blocked->Do();
  }
  if ( m_detached_chan_comm )
  {
    m_detached_chan_comm->Do();
  }

  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Has to be the case or the event wouldn't have been generated.

  connection* conn = dynamic_cast<connection*> ( find_object( m_conn ) );
  assert( conn != 0 );
  dia_ptr->add_visible( m_vis, m_coord, m_def_vis_width, m_def_vis_height, conn );

  finish_modification();
  return true;
}

bool grape_event_add_visible::Undo( void )
{
  // find the diagram the visible was added to
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the visible that is to be removed
  visible* vis_ptr = static_cast<visible*> ( find_object( m_vis, VISIBLE, dia_ptr->get_id() ) );
  dia_ptr->remove_visible( vis_ptr );

  if ( m_detached_chan_comm )
  {
    m_detached_chan_comm->Undo();
  }
  if ( m_removed_visible )
  {
    m_removed_visible->Undo();
  }
  if ( m_removed_blocked )
  {
    m_removed_blocked->Undo();
  }
  finish_modification();
  return true;
}

grape_event_remove_visible::grape_event_remove_visible( grape_frame *p_main_frame, visible* p_vis, architecture_diagram* p_arch_dia_ptr )
: grape_event_base( p_main_frame, true, _T( "remove visible" ) )
{
  m_vis = p_vis->get_id();
  m_name = p_vis->get_name();
  connection* conn = p_vis->get_attached_connection();
  if ( conn != 0 )
  {
    m_connection = conn->get_id();
  }
  else
  {
    m_connection = -1;
  }
  m_coordinate = p_vis->get_coordinate();
  m_width = p_vis->get_width();
  m_height = p_vis->get_height();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_vis->count_comment(); ++i )
  {
    comment* comm_ptr = p_vis->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_arch_dia_ptr->get_id();
}

grape_event_remove_visible::~grape_event_remove_visible(  void  )
{
  m_comments.Clear();
}

bool grape_event_remove_visible::Do(  void  )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  visible* vis_ptr = dynamic_cast<visible*> ( find_object( m_vis, VISIBLE, dia_ptr->get_id() ) );
  assert( vis_ptr != 0 );

  dia_ptr->remove_visible( vis_ptr );

  finish_modification();
  return true;
}

bool grape_event_remove_visible::Undo(  void  )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );

  object* obj = find_object( m_connection );
  if ( obj != 0 )
  {
    connection* conn = dynamic_cast<connection*> ( obj );
    visible* new_vis = dia_ptr->add_visible( m_vis, m_coordinate, m_width, m_height, conn );
    new_vis->set_name( m_name );
  }
  else
  {
    visible* new_vis = dia_ptr->add_visible( m_vis, m_coordinate, m_width, m_height, 0 );
    new_vis->set_name( m_name );
  }

  finish_modification();
  return true;
}


grape_event_change_visible::grape_event_change_visible( grape_frame *p_main_frame, visible* p_visible )
: grape_event_base( p_main_frame, true, _T( "change channel name" ) )
{
  m_visible = p_visible->get_id();
  m_old_text = p_visible->get_name();

  grape_text_dlg dialog( _T("Change visible name"), _T("Give the new name for the visible."), m_old_text, false /* no multiline */ );

  m_pressed_ok = true;
  if ( !dialog.show_modal( m_new_text ) ) // Note Diana: show_modal returns an integer as far as I know, cannot be assigned to m_pressed_ok as boolean.
  {
    // user cancelled
    m_pressed_ok = false;
  }  
}

grape_event_change_visible::~grape_event_change_visible( void )
{
}

bool grape_event_change_visible::Do( void )
{
  if ( !m_pressed_ok )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  visible* visible_ptr = static_cast<visible*> ( find_object( m_visible, VISIBLE ) );
  visible_ptr->set_name( m_new_text );

  finish_modification();
  return true;
}

bool grape_event_change_visible::Undo( void )
{
  visible* visible_ptr = static_cast<visible*> ( find_object( m_visible, VISIBLE ) );
  visible_ptr->set_name( m_old_text );

  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_visible );
