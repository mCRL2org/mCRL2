// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_glcanvas.cpp
//
// Implements the OpenGL canvas used to draw objects.

#include <cmath>
#include <algorithm>

#include "grape_glcanvas.h"
#include "grape_frame.h"
#include "architecturereference.h"
#include "mcrl2/utilities/font_renderer.h"
#include "visuals/visualobject.h"
#include "visuals/visualblocked.h"
#include "visuals/visualchannel_communication.h"
#include "visuals/visualcomment.h"
#include "visuals/visualpreamble.h"
#include "visuals/visualvisible.h"
#include "visuals/visualterminating_transition.h"
#include "visuals/visualnonterminating_transition.h"
#include "visuals/visualstate.h"
#include "visuals/visualreference_state.h"
#include "visuals/visualinitial_designator.h"
#include "visuals/visualprocess_reference.h"
#include "visuals/visualarchitecture_reference.h"
#include "visuals/visualchannel.h"
#include "visuals/visualvisibilityframe.h"

using namespace grape::grapeapp;
using namespace mcrl2::utilities;

const float g_frame_border_space = 0.4f;

grape_glcanvas::grape_glcanvas(wxWindow *p_parent, int *p_args, grape_frame* p_main_frame) : wxGLCanvas(p_parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, _T("grape_glcanvas"), p_args, wxNullPalette)
{
  m_main_frame = p_main_frame;

  // initialize glcanvas
  m_viewport_width = 1;
  m_viewport_height = 1;
  m_canvas_state = IDLE;
  m_initialized = false;
  m_scroll_x = 0.0f;
  m_scroll_y = 0.0f;

  // 1.0f GL coords correspond to 300 viewport pixels
  // initial diagram dimensions: [0.0f, 10.0f] x [0.0f, -10.0f]
  m_min_size_x = 0.0f;
  m_max_size_x = 10.0f;
  m_min_size_y = 0.0f;
  m_max_size_y = 10.0f;

  m_h_scroll_range = 1;
  m_v_scroll_range = 1;
  m_h_scroll_pos = 0;
  m_v_scroll_pos = 0;

  update_scrollbars();
  m_visual_objects.Empty();
  m_touched_visual_object = 0;
  m_dragging = false;
  m_mousedown = false;
  m_diagram = 0;

  m_visibility_frame_height = m_max_size_y - 0.5 * g_frame_border_space;
  m_visibility_frame_width = m_max_size_x - 2 * g_frame_border_space;
  m_visibility_frame_coordinate.m_x = m_min_size_x + g_frame_border_space + 0.5 * m_visibility_frame_width;
  m_visibility_frame_coordinate.m_y = -1 * ( m_min_size_y + 0.1f + 0.5 * m_visibility_frame_height );
}

void grape_glcanvas::update_scrollbars(void)
{
  int h_range = (int)m_max_size_x * 300;
  int v_range = (int)m_max_size_y * 300;
  int h_thumb = m_viewport_width;
  int v_thumb = m_viewport_height;
  int h_pos = int(((float)m_h_scroll_pos / (float)m_h_scroll_range) * h_range);
  int v_pos = int(((float)m_v_scroll_pos / (float)m_v_scroll_range) * v_range);
  SetScrollbar(wxHORIZONTAL, h_pos, h_thumb, h_range, true);
  SetScrollbar(wxVERTICAL, v_pos, v_thumb, v_range, true);

  m_h_scroll_range = h_range;
  m_v_scroll_range = v_range;
  m_h_scroll_pos = h_pos;
  m_v_scroll_pos = v_pos;
}

grape_glcanvas::~grape_glcanvas(void)
{
  // free textures in video memory
//  glDeleteTextures(256, &g_pool.m_textures[0]);

  m_visual_objects.Clear();
}

void grape_glcanvas::init_gl(void)
{
  wxSizeEvent sz_evt;
  event_size(sz_evt);

  glShadeModel(GL_SMOOTH);        // enable smooth color shading
  glClearDepth(1.0f);             // depth buffer setup
  glDisable(GL_DEPTH_TEST);       // enable depth buffer
  glDepthFunc(GL_LESS);           // type of depth test

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // nice perspective correction

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // blending necessary for text

  // initialize texturing
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  // load used textures
//  init_textures();

  m_initialized = true;
}

