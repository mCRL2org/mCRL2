// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_nonterminatingtransition.h
//
// Defines GraPE events for nonterminating transitions.

#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "dialogs/labeldialog.h"
#include "event_transition.h"
#include "grape_events.h"


using namespace grape::grapeapp;

grape_event_attach_transition_beginstate::grape_event_attach_transition_beginstate( grape_frame *p_main_frame, transition* p_trans, compound_state* p_state )
: grape_event_base( p_main_frame, true, _T( "attach transition to beginstate" ) )
{
  m_trans = p_trans->get_id();
  m_state = p_state->get_id();
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_diagram = dia_ptr->get_id();
}

grape_event_attach_transition_beginstate::~grape_event_attach_transition_beginstate( void )
{
}

bool grape_event_attach_transition_beginstate::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  object* trans_ptr = find_object( m_trans );
  terminating_transition* tt_ptr = dynamic_cast<terminating_transition*> ( trans_ptr );
  if ( tt_ptr ) // cast succesful
  {
    object* compound_state_ptr = find_object( m_state );
    state* state_ptr = dynamic_cast<state*> ( compound_state_ptr );
    if ( state_ptr ) // cast succesful
    {
      dia_ptr->attach_transition_beginstate( tt_ptr, state_ptr );
    }
    else
    {
      reference_state* ref_state_ptr = dynamic_cast<reference_state*> ( compound_state_ptr );
      assert( ref_state_ptr );
      dia_ptr->attach_transition_beginstate( tt_ptr, ref_state_ptr );
    }
    finish_modification();
  }
  else
  {
    nonterminating_transition* ntt_ptr = dynamic_cast<nonterminating_transition*> ( trans_ptr );
    assert( ntt_ptr );
    object* compound_state_ptr = find_object( m_state );
    state* state_ptr = dynamic_cast<state*> ( compound_state_ptr );
    if ( state_ptr ) // cast succesful
    {
      dia_ptr->attach_transition_beginstate( ntt_ptr, state_ptr );
    }
    else
    {
      reference_state* ref_state_ptr = dynamic_cast<reference_state*> ( compound_state_ptr );
      assert( ref_state_ptr );
      dia_ptr->attach_transition_beginstate( ntt_ptr, ref_state_ptr );
    }

    finish_modification();
  }

  return true;
}

bool grape_event_attach_transition_beginstate::Undo( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  object* trans_ptr = find_object( m_trans );
  terminating_transition* tt_ptr = dynamic_cast<terminating_transition*> ( trans_ptr );
  if ( tt_ptr ) // cast succesful
  {
    dia_ptr->detach_transition_beginstate( tt_ptr );
  }
  else
  {
    nonterminating_transition* ntt_ptr = dynamic_cast<nonterminating_transition*> ( trans_ptr );
    assert( ntt_ptr );
    dia_ptr->detach_transition_beginstate( ntt_ptr );
  }

  finish_modification();
  return true;
}

grape_event_detach_transition_beginstate::grape_event_detach_transition_beginstate( grape_frame *p_main_frame, transition* p_trans )
: grape_event_base( p_main_frame, true, _T( "detach transition from beginstate" ) )
{
  m_trans = p_trans->get_id();
  compound_state* state_ptr = p_trans->get_beginstate();
  m_state = state_ptr ? state_ptr->get_id() : -1;
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_diagram = dia_ptr->get_id();
}

grape_event_detach_transition_beginstate::~grape_event_detach_transition_beginstate( void )
{
}

bool grape_event_detach_transition_beginstate::Do( void )
{
  if ( m_state == -1 )
  {
    return false;
  }

  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.
    
  object* trans_ptr = find_object( m_trans );
  terminating_transition* tt_ptr = dynamic_cast<terminating_transition*> ( trans_ptr );
  if ( tt_ptr ) // cast succesful
  {
    dia_ptr->detach_transition_beginstate( tt_ptr );  
  }
  else
  {
    nonterminating_transition* ntt_ptr = dynamic_cast<nonterminating_transition*> ( trans_ptr );
    assert( ntt_ptr );    
    dia_ptr->detach_transition_beginstate( ntt_ptr );
  }
  finish_modification();
    
  return true;
}

bool grape_event_detach_transition_beginstate::Undo( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  object* trans_ptr = find_object( m_trans );
  terminating_transition* tt_ptr = dynamic_cast<terminating_transition*> ( trans_ptr );
  if ( tt_ptr ) // cast succesful
  {
    object* compound_state_ptr = find_object( m_state );
    state* state_ptr = dynamic_cast<state*> ( compound_state_ptr );
    if ( state_ptr ) // cast succesful
    {
      dia_ptr->attach_transition_beginstate( tt_ptr, state_ptr );
    }
    else
    {
      reference_state* ref_state_ptr = dynamic_cast<reference_state*> ( compound_state_ptr );
      assert( ref_state_ptr );
      dia_ptr->attach_transition_beginstate( tt_ptr, ref_state_ptr );
    }
  }
  else
  {
    nonterminating_transition* ntt_ptr = dynamic_cast<nonterminating_transition*> ( trans_ptr );
    assert( ntt_ptr );
    object* compound_state_ptr = find_object( m_state );
    state* state_ptr = dynamic_cast<state*> ( compound_state_ptr );
    if ( state_ptr ) // cast succesful
    {
      dia_ptr->attach_transition_beginstate( ntt_ptr, state_ptr );
    }
    else
    {
      reference_state* ref_state_ptr = dynamic_cast<reference_state*> ( compound_state_ptr );
      assert( ref_state_ptr );
      dia_ptr->attach_transition_beginstate( ntt_ptr, ref_state_ptr );
    }
  }
  finish_modification();
  return true;
}

grape_event_change_transition::grape_event_change_transition(grape_frame *p_main_frame, transition *p_transition)
: grape_event_base( p_main_frame, true, _T( "edit transition label" ) )
{
  m_trans = p_transition->get_id();

  m_old_text = p_transition->get_label()->get_text();
  
 
  //grape_text_dlg dialog( _T("Change transition label"), _T("Enter the new transition label."), m_old_text, false /* no multiline */ );
  grape_label_dialog dialog( m_old_text );
  
  m_pressed_ok = dialog.show_modal( m_new_text );
}

grape_event_change_transition::~grape_event_change_transition(void)
{
}

bool grape_event_change_transition::Do(void)
{
  if ( !m_pressed_ok )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  transition* transition_ptr = static_cast<transition*> ( find_object( m_trans, NONTERMINATING_TRANSITION ) );
  if(transition_ptr == 0)
  {
    transition_ptr = static_cast<transition*>(find_object(m_trans, TERMINATING_TRANSITION));
  }
  transition_ptr->get_label()->set_text( m_new_text );

  finish_modification();
  return true;
}

bool grape_event_change_transition::Undo(void)
{
  transition* transition_ptr = static_cast<transition*> ( find_object( m_trans, NONTERMINATING_TRANSITION ) );
  if(transition_ptr == 0)
  {
    transition_ptr = static_cast<transition*>(find_object(m_trans, TERMINATING_TRANSITION));
  }
  transition_ptr->get_label()->set_text( m_old_text );
 
  finish_modification();
  return true;
}

