// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_referencestate.cpp
//
// Defines GraPE events for reference states.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "dialogs/referencedialog.h"

#include "event_referencestate.h"

const float DEFAULT_REFERENCE_STATE_WIDTH = 0.3f;
const float DEFAULT_REFERENCE_STATE_HEIGHT = 0.3f;

using namespace grape::grapeapp;

grape_event_add_reference_state::grape_event_add_reference_state( grape_frame *p_main_frame, coordinate &p_coord )
: grape_event_base( p_main_frame, true, _T( "add reference state" ) )
{
  m_ref_state = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_def_ref_state_width = DEFAULT_REFERENCE_STATE_WIDTH;
  m_def_ref_state_height = DEFAULT_REFERENCE_STATE_HEIGHT;

  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();

  	assert( dynamic_cast<process_diagram*> ( dia_ptr ) != 0 );// The diagram has to be of the casted type, or else this event could not have been generated.
}

grape_event_add_reference_state::~grape_event_add_reference_state( void )
{
}

bool grape_event_add_reference_state::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Has to be the case or the event wouldn't have been generated.
  dia_ptr->add_reference_state( m_ref_state, m_coord, m_def_ref_state_width, m_def_ref_state_height );

  finish_modification();
  return true;
}

bool grape_event_add_reference_state::Undo( void )
{
  // find the diagram the state was added to
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the state that is to be removed
  reference_state* ref_state_ptr = static_cast<reference_state*> ( find_object( m_ref_state, REFERENCE_STATE, dia_ptr->get_id() ) );
  dia_ptr->remove_reference_state( ref_state_ptr );

  finish_modification();
  return true;
}

