// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_frame_events.cpp
//
// Implements the event handlers of the main frame.

#include "wx.hpp" // precompiled headers

#include <wx/msgdlg.h>
#include <wx/textdlg.h>

#include "grape_frame.h"
#include "grape_events.h"
#include "grape_glcanvas.h"
#include "grape_listbox.h"
#include "grape_ids.h"

using namespace grape::grapeapp;

void grape_frame::event_click( coordinate &p_coord, visual_object* p_vis_obj, bool p_shift_pressed )
{
  // create new event and submit to event handler
  grape_event_click *event = new grape_event_click( this, p_coord, p_vis_obj, p_shift_pressed );
  m_event_handler->Submit( event, false );
  update_bars();
}
void grape_frame::event_click( coordinate &p_coord )
{
  // create new event and submit to event handler
  grape_event_click *event = new grape_event_click( this, p_coord  );
  m_event_handler->Submit( event, false );
  update_bars();
}

void grape_frame::event_doubleclick( visual_object* p_vis_obj )
{
  grape_event_doubleclick *event = new grape_event_doubleclick( this, p_vis_obj );
  m_event_handler->Submit( event, false );
  update_bars();
}

void grape_frame::event_drag( int p_vis_obj_id, coordinate &p_down, coordinate &p_up, grape_direction p_click_location, bool p_mousedown )
{
  // create new event and submit to event handler
  grape_event_drag *event = new grape_event_drag(this, p_vis_obj_id, p_down, p_up, p_click_location, p_mousedown );
  m_event_handler->Submit(event, false);

  // only update toolbar when the mouse is up again; drag complete
  if ( !p_mousedown )
  {
    update_bars();
  }
}

void grape_frame::event_tool_selected(wxCommandEvent& p_event)
{
  canvas_state newstate;

  // select too in toolbar
  GetToolBar()->ToggleTool(p_event.GetId(), true);

  switch(p_event.GetId())
  {
    case GRAPE_TOOL_SELECT:
      newstate = SELECT; break;
    case GRAPE_TOOL_ATTACH:
      newstate = ATTACH; break;
    case GRAPE_TOOL_DETACH:
      newstate = DETACH; break;
    case GRAPE_TOOL_ADD_COMMENT:
      newstate = ADD_COMMENT; break;
    case GRAPE_TOOL_ADD_TERMINATING_TRANSITION:
      newstate = ADD_TERMINATING_TRANSITION; break;
    case GRAPE_TOOL_ADD_NONTERMINATING_TRANSITION:
      newstate = ADD_NONTERMINATING_TRANSITION; break;
    case GRAPE_TOOL_ADD_INITIAL_DESIGNATOR:
      newstate = ADD_INITIAL_DESIGNATOR; break;
    case GRAPE_TOOL_ADD_STATE:
      newstate = ADD_STATE; break;
    case GRAPE_TOOL_ADD_REFERENCE_STATE:
      newstate = ADD_REFERENCE_STATE; break;
    case GRAPE_TOOL_ADD_PROCESS_REFERENCE:
      newstate = ADD_PROCESS_REFERENCE; break;
    case GRAPE_TOOL_ADD_ARCHITECTURE_REFERENCE:
      newstate = ADD_ARCHITECTURE_REFERENCE; break;
    case GRAPE_TOOL_ADD_CHANNEL:
      newstate = ADD_CHANNEL; break;
    case GRAPE_TOOL_ADD_CHANNEL_COMMUNICATION:
      newstate = ADD_CHANNEL_COMMUNICATION; break;
    default: newstate = IDLE; break;
  }

  m_glcanvas->set_canvas_state( newstate );

  // deselect all objects
  grape_event_deselect_all *event = new grape_event_deselect_all(this);
  m_event_handler->Submit(event, false);
  m_glcanvas->SetFocus();
  update_bars();
}

