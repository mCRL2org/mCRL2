// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_object.cpp
//
// Defines GraPE events for objects.

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "visuals/visualobject.h"
#include "visuals/visualpreamble.h"

#include "event_architecturereference.h"
#include "event_channel.h"
#include "event_channelcommunication.h"
#include "event_comment.h"
#include "event_diagram.h"
#include "event_object.h"
#include "event_nonterminatingtransition.h"
#include "event_processdiagram.h"
#include "event_processreference.h"
#include "event_referencestate.h"
#include "event_select.h"
#include "event_state.h"
#include "event_transition.h"

#include "event_object.h"

using namespace grape::grapeapp;

grape_event_move::grape_event_move( grape_frame *p_main_frame, object* p_obj, coordinate &p_old_coord, coordinate &p_new_coord, bool p_undo, int p_flag )
: grape_event_base( p_main_frame, p_undo, _T( "move item" ) )
{
  m_obj_id = p_obj->get_id();
  m_obj_type = p_obj->get_type();

  // remember the initial object coordinate, not where the mouse was initially pressed
  static bool s_new_move = true;
  if ( !p_undo && s_new_move )
  {
    s_new_move = false;
  }
  else if ( p_undo )
  {
    s_new_move = true;
  }
  m_old_coord = p_old_coord;
  m_new_coord = p_new_coord;
  m_flag = p_flag;
  m_diagram_id = m_main_frame->get_glcanvas()->get_diagram()->get_id();
}

grape_event_move::~grape_event_move( void )
{
}