void grape_glcanvas::reset()
{
  // initialize glcanvas
  m_viewport_width = 1;
  m_viewport_height = 1;
  m_canvas_state = IDLE;
  m_initialized = false;
  m_scroll_x = 0.0f;
  m_scroll_y = 0.0f;

  // 1.0f GL coords correspond to 300 viewport pixels
  // initial diagram dimensions: [0.0f, 10.0f] x [0.0f, -10.0f]
  m_min_size_x = 0.0f;
  m_max_size_x = 10.0f;
  m_min_size_y = 0.0f;
  m_max_size_y = 10.0f;

  m_h_scroll_range = 1;
  m_v_scroll_range = 1;
  m_h_scroll_pos = 0;
  m_v_scroll_pos = 0;

  update_scrollbars();
  m_visual_objects.Empty();
  m_touched_visual_object = 0;
  m_dragging = false;
  m_mousedown = false;
  m_diagram = 0;

  m_visibility_frame_height = m_max_size_y - 0.5 * g_frame_border_space;
  m_visibility_frame_width = m_max_size_x - 2 * g_frame_border_space;
  m_visibility_frame_coordinate.m_x = m_min_size_x + g_frame_border_space + 0.5 * m_visibility_frame_width;
  m_visibility_frame_coordinate.m_y = -1 * ( m_min_size_y + 0.1f + 0.5 * m_visibility_frame_height );
}
void grape_glcanvas::draw_visual_objects()
{
  for ( unsigned int i = 0; i < m_visual_objects.GetCount(); ++i )
  {
    visual_object* vis_obj_ptr = m_visual_objects.Item( i );
    vis_obj_ptr->draw();
  }
}

void grape_glcanvas::paint_coordinate( coordinate translation_coordinate )
{
  if( !GetContext() )
  {
    // unable to get device context
    return;
  }

  SetCurrent();


  if( !m_initialized )
  {
    // initialize GL statemachine
    init_gl();
  }

  // clear background
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  glColor3f( 1.0f, 1.0f, 1.0f );

  // translate canvas to account for scrolling
  glTranslatef(translation_coordinate.m_x, translation_coordinate.m_y, 0.0f);

  // draw objects
  draw_visual_objects();

  // flush GL pipeline and don't swap the buffers (hiding this action)
  glFlush();
}

void grape_glcanvas::paint( wxDC &dc )
{
  if( !GetContext() )
  {
    // unable to get device context
    return;
  }

  SetCurrent();


  if( !m_initialized )
  {
    // initialize GL statemachine
    init_gl();
  }

  // clear background
  glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glLoadIdentity();
  glDisable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  glColor3f( 1.0f, 1.0f, 1.0f );

  // translate canvas to account for scrolling
  glTranslatef(m_scroll_x, -m_scroll_y, 0.0f);

  // draw objects
  draw_visual_objects();

  // flush GL pipeline and swap buffers
  glFlush();
  SwapBuffers();
}

void grape_glcanvas::set_max_size( float p_x, float p_y)
{
  m_max_size_x = p_x;
  m_max_size_y = p_y;
}

float grape_glcanvas::get_max_x( void )
{
  return m_max_size_x;
}

float grape_glcanvas::get_max_y( void )
{
  return m_max_size_y;
}

void grape_glcanvas::draw(void)
{
  // reacquire device context and paint
  wxClientDC dc(this);
  paint(dc);
}

void grape_glcanvas::event_paint(wxPaintEvent &p_event)
{
  // acquire device context and paint
  wxPaintDC dc(this);
  paint(dc);
  p_event.Skip();
}

void grape_glcanvas::event_erase_background(wxEraseEvent &p_event)
{
  // do nothing to prevent flickering
}

void grape_glcanvas::event_idle(wxIdleEvent &p_event)
{
  // do not redraw the canvas
}

void grape_glcanvas::event_size(wxSizeEvent &p_event)
{
  // on some platforms this is necessary to update the context properly
  wxGLCanvas::OnSize(p_event);

  GetClientSize(&m_viewport_width, &m_viewport_height); // retrieve viewport

  if(GetContext())
  {
    SetCurrent();                 // bind canvas context

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, m_viewport_width, m_viewport_height);

    // calc new size
    float new_x = (((float)m_viewport_width)/((float)300));
    float new_y = (((float)m_viewport_height)/((float)300));
    gluOrtho2D(0, new_x, -new_y, 0);

    glMatrixMode(GL_MODELVIEW);
  }
  
  update_scrollbars();

  Refresh();
}