void grape_frame::event_menu_new(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_new *event = new grape_event_new(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_open(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_open *event = new grape_event_open(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_close(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_close *event = new grape_event_close(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_save(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_save *event = new grape_event_save(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_saveas(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_saveas *event = new grape_event_saveas(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_exportmcrl2(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_export_current_diagram_mcrl2 *event = new grape_event_export_current_diagram_mcrl2(this);
  m_event_handler->Submit(event, false);
}

void grape_frame::event_menu_validate_specification(wxCommandEvent &WXUNUSED(p_event))
{
  grape_event_validate_specification *event = new grape_event_validate_specification(this);
  m_event_handler->Submit(event, false);
}

void grape_frame::event_menu_validate_diagram(wxCommandEvent &WXUNUSED(p_event))
{
  grape_event_validate_diagram *event = new grape_event_validate_diagram(this);
  m_event_handler->Submit(event, false);
}

void grape_frame::event_menu_exportimage(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_export_current_diagram_image* event = new grape_event_export_current_diagram_image( this );
  m_event_handler->Submit( event, false );
}

void grape_frame::event_menu_exporttext(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_export_datatype_specification_text* event = new grape_event_export_datatype_specification_text( this );
  m_event_handler->Submit( event, false );
}

void grape_frame::event_menu_print(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_print *event = new grape_event_print(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_quit(wxCommandEvent& WXUNUSED(p_event))
{
  Close( FALSE );
}

void grape_frame::event_menu_undo(wxCommandEvent& WXUNUSED(p_event))
{
  // undo last event
  if(m_event_handler->CanUndo())
  {
    set_mode(GRAPE_MODE_SPEC);
    m_event_handler->Undo();
    wxCommandEvent event;
    event_menu_deselect_all(event);
    m_statusbar->SetStatusText(wxEmptyString);
    update_bars();

    // mark as modified
    set_is_modified( true );
  }
}

void grape_frame::event_menu_redo(wxCommandEvent& WXUNUSED(p_event))
{
  // redo last event
  set_mode(GRAPE_MODE_SPEC);
  m_event_handler->Redo();
  wxCommandEvent event;
  event_menu_deselect_all(event);
  m_statusbar->PopStatusText();
  m_statusbar->PushStatusText( _T("Click -> select object. Drag -> move object. Drag border -> resize object. Double click -> edit object properties.") );
  update_bars();

  // mark as modified
  set_is_modified( true );
}

void grape_frame::event_menu_cut(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_cut *event = new grape_event_cut(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_copy(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_copy *event = new grape_event_copy(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_paste(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_paste *event = new grape_event_paste(this);
  m_event_handler->Submit(event, true);
  update_bars();
}

void grape_frame::event_menu_delete(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_delete_selected_objects* event = new grape_event_delete_selected_objects(this);
  m_event_handler->Submit( event, true );
  update_bars();
}

void grape_frame::event_menu_properties(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_properties* event = new grape_event_properties(this);
  m_event_handler->Submit( event, false );
  update_bars();
}

void grape_frame::event_menu_select_all(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_select_all *event = new grape_event_select_all(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_deselect_all(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_deselect_all *event = new grape_event_deselect_all(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_add_architecture_diagram(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_add_architecture_diagram *event = new grape_event_add_architecture_diagram(this);
  m_event_handler->Submit(event, true);
  update_bars();
}

void grape_frame::event_menu_add_process_diagram(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_add_process_diagram *event = new grape_event_add_process_diagram(this);
  m_event_handler->Submit(event, true);
  update_bars();
}

void grape_frame::event_menu_help(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_help *event = new grape_event_help(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_about(wxCommandEvent& WXUNUSED(p_event))
{
  grape_event_about *event = new grape_event_about(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_select_diagram( wxCommandEvent &p_event )
{
  m_architecture_diagram_list->SetStringSelection(p_event.GetString());
  m_process_diagram_list->SetStringSelection(p_event.GetString());
  grape_event_select_diagram *event = new grape_event_select_diagram(this, p_event.GetString());
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_rename_diagram( wxCommandEvent &p_event)
{
  grape_event_dialog_rename_diagram *event = new grape_event_dialog_rename_diagram(this);
  m_event_handler->Submit(event, false);
  update_bars();
}

void grape_frame::event_menu_remove_diagram( wxCommandEvent &p_event )
{
  diagram* dia_ptr = get_glcanvas()->get_diagram();

  if ( !dia_ptr )
  {
    return;
  }

  int arch = get_architecture_diagram_listbox()->FindString(dia_ptr->get_name());
  int proc = get_process_diagram_listbox()->FindString(dia_ptr->get_name());
  if ( ( ( arch != wxNOT_FOUND ) && ( get_architecture_diagram_listbox()->IsSelected(arch) ) ) || ( ( proc != wxNOT_FOUND ) && ( get_process_diagram_listbox()->IsSelected(proc) ) ) )
  {
    wxString s = _T( "Do you wish to remove the diagram " );
    s += dia_ptr->get_name();
    s += _T( "?" );
    int result = wxMessageBox( s, _T("Question"), wxICON_QUESTION | wxYES_NO, this );

    if ( result == wxYES )
    {
      grape_event_remove_diagram *event = new grape_event_remove_diagram( this, dia_ptr, false );
      m_event_handler->Submit( event, true );
      update_bars();
    }
  }
}

void grape_frame::event_listbox_remove_diagram( int p_diagram_type )
{
  diagram* dia_ptr = get_glcanvas()->get_diagram();

  if ( !dia_ptr )
  {
    return;
  }

  int arch = wxNOT_FOUND;
  int proc = wxNOT_FOUND;
  if (p_diagram_type == GRAPE_ARCHITECTURE_DIAGRAM_LIST)
  {
    arch = get_architecture_diagram_listbox()->FindString(dia_ptr->get_name());
  }
  else if (p_diagram_type == GRAPE_PROCESS_DIAGRAM_LIST)
  {
    proc = get_process_diagram_listbox()->FindString(dia_ptr->get_name());
  }

  if ( ( ( arch != wxNOT_FOUND ) && ( get_architecture_diagram_listbox()->IsSelected(arch) ) ) || ( ( proc != wxNOT_FOUND ) && ( get_process_diagram_listbox()->IsSelected(proc) ) ) )
  {
    wxString s = _T( "Do you wish to remove the diagram " );
    s += dia_ptr->get_name();
    s += _T( "?" );
    int result = wxMessageBox( s, _T("Question"), wxICON_QUESTION | wxYES_NO, this );

    if ( result == wxYES )
    {
      grape_event_remove_diagram *event = new grape_event_remove_diagram( this, dia_ptr, false );
      m_event_handler->Submit( event, true );
      update_bars();
    }
  }
}

void grape_frame::event_window_close( wxCloseEvent &p_event )
{
  grape_event_close *event = new grape_event_close( this );
  bool result = m_event_handler->Submit( event, false );

  // destroy the window when closing went successful,
  // or when we're forced to destroy the window
  if ( result || !p_event.CanVeto() )
  {
    this->Destroy();
  }
  else
  {
    // somehow closing failed, don't kill the window. use our veto.
    p_event.Veto();
  }
}

void grape_frame::event_datatype_spec( wxCommandEvent& p_event )
{
  // if the datatype button was toggled (checked), load the data specification
  set_mode( p_event.IsChecked() ? GRAPE_MODE_DATASPEC : GRAPE_MODE_SPEC );
  update_bars();
}

void grape_frame::grape_event_timer( wxTimerEvent &p_event )
{
  m_splitter->SetSashGravity( 1.0 );
}

void grape_frame::dataspec_modified( wxCommandEvent & WXUNUSED(p_event) )
{
  set_is_modified( true );
}