bool grape_event_move::Do( void )
{
  object* obj_ptr = find_object( m_obj_id, m_obj_type );
  coordinate original_object_coordinate = obj_ptr->get_coordinate();
  switch ( obj_ptr->get_type() )
  {
    // If the object is a compound reference, move all its channels as well.
    case PROCESS_REFERENCE:
    {
      obj_ptr->set_coordinate( m_new_coord );
      process_reference* proc_ref_ptr = static_cast<process_reference*> ( obj_ptr );
      for ( unsigned int i = 0; i < proc_ref_ptr->count_channel(); ++i )
      {
        channel* chan_ptr = proc_ref_ptr->get_channel( i );
        coordinate chan_coord = chan_ptr->get_coordinate();
        chan_coord = chan_coord + ( m_new_coord - original_object_coordinate );
        chan_ptr->set_coordinate( chan_coord );
      }
      break;
    }
    case ARCHITECTURE_REFERENCE:
    {
      obj_ptr->set_coordinate( m_new_coord );
      architecture_reference* arch_ref_ptr = static_cast<architecture_reference*> ( obj_ptr );
      for ( unsigned int i = 0; i < arch_ref_ptr->count_channel(); ++i )
      {
        channel* chan_ptr = arch_ref_ptr->get_channel( i );
        coordinate chan_coord = chan_ptr->get_coordinate();
        chan_coord = chan_coord + ( m_new_coord - original_object_coordinate );
        chan_ptr->set_coordinate( chan_coord );
      }
      break;
    }
    // If the object is a compound state, move the initial designator and terminating transitions as well
    case STATE:
    {
      obj_ptr->set_coordinate( m_new_coord );
      state* state_ptr = static_cast<state*> ( obj_ptr );
      for ( unsigned int i = 0; i < state_ptr->count_initial_designator(); ++i )
      {
        initial_designator* init_ptr = state_ptr->get_initial_designator( i );
        if ( init_ptr )
        {
          coordinate init_coord = init_ptr->get_coordinate();
          init_coord = init_coord + ( m_new_coord - original_object_coordinate );
          init_ptr->set_coordinate( init_coord );
        }
      }
      for ( unsigned int i = 0; i < state_ptr->count_transition_beginstate(); ++i )
      {
        transition* trans_ptr = state_ptr->get_transition_beginstate( i );
        terminating_transition* tt_ptr = dynamic_cast<terminating_transition*> ( trans_ptr );
        if ( tt_ptr )
        {
          coordinate tt_coord = tt_ptr->get_coordinate();
          tt_coord = tt_coord + ( m_new_coord - original_object_coordinate );
          tt_ptr->set_coordinate( tt_coord );
        }
      }
      break;
    }
    case REFERENCE_STATE:
    {
      obj_ptr->set_coordinate( m_new_coord );
      reference_state* ref_state_ptr = static_cast<reference_state*> ( obj_ptr );
      for ( unsigned int i = 0; i < ref_state_ptr->count_initial_designator(); ++i )
      {
        initial_designator* init_ptr = ref_state_ptr->get_initial_designator( i );
        if ( init_ptr )
        {
          coordinate init_coord = init_ptr->get_coordinate();
          init_coord = init_coord + ( m_new_coord - original_object_coordinate );
          init_ptr->set_coordinate( init_coord );
        }
      }
      for ( unsigned int i = 0; i < ref_state_ptr->count_transition_beginstate(); ++i )
      {
        transition* trans_ptr = ref_state_ptr->get_transition_beginstate( i );
        terminating_transition* tt_ptr = dynamic_cast<terminating_transition*> ( trans_ptr );
        if ( tt_ptr )
        {
          coordinate tt_coord = tt_ptr->get_coordinate();
          tt_coord = tt_coord + ( m_new_coord - original_object_coordinate );
          tt_ptr->set_coordinate( tt_coord );
        }
      }
      break;
    }
    case CHANNEL:
    {
      // Determine if the move was on the border of the reference the channel is on.
      channel* chan_ptr = static_cast<channel*> ( obj_ptr );
      process_reference* proc_ref_ptr = dynamic_cast<process_reference*> ( chan_ptr->get_reference() );
      if ( proc_ref_ptr )
      {
        if ( is_on_border_rectangle( proc_ref_ptr->get_coordinate(), proc_ref_ptr->get_width(), proc_ref_ptr->get_height(), m_new_coord ) != GRAPE_DIR_NONE )
        {
          coordinate new_coord = move_to_border_rectangle( proc_ref_ptr->get_coordinate(), proc_ref_ptr->get_width(), proc_ref_ptr->get_height(), m_new_coord );
          chan_ptr->set_coordinate( new_coord );
        }
      }
      else
      {
        architecture_reference* arch_ref_ptr = dynamic_cast<architecture_reference*> ( chan_ptr->get_reference() );
        assert( arch_ref_ptr );
        if ( is_on_border_rectangle( arch_ref_ptr->get_coordinate(), arch_ref_ptr->get_width(), arch_ref_ptr->get_height(), m_new_coord ) != GRAPE_DIR_NONE )
        {
          coordinate new_coord = move_to_border_rectangle( arch_ref_ptr->get_coordinate(), arch_ref_ptr->get_width(), arch_ref_ptr->get_height(), m_new_coord );
          chan_ptr->set_coordinate( new_coord );
        }
      }
      break;
    }
    case NONTERMINATING_TRANSITION:
    {
      nonterminating_transition* ntt_ptr = static_cast<nonterminating_transition*> ( obj_ptr );

      if (m_flag == -1) //if we selected the transition
      {
        ntt_ptr->set_coordinate( m_new_coord );
      } else if (m_flag == 0) //if we selected the end state
      {
        coordinate delta = m_new_coord - m_old_coord;

        ntt_ptr->set_width(delta.m_x);
        ntt_ptr->set_height(delta.m_y);
      } else if (m_flag == 1) //if we selected the begin state
      {
      	 // dragging is only possible when the begin state doesn't exist
         if (ntt_ptr->get_beginstate() == 0) ntt_ptr->set_coordinate( m_new_coord );
      }
      break;
    }
    default: obj_ptr->set_coordinate( m_new_coord ); break;
  }

  finish_modification();
  return true;
}

