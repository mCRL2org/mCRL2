		// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_property.cpp
//
// Defines GraPE events for properties

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "event_property.h"

using namespace grape::grapeapp;

grape_event_attach_property::grape_event_attach_property( grape_frame *p_main_frame, connection_property* p_prop, connection* p_conn )
: grape_event_base( p_main_frame, true, _T( "attach property" ) )
{
  m_property = p_prop->get_id();
  m_connection = p_conn->get_id();
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  m_diagram = dia_ptr->get_id();

  assert( ( p_prop->get_diagram() == dia_ptr ) && ( p_conn->get_diagram() == dia_ptr ) ); // The objects have to be in the same diagram.

  m_detach_prop = 0;
  connection_property* prop_ptr = p_conn->get_property();
  if ( prop_ptr != 0 )
  {
    m_detach_prop = new grape_event_detach_property( m_main_frame, p_prop );
  }

  m_channel_communication = 0;
  channel* chan_ptr = dynamic_cast<channel*> ( p_conn );
  if ( chan_ptr )
  {
    channel_communication* comm_ptr = chan_ptr->get_channel_communication();
    if ( comm_ptr )
    {
      m_channel_communication = new grape_event_detach_channel_communication( m_main_frame, comm_ptr, chan_ptr );
    }
  }
}

grape_event_attach_property::~grape_event_attach_property(  void  )
{
}

bool grape_event_attach_property::Do(  void  )
{
  if ( m_detach_prop )
  {
    m_detach_prop->Do();
  }
  if ( m_channel_communication  )
  {
    m_channel_communication->Do();
  }

  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // The diagram has to exist and be of the specified type or this event could not be generated.

  connection_property* prop = static_cast<connection_property*> ( find_object( m_property ) );
  object* obj_ptr = find_object( m_connection );
  channel* chan_ptr = dynamic_cast<channel*> ( obj_ptr );
  if ( chan_ptr != 0 )
  {
    dia_ptr->attach_property_to_connection( prop, chan_ptr );
  }
  else
  {
    channel_communication* c_comm_ptr = dynamic_cast<channel_communication*> ( obj_ptr );
    dia_ptr->attach_property_to_connection( prop, c_comm_ptr );
  }

  finish_modification();
  return true;
}

bool grape_event_attach_property::Undo(  void  )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // The diagram has to exist and be of the specified type or this event could not be generated.
  connection_property* prop = static_cast<connection_property*> ( find_object( m_property ) );
  dia_ptr->detach_property_from_connection( prop );

  if ( m_detach_prop != 0 )
  {
    m_detach_prop->Do();
  }

  finish_modification();
  return true;
}

grape_event_detach_property::grape_event_detach_property( grape_frame *p_main_frame, connection_property* p_prop )
: grape_event_base( p_main_frame, true, _T( "detach property" ) )
{
  m_property = p_prop->get_id();
  connection* conn_ptr = p_prop->get_attached_connection();
  if ( conn_ptr )
  {
    m_connection = conn_ptr->get_id();
  }
  else
  {
    m_connection = -1;
  }
  m_diagram = m_main_frame->get_glcanvas()->get_diagram()->get_id();
}

grape_event_detach_property::~grape_event_detach_property(  void  )
{
}

bool grape_event_detach_property::Do(  void  )
{
  if ( m_connection == -1 )
  {
    return false;
  }
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( m_main_frame->get_glcanvas()->get_diagram() );
  assert( dia_ptr != 0 ); // The diagram has to exist and be of the specified type or this event could not be generated.
  connection_property* prop = static_cast<connection_property*> ( find_object( m_property ) );
  dia_ptr->detach_property_from_connection( prop );

  finish_modification();
  return true;
}

bool grape_event_detach_property::Undo(  void  )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // The diagram has to exist and be of the specified type or this event could not be generated.

  connection_property* prop = static_cast<connection_property*> ( find_object( m_property ) );
  object* obj_ptr = find_object( m_connection );
  channel* chan_ptr = dynamic_cast<channel*> ( obj_ptr );
  if ( chan_ptr != 0 )
  {
    dia_ptr->attach_property_to_connection( prop, chan_ptr );
  }
  else
  {
    channel_communication* c_comm_ptr = dynamic_cast<channel_communication*> ( obj_ptr );
    dia_ptr->attach_property_to_connection( prop, c_comm_ptr );
  }

  finish_modification();
  return true;
}