void grape_glcanvas::store_scroll_info(void)
{
  m_h_scroll_range = GetScrollRange(wxHORIZONTAL);
  m_v_scroll_range = GetScrollRange(wxVERTICAL);
  m_h_scroll_pos = GetScrollPos(wxHORIZONTAL);
  m_v_scroll_pos = GetScrollPos(wxVERTICAL);
}

coordinate grape_glcanvas::get_canvas_coordinate( int p_event_x, int p_event_y )
{
  coordinate result;
  int logical_x = GetScrollPos(wxHORIZONTAL) + p_event_x;
  int logical_y = GetScrollPos(wxVERTICAL) + p_event_y;
  result.m_x = (((float)logical_x) / ((float)GetScrollRange(wxHORIZONTAL)))*m_max_size_x;
  result.m_y = -(((float)logical_y) / ((float)GetScrollRange(wxVERTICAL)))*m_max_size_y;
  return result;
}

void grape_glcanvas::event_scroll_lineup(wxScrollWinEvent &p_event)
{
  if(p_event.GetOrientation() == wxHORIZONTAL)
  {
    m_scroll_x += 1.0f/3.0f;
    if(m_scroll_x > m_min_size_x)
    {
      m_scroll_x = m_min_size_x;
    }
    SetScrollbar(wxHORIZONTAL, (std::max)(0, GetScrollPos(wxHORIZONTAL)-99), GetScrollThumb(wxHORIZONTAL), GetScrollRange(wxHORIZONTAL));
  }
  else
  {
    m_scroll_y += 1.0f/3.0f;
    if(m_scroll_y > m_min_size_y)
    {
      m_scroll_y = m_min_size_y;
    }
    SetScrollbar(wxVERTICAL, (std::max)(0, GetScrollPos(wxVERTICAL)-99), GetScrollThumb(wxVERTICAL), GetScrollRange(wxVERTICAL));
  }

  store_scroll_info();
  Refresh();
}

void grape_glcanvas::event_scroll_linedown(wxScrollWinEvent &p_event)
{
  if(p_event.GetOrientation() == wxHORIZONTAL)
  {
    m_scroll_x -= 1.0f/3.0f;
    if(m_scroll_x < ((-m_max_size_x)+((float)GetScrollThumb(wxHORIZONTAL)/300.0f)))
    {
      m_scroll_x = ((-m_max_size_x)+((float)GetScrollThumb(wxHORIZONTAL)/300.0f));
    }
    SetScrollbar(wxHORIZONTAL, (std::min)(GetScrollRange(wxHORIZONTAL)-GetScrollThumb(wxHORIZONTAL), GetScrollPos(wxHORIZONTAL)+99), GetScrollThumb(wxHORIZONTAL), GetScrollRange(wxHORIZONTAL));
  }
  else
  {
    m_scroll_y -= 1.0f/3.0f;
    if(m_scroll_y < ((-m_max_size_y)+((float)GetScrollThumb(wxVERTICAL)/300.0f)))
    {
      m_scroll_y = ((-m_max_size_y)+((float)GetScrollThumb(wxVERTICAL)/300.0f));
    }
    SetScrollbar(wxVERTICAL, (std::min)(GetScrollRange(wxVERTICAL)-GetScrollThumb(wxVERTICAL), GetScrollPos(wxVERTICAL)+99), GetScrollThumb(wxVERTICAL), GetScrollRange(wxVERTICAL));
  }

  store_scroll_info();
  Refresh();
}

void grape_glcanvas::event_scroll_thumbtrack(wxScrollWinEvent &p_event)
{
  int thumb_pos = p_event.GetPosition();

  if(p_event.GetOrientation() == wxHORIZONTAL)
  {
      m_scroll_x = -((float)thumb_pos/(float)GetScrollRange(wxHORIZONTAL)) * m_max_size_x;
  }
  else
  {
      m_scroll_y = -((float)thumb_pos/(float)GetScrollRange(wxVERTICAL)) * m_max_size_y;
  }

  store_scroll_info();
  Refresh();
}

