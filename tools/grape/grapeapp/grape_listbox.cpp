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
#include "grape_ids.h"

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

#ifndef __WINDOWS__
void grape_listbox::event_click( wxMouseEvent &p_event )
{
  if ( m_main_frame->get_mode() != GRAPE_MODE_DATASPEC )
  {
    if (!IsEmpty())
    {
      if ( m_main_frame->get_statusbar()->GetStatusText() != _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") )
      {
        if ( m_main_frame->get_statusbar()->GetStatusText() == wxEmptyString )
        {
          m_main_frame->get_statusbar()->PopStatusText();
        }
        m_main_frame->get_statusbar()->PushStatusText( _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") );

        wxCommandEvent event;
        m_main_frame->event_menu_deselect_all( event );
      }
    }
    else
    {
      if ( m_main_frame->get_statusbar()->GetStatusText() == _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") )
      {
        m_main_frame->get_statusbar()->PopStatusText();
      }
      m_main_frame->get_statusbar()->PushStatusText(wxEmptyString);
    }
  }
}
#endif

void grape_listbox::event_select( wxCommandEvent &p_event )
{
  if (!IsEmpty())
  {
    if ( m_main_frame->get_statusbar()->GetStatusText() != _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") )
    {
      if ( m_main_frame->get_statusbar()->GetStatusText() != wxEmptyString )
      {
        m_main_frame->get_statusbar()->PopStatusText();
      }
      m_main_frame->get_statusbar()->PushStatusText( _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") );

      wxCommandEvent event;
      m_main_frame->event_menu_deselect_all( event );
    }
  }
  else
  {
    if ( m_main_frame->get_statusbar()->GetStatusText() == _T("Click to select. Double click -> Rename current diagram. Press Delete -> Remove current diagram.") )
    {
      m_main_frame->get_statusbar()->PopStatusText();
    }
    m_main_frame->get_statusbar()->PushStatusText(wxEmptyString);
  }
  
  // process further
  p_event.Skip();
}

BEGIN_EVENT_TABLE(grape_listbox, wxListBox)
  EVT_LISTBOX( GRAPE_ARCHITECTURE_DIAGRAM_LIST, grape_listbox::event_select )
  EVT_LISTBOX( GRAPE_PROCESS_DIAGRAM_LIST, grape_listbox::event_select ) 
  EVT_KEY_UP( grape_listbox::event_delete )
#ifndef __WINDOWS__
  EVT_LEFT_UP( grape_listbox::event_click )
#endif
END_EVENT_TABLE()
