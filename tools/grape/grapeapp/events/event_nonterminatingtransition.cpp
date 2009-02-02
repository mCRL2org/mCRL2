// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_nonterminatingtransition.h
//
// Defines GraPE events for nonterminating transitions.

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "event_nonterminatingtransition.h"
#include "event_transition.h"

using namespace grape::grapeapp;

grape_event_add_nonterminating_transition::grape_event_add_nonterminating_transition( grape_frame *p_main_frame, compound_state* p_beginstate, compound_state* p_endstate )
: grape_event_base( p_main_frame, true, _T( "add transition" ) )
{
  m_ntt = m_main_frame->get_new_id();
  m_beginstate = p_beginstate->get_id();
  m_endstate = p_endstate->get_id();


  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();

  assert( dynamic_cast<process_diagram*> ( dia_ptr ) != 0 );// The diagram has to be of the casted type, or else this event could not have been generated.

  assert( dynamic_cast<process_diagram*> ( p_beginstate->get_diagram() ) != 0 );
  assert( ( dynamic_cast<process_diagram*> ( p_beginstate->get_diagram() ) == dynamic_cast<process_diagram*> ( dia_ptr ) ) && ( dynamic_cast<process_diagram*> ( p_endstate->get_diagram() ) == dynamic_cast<process_diagram*> ( dia_ptr ) ) ); // Objects have to be in the same diagram.
}

grape_event_add_nonterminating_transition::~grape_event_add_nonterminating_transition( void )
{
}

bool grape_event_add_nonterminating_transition::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Has to be the case or the event wouldn't have been generated.

  compound_state* beginstate = dynamic_cast<compound_state*> ( find_object( m_beginstate ) );
  assert( beginstate != 0 );

  compound_state* endstate = dynamic_cast<compound_state*> ( find_object( m_endstate ) );
  assert( endstate != 0 );

  dia_ptr->add_nonterminating_transition( m_ntt, beginstate, endstate );

  finish_modification();
  return true;
}

bool grape_event_add_nonterminating_transition::Undo( void )
{
  // find the diagram the nonterminating transition was added to
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the nonterminating transition that is to be removed
  nonterminating_transition* trans_ptr = static_cast<nonterminating_transition*> ( find_object( m_ntt, NONTERMINATING_TRANSITION, dia_ptr->get_id() ) );
  dia_ptr->remove_nonterminating_transition( trans_ptr );

  finish_modification();
  return true;
}

grape_event_remove_nonterminating_transition::grape_event_remove_nonterminating_transition( grape_frame *p_main_frame, nonterminating_transition* p_ntt, process_diagram* p_proc_dia_ptr )
: grape_event_base( p_main_frame, true, _T( "remove transition" ) )
{
  m_ntt = p_ntt->get_id();
  m_label = *p_ntt->get_label();
  compound_state* beginstate = p_ntt->get_beginstate();
  if ( beginstate != 0 )
  {
    m_beginstate = beginstate->get_id();
  }
  else
  {
    m_beginstate = -1;
  }
  compound_state* endstate = p_ntt->get_endstate();
  if ( endstate != 0 )
  {
    m_endstate = endstate->get_id();
  }
  else
  {
    m_endstate = -1;
  }
  m_coordinate = p_ntt->get_coordinate();
  m_width = p_ntt->get_width();
  m_height = p_ntt->get_height();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_ntt->count_comment(); ++i )
  {
    comment* comm_ptr = p_ntt->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_proc_dia_ptr->get_id();
}

grape_event_remove_nonterminating_transition::~grape_event_remove_nonterminating_transition( void )
{
  m_comments.Clear();
}

bool grape_event_remove_nonterminating_transition::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  object* obj_ptr = find_object( m_ntt, NONTERMINATING_TRANSITION );
  if ( obj_ptr )
  {
    nonterminating_transition* ntt_ptr = static_cast<nonterminating_transition*> ( obj_ptr );
    dia_ptr->remove_nonterminating_transition( ntt_ptr );

    finish_modification();
    return true;
  }

  return false;
}