void grape_glcanvas::event_scroll_thumbrelease(wxScrollWinEvent &p_event)
{
  SetScrollPos(p_event.GetOrientation(), p_event.GetPosition(), true);
}

void grape_glcanvas::event_scroll_pageup(wxScrollWinEvent &p_event)
{
  if(p_event.GetOrientation() == wxHORIZONTAL)
  {
    m_scroll_x += ((float)GetScrollThumb(wxHORIZONTAL))/300.0f;
    if(m_scroll_x > m_min_size_x)
    {
      m_scroll_x = m_min_size_x;
    }
  }
  else
  {
    m_scroll_y += ((float)GetScrollThumb(wxVERTICAL))/300.0f;
    if(m_scroll_y > m_min_size_y)
    {
      m_scroll_y = m_min_size_y;
    }
  }

  store_scroll_info();
  Refresh();
}

void grape_glcanvas::event_scroll_pagedown(wxScrollWinEvent &p_event)
{
  if(p_event.GetOrientation() == wxHORIZONTAL)
  {
    m_scroll_x -= ((float)GetScrollThumb(wxHORIZONTAL))/300.0f;
    if(m_scroll_x < ((-m_max_size_x)+((float)GetScrollThumb(wxHORIZONTAL)/300.0f)))
    {
      m_scroll_x = ((-m_max_size_x)+((float)GetScrollThumb(wxHORIZONTAL)/300.0f));
    }
  }
  else
  {
    m_scroll_y -= ((float)GetScrollThumb(wxVERTICAL))/300.0f;
    if(m_scroll_y < ((-m_max_size_y)+((float)GetScrollThumb(wxVERTICAL)/300.0f)))
    {
      m_scroll_y = ((-m_max_size_y)+((float)GetScrollThumb(wxVERTICAL)/300.0f));
    }
  }

  store_scroll_info();
  Refresh();
}

void grape_glcanvas::event_mouse_move( wxMouseEvent &p_event )
{
  if ( p_event.Dragging() )
  {
    coordinate clicked_coord = get_canvas_coordinate( p_event.GetX(), p_event.GetY() ) ;

    if ( m_lmouse_down_coordinate.m_x != clicked_coord.m_x || m_lmouse_down_coordinate.m_y != clicked_coord.m_y ) //we are only dragging if there was a displacement
    {
      if ( m_touched_visual_object )
      {
        // select object
        object *obj_ptr = m_touched_visual_object->get_selectable_object();
        if ( obj_ptr && (int)obj_ptr->get_id() == m_touched_visual_object_id )
        {
          m_diagram->select_object( obj_ptr );
        }

        m_dragging = true;
        
        m_main_frame->event_drag( m_touched_visual_object_id, m_lmouse_down_coordinate, clicked_coord, m_touched_click_location, true );
        // clear_visual_objects was called while processing the event, renew m_touched_visual_object properly
        m_touched_visual_object = get_visual_object( obj_ptr );
      }
    }
  }
}

void grape_glcanvas::event_lmouse_down( wxMouseEvent &p_event )
{
  coordinate clicked_coord = get_canvas_coordinate( p_event.GetX(), p_event.GetY() ) ;

  if ( !m_mousedown ) // first mousedown event
  {
    m_mousedown = true;
    m_touched_visual_object = get_selectable_visual_object( clicked_coord );
    if ( m_touched_visual_object )
    {
      object *obj_ptr = m_touched_visual_object->get_selectable_object();
      if ( obj_ptr )
      {
        m_touched_visual_object_id = obj_ptr->get_id();
      }
      else
      {
        m_touched_visual_object_id = -1;
      }
      m_touched_click_location = m_touched_visual_object->is_on_border( clicked_coord );
    }
    else
    {
      m_touched_visual_object_id = -1;
      m_touched_click_location = GRAPE_DIR_NONE;
    }
  }

  m_lmouse_down_coordinate = clicked_coord;
}

