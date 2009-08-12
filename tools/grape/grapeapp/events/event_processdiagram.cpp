// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_processdiagram.cpp
//
// Defines GraPE events for process diagrams

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "grape_listbox.h"
#include "dialogs/preambledialog.h"

#include "event_diagram.h"

#include "event_processdiagram.h"

using namespace grape::grapeapp;

grape_event_add_process_diagram::grape_event_add_process_diagram( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, true, _T( "add process diagram" ) )
{
  m_proc = m_main_frame->get_new_id();
}

grape_event_add_process_diagram::~grape_event_add_process_diagram( void )
{
}

bool grape_event_add_process_diagram::Do( void )
{
  // add process diagram to grapespecification
  grape_specification* spec = m_main_frame->get_grape_specification();
  assert( spec != 0 );
  process_diagram* new_proc_ptr = spec->add_process_diagram( m_proc );

  m_main_frame->get_glcanvas()->set_diagram( new_proc_ptr );

  m_main_frame->set_mode( GRAPE_MODE_PROC );

  // update process diagram listbox
  grape_listbox *proc_listbox = m_main_frame->get_process_diagram_listbox();
  int pos = proc_listbox->Append( new_proc_ptr->get_name() );
  proc_listbox->Select( pos );

  // check whether any processreferences or referencestates exist that point to the new diagram.
  spec->check_references( new_proc_ptr->get_name(), new_proc_ptr );

  finish_modification();
  return true;
}

bool grape_event_add_process_diagram::Undo( void )
{
  grape_specification *spec = m_main_frame->get_grape_specification();
  // Retrieve the to be deleted process diagram
  process_diagram* del_proc_ptr = static_cast<process_diagram*> ( find_diagram( m_proc , GRAPE_PROCESS_DIAGRAM ) );

  // check whether any processreferences or referencestates exist that should no longer point to the diagram.
  process_diagram* dummy_ptr = 0;
  spec->check_references( del_proc_ptr->get_name(), dummy_ptr );

  // update process listbox
  grape_listbox *proc_listbox = m_main_frame->get_process_diagram_listbox();
  int pos = proc_listbox->FindString( del_proc_ptr->get_name(), true );
  proc_listbox->Delete( pos );
  if ( pos > 0 )
  {
    proc_listbox->Select( pos - 1 );
  }
  else
  {
    // if the process diagram listbox is empty, there will be no selection, try architecture diagram listbox
    grape_listbox *arch_listbox = m_main_frame->get_architecture_diagram_listbox();
    if ( !arch_listbox->IsEmpty() )
    {
      arch_listbox->Select( 0 );
    }
  }

  // update grape mode
  wxString m_selected_diagram = proc_listbox->GetStringSelection();
  if ( m_selected_diagram == wxEmptyString )
  {
    // if this is also empty there will be no selection; grape_event_select_diagram will process according to that.
    m_selected_diagram = m_main_frame->get_architecture_diagram_listbox()->GetStringSelection();
  }

  // select new diagram
  grape_event_select_diagram *event = new grape_event_select_diagram(m_main_frame, m_selected_diagram);
  m_main_frame->get_event_handler()->Submit(event, false);

  // remove process diagram from grapespecification
  spec->remove_process_diagram( del_proc_ptr );

  finish_modification();
  return true;
}



grape_event_change_preamble::grape_event_change_preamble( grape_frame *p_main_frame, preamble *p_preamble, bool p_edit_parameter )
: grape_event_base( p_main_frame, true, _T( "edit preamble" ) )
, m_preamble( p_preamble )
{
  m_old_parameter_decls = p_preamble->get_parameter_declarations();
  m_old_local_var_decls = p_preamble->get_local_variable_declarations();

  grape_preamble_dialog dialog( m_preamble, p_edit_parameter );
  m_ok_pressed = dialog.ShowModal();

  if ( m_ok_pressed )
  {
    m_new_parameter_decls = dialog.get_parameter_declarations();
    m_new_local_var_decls = dialog.get_local_variable_declarations();
  }
}

grape_event_change_preamble::~grape_event_change_preamble( void )
{
}

bool grape_event_change_preamble::Do( void )
{
  if ( !m_ok_pressed )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  m_preamble->set_parameter_declarations( m_new_parameter_decls );
  m_preamble->set_local_variable_declarations( m_new_local_var_decls );

  finish_modification();
  return true;
}

bool grape_event_change_preamble::Undo( void )
{
  m_preamble->set_parameter_declarations( m_old_parameter_decls );
  m_preamble->set_local_variable_declarations( m_old_local_var_decls );

  finish_modification();
  return true;
}