bool grape_event_move::Undo( void )
{
  object* obj_ptr = find_object( m_obj_id, m_obj_type );
  coordinate original_object_coordinate = obj_ptr->get_coordinate();
  switch ( obj_ptr->get_type() )
  {
    // If the object is a compound reference, move all its channels as well.
    case PROCESS_REFERENCE:
    {
      obj_ptr->set_coordinate( m_old_coord );
      process_reference* proc_ref_ptr = static_cast<process_reference*> ( obj_ptr );
      for ( unsigned int i = 0; i < proc_ref_ptr->count_channel(); ++i )
      {
        channel* chan_ptr = proc_ref_ptr->get_channel( i );
        coordinate chan_coord = chan_ptr->get_coordinate();
        chan_coord = chan_coord + ( m_old_coord - original_object_coordinate );
        chan_ptr->set_coordinate( chan_coord );
      }
      break;
    }
    case ARCHITECTURE_REFERENCE:
    {
      obj_ptr->set_coordinate( m_old_coord );
      architecture_reference* arch_ref_ptr = static_cast<architecture_reference*> ( obj_ptr );
      for ( unsigned int i = 0; i < arch_ref_ptr->count_channel(); ++i )
      {
        channel* chan_ptr = arch_ref_ptr->get_channel( i );
        coordinate chan_coord = chan_ptr->get_coordinate();
        chan_coord = chan_coord + ( m_old_coord - original_object_coordinate );
        chan_ptr->set_coordinate( chan_coord );
      }
      break;
    }
    // If the object is a compound state, move the initial designator and terminating transitions as well
    case STATE:
    {
      obj_ptr->set_coordinate( m_old_coord );
      state* state_ptr = static_cast<state*> ( obj_ptr );
      for ( unsigned int i = 0; i < state_ptr->count_initial_designator(); ++i )
      {
        initial_designator* init_ptr = state_ptr->get_initial_designator( i );
        if ( init_ptr )
        {
          coordinate init_coord = init_ptr->get_coordinate();
          init_coord = init_coord + ( m_old_coord - original_object_coordinate );
          init_ptr->set_coordinate( init_coord );
        }
      }
      for ( unsigned int i = 0; i < state_ptr->count_transition_beginstate(); ++i )
      {
        transition* trans_ptr = state_ptr->get_transition_beginstate( i );
        terminating_transition* tt_ptr = dynamic_cast<terminating_transition*> ( trans_ptr );
        if ( tt_ptr )
        {
          coordinate tt_coord = tt_ptr->get_coordinate();
          tt_coord = tt_coord + ( m_old_coord - original_object_coordinate );
          tt_ptr->set_coordinate( tt_coord );
        }
      }
      break;
    }
    case REFERENCE_STATE:
    {
      obj_ptr->set_coordinate( m_old_coord );
      reference_state* ref_state_ptr = static_cast<reference_state*> ( obj_ptr );
      for ( unsigned int i = 0; i < ref_state_ptr->count_initial_designator(); ++i )
      {
        initial_designator* init_ptr = ref_state_ptr->get_initial_designator( i );
        if ( init_ptr )
        {
          coordinate init_coord = init_ptr->get_coordinate();
          init_coord = init_coord + ( m_old_coord - original_object_coordinate );
          init_ptr->set_coordinate( init_coord );
        }
      }
      for ( unsigned int i = 0; i < ref_state_ptr->count_transition_beginstate(); ++i )
      {
        transition* trans_ptr = ref_state_ptr->get_transition_beginstate( i );
        terminating_transition* tt_ptr = dynamic_cast<terminating_transition*> ( trans_ptr );
        if ( tt_ptr )
        {
          coordinate tt_coord = tt_ptr->get_coordinate();
          tt_coord = tt_coord + ( m_old_coord - original_object_coordinate );
          tt_ptr->set_coordinate( tt_coord );
        }
      }
      break;
    }
    case CHANNEL:
    {
      // Determine if the move was on the border of the reference the channel is on.
      channel* chan_ptr = static_cast<channel*> ( obj_ptr );
      process_reference* proc_ref_ptr = dynamic_cast<process_reference*> ( chan_ptr->get_reference() );
      if ( proc_ref_ptr )
      {
        if ( is_on_border_rectangle( proc_ref_ptr->get_coordinate(), proc_ref_ptr->get_width(), proc_ref_ptr->get_height(), m_old_coord ) != GRAPE_DIR_NONE )
        {
          coordinate new_coord = move_to_border_rectangle( proc_ref_ptr->get_coordinate(), proc_ref_ptr->get_width(), proc_ref_ptr->get_height(), m_old_coord );
          chan_ptr->set_coordinate( new_coord );
        }
      }
      else
      {
        architecture_reference* arch_ref_ptr = dynamic_cast<architecture_reference*> ( chan_ptr->get_reference() );
        assert( arch_ref_ptr );
        if ( is_on_border_rectangle( arch_ref_ptr->get_coordinate(), arch_ref_ptr->get_width(), arch_ref_ptr->get_height(), m_old_coord ) != GRAPE_DIR_NONE )
        {
          coordinate new_coord = move_to_border_rectangle( arch_ref_ptr->get_coordinate(), arch_ref_ptr->get_width(), arch_ref_ptr->get_height(), m_old_coord );
          chan_ptr->set_coordinate( new_coord );
        }
      }
      break;
    }
    case NONTERMINATING_TRANSITION:
    {
      nonterminating_transition* ntt_ptr = static_cast<nonterminating_transition*> ( obj_ptr );
      ntt_ptr->set_coordinate( m_old_coord );
      break;
    }
    default: obj_ptr->set_coordinate( m_old_coord ); break;
  }

  finish_modification();
  return true;
}