grape_event_remove_reference_state::grape_event_remove_reference_state( grape_frame *p_main_frame, reference_state* p_ref_state, process_diagram* p_proc_dia_ptr, bool p_normal )
: grape_event_base( p_main_frame, true, _T( "remove reference state" ) )
{
  m_normal = p_normal;
  m_ref_state = p_ref_state->get_id();
  m_name = p_ref_state->get_name();
  process_diagram* proc_dia = p_ref_state->get_relationship_refers_to();
  m_property_of = -1;
  if (proc_dia != 0)
  {
    m_property_of = proc_dia->get_id();
  }
  m_coordinate = p_ref_state->get_coordinate();
  m_width = p_ref_state->get_width();
  m_height = p_ref_state->get_height();

  m_comments.Empty();
  for ( unsigned int i = 0; i < p_ref_state->count_comment(); ++i )
  {
    comment* comm_ptr = p_ref_state->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_proc_dia_ptr->get_id();

  m_terminating_transitions.Empty();
  m_nonterminating_transitions_beginstate.Empty();
  m_nonterminating_transitions_endstate.Empty();
  m_initial_designators.Empty();
  if ( p_normal )
  {
    // Create remove event for transitions that have the state as beginstate.
    int count = p_ref_state->count_transition_beginstate();
    for (int i = 0; i < count; ++i)
    {
      transition* trans_ptr = p_ref_state->get_transition_beginstate( i );
      nonterminating_transition* ntt_ptr = dynamic_cast<nonterminating_transition*> ( trans_ptr );
      if ( ntt_ptr != 0 ) // Cast succesful
      {
        grape_event_remove_nonterminating_transition* event = new grape_event_remove_nonterminating_transition( m_main_frame, ntt_ptr, p_proc_dia_ptr );
        m_nonterminating_transitions_beginstate.Add( event );
      }
      else
      {
        terminating_transition* tt_ptr = dynamic_cast<terminating_transition*> ( trans_ptr );
        assert( tt_ptr != 0 ); // Cast should be succesful (mutual exclusion)
        grape_event_remove_terminating_transition* event = new grape_event_remove_terminating_transition( m_main_frame, tt_ptr, p_proc_dia_ptr );
        m_terminating_transitions.Add( event );
      }
    } // end for

    // Create remove event for nonterminating transitions that have the state as endstate.
    count = p_ref_state->count_transition_endstate();
    for ( int i = 0; i < count; ++i )
    {
      nonterminating_transition* ntt_ptr = p_ref_state->get_transition_endstate( i );
      grape_event_remove_nonterminating_transition* event = new grape_event_remove_nonterminating_transition( m_main_frame, ntt_ptr, p_proc_dia_ptr );
      m_nonterminating_transitions_endstate.Add( event );
    }

    // Backup initial designators that designate this state.
    count = p_ref_state->count_initial_designator();
    for ( int i = 0; i < count; ++i )
    {
      initial_designator* init_ptr = p_ref_state->get_initial_designator( i );
      grape_event_remove_initial_designator* event = new grape_event_remove_initial_designator( m_main_frame, init_ptr, p_proc_dia_ptr );
      m_initial_designators.Add( event );
    }
  }

  m_parameter_assignments = p_ref_state->get_parameter_updates();
}

grape_event_remove_reference_state::~grape_event_remove_reference_state( void )
{
  m_parameter_assignments.Clear();
  m_comments.Clear();

  m_terminating_transitions.Clear();
  m_nonterminating_transitions_beginstate.Clear();
  m_nonterminating_transitions_endstate.Clear();
  m_initial_designators.Clear();
}

bool grape_event_remove_reference_state::Do( void )
{
  if ( m_normal )
    {
    // Perform remove event Do for transitions and initial designators
    for ( unsigned int i = 0; i < m_terminating_transitions.GetCount(); ++i )
    {
      grape_event_remove_terminating_transition event = m_terminating_transitions.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_nonterminating_transitions_beginstate.GetCount(); ++i )
    {
      grape_event_remove_nonterminating_transition event = m_nonterminating_transitions_beginstate.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_nonterminating_transitions_endstate.GetCount(); ++i )
    {
      grape_event_remove_nonterminating_transition event = m_nonterminating_transitions_endstate.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_initial_designators.GetCount(); ++i )
    {
      grape_event_remove_initial_designator event = m_initial_designators.Item( i );
      event.Do();
    }
  }

  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  reference_state* ref_state_ptr = dynamic_cast<reference_state*> ( find_object( m_ref_state, REFERENCE_STATE, dia_ptr->get_id() ) );
  assert( ref_state_ptr != 0 );

  dia_ptr->remove_reference_state( ref_state_ptr );

  finish_modification();
  return true;
}

bool grape_event_remove_reference_state::Undo( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  reference_state* new_ref_state = dia_ptr->add_reference_state( m_ref_state, m_coordinate, m_width, m_height );
  new_ref_state->set_name( m_name );
  process_diagram* proc_dia = dynamic_cast<process_diagram*> ( find_diagram( m_property_of ) );
  new_ref_state->set_relationship_refers_to( proc_dia );
  // Restore parameter assignments
  new_ref_state->set_parameter_updates( m_parameter_assignments );

  // Restore comment connections.
  for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
  {
    unsigned int identifier = m_comments.Item( i );
    comment* comm_ptr = static_cast<comment*> ( find_object( identifier, COMMENT, dia_ptr->get_id() ) );
    dia_ptr->attach_comment_to_object( comm_ptr, new_ref_state );
  }

  if ( m_normal )
  {
    // Perform remove event Undo for transitions and initial designators.
    for ( unsigned int i = 0; i < m_terminating_transitions.GetCount(); ++i )
    {
      grape_event_remove_terminating_transition event = m_terminating_transitions.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_nonterminating_transitions_beginstate.GetCount(); ++i )
    {
      grape_event_remove_nonterminating_transition event = m_nonterminating_transitions_beginstate.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_nonterminating_transitions_endstate.GetCount(); ++i )
    {
      grape_event_remove_nonterminating_transition event = m_nonterminating_transitions_endstate.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_initial_designators.GetCount(); ++i )
    {
      grape_event_remove_initial_designator event = m_initial_designators.Item( i );
      event.Undo();
    }
  }

  finish_modification();
  return true;
}

grape_event_change_refstate::grape_event_change_refstate( grape_frame *p_main_frame, reference_state* p_ref_state )
: grape_event_base( p_main_frame, true, _T( "change process reference" ) ) // description != bug
{
  m_ref_state_id = p_ref_state->get_id();
  m_old_proc_name = p_ref_state->get_name();

  if ( p_ref_state->get_relationship_refers_to() )
  {
    m_old_proc_id = p_ref_state->get_relationship_refers_to()->get_id();
  }
  else
  {
    m_old_proc_id = -1;
  }
  m_old_text = p_ref_state->get_text();

  grape_reference_dialog dialog( p_main_frame, p_ref_state, m_main_frame->get_grape_specification() );

  m_ok_pressed = dialog.show_modal();
  if ( m_ok_pressed )
  {
    m_new_proc_id = dialog.get_diagram_id();
    m_new_proc_name = dialog.get_diagram_name();
    m_new_text = dialog.get_initializations();
  }
}

grape_event_change_refstate::~grape_event_change_refstate( void )
{
}

bool grape_event_change_refstate::Do( void )
{
  if ( !m_ok_pressed )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  process_diagram *diagram_ptr = static_cast<process_diagram*>(find_diagram( m_new_proc_id ) );
  reference_state *refstate = static_cast<reference_state*>(find_object( m_ref_state_id, REFERENCE_STATE ) );

  refstate->set_relationship_refers_to( diagram_ptr );
  refstate->set_name( m_new_proc_name );
  refstate->set_text( m_new_text );

  finish_modification();
  return true;
}

bool grape_event_change_refstate::Undo( void )
{
  process_diagram *diagram_ptr = static_cast<process_diagram*>(find_diagram( m_old_proc_id ));
  reference_state *refstate = static_cast<reference_state*>(find_object( m_ref_state_id, REFERENCE_STATE ) );

  refstate->set_relationship_refers_to( diagram_ptr );
  refstate->set_name( m_old_proc_name );
  refstate->set_text( m_old_text );

  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_ref_state )
