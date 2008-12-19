// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_initialdesignator.cpp
//
// Defines GraPE events for the initial designator

#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "event_initialdesignator.h"

const float DEFAULT_INITIAL_DESIGNATOR_WIDTH = 0.2f;
const float DEFAULT_INITIAL_DESIGNATOR_HEIGHT = 0.2f;

using namespace grape::grapeapp;

grape_event_add_initial_designator::grape_event_add_initial_designator( grape_frame *p_main_frame, compound_state* p_state, coordinate &p_coord )
: grape_event_base( p_main_frame, true, _T( "add initial designator" ) )
{
  m_init = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_state = p_state->get_id();
  m_def_init_width = DEFAULT_INITIAL_DESIGNATOR_WIDTH;
  m_def_init_height = DEFAULT_INITIAL_DESIGNATOR_HEIGHT;
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();

  assert( dynamic_cast<process_diagram*> ( p_state->get_diagram() ) != 0 );
  assert( ( dynamic_cast<process_diagram*> ( p_state->get_diagram() ) == dynamic_cast<process_diagram*> ( dia_ptr ) ) ); // Both have to be in the same diagram.
}

grape_event_add_initial_designator::~grape_event_add_initial_designator( void )
{
}

bool grape_event_add_initial_designator::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.
  compound_state* state_ptr = static_cast<compound_state*> ( find_object( m_state ) );
  dia_ptr->add_initial_designator( m_init, state_ptr, m_def_init_width, m_def_init_height, m_coord );

  finish_modification();
  return true;
}

bool grape_event_add_initial_designator::Undo( void )
{
  // find the diagram the initial designator was added to
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the initial designator that is to be removed
  initial_designator* init_ptr = static_cast<initial_designator*> ( find_object( m_init, INITIAL_DESIGNATOR, dia_ptr->get_id() ) );
  dia_ptr->remove_initial_designator( init_ptr );

  finish_modification();
  return true;
}

grape_event_remove_initial_designator::grape_event_remove_initial_designator( grape_frame *p_main_frame, initial_designator* p_init, process_diagram* p_proc_dia_ptr )
: grape_event_base( p_main_frame, true, _T( "remove initial designator" ) )
{
  m_init = p_init->get_id();
  m_coord = p_init->get_coordinate();
  compound_state* attached_state = p_init->get_attached_state();
  if ( attached_state != 0 )
  {
    m_designates = attached_state->get_id();
  }
  else
  {
    m_designates = -1;
  }
  m_coordinate = p_init->get_coordinate();
  m_width = p_init->get_width();
  m_height = p_init->get_height();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_init->count_comment(); ++i )
  {
    comment* comm_ptr = p_init->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_proc_dia_ptr->get_id();
}

grape_event_remove_initial_designator::~grape_event_remove_initial_designator( void )
{
  m_comments.Clear();
}

bool grape_event_remove_initial_designator::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  initial_designator* init_ptr = static_cast<initial_designator*> ( find_object( m_init, INITIAL_DESIGNATOR ) );
  dia_ptr->remove_initial_designator( init_ptr );

  finish_modification();
  return true;
}

bool grape_event_remove_initial_designator::Undo( void )
{
  // find the diagram the initial designator was removed from
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0); // Should be the case or something went wrong..

  compound_state* designated = static_cast<compound_state*> ( find_object( m_designates ) );
  initial_designator* new_init = dia_ptr->add_initial_designator( m_init, designated, m_width, m_height, m_coord );
  new_init->set_coordinate( m_coordinate );
  for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
  {
    unsigned int identifier = m_comments.Item( i );
    comment* comm_ptr = static_cast<comment*> ( find_object( identifier, COMMENT, dia_ptr->get_id() ) );
    dia_ptr->attach_comment_to_object( comm_ptr, new_init );
  }

  finish_modification();
  return true;
}

grape_event_attach_initial_designator::grape_event_attach_initial_designator( grape_frame *p_main_frame, initial_designator* p_init, compound_state* p_state )
: grape_event_base( p_main_frame, true, _T( "attach initial designator" ) )
{
  m_init = p_init->get_id();
  m_state = p_state->get_id();
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_diagram = dia_ptr->get_id();
}

grape_event_attach_initial_designator::~grape_event_attach_initial_designator( void )
{
}

bool grape_event_attach_initial_designator::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  initial_designator* init_ptr = static_cast<initial_designator*> ( find_object( m_init, INITIAL_DESIGNATOR ) );
  object* obj_ptr = find_object( m_state );
  state* state_ptr = dynamic_cast<state*> ( obj_ptr );
  if ( state_ptr )
  {
    dia_ptr->attach_initial_designator( init_ptr, state_ptr );
  }
  else
  {
    reference_state* ref_state_ptr = dynamic_cast<reference_state*> ( obj_ptr );
    assert( ref_state_ptr );
    dia_ptr->attach_initial_designator( init_ptr, ref_state_ptr );
  }

  finish_modification();
  return true;
}

bool grape_event_attach_initial_designator::Undo( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  initial_designator* init_ptr = static_cast<initial_designator*> ( find_object( m_init, INITIAL_DESIGNATOR ) );
  coordinate init_coord = init_ptr->get_coordinate();
  dia_ptr->detach_initial_designator( init_ptr, init_coord );

  finish_modification();
  return true;
}

grape_event_detach_initial_designator::grape_event_detach_initial_designator( grape_frame *p_main_frame, initial_designator* p_init )
: grape_event_base( p_main_frame, true, _T( "detach initial designator" ) )
{
  m_init = p_init->get_id();
  compound_state* state_ptr = p_init->get_attached_state();
  m_state = state_ptr ? state_ptr->get_id() : -1; 
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_diagram = dia_ptr->get_id();
}

grape_event_detach_initial_designator::~grape_event_detach_initial_designator( void )
{
}

bool grape_event_detach_initial_designator::Do( void )
{
  if ( m_state == -1 )
  {
    return false; //initial designator cannot be detached because it isn't attached
  }

  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  initial_designator* init_ptr = static_cast<initial_designator*> ( find_object( m_init, INITIAL_DESIGNATOR ) );
  coordinate init_coord = init_ptr->get_coordinate();
  dia_ptr->detach_initial_designator( init_ptr, init_coord );

  finish_modification();
  return true;
}

bool grape_event_detach_initial_designator::Undo( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  initial_designator* init_ptr = static_cast<initial_designator*> ( find_object( m_init, INITIAL_DESIGNATOR ) );
  object* obj_ptr = find_object( m_state );
  state* state_ptr = dynamic_cast<state*> ( obj_ptr );
  if ( state_ptr )
  {
    dia_ptr->attach_initial_designator( init_ptr, state_ptr );
  }
  else
  {
    reference_state* ref_state_ptr = dynamic_cast<reference_state*> ( obj_ptr );
    assert( ref_state_ptr );
    dia_ptr->attach_initial_designator( init_ptr, ref_state_ptr );
  }

  finish_modification();
  return true;
}

//void process_diagram::detach_initial_designator( initial_designator* p_init, coordinate &p_coord )

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_init );