grape_event_resize::grape_event_resize( grape_frame *p_main_frame, object* p_obj, const grape_geometry &p_old_geo, const grape_geometry &p_new_geo, bool p_undo )
: grape_event_base( p_main_frame, p_undo, _T( "resize item" ) )
{
  m_obj_id = p_obj->get_id();
  m_obj_type = p_obj->get_type();
  m_old_geo = p_old_geo;
  m_new_geo = p_new_geo;
}

grape_event_resize::~grape_event_resize( void )
{
}

bool grape_event_resize::Do( void )
{
  object* obj_ptr = find_object( m_obj_id, m_obj_type );

  // Do not resize if it's a channel.
  if ( obj_ptr->get_type() != CHANNEL )
  {
    obj_ptr->set_coordinate( m_new_geo.m_coord );
    obj_ptr->set_width( m_new_geo.m_width );
    obj_ptr->set_height( m_new_geo.m_height );
  }

  // If it's a reference, make sure the channels stay on the border
  if ( obj_ptr->get_type() == ARCHITECTURE_REFERENCE )
  {
    architecture_reference* arch_ref_ptr = static_cast<architecture_reference*> ( obj_ptr );
    for ( unsigned int i = 0; i < arch_ref_ptr->count_channel(); ++i )
    {
      channel* chan_ptr = arch_ref_ptr->get_channel( i );
      coordinate new_coord = move_to_border_rectangle( arch_ref_ptr->get_coordinate(), arch_ref_ptr->get_width(), arch_ref_ptr->get_height(), chan_ptr->get_coordinate() );
      chan_ptr->set_coordinate( new_coord );
    }
  }
  else if ( obj_ptr->get_type() == PROCESS_REFERENCE )
  {
    process_reference* proc_ref_ptr = static_cast<process_reference*> ( obj_ptr );
    for ( unsigned int i = 0; i < proc_ref_ptr->count_channel(); ++i )
    {
      channel* chan_ptr = proc_ref_ptr->get_channel( i );
      coordinate new_coord = move_to_border_rectangle( proc_ref_ptr->get_coordinate(), proc_ref_ptr->get_width(), proc_ref_ptr->get_height(), chan_ptr->get_coordinate() );
      chan_ptr->set_coordinate( new_coord );
    }
  }

  finish_modification();
  return true;
}

bool grape_event_resize::Undo( void )
{
  object* obj_ptr = find_object( m_obj_id, m_obj_type );

  // Do not resize if it's a channel.
  if ( obj_ptr->get_type() != CHANNEL )
  {
    obj_ptr->set_coordinate( m_old_geo.m_coord );
    obj_ptr->set_width( m_old_geo.m_width );
    obj_ptr->set_height( m_old_geo.m_height );
  }

  // If it's a reference, make sure the channels stay on the border
  if ( obj_ptr->get_type() == ARCHITECTURE_REFERENCE )
  {
    architecture_reference* arch_ref_ptr = static_cast<architecture_reference*> ( obj_ptr );
    for ( unsigned int i = 0; i < arch_ref_ptr->count_channel(); ++i )
    {
      channel* chan_ptr = arch_ref_ptr->get_channel( i );
      coordinate new_coord = move_to_border_rectangle( arch_ref_ptr->get_coordinate(), arch_ref_ptr->get_width(), arch_ref_ptr->get_height(), chan_ptr->get_coordinate() );
      chan_ptr->set_coordinate( new_coord );
    }
  }
  else if ( obj_ptr->get_type() == PROCESS_REFERENCE )
  {
    process_reference* proc_ref_ptr = static_cast<process_reference*> ( obj_ptr );
    for ( unsigned int i = 0; i < proc_ref_ptr->count_channel(); ++i )
    {
      channel* chan_ptr = proc_ref_ptr->get_channel( i );
      coordinate new_coord = move_to_border_rectangle( proc_ref_ptr->get_coordinate(), proc_ref_ptr->get_width(), proc_ref_ptr->get_height(), chan_ptr->get_coordinate() );
      chan_ptr->set_coordinate( new_coord );
    }
  }

  finish_modification();
  return true;
}

