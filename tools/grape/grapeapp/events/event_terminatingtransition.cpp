// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_terminatingtransition.cpp
//
// Defines GraPE events for terminating transitions

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "event_terminatingtransition.h"

using namespace grape::grapeapp;

grape_event_add_terminating_transition::grape_event_add_terminating_transition( grape_frame *p_main_frame, compound_state* p_beginstate, coordinate &p_coord )
: grape_event_base( p_main_frame, true, _T( "add terminating transition" ) )
{
  m_tt = m_main_frame->get_new_id();
  m_beginstate = p_beginstate->get_id();
  m_coord = p_coord;

  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();

  assert( dynamic_cast<process_diagram*> ( dia_ptr ) != 0 );// The diagram has to be of the casted type, or else this event could not have been generated.

  assert( dynamic_cast<process_diagram*> ( p_beginstate->get_diagram() ) != 0 );
  assert( ( dynamic_cast<process_diagram*> ( p_beginstate->get_diagram() ) == dynamic_cast<process_diagram*> ( dia_ptr ) ) ); // Objects have to be in the same diagram.
}

grape_event_add_terminating_transition::~grape_event_add_terminating_transition( void )
{
}

bool grape_event_add_terminating_transition::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Has to be the case or the event wouldn't have been generated.
  object* obj_ptr = find_object( m_beginstate );
  assert( obj_ptr != 0 );
  state* beginstate = dynamic_cast<state*> ( obj_ptr );
  if ( beginstate != 0 )
  {
    dia_ptr->add_terminating_transition( m_tt, beginstate, m_coord );
  }
  else
  {
    reference_state* beginrefstate = dynamic_cast<reference_state*> ( obj_ptr );
    assert( beginrefstate != 0 );
    dia_ptr->add_terminating_transition( m_tt, beginrefstate, m_coord );
  }

  finish_modification();
  return true;
}

bool grape_event_add_terminating_transition::Undo( void )
{
  // find the diagram the terminating transition was added to
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the terminating transition that is to be removed
  terminating_transition* trans_ptr = static_cast<terminating_transition*> ( find_object( m_tt, TERMINATING_TRANSITION, dia_ptr->get_id() ) );
  dia_ptr->
remove_terminating_transition( trans_ptr );

  finish_modification();
  return true;
}

grape_event_remove_terminating_transition::grape_event_remove_terminating_transition( grape_frame *p_main_frame, terminating_transition* p_tt, process_diagram* p_dia_ptr  )
: grape_event_base( p_main_frame, true, _T( "remove terminating transition" ) )
{
  m_tt = p_tt->get_id();
  m_label = *p_tt->get_label();
  compound_state* beginstate = p_tt->get_beginstate();
  if ( beginstate != 0 )
  {
    m_beginstate = beginstate->get_id();
  }
  else
  {
    m_beginstate = -1;
  }
  m_coordinate = p_tt->get_coordinate();
  m_width = p_tt->get_width();
  m_height = p_tt->get_height();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_tt->count_comment(); ++i )
  {
    comment* comm_ptr = p_tt->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_dia_ptr->get_id();
}

grape_event_remove_terminating_transition::~grape_event_remove_terminating_transition( void )
{
  m_comments.Clear();
}

bool grape_event_remove_terminating_transition::Do( void )
{
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  terminating_transition* trans_ptr = static_cast<terminating_transition*> ( find_object( m_tt, TERMINATING_TRANSITION, dia_ptr->get_id() ) );
  dia_ptr->remove_terminating_transition( trans_ptr );

  finish_modification();
  return true;
}

bool grape_event_remove_terminating_transition::Undo( void )
{
  // find the diagram the terminating transition was removed from
  process_diagram* dia_ptr = dynamic_cast<process_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0); // Should be the case or something went wrong..

  compound_state* beginstate = dynamic_cast<compound_state*> ( find_object( m_beginstate ) );
  terminating_transition* new_tt = dia_ptr->add_terminating_transition( m_tt, beginstate, m_coordinate);
  new_tt->set_label( m_label );
  new_tt->set_width( m_width );
  new_tt->set_height( m_height );
  new_tt->set_coordinate( m_coordinate );
  for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
  {
    unsigned int identifier = m_comments.Item( i );
    comment* comm_ptr = static_cast<comment*> ( find_object( identifier, COMMENT, dia_ptr->get_id() ) );
    dia_ptr->attach_comment_to_object( comm_ptr, new_tt );
  }

  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_tt )
