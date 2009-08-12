// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_input.cpp
//
// Defines GraPE events for input actions.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "visuals/geometric.h"

#include "visuals/visualobject.h"
#include "visuals/visualarchitecture_reference.h"
#include "visuals/visualnonterminating_transition.h"
#include "visuals/visualprocess_reference.h"
#include "visuals/visualpreamble.h"
#include "visuals/visualcomment.h"

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

#include "event_input.h"

using namespace grape::grapeapp;

grape_event_click::grape_event_click( grape_frame *p_main_frame, coordinate &p_coord, visual_object* p_vis_obj, bool p_shift_pressed )

: grape_event_base( p_main_frame, false, _T( "click" ) )
{
  m_coord = p_coord;
  m_vis_obj = p_vis_obj;
  m_shift_pressed = p_shift_pressed;
}

grape_event_click::~grape_event_click( void )
{
}

bool grape_event_click::Do( void )
{
  m_main_frame->get_glcanvas()->SetFocus();

  canvas_state state = m_main_frame->get_glcanvas()->get_canvas_state();

  switch ( state )
  {
    case IDLE: break;
    case SELECT:
    {
      if ( m_vis_obj != 0 ) // if an object was clicked.
      {
        object* obj_ptr = m_vis_obj->get_selectable_object();
        if ( obj_ptr != 0 )
        {
          if ( !obj_ptr->get_selected() || !m_shift_pressed )
          {
            // select object
            grape_event_select* event = new grape_event_select( m_main_frame, obj_ptr, m_shift_pressed );
            m_main_frame->get_event_handler()->Submit( event, false );
          }
          else
          {
            // deselect object
            grape_event_deselect* event = new grape_event_deselect( m_main_frame, obj_ptr );
            m_main_frame->get_event_handler()->Submit( event, false );
          }
        }
      }
      else // no objects were touched, deselect everything
      {
        // deselect all objects
        grape_event_deselect_all* event = new grape_event_deselect_all( m_main_frame );
        m_main_frame->get_event_handler()->Submit(event, false );
      }
      break;
    }
    case ADD_STATE:
    {
      if ( m_vis_obj == 0 )
      {
        // create new event and submit to event handler
        grape_event_add_state *event = new grape_event_add_state( m_main_frame, m_coord );
        m_main_frame->get_event_handler()->Submit( event, true );
      }
      break;
    }
    case ADD_REFERENCE_STATE:
    {
      if ( m_vis_obj == 0 )
      {
        // create new event and submit to event handler
        grape_event_add_reference_state *event = new grape_event_add_reference_state(
                                                m_main_frame, m_coord );
        m_main_frame->get_event_handler()->Submit( event, true );
      }
      break;
    }
    case ADD_PROCESS_REFERENCE:
    {
      if ( m_vis_obj == 0 )
      {
        // create new event and submit to event handler
        grape_event_add_process_reference *event = new grape_event_add_process_reference(
                                                  m_main_frame, m_coord );
        m_main_frame->get_event_handler()->Submit( event, true );
      }
      break;
    }
    case ADD_ARCHITECTURE_REFERENCE:
    {
      if ( m_vis_obj == 0 )
      {
        // create new event and submit to event handler
        grape_event_add_architecture_reference *event = new grape_event_add_architecture_reference(
                                                        m_main_frame, m_coord );
        m_main_frame->get_event_handler()->Submit( event, true );
      }
      break;
    }
    case ADD_COMMENT:
    {
      if ( m_vis_obj == 0 )
      {
        // create new event and submit to event handler
        grape_event_add_comment *event = new grape_event_add_comment( m_main_frame, m_coord );
        m_main_frame->get_event_handler()->Submit( event, true );
      }
      break;
    }
    case ADD_CHANNEL:
    {
      if ( m_vis_obj != 0 )
      {
        object* obj = m_vis_obj->get_selectable_object();
        if ( obj != 0 )
        {
          if ( obj->get_type() == PROCESS_REFERENCE )
          {
            visualprocess_reference* vis_proc_ref = static_cast<visualprocess_reference*> ( m_vis_obj );
            // if the coordinate is not neatly on the border of a reference,
            // find a more suitable coordinate for the channel.
            m_coord = vis_proc_ref->move_to_border( m_coord );
            grape_event_add_channel* event = new grape_event_add_channel( m_main_frame,
              m_coord, static_cast<process_reference*> ( obj ) );
            m_main_frame->get_event_handler()->Submit( event, true );
          }
          else if ( obj->get_type() == ARCHITECTURE_REFERENCE )
          {
            visualarchitecture_reference* vis_arch_ref = static_cast<visualarchitecture_reference*> ( m_vis_obj );
            // if the coordinate is not neatly on the border of a reference,
            // find a more suitable coordinate for the channel.
            m_coord = vis_arch_ref->move_to_border( m_coord );
            grape_event_add_channel* event = new grape_event_add_channel( m_main_frame,
              m_coord, static_cast<architecture_reference*> ( obj ) );
            m_main_frame->get_event_handler()->Submit( event, true );
          }
        }
      }
      break;
    }
    case ADD_INITIAL_DESIGNATOR:
    {
      if ( m_vis_obj != 0 ) // Only do something if the mouse clicked on a state.
      {
        object* begin_object_ptr = m_vis_obj->get_selectable_object();
        if ( ( begin_object_ptr != 0 ) && ( begin_object_ptr->get_type() == STATE ) )  // If it is a state
        {
          libgrape::state* designated_state_ptr = static_cast<libgrape::state*> ( begin_object_ptr );

          // Initial values
          coordinate coord = { designated_state_ptr->get_coordinate().m_x, designated_state_ptr->get_coordinate().m_y + designated_state_ptr->get_height()*0.5 + 0.1 };
          coordinate end_coord = get_coordinate_on_edge(coord, designated_state_ptr);
          coordinate mid_coord = { (coord.m_x + end_coord.m_x)*0.5, (coord.m_y + end_coord.m_y)*0.5 };
          double displacement = 0.01;

          // Find a decent coordinate to place the designator
          while ( m_main_frame->get_glcanvas()->get_selectable_visual_object( mid_coord ) != 0)
          {
            displacement = -displacement*1.1;
            coord.m_x = coord.m_x + displacement;
            // Update values
            end_coord = get_coordinate_on_edge(coord, designated_state_ptr);
            mid_coord.m_x = (coord.m_x + end_coord.m_x)*0.5;
            mid_coord.m_y = (coord.m_y + end_coord.m_y)*0.5;
          }

          grape_event_add_initial_designator* event = new grape_event_add_initial_designator( m_main_frame, designated_state_ptr, coord );
          m_main_frame->get_event_handler()->Submit( event, true );
        }
        else if ( ( begin_object_ptr != 0 ) && ( begin_object_ptr->get_type() == REFERENCE_STATE ) )  // If it is a reference state
        {
          libgrape::reference_state* designated_reference_ptr = static_cast<libgrape::reference_state*> ( begin_object_ptr );

          // Initial values
          coordinate coord = { designated_reference_ptr->get_coordinate().m_x, designated_reference_ptr->get_coordinate().m_y + designated_reference_ptr->get_height()*0.5 + 0.1 };
          coordinate end_coord = get_coordinate_on_edge(coord, designated_reference_ptr);
          coordinate mid_coord = { (coord.m_x + end_coord.m_x)*0.5, (coord.m_y + end_coord.m_y)*0.5 };
          double displacement = 0.01;

          // Find a decent coordinate to place the designator
          while ( m_main_frame->get_glcanvas()->get_selectable_visual_object( mid_coord ) != 0)
          {
            displacement = -displacement*1.1;
            coord.m_x = coord.m_x + displacement;
            // Update values
            end_coord = get_coordinate_on_edge(coord, designated_reference_ptr);
            mid_coord.m_x = (coord.m_x + end_coord.m_x)*0.5;
            mid_coord.m_y = (coord.m_y + end_coord.m_y)*0.5;
          }

          grape_event_add_initial_designator* event = new grape_event_add_initial_designator( m_main_frame, designated_reference_ptr, coord );
          m_main_frame->get_event_handler()->Submit( event, true );
        }
      }
      break;
    }
    case ADD_NONTERMINATING_TRANSITION:
    {
      if ( m_vis_obj != 0 ) // Only do something if the mouse clicked on a state.
      {
        object* begin_object_ptr = m_vis_obj->get_selectable_object();
        if ( ( begin_object_ptr != 0 ) && ( begin_object_ptr->get_type() == STATE ) )
        {
          libgrape::state* state_ptr = static_cast<libgrape::state*> ( begin_object_ptr );
          grape_event_add_nonterminating_transition* event = new
            grape_event_add_nonterminating_transition( m_main_frame, state_ptr, state_ptr );
          m_main_frame->get_event_handler()->Submit( event, true );
        }
        else if ( ( begin_object_ptr != 0 ) && ( begin_object_ptr->get_type() == REFERENCE_STATE ) )
        {
          libgrape::reference_state* ref_state_ptr = static_cast<libgrape::reference_state*> ( begin_object_ptr );
          grape_event_add_nonterminating_transition* event = new
            grape_event_add_nonterminating_transition( m_main_frame, ref_state_ptr, ref_state_ptr );
          m_main_frame->get_event_handler()->Submit( event, true );
        }
      }
      break;
    }
    // Below are the drag-events that cannot be handled in this event.
    case ADD_TERMINATING_TRANSITION: break;
    case ADD_CHANNEL_COMMUNICATION: break;
    default:
    {
      // Cannot be correct
      /* assert( false ); */
      m_main_frame->get_glcanvas()->set_canvas_state( IDLE );
      break;
    }
  } // end switch

  //if we added an object
  if ((state == ADD_NONTERMINATING_TRANSITION) || (state == ADD_TERMINATING_TRANSITION) || (state == ADD_STATE) || (state == ADD_REFERENCE_STATE) || (state == ADD_PROCESS_REFERENCE) || (state == ADD_ARCHITECTURE_REFERENCE) || (state == ADD_COMMENT) || (state == ADD_CHANNEL) || (state == ADD_INITIAL_DESIGNATOR) || (state == ADD_NONTERMINATING_TRANSITION))
  {
    //deselect all objects
    grape_event_deselect_all *event = new grape_event_deselect_all(m_main_frame);
    m_main_frame->get_event_handler()->Submit(event, false);
  }
  return true;
}