grape_event_properties::grape_event_properties( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, false, _T( "properties" ) )
{
  m_vis_obj = 0;
  m_obj_ptr = 0;
  diagram *dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  if ( dia_ptr )
  {
    if ( dia_ptr->count_selected_objects() == 1 )
    {
      m_obj_ptr = dia_ptr->get_selected_object( 0 );
    }
  }
}

grape_event_properties::grape_event_properties( grape_frame *p_main_frame, visual_object* p_vis_obj )
: grape_event_base( p_main_frame, false, _T( "properties" ) )
{
  m_vis_obj = p_vis_obj;
  m_obj_ptr = 0;
  if ( m_vis_obj )
  {
    m_obj_ptr = p_vis_obj->get_selectable_object();
  }
}

grape_event_properties::~grape_event_properties( void )
{
}

bool grape_event_properties::Do( void )
{
  m_main_frame->get_glcanvas()->SetFocus();

  // Determine what object was selected
  if ( m_obj_ptr )
  {
    grape_event_select* event = new grape_event_select( m_main_frame, m_obj_ptr, false );
    m_main_frame->get_event_handler()->Submit( event, false );

    switch ( m_obj_ptr->get_type() )
    {
      case COMMENT:
      {
        comment *comment_ptr = static_cast<comment*> ( m_obj_ptr );
        grape_event_change_comment_text* event = new grape_event_change_comment_text( m_main_frame, comment_ptr );
        m_main_frame->get_event_handler()->Submit( event, true );
        break;
      }
      case INITIAL_DESIGNATOR: break;
      case STATE:
      {
        grape_event_change_state* event = new grape_event_change_state( m_main_frame, static_cast<state*> ( m_obj_ptr ) );
        m_main_frame->get_event_handler()->Submit( event, true );
        break;
      }
      case REFERENCE_STATE:
      {
        grape_event_change_refstate* event = new grape_event_change_refstate( m_main_frame, static_cast<reference_state*> ( m_obj_ptr ) );
        m_main_frame->get_event_handler()->Submit( event, true );
        break;
      }
      case NONTERMINATING_TRANSITION:
      {
        grape_event_change_transition *event = new grape_event_change_transition(m_main_frame, static_cast<nonterminating_transition*>(m_obj_ptr));
        m_main_frame->get_event_handler()->Submit(event, true);
        break;
      }
      case TERMINATING_TRANSITION:
      {
        grape_event_change_transition *event = new grape_event_change_transition(m_main_frame, static_cast<terminating_transition*>(m_obj_ptr));
        m_main_frame->get_event_handler()->Submit(event, true);
        break;
      }
      case CHANNEL:
      {
        grape_event_change_channel* event = new grape_event_change_channel( m_main_frame, static_cast<channel*> ( m_obj_ptr ) );
        m_main_frame->get_event_handler()->Submit( event, true );
        break;
      }
      case CHANNEL_COMMUNICATION:
      {
        grape_event_change_channel_communication* event = new grape_event_change_channel_communication( m_main_frame, static_cast<channel_communication*> ( m_obj_ptr ) );
        m_main_frame->get_event_handler()->Submit( event, true );
        break;
      }
      case PROCESS_REFERENCE:
      {
        process_reference *proc_ref = static_cast<process_reference*> ( m_obj_ptr );
        grape_event_change_procref* event = new grape_event_change_procref( m_main_frame, proc_ref );
        m_main_frame->get_event_handler()->Submit( event, true );
        break;
      }
      case ARCHITECTURE_REFERENCE:
      {
        architecture_reference *arch_ref = static_cast<architecture_reference*> ( m_obj_ptr );
        grape_event_change_archref* event = new grape_event_change_archref( m_main_frame, arch_ref );
        m_main_frame->get_event_handler()->Submit( event, true );
        break;
      }
      default: break;
    }
  }
  else if ( m_vis_obj )
  {
    if ( m_vis_obj->get_type() == NONE )
    {
      visualpreamble *vis_preamble_ptr = dynamic_cast< visualpreamble* >( m_vis_obj );
      if ( vis_preamble_ptr )
      {
        grape_event_change_preamble *event = new grape_event_change_preamble( m_main_frame, vis_preamble_ptr->get_preamble() );
        m_main_frame->get_event_handler()->Submit( event, true );
      }
    }
  }
  return true;
}

bool grape_event_properties::Undo( void )
{
  return true;
}
