// Author(s): Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_listbox.cpp
//
// Implements the listbox class used to list diagrams

#include "grape_listbox.h"
#include "grape_frame.h"

using namespace grape::grapeapp;

grape_listbox::grape_listbox( wxWindow* p_parent, wxWindowID p_id, grape_frame* p_main_frame ) : wxListBox(p_parent, p_id, wxDefaultPosition, wxSize( 200, 200 ), 0, 0, wxLB_SORT )
{
  m_main_frame = p_main_frame;
  m_diagram_type = p_id;
}

grape_listbox::~grape_listbox( void )
{
}

void grape_listbox::event_delete( wxKeyEvent &p_event )
{
  if ( p_event.GetKeyCode() == WXK_DELETE )
  {
    m_main_frame->event_listbox_remove_diagram( m_diagram_type );
  }
}

void grape_listbox::event_click( wxMouseEvent &p_event )
{
  if (!IsEmpty())
  {
    wxCommandEvent event;
    m_main_frame->event_menu_deselect_all( event );
  }
}

void grape_listbox::event_enter( wxMouseEvent &p_event )
{
  if ( m_main_frame->get_statusbar()->GetStatusText() != _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") )
  {
    m_main_frame->get_statusbar()->PushStatusText( _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") );
  }
}

BEGIN_EVENT_TABLE(grape_listbox, wxListBox)
  EVT_KEY_UP( grape_listbox::event_delete )
  EVT_LEFT_UP( grape_listbox::event_click )
  EVT_MOTION( grape_listbox::event_enter )
END_EVENT_TABLE()