void grape_glcanvas::event_lmouse_up(wxMouseEvent &p_event)
{
  // update statusbar
  if ( m_main_frame->get_statusbar()->GetStatusText() == _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") ) {
    if ( ( m_main_frame->get_mode() == GRAPE_MODE_ARCH ) || ( m_main_frame->get_mode() == GRAPE_MODE_PROC ) )
    {
      m_main_frame->get_statusbar()->PopStatusText();
    }
  }

  coordinate released_coordinate = get_canvas_coordinate( p_event.GetX(), p_event.GetY() );
  // if the released_coordinate was outside of the canvas, put it back inside the canvas
  // g_cursor_margin is defined in visuals/geometric.h
  if ( released_coordinate.m_x < 0 )
  {
    released_coordinate.m_x = g_cursor_margin;
  }
  else if ( released_coordinate.m_x > m_max_size_x )
  {
    released_coordinate.m_x = m_max_size_x - g_cursor_margin;
  }
  if ( released_coordinate.m_y > 0 )
  {
    released_coordinate.m_y = -1 * g_cursor_margin;
  }
  else if ( released_coordinate.m_y < -1 * m_max_size_y )
  {
    released_coordinate.m_y = -1 * ( m_max_size_y - g_cursor_margin );
  }
      
  // Determine if we had touched an object upon mouse down
  if ( m_touched_visual_object && m_touched_visual_object_id >= 0 )
  {
   // m_dragging = false;
    if ( m_dragging ) // if touched an object and then dragged
    {
      // inform the frame of the performed event.
      m_main_frame->event_drag( m_touched_visual_object_id, m_lmouse_down_coordinate, released_coordinate, m_touched_click_location, false );
    }
    else // we've clicked the object
    {
      // determine if a Shift key was pressed and inform the frame of the performed event:
      // m_main_frame->event_click( released_coordinate, m_touched_visual_object, p_event.ShiftDown() );
      // multiple selection has been disabled by replacing the shift check with false.
      // multiple selection has been disabled because it caused sementation faults when two objects are selected, one of which attempts to remove the other as result of its own removal. Due to the fact that this second object is selected as well, a second attempt to remove it is performed, and fails horribly.
      m_main_frame->event_click( released_coordinate, m_touched_visual_object, false );
    }
  }
  else // We've not touched an object
  {
    // inform the frame of the performed event.
    m_main_frame->event_click( released_coordinate );
  }

  // reset all information
  m_mousedown = false;
  m_dragging = false;
  m_touched_visual_object = 0;
}

void grape_glcanvas::event_lmouse_doubleclick( wxMouseEvent &p_event )
{
  coordinate coord = get_canvas_coordinate( p_event.GetX(), p_event.GetY() );

  visual_object *selected_obj = get_selectable_visual_object( coord );
  if ( selected_obj )
  {
    m_main_frame->event_doubleclick( selected_obj );
  }
}

canvas_state grape_glcanvas::get_canvas_state( void ) const
{
  return m_canvas_state;
}

void grape_glcanvas::set_canvas_state( const canvas_state &p_canvas_state )
{
  m_canvas_state = p_canvas_state;
}

unsigned int grape_glcanvas::count_visual_object( void )
{
  return m_visual_objects.GetCount();
}

visual_object* grape_glcanvas::get_visual_object( int p_i )
{
  return m_visual_objects.Item( p_i );
}

visual_object *grape_glcanvas::get_visual_object( object *p_obj )
{
  unsigned int count = m_visual_objects.GetCount();
  for ( unsigned int i = 0; i < count; ++i )
  {
    visual_object* vis_obj = m_visual_objects.Item( i );
    if ( vis_obj->get_selectable_object() == p_obj )
    {
      return vis_obj;
    }
  }

  return 0;
}

visual_object* grape_glcanvas::get_selectable_visual_object( coordinate &p_coord )
{
  visual_object* result = 0;
  int count = m_visual_objects.GetCount();
  for ( int i = count - 1; i >= 0; --i )
  {
    visual_object* vis_obj_ptr = m_visual_objects.Item( i );
    if ( vis_obj_ptr->is_inside( p_coord ) )
    {
      return vis_obj_ptr;
    }
  }
  return result;
}

void grape_glcanvas::add_visual_object( visual_object* p_obj )
{
  assert ( p_obj != 0 );
  m_visual_objects.Add( p_obj );
}