bool grape_event_click::Undo( void )
{
  return true;
}



grape_event_doubleclick::grape_event_doubleclick( grape_frame *p_main_frame, visual_object *p_vis_obj, wxMouseEvent &p_event )
: grape_event_base( p_main_frame, false, _T( "doubleclick" ) )
{
  m_vis_obj = p_vis_obj;
  m_mouse_event = p_event;
}

grape_event_doubleclick::~grape_event_doubleclick( void )
{
}

bool grape_event_doubleclick::Do( void )
{
  grape_event_properties *event = new grape_event_properties( m_main_frame, m_vis_obj, m_mouse_event );
  return m_main_frame->get_event_handler()->Submit( event, false );
}

bool grape_event_doubleclick::Undo( void )
{
  return true;
}

grape_event_drag::grape_event_drag( grape_frame *p_main_frame, int p_vis_obj_id, coordinate &p_down, coordinate &p_up, grape_direction p_click_location, bool p_mousedown )
: grape_event_base( p_main_frame, false, _T( "drag item" ) )
{
  m_obj = p_vis_obj_id;
  m_down = p_down;
  m_up = p_up;
  m_click_location = p_click_location;
  m_mousedown = p_mousedown;
}

grape_event_drag::~grape_event_drag( void )
{
}

bool grape_event_drag::Do( void )
{
  canvas_state state = m_main_frame->get_glcanvas()->get_canvas_state();
  
  // only add objects when the mousebutton is up again.
  if ( m_mousedown && state != SELECT )
  {
    return false;
  }
  object* begin_object_ptr = find_object( m_obj );
  // Determine if the mouse was released on a visual object.
  visual_object* visual_endobject = m_main_frame->get_glcanvas()->get_selectable_visual_object( m_up );

  // don't do anything if a drag started from empty space.
  if ( !begin_object_ptr )
  {
    return false;
  }
  
  switch ( state )
  {
    case IDLE: break;
    case SELECT:
    {
      // Make sure the new geometry of the object is based upon the original
      // geometry since the mouse was pressed down, and not based on the geometry
      // caused by the previous drag event.
      // The statics make sure the original geometry is preserved during several
      // drag events.
      static bool new_drag = true;
      static coordinate s_coord_mousedown;
      static coordinate s_orig_center;
      static coordinate s_dif;
      // dynamic array containing initial transition coordinates
      static coordinate* s_orig_ntt = 0;
      static float s_orig_width;
      static float s_orig_height;
      static int s_flag;

      // if we started a new drag
      if ( m_mousedown && new_drag)
      {
        s_orig_center = begin_object_ptr->get_coordinate();
        s_orig_width = begin_object_ptr->get_width();
        s_orig_height = begin_object_ptr->get_height();
        s_coord_mousedown = m_up;
        s_dif = s_orig_center - s_coord_mousedown;

        // store the initial position of the nonterminating transitions attached to the state
        visual_object* v_obj = m_main_frame->get_glcanvas()->get_selectable_visual_object( s_orig_center );
        if ((v_obj != 0) && (v_obj->get_type() == STATE || v_obj->get_type() == REFERENCE_STATE))
        {
          compound_state* state = static_cast<libgrape::compound_state*> ( v_obj->get_selectable_object() );
          // set length of dynamic array
          delete [] s_orig_ntt;
          s_orig_ntt = new coordinate[state->count_transition_endstate()];

          // fill dynamic array with initial coordinates
          for ( unsigned int i = 0; i < state->count_transition_endstate(); ++i )
          {
            nonterminating_transition* ntt = state->get_transition_endstate( i );
            s_orig_ntt[i] = ntt->get_coordinate();
          }
        }
        if (new_drag == true)
        {
          s_flag = -1;
          new_drag = false;

          if (begin_object_ptr->get_type() == NONTERMINATING_TRANSITION)  //change flag if the object is a nonterminating transition
          {
            nonterminating_transition* ntt_ptr = static_cast<nonterminating_transition*> ( begin_object_ptr );
            //dragging begin coordinate
            coordinate tail_coordinate = ntt_ptr->get_begin_coordinate() + ntt_ptr->get_coordinate();
            if (distance(tail_coordinate, m_up) < 0.05f) s_flag = 1;

            //dragging end coordinate
            coordinate head_coordinate = ntt_ptr->get_end_coordinate() + ntt_ptr->get_coordinate();
            if (distance(head_coordinate, m_up) < 0.05f) s_flag = 0;
          }
        }
      } else if ( !m_mousedown ) new_drag = true;

      coordinate delta = m_up - s_coord_mousedown;

      // only update up coordinate with initial coordinate difference if the type is not a NONTERMINATING_TRANSITION
      if (begin_object_ptr->get_type() != NONTERMINATING_TRANSITION)
      {
        m_up.m_x = m_up.m_x + s_dif.m_x;
        m_up.m_y = m_up.m_y + s_dif.m_y;
      }

      if ( m_click_location == GRAPE_DIR_NONE ) // no border clicked, thus a move
      {
        if (begin_object_ptr->get_type() == NONTERMINATING_TRANSITION)
        {
          nonterminating_transition* ntt_ptr = static_cast<nonterminating_transition*> ( begin_object_ptr );
          visual_object* v_obj = m_main_frame->get_glcanvas()->get_selectable_visual_object( m_up );

          //detach or attach begin state
          if (s_flag == 1)
          {
            if ((v_obj != 0) && (v_obj->get_type() == STATE || v_obj->get_type() == REFERENCE_STATE))
            {
              if (ntt_ptr->get_beginstate() == 0)
              {
                compound_state* state = static_cast<libgrape::compound_state*> ( v_obj->get_selectable_object() );
                grape_event_attach_transition_beginstate* attach_event = new grape_event_attach_transition_beginstate( m_main_frame, ntt_ptr, state);
                m_main_frame->get_event_handler()->Submit( attach_event, true );

                // if there is a begin and end state set the coordinate of the transition in the middle of the begin and end coordinate
                if ((ntt_ptr->get_beginstate() != 0) && (ntt_ptr->get_endstate() != 0))
                {
                  coordinate new_coordinate;
                  // if it is a loop
                  if ( ntt_ptr->get_beginstate() == ntt_ptr->get_endstate() )
                  {
                    new_coordinate.m_x = ntt_ptr->get_beginstate()->get_coordinate().m_x - ntt_ptr->get_beginstate()->get_width() * 0.5 - 0.12;
                    new_coordinate.m_y = ntt_ptr->get_beginstate()->get_coordinate().m_y;
                  } else {
                    // if the begin and endstate are different
                    new_coordinate.m_x = (ntt_ptr->get_beginstate()->get_coordinate().m_x + ntt_ptr->get_endstate()->get_coordinate().m_x) * 0.5;
                    new_coordinate.m_y = (ntt_ptr->get_beginstate()->get_coordinate().m_y + ntt_ptr->get_endstate()->get_coordinate().m_y) * 0.5;
                  }
                  ntt_ptr->set_coordinate(new_coordinate);
                }
              }
            } else {
              if (ntt_ptr->get_beginstate() != 0)
              {
                grape_event_detach_transition_beginstate* detach_event = new grape_event_detach_transition_beginstate( m_main_frame, ntt_ptr);
                m_main_frame->get_event_handler()->Submit( detach_event, true );
              }
            }
          }

          //detach or attach endstate
          if (s_flag == 0)
          {
            visual_object* v_obj = m_main_frame->get_glcanvas()->get_selectable_visual_object( m_up );
            if ((v_obj != 0) && (v_obj->get_type() == STATE || v_obj->get_type() == REFERENCE_STATE))
            {
              //attach endstate
              if ( ntt_ptr->get_endstate() == 0 )
              {
                compound_state* state = static_cast<libgrape::compound_state*> ( v_obj->get_selectable_object() );
                grape_event_attach_nonterminating_transition_endstate* attach_event = new grape_event_attach_nonterminating_transition_endstate( m_main_frame, ntt_ptr, state);
                m_main_frame->get_event_handler()->Submit( attach_event, true );
              }
            } else {
              //detach endstate
              if ( ntt_ptr->get_endstate() != 0 )
              {
                grape_event_detach_nonterminating_transition_endstate* detach_event = new grape_event_detach_nonterminating_transition_endstate( m_main_frame, ntt_ptr);
                m_main_frame->get_event_handler()->Submit( detach_event, true );
              }
            }
          }
        } else {
          //other object
        }
        
        // get initial object
        visual_object* visual_beginobject = m_main_frame->get_glcanvas()->get_visual_object( begin_object_ptr );
        visualcomment* v_com = static_cast<grapeapp::visualcomment*> (visual_beginobject);
   
        if (visual_beginobject->get_type() == COMMENT)
        {       
          comment* com = static_cast<libgrape::comment*> (begin_object_ptr); 
          // test if the reference on the comment is selected
          if (v_com->is_inside_reference(s_coord_mousedown))
          {
            if (visual_endobject && (visual_endobject->get_type() != COMMENT)) //only attach if it is not attached to a comment
            { 
              object* end_object_ptr = visual_endobject->get_selectable_object();      
              if (end_object_ptr && (end_object_ptr != begin_object_ptr))
              {
                grape_event_attach_comment* event = new grape_event_attach_comment( m_main_frame, com, end_object_ptr );
                m_main_frame->get_event_handler()->Submit( event, true );
              }
            }
            else 
            {
              comment* com = static_cast<libgrape::comment*> (begin_object_ptr);
              grape_event_detach_comment* event = new grape_event_detach_comment( m_main_frame, com );
              m_main_frame->get_event_handler()->Submit( event, true );
            }
          }
          else
          {
            //move object
            grape_event_move *event = new grape_event_move( m_main_frame, begin_object_ptr, s_orig_center, m_up, !m_mousedown, s_flag );
            m_main_frame->get_event_handler()->Submit( event, !m_mousedown );
          } 
        }
        else if (visual_beginobject->get_type() == CHANNEL)  
        {
          //find position near border
          channel* chan = static_cast<libgrape::channel*> (begin_object_ptr);
          compound_reference* ref = chan->get_reference();
          coordinate coord = move_to_border_rectangle( ref->get_coordinate(), ref->get_width(), ref->get_height(), m_up );
    
          //move channel
          grape_event_move *event = new grape_event_move( m_main_frame, begin_object_ptr, s_orig_center, coord, !m_mousedown, s_flag );
          m_main_frame->get_event_handler()->Submit( event, !m_mousedown ); 
        }
        else 
        {
          //move object
          grape_event_move *event = new grape_event_move( m_main_frame, begin_object_ptr, s_orig_center, m_up, !m_mousedown, s_flag );
          m_main_frame->get_event_handler()->Submit( event, !m_mousedown );
        }
        
        //also move the nonterminating transitions attached to the state
        if ((begin_object_ptr != 0) && (begin_object_ptr->get_type() == STATE || begin_object_ptr->get_type() == REFERENCE_STATE))
        {
          compound_state* state = static_cast<libgrape::compound_state*> (begin_object_ptr);

          for ( unsigned int i = 0; i < state->count_transition_endstate(); ++i )
          {
            nonterminating_transition* ntt = state->get_transition_endstate( i );
            if (ntt->get_beginstate() == ntt->get_endstate())
            {
              coordinate c = s_orig_ntt[i]+m_up-s_orig_center;
              //move the nonterminating transition object
              grape_event_move *event = new grape_event_move( m_main_frame, ntt, s_orig_ntt[i], c, !m_mousedown, s_flag );
              m_main_frame->get_event_handler()->Submit( event, !m_mousedown );
            }
          }
        }
      }
      else // a border was selected, thus a resize
      {
        float new_x = 0, new_y = 0, new_width = 0, new_height = 0;

        switch ( m_click_location )
        {
          case GRAPE_DIR_NORTH:
          {
            new_width = s_orig_width;
            new_x = s_orig_center.m_x;
            new_height = s_orig_height + delta.m_y;
            new_y = s_orig_center.m_y + 0.5 * delta.m_y;
            break;
          }
          case GRAPE_DIR_NORTHEAST:
          {
            new_width = s_orig_width + delta.m_x;
            new_x = s_orig_center.m_x + 0.5 * delta.m_x;
            new_height = s_orig_height + delta.m_y;
            new_y = s_orig_center.m_y + 0.5 * delta.m_y;
            break;
          }
          case GRAPE_DIR_EAST:
          {
            new_width = s_orig_width + delta.m_x;
            new_x = s_orig_center.m_x + 0.5 * delta.m_x;
            new_height = s_orig_height;
            new_y = s_orig_center.m_y;
            break;
          }
          case GRAPE_DIR_SOUTHEAST:
          {
            new_width = s_orig_width + delta.m_x;
            new_x = s_orig_center.m_x + 0.5 * delta.m_x;
            new_height = s_orig_height - delta.m_y;
            new_y = s_orig_center.m_y + 0.5 * delta.m_y;
            break;
          }
          case GRAPE_DIR_SOUTH:
          {
            new_width = s_orig_width;
            new_x = s_orig_center.m_x;
            new_height = s_orig_height - delta.m_y;
            new_y = s_orig_center.m_y + 0.5 * delta.m_y;
            break;
          }
          case GRAPE_DIR_SOUTHWEST:
          {
            new_width = s_orig_width - delta.m_x;
            new_x = s_orig_center.m_x + 0.5 * delta.m_x;
            new_height = s_orig_height - delta.m_y;
            new_y = s_orig_center.m_y + 0.5 * delta.m_y;
            break;
          }
          case GRAPE_DIR_WEST:
          {
            new_width = s_orig_width - delta.m_x;
            new_x = s_orig_center.m_x + 0.5 * delta.m_x;
            new_height = s_orig_height;
            new_y = s_orig_center.m_y;
            break;
          }
          case GRAPE_DIR_NORTHWEST:
          {
            new_width = s_orig_width - delta.m_x;
            new_x = s_orig_center.m_x + 0.5 * delta.m_x;
            new_height = s_orig_height + delta.m_y;
            new_y = s_orig_center.m_y + 0.5 * delta.m_y;
            break;
          }
          case GRAPE_DIR_ANY: // in case of an ellipse or initial designator
          {
            new_width = s_orig_width + delta.m_x;
            new_width = s_orig_width + 0.5 * delta.m_x;
            new_height = s_orig_height + delta.m_y;
            new_y = s_orig_center.m_y + 0.5 * delta.m_y;
            break;
          }
          default: break;
        }

        if ( new_width < g_min_object_width )
        {
          new_width = begin_object_ptr->get_width();
          new_x = begin_object_ptr->get_coordinate().m_x;
        }

        if ( new_height < g_min_object_height )
        {
          new_height = begin_object_ptr->get_height();
          new_y = begin_object_ptr->get_coordinate().m_y;
        }

        coordinate new_center = { new_x, new_y };

        grape_geometry old_geo = { s_orig_center, s_orig_width, s_orig_height };
        grape_geometry new_geo = { new_center, new_width, new_height };

        grape_event_resize *event = new grape_event_resize( m_main_frame, begin_object_ptr, old_geo, new_geo, !m_mousedown );
        m_main_frame->get_event_handler()->Submit( event, !m_mousedown );
      }
      break;
    }
    case ADD_TERMINATING_TRANSITION:
    {
      // Determine the type of the object in order the perform the correct cast.
      if ( begin_object_ptr->get_type() == STATE )
      {
        grape_event_add_terminating_transition* event = new
            grape_event_add_terminating_transition( m_main_frame,
            static_cast<libgrape::state*> ( begin_object_ptr ), m_up );
        m_main_frame->get_event_handler()->Submit( event, true );
      }
      else if ( begin_object_ptr->get_type() == REFERENCE_STATE )
      {
        grape_event_add_terminating_transition* event = new
            grape_event_add_terminating_transition( m_main_frame,
            static_cast<libgrape::reference_state*> ( begin_object_ptr ), m_up );
        m_main_frame->get_event_handler()->Submit( event, true );
      }
      break;
    }
    case ADD_NONTERMINATING_TRANSITION:
    {
      object* endstate = 0;
      // If so, determine whether the object is a selectable object.
      if ( visual_endobject != 0 ) endstate = visual_endobject->get_selectable_object();
      // If a selectable object is the endstate, determine the type of the begin and end object in order to perform the correct casts.
      if ( ( endstate != 0 ) && ( endstate->get_type() == STATE ) )
      {
        if ( begin_object_ptr->get_type() == STATE )
        {
          grape_event_add_nonterminating_transition* event = new
            grape_event_add_nonterminating_transition( m_main_frame,
            static_cast<libgrape::state*> ( begin_object_ptr ),
            static_cast<libgrape::state*> ( endstate ) );
          m_main_frame->get_event_handler()->Submit( event, true );
        }
        else if ( begin_object_ptr->get_type() == REFERENCE_STATE )
        {
          grape_event_add_nonterminating_transition* event = new
            grape_event_add_nonterminating_transition( m_main_frame,
            static_cast<libgrape::reference_state*> ( begin_object_ptr ),
            static_cast<libgrape::state*> ( endstate ) );
          m_main_frame->get_event_handler()->Submit( event, true );
        }
      }
      else if ( ( endstate != 0 ) && ( endstate->get_type() == REFERENCE_STATE ) )
      {
        if ( begin_object_ptr->get_type() == STATE )
        {
          grape_event_add_nonterminating_transition* event = new
            grape_event_add_nonterminating_transition( m_main_frame,
            static_cast<libgrape::state*> ( begin_object_ptr ),
            static_cast<libgrape::reference_state*> ( endstate ) );
          m_main_frame->get_event_handler()->Submit( event, true );
        }
        else if ( begin_object_ptr->get_type() == REFERENCE_STATE )
        {
          grape_event_add_nonterminating_transition* event = new
            grape_event_add_nonterminating_transition( m_main_frame,
            static_cast<libgrape::reference_state*> ( begin_object_ptr ),
            static_cast<libgrape::reference_state*> ( endstate ) );
          m_main_frame->get_event_handler()->Submit( event, true );
        }
      }
      break;
    }
    case ADD_CHANNEL_COMMUNICATION:
    {
      object* end_object_ptr = 0;
      if ( visual_endobject != 0 ) end_object_ptr = visual_endobject->get_selectable_object();

      // we began in a channel
      if ( begin_object_ptr->get_type() == CHANNEL )
      {
        // and ended in a channel
        if ( ( end_object_ptr != 0 ) && ( end_object_ptr->get_type() == CHANNEL ) )
        {
          channel* chan_1 = static_cast<channel*> ( begin_object_ptr );
          channel* chan_2 = static_cast<channel*> ( end_object_ptr );
          coordinate coord;
          coord.m_x = ( chan_1->get_coordinate().m_x + chan_2->get_coordinate().m_x ) / 2;
          coord.m_y = ( chan_1->get_coordinate().m_y + chan_2->get_coordinate().m_y ) / 2;
          grape_event_add_channel_communication* event = new grape_event_add_channel_communication( m_main_frame, coord, chan_1, chan_2 );
          // Only do something if the two channels are not on the same reference; note this check was disabled because attach DOES allow it. It is enabled because we don't use attach.
          if ( chan_1->get_reference() != chan_2->get_reference() )
          {
            m_main_frame->get_event_handler()->Submit( event, true );
          }
          else
          {
            delete event;
          }
        }
        // or ended in a channel communication
        else if ( ( end_object_ptr != 0 ) && ( end_object_ptr->get_type() == CHANNEL_COMMUNICATION ) )
        {
          channel* chan_ptr = static_cast<channel*> ( begin_object_ptr );
          channel_communication* comm_ptr = static_cast<channel_communication*> ( end_object_ptr );

          grape_event_attach_channel_communication* event = new grape_event_attach_channel_communication( m_main_frame, comm_ptr, chan_ptr );
          // Only do something if none of the channels in the communication are on the same reference as this one.
          // Only do something if the channel is not already in the communication.
          // note this check was disabled because attach DOES allow it. It is enabled because we don't use attach.
          bool all_different_references = true;
          for ( unsigned int i = 0; i < comm_ptr->count_channel(); ++i )
          {
            channel* existing_chan_ptr = comm_ptr->get_attached_channel( i );
            if ( existing_chan_ptr->get_reference() == chan_ptr->get_reference() || existing_chan_ptr->get_id() == chan_ptr->get_id() )
            {
              all_different_references = false;
              break; // break from the for loop, the boolean can never become true again anyway.
            } // end if
          } // end for
          if ( all_different_references )
          {
            m_main_frame->get_event_handler()->Submit( event, true );
          }
          else
          {
            delete event;
          }
        } // end else if
      }
      // or we began in a channel communication
      else if ( begin_object_ptr->get_type() == CHANNEL_COMMUNICATION )
      {
        // and ended in a channel
        if ( ( end_object_ptr != 0 ) && ( end_object_ptr->get_type() == CHANNEL ) )
        {
          channel_communication* comm_ptr = static_cast<channel_communication*> ( begin_object_ptr );
          channel* chan_ptr = static_cast<channel*> ( end_object_ptr );

          grape_event_attach_channel_communication* event = new grape_event_attach_channel_communication( m_main_frame, comm_ptr, chan_ptr );
          // Only do something if none of the channels in the communication are on the same reference as this one.
          // Only do something if the channel is not already in the communication.
          // note this check was disabled because attach DOES allow it. It is enabled because we don't use attach.
          bool all_different_references = true;
          for ( unsigned int i = 0; i < comm_ptr->count_channel(); ++i )
          {
            channel* existing_chan_ptr = comm_ptr->get_attached_channel( i );
            if ( existing_chan_ptr->get_reference() == chan_ptr->get_reference() || existing_chan_ptr->get_id() == chan_ptr->get_id() )
            {
              all_different_references = false;
              break; // break from the for loop, the boolean can never become true again anyway.
            } // end if
          } // end for
          if ( all_different_references )
          {
            m_main_frame->get_event_handler()->Submit( event, true );
          }
          else
          {
            delete event;
          }
        }
      }
      break;
    }
    // Below are the click-events that cannot be handled in this event, because these are simple objects that require just a click.
    case ADD_INITIAL_DESIGNATOR: break;
    case ADD_CHANNEL: break;
    case ADD_STATE: break;
    case ADD_REFERENCE_STATE: break;
    case ADD_PROCESS_REFERENCE: break;
    case ADD_ARCHITECTURE_REFERENCE: break;
    case ADD_COMMENT: break;
    default:
    {
      // Cannot be correct
      // assert( false );
      m_main_frame->get_glcanvas()->set_canvas_state( IDLE );
      break;
    }
  } // end switch
  
  
  //if we added an object
  if ((state == ADD_NONTERMINATING_TRANSITION) || (state == ADD_TERMINATING_TRANSITION) || (state == ADD_STATE) || (state == ADD_REFERENCE_STATE) || (state == ADD_PROCESS_REFERENCE) || (state == ADD_ARCHITECTURE_REFERENCE) || (state == ADD_COMMENT) || (state == ADD_CHANNEL) || (state == ADD_INITIAL_DESIGNATOR) || (state == ADD_NONTERMINATING_TRANSITION))
  {
    //deselect all objects
    grape_event_deselect_all *deselect_event = new grape_event_deselect_all(m_main_frame);
    m_main_frame->get_event_handler()->Submit(deselect_event, false);
  }
  
  return true;
}

bool grape_event_drag::Undo( void )
{
  // cannot be undone
  return true;
}