bool grape_event_remove_nonterminating_transition::Undo( void )
{
  // find the diagram the nonterminating transition was removed from
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0); // Should be the case or something went wrong..

  compound_state* beginstate = static_cast<compound_state*> ( find_object( m_beginstate ) );
  compound_state* endstate = static_cast<compound_state*> ( find_object( m_endstate ) );

  // if one of the states do not exist (yet), stop reinstantiating the transition.
  if ( !beginstate || !endstate )
  {
    // nonterminating transitions can exist without begin or endstate
    // return false;
  }

  nonterminating_transition* new_ntt = dia_ptr->add_nonterminating_transition( m_ntt, beginstate, endstate);
  new_ntt->set_label( m_label );
  new_ntt->set_width( m_width );
  new_ntt->set_height( m_height );
  new_ntt->set_coordinate( m_coordinate );
  for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
  {
    unsigned int identifier = m_comments.Item( i );
    comment* comm_ptr = static_cast<comment*> ( find_object( identifier, COMMENT, dia_ptr->get_id() ) );
    dia_ptr->attach_comment_to_object( comm_ptr, new_ntt );
  }

  finish_modification();
  return true;
}

grape_event_attach_nonterminating_transition_endstate::grape_event_attach_nonterminating_transition_endstate( grape_frame *p_main_frame, nonterminating_transition* p_ntt, compound_state* p_state )
: grape_event_base( p_main_frame, true, _T( "attach transition to endstate" ) )
{
  m_ntt = p_ntt->get_id();
  m_state = p_state->get_id();
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_diagram = dia_ptr->get_id();
}

grape_event_attach_nonterminating_transition_endstate::~grape_event_attach_nonterminating_transition_endstate( void )
{
}

bool grape_event_attach_nonterminating_transition_endstate::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  nonterminating_transition* ntt_ptr = static_cast<nonterminating_transition*> ( find_object( m_ntt, NONTERMINATING_TRANSITION) );
  object* obj_ptr = find_object( m_state );
  state* state_ptr = dynamic_cast<state*> ( obj_ptr );
  if ( state_ptr )
  {
    dia_ptr->attach_nonterminating_transition_endstate( ntt_ptr, state_ptr );
  }
  else
  {
    reference_state* ref_state_ptr = dynamic_cast<reference_state*> ( obj_ptr );
    assert( ref_state_ptr );
    dia_ptr->attach_nonterminating_transition_endstate( ntt_ptr, ref_state_ptr );
  }

  finish_modification();
  return true;
}

bool grape_event_attach_nonterminating_transition_endstate::Undo( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  nonterminating_transition* ntt_ptr = static_cast<nonterminating_transition*> ( find_object( m_ntt, NONTERMINATING_TRANSITION) );
  dia_ptr->detach_nonterminating_transition_endstate( ntt_ptr );

  finish_modification();
  return true;
}

grape_event_detach_nonterminating_transition_endstate::grape_event_detach_nonterminating_transition_endstate( grape_frame *p_main_frame, nonterminating_transition* p_ntt )
: grape_event_base( p_main_frame, true, _T( "detach transition from endstate" ) )
{
  m_ntt = p_ntt->get_id();
  compound_state* state_ptr = p_ntt->get_endstate();
  m_state = state_ptr ? state_ptr->get_id() : -1;
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_diagram = dia_ptr->get_id();
}

grape_event_detach_nonterminating_transition_endstate::~grape_event_detach_nonterminating_transition_endstate( void )
{
}

bool grape_event_detach_nonterminating_transition_endstate::Do( void )
{
  if ( m_state == -1 )
  {
    return false;
  }

  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  nonterminating_transition* ntt_ptr = static_cast<nonterminating_transition*> ( find_object( m_ntt, NONTERMINATING_TRANSITION) );
  dia_ptr->detach_nonterminating_transition_endstate( ntt_ptr );

  finish_modification();
  return true;
}

bool grape_event_detach_nonterminating_transition_endstate::Undo( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  nonterminating_transition* ntt_ptr = static_cast<nonterminating_transition*> ( find_object( m_ntt, NONTERMINATING_TRANSITION) );
  object* obj_ptr = find_object( m_state );
  state* state_ptr = dynamic_cast<state*> ( obj_ptr );
  if ( state_ptr )
  {
    dia_ptr->attach_nonterminating_transition_endstate( ntt_ptr, state_ptr );
  }
  else
  {
    reference_state* ref_state_ptr = dynamic_cast<reference_state*> ( obj_ptr );
    assert( ref_state_ptr );
    dia_ptr->attach_nonterminating_transition_endstate( ntt_ptr, ref_state_ptr );
  }

  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_ntt );
