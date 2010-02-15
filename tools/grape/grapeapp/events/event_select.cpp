// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_select.cpp
//
// Defines GraPE events for selection.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "grape_menubar.h"
#include "grape_ids.h"

#include "event_select.h"

using namespace grape::grapeapp;

grape_event_select::grape_event_select( grape_frame *p_main_frame, object* p_obj, bool p_shift_pressed )
: grape_event_base( p_main_frame, false, _T( "select item" ) )
{
  m_obj = p_obj;
  m_shift_pressed = p_shift_pressed;
}

grape_event_select::~grape_event_select( void )
{
}

bool grape_event_select::Do( void )
{
  diagram* dia = m_main_frame->get_glcanvas()->get_diagram();
  if ( m_shift_pressed )
  {
    dia->plus_select_object( m_obj );
  }
  else
  {
    dia->select_object( m_obj );
  }

  // update menubar
  m_main_frame->get_menubar()->Enable( wxID_DELETE, true );
  //m_main_frame->get_menubar()->Enable( GRAPE_MENU_DESELECT_ALL, true );
  // refresh visuals
  m_main_frame->get_glcanvas()->reload_visual_objects();
  return true;
}

bool grape_event_select::Undo( void )
{
  return true;
}



grape_event_select_all::grape_event_select_all( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, false, _T( "select all" ) )
{
}

grape_event_select_all::~grape_event_select_all( void )
{
}

bool grape_event_select_all::Do( void )
{
  diagram* dia = m_main_frame->get_glcanvas()->get_diagram();
  dia->select_all_objects();

  // canvas refresh
  m_main_frame->get_glcanvas()->Refresh();

  // update menubar
  m_main_frame->get_menubar()->Enable( wxID_DELETE, true );
  //m_main_frame->get_menubar()->Enable( GRAPE_MENU_DESELECT_ALL, true );
  return true;
}

bool grape_event_select_all::Undo( void )
{
  return true;
}



grape_event_plus_select::grape_event_plus_select( grape_frame *p_main_frame, object* p_obj )
: grape_event_base( p_main_frame, false, _T( "select several items" ) )
{
  m_obj = p_obj;
}

grape_event_plus_select::~grape_event_plus_select( void )
{
}

bool grape_event_plus_select::Do( void )
{
  diagram* dia = m_main_frame->get_glcanvas()->get_diagram();
  dia->plus_select_object( m_obj );

  // refresh canvas
  m_main_frame->get_glcanvas()->Refresh();

  // update menubar
  m_main_frame->get_menubar()->Enable( wxID_DELETE, true );
  //m_main_frame->get_menubar()->Enable( GRAPE_MENU_DESELECT_ALL, true );
  return true;
}

bool grape_event_plus_select::Undo( void )
{
  return true;
}



grape_event_deselect::grape_event_deselect( grape_frame *p_main_frame, object* p_obj )
: grape_event_base( p_main_frame, false, _T( "deselect item" ) )
{
  m_obj = p_obj;
}

grape_event_deselect::~grape_event_deselect( void )
{
}

bool grape_event_deselect::Do( void )
{
  diagram* dia = m_main_frame->get_glcanvas()->get_diagram();
  dia->deselect_object( m_obj );

  // refresh canvas
  m_main_frame->get_glcanvas()->Refresh();

  if ( dia->count_selected_objects() == 0 )
  {
    // update menubar
    m_main_frame->get_menubar()->Enable( wxID_DELETE, false );
    //m_main_frame->get_menubar()->Enable( GRAPE_MENU_DESELECT_ALL, false );
  }
  return true;
}

bool grape_event_deselect::Undo( void )
{
  return true;
}

grape_event_deselect_all::grape_event_deselect_all( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, true, _T( "deselect all items" ) )
{
}

grape_event_deselect_all::~grape_event_deselect_all( void )
{
}

bool grape_event_deselect_all::Do( void )
{
  diagram* dia = m_main_frame->get_glcanvas()->get_diagram();
  if (dia != 0)
  {
    dia->deselect_all_objects();
  }

  // refresh canvas
  m_main_frame->get_glcanvas()->Refresh();

  // update menubar
  m_main_frame->get_menubar()->Enable( wxID_DELETE, false );
  //m_main_frame->get_menubar()->Enable( GRAPE_MENU_DESELECT_ALL, false );
  return true;
}

bool grape_event_deselect_all::Undo( void )
{
  return true;
}
