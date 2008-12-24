// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_blocked.cpp
//
// Defines GraPE events for blocked property.

#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "event_channelcommunication.h"
#include "event_visible.h"

#include "event_blocked.h"

const float DEFAULT_BLOCKED_WIDTH = 0.1f;
const float DEFAULT_BLOCKED_HEIGHT = 0.1f;

using namespace grape::grapeapp;

grape_event_add_blocked::grape_event_add_blocked( grape_frame *p_main_frame, coordinate &p_coord, connection* p_conn )
: grape_event_base( p_main_frame, true, _T( "add blocked" ) )
{
  m_block = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_conn = p_conn->get_id();
  m_def_block_width = DEFAULT_BLOCKED_WIDTH;
  m_def_block_height = DEFAULT_BLOCKED_HEIGHT;

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

grape_event_add_blocked::~grape_event_add_blocked( void )
{
}

bool grape_event_add_blocked::Do( void )
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
  dia_ptr->add_blocked( m_block, m_coord, m_def_block_width, m_def_block_height, conn );

  finish_modification();
  return true;
}

bool grape_event_add_blocked::Undo( void )
{
  // find the diagram the blocked was added to
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the blocked that is to be removed
  blocked* block_ptr = static_cast<blocked*> ( find_object( m_block, BLOCKED, dia_ptr->get_id() ) );
  dia_ptr->remove_blocked( block_ptr );

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

grape_event_remove_blocked::grape_event_remove_blocked( grape_frame *p_main_frame, blocked* p_block, architecture_diagram* p_arch_dia_ptr )
: grape_event_base( p_main_frame, true, _T( "remove blocked" ) )
{
  m_block = p_block->get_id();
  connection* conn = p_block->get_attached_connection();
  if ( conn != 0 )
  {
    m_connection = conn->get_id();
  }
  else
  {
    m_connection = -1;
  }
  m_coordinate = p_block->get_coordinate();
  m_width = p_block->get_width();
  m_height = p_block->get_height();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_block->count_comment(); ++i )
  {
    comment* comm_ptr = p_block->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_arch_dia_ptr->get_id();
}

grape_event_remove_blocked::~grape_event_remove_blocked(  void  )
{
  m_comments.Clear();
}

bool grape_event_remove_blocked::Do(  void  )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  blocked* block_ptr = dynamic_cast<blocked*> ( find_object( m_block, BLOCKED, dia_ptr->get_id() ) );
  assert( block_ptr != 0 );

  dia_ptr->remove_blocked( block_ptr );

  finish_modification();
  return true;
}

bool grape_event_remove_blocked::Undo(  void  )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );

  object* obj = find_object( m_connection );
  if ( obj != 0 )
  {
    connection* conn = dynamic_cast<connection*> ( obj );
    dia_ptr->add_blocked( m_block, m_coordinate, m_width, m_height, conn );
  }
  else
  {
    dia_ptr->add_blocked( m_block, m_coordinate, m_width, m_height, 0 );
  }

  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_blocked );
