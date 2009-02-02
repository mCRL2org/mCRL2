// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_state.cpp
//
// Defines GraPE events for states

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "dialogs/textdialog.h"

#include "event_initialdesignator.h"
#include "event_nonterminatingtransition.h"
#include "event_terminatingtransition.h"

#include "event_state.h"

const float DEFAULT_STATE_WIDTH = 0.3f;
const float DEFAULT_STATE_HEIGHT = 0.2f;

using namespace grape::grapeapp;

grape_event_add_state::grape_event_add_state( grape_frame *p_main_frame, coordinate &p_coord )
: grape_event_base( p_main_frame, true, _T( "add state" ) )
{
  m_state = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_def_state_width = DEFAULT_STATE_WIDTH;
  m_def_state_height = DEFAULT_STATE_HEIGHT;

  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();

  assert( dynamic_cast<process_diagram*> ( dia_ptr ) != 0 );// The diagram has to be of the casted type, or else this event could not have been generated.
}

grape_event_add_state::~grape_event_add_state( void )
{
}

bool grape_event_add_state::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Has to be the case or the event wouldn't have been generated.
  state *state_ptr = dia_ptr->add_state( m_state, m_coord, m_def_state_width, m_def_state_height );
  state_ptr->set_width( visual_object::get_width_hint( state_ptr->get_name() ) );

  finish_modification();
  return true;
}

bool grape_event_add_state::Undo( void )
{
  // find the diagram the state was added to
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the state that is to be removed
  state* state_ptr = static_cast<state*> ( find_object( m_state, STATE, dia_ptr->get_id() ) );
  dia_ptr->remove_state( state_ptr );

  finish_modification();
  return true;
}

grape_event_remove_state::grape_event_remove_state( grape_frame *p_main_frame, state* p_state, process_diagram* p_proc_dia_ptr, bool p_normal )
: grape_event_base( p_main_frame, true, _T( "remove state" ) )
{
  m_normal = p_normal;
  m_state = p_state->get_id();
  m_name = p_state->get_name();
  m_coordinate = p_state->get_coordinate();
  m_width = p_state->get_width();
  m_height = p_state->get_height();

  m_comments.Empty();
  for ( unsigned int i = 0; i < p_state->count_comment(); ++i )
  {
    comment* comm_ptr = p_state->get_comment( i );
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
    int count = p_state->count_transition_beginstate();
    for (int i = 0; i < count; ++i)
    {
      transition* trans_ptr = p_state->get_transition_beginstate( i );
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
    count = p_state->count_transition_endstate();
    for ( int i = 0; i < count; ++i )
    {
      nonterminating_transition* ntt_ptr = p_state->get_transition_endstate( i );
      grape_event_remove_nonterminating_transition* event = new grape_event_remove_nonterminating_transition( m_main_frame, ntt_ptr, p_proc_dia_ptr );
      m_nonterminating_transitions_endstate.Add( event );
    }

    // Backup initial designators that designate this state.
    count = p_state->count_initial_designator();
    for ( int i = 0; i < count; ++i )
    {
      initial_designator* init_ptr = p_state->get_initial_designator( i );
      grape_event_remove_initial_designator* event = new grape_event_remove_initial_designator( m_main_frame, init_ptr, p_proc_dia_ptr );
      m_initial_designators.Add( event );
    }
  }
}

grape_event_remove_state::~grape_event_remove_state( void )
{
  m_comments.Clear();

  m_terminating_transitions.Clear();
  m_nonterminating_transitions_beginstate.Clear();
  m_nonterminating_transitions_endstate.Clear();
  m_initial_designators.Clear();
}

bool grape_event_remove_state::Do( void )
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
  state* state_ptr = dynamic_cast<state*> ( find_object( m_state, STATE, dia_ptr->get_id() ) );
  assert( state_ptr != 0 );

  dia_ptr->remove_state( state_ptr );

  finish_modification();
  return true;
}

bool grape_event_remove_state::Undo( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  state* new_state = dia_ptr->add_state( m_state, m_coordinate, m_width, m_height );
  new_state->set_name( m_name );
  // Restore comment connections.
  for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
  {
    unsigned int identifier = m_comments.Item( i );
    comment* comm_ptr = static_cast<comment*> ( find_object( identifier, COMMENT, dia_ptr->get_id() ) );
    dia_ptr->attach_comment_to_object( comm_ptr, new_state );
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

grape_event_change_state::grape_event_change_state( grape_frame *p_main_frame, state* p_state )
: grape_event_base( p_main_frame, true, _T( "edit state name" ) )
{
  m_state = p_state->get_id();
  m_old_text = p_state->get_name();

  grape_text_dlg dialog( _T("Change state name"), _T("Give the new state name."), m_old_text, false /* no multiline */ );

  m_pressed_ok = dialog.show_modal( m_new_text );
}

grape_event_change_state::~grape_event_change_state( void )
{
}

bool grape_event_change_state::Do( void )
{
  if ( !m_pressed_ok )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  state* state_ptr = static_cast<state*> ( find_object( m_state, STATE ) );
  state_ptr->set_name( m_new_text );

  finish_modification();
  return true;
}

bool grape_event_change_state::Undo( void )
{
  state* state_ptr = static_cast<state*> ( find_object( m_state, STATE ) );
  state_ptr->set_name( m_old_text );

  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_state );