void grape_glcanvas::clear_visual_objects( void )
{
  unsigned int count = m_visual_objects.GetCount();
  for ( unsigned int i = 0; i < count; ++i )
  {
    visual_object* del_obj = m_visual_objects.Item( i );
    delete del_obj;
  }
  m_visual_objects.Empty();
}

int grape_glcanvas::count_selected_objects( void )
{
  int result = 0;
  for ( unsigned int i = 0; i < m_visual_objects.GetCount(); ++i)
  {
    visual_object* count_obj = m_visual_objects.Item( i );
    result += count_obj->get_selected() ? 1 : 0;
  }
  return result;
}

void grape_glcanvas::reload_visual_objects( void )
{
  clear_visual_objects();
  if ( m_diagram != 0 ) // if there is no diagram in view, do nothing.
  {
    architecture_diagram* arch_dia = dynamic_cast<architecture_diagram*> ( m_diagram );
    if ( arch_dia != 0 )
    {
      // add the visibility frame.
      visualvisibility_frame* new_frame = new visualvisibility_frame( m_visibility_frame_coordinate, m_visibility_frame_width, m_visibility_frame_height);
      add_visual_object( new_frame );

      // add visuals comment
      for ( unsigned int i = 0; i < arch_dia->count_comment(); ++i )
      {
        comment* comm_ptr = arch_dia->get_comment( i );
        visualcomment* visual_comm = new visualcomment( comm_ptr );
        add_visual_object( visual_comm );
      }

      // add visuals blocked
      for ( unsigned int i = 0; i < arch_dia->count_blocked(); ++i )
      {
        blocked* block_ptr = arch_dia->get_blocked( i );
        visualblocked* visual_block = new visualblocked( block_ptr );
        add_visual_object( visual_block );
      }
      // add visuals visible
      for ( unsigned int i = 0; i < arch_dia->count_visible(); ++i )
      {
        // pass the coordinate, width and height of the visibility frame to the visibles.
        visible* vis_ptr = arch_dia->get_visible( i );
        visualvisible* visual_vis = new visualvisible( vis_ptr, m_visibility_frame_coordinate, m_visibility_frame_width, m_visibility_frame_height );
        add_visual_object( visual_vis );
      }

      // Draw a reference and its own channels immediately thereafter so that no other objects can be placed between them.
      // add visuals architecture reference
      for ( unsigned int i = 0; i < arch_dia->count_architecture_reference(); ++i )
      {
        architecture_reference* arch_ref_ptr = arch_dia->get_architecture_reference( i );
        visualarchitecture_reference* vis_arch_ref = new visualarchitecture_reference( arch_ref_ptr );
        add_visual_object( vis_arch_ref );
      }

      // add visuals process reference
      for ( unsigned int i = 0; i < arch_dia->count_process_reference(); ++i )
      {
        process_reference* proc_ref_ptr = arch_dia->get_process_reference( i );
        visualprocess_reference* vis_proc_ref = new visualprocess_reference( proc_ref_ptr );
        add_visual_object( vis_proc_ref );
      }

      // add visuals channel communication
      for ( unsigned int i = 0; i < arch_dia->count_channel_communication(); ++i )
      {
        channel_communication* c_comm_ptr = arch_dia->get_channel_communication( i );
        visualchannel_communication* visual_c_comm = new visualchannel_communication( c_comm_ptr );
        add_visual_object( visual_c_comm );
      }

      // add visuals channel
      for ( unsigned int i = 0; i < arch_dia->count_channel(); ++i )
      {
        channel* chan_ptr = arch_dia->get_channel( i );
        visualchannel* visual_chan = new visualchannel( chan_ptr );
        add_visual_object( visual_chan );
      }
    }
    else
    {
      process_diagram* proc_dia = dynamic_cast<process_diagram*> ( m_diagram );
      assert( proc_dia != 0 );
      // add visual preamble
      preamble* pre_ptr = proc_dia->get_preamble();
      visualpreamble* vis_pre = new visualpreamble( pre_ptr );
      add_visual_object( vis_pre );

      // add visuals comment
      for ( unsigned int i = 0; i < proc_dia->count_comment(); ++i )
      {
        comment* comm_ptr = proc_dia->get_comment( i );
        visualcomment* visual_comm = new visualcomment( comm_ptr );
        add_visual_object( visual_comm );
      }

      // add visuals initial designator
      for ( unsigned int i = 0; i < proc_dia->count_initial_designator(); ++i )
      {
        initial_designator* init_ptr = proc_dia->get_initial_designator( i );
        visualinitial_designator* vis_init = new visualinitial_designator( init_ptr );
        add_visual_object( vis_init );
      }
      // add visuals terminating transition
      for ( unsigned int i = 0; i < proc_dia->count_terminating_transition(); ++i )
      {
        terminating_transition* trans_ptr = proc_dia->get_terminating_transition( i );
        visualterminating_transition* vis_trans = new visualterminating_transition( trans_ptr );
        add_visual_object( vis_trans );
      }
      // add visuals nonterminating transition
      for ( unsigned int i = 0; i < proc_dia->count_nonterminating_transition(); ++i )
      {
        nonterminating_transition* ntt_ptr = proc_dia->get_nonterminating_transition( i );
        visualnonterminating_transition* vis_ntt = new visualnonterminating_transition( ntt_ptr );
        add_visual_object( vis_ntt );
      }
      // add visuals reference state
      for ( unsigned int i = 0; i < proc_dia->count_reference_state(); ++i )
      {
        reference_state* ref_state_ptr = proc_dia->get_reference_state( i );
        visualreference_state* vis_ref_state = new visualreference_state( ref_state_ptr );
        add_visual_object( vis_ref_state );
      }
      // add visuals state
      for ( unsigned int i = 0; i < proc_dia->count_state(); ++i )
      {
        state* state_ptr = proc_dia->get_state( i );
        visualstate* vis_state = new visualstate( state_ptr );
        add_visual_object( vis_state );
      }
    }
  }
  Refresh();
}

void grape_glcanvas::set_diagram( diagram *p_diagram )
{
  m_diagram = p_diagram;

  if ( !m_diagram )
  {
    clear_visual_objects();
  }
}

diagram* grape_glcanvas::get_diagram( void )
{
  return m_diagram;
}

wxImage grape_glcanvas::get_image( void )
{
  wxInitAllImageHandlers();
  wxSize canvas_size( m_viewport_width, m_viewport_height );
  wxImage canvas_image( canvas_size.x, canvas_size.y);

  // 1.0f GL coords correspond to 300 viewport pixels
  glReadPixels(0, 0, canvas_size.x, canvas_size.y, GL_RGB, GL_UNSIGNED_BYTE, canvas_image.GetData() );
  canvas_image = canvas_image.Mirror( false );
  return canvas_image;
}

bool grape_glcanvas::is_inside_visibility_frame( coordinate &p_coord )
{
  return is_inside_rectangle( m_visibility_frame_coordinate, m_visibility_frame_width, m_visibility_frame_height, p_coord );
}

font_renderer* grape_glcanvas::get_font_renderer( void )
{
  static font_renderer  font_renderer;           /**< A pointer to the font renderer for the text on the visual objects. */
  return &font_renderer;
}

BEGIN_EVENT_TABLE(grape_glcanvas, wxGLCanvas)
  EVT_SIZE(grape_glcanvas::event_size)
  EVT_PAINT(grape_glcanvas::event_paint)
  EVT_ERASE_BACKGROUND(grape_glcanvas::event_erase_background)
  EVT_IDLE(grape_glcanvas::event_idle)
  EVT_SCROLLWIN_LINEUP(grape_glcanvas::event_scroll_lineup)
  EVT_SCROLLWIN_LINEDOWN(grape_glcanvas::event_scroll_linedown)
  EVT_SCROLLWIN_THUMBTRACK(grape_glcanvas::event_scroll_thumbtrack)
  EVT_SCROLLWIN_THUMBRELEASE(grape_glcanvas::event_scroll_thumbrelease)
  EVT_SCROLLWIN_PAGEUP(grape_glcanvas::event_scroll_pageup)
  EVT_SCROLLWIN_PAGEDOWN(grape_glcanvas::event_scroll_pagedown)
  EVT_MOTION(grape_glcanvas::event_mouse_move)
  EVT_LEFT_DCLICK(grape_glcanvas::event_lmouse_doubleclick)
  EVT_LEFT_DOWN(grape_glcanvas::event_lmouse_down)
  EVT_LEFT_UP(grape_glcanvas::event_lmouse_up)
END_EVENT_TABLE()
