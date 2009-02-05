// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_diagram.cpp
//
// Defines GraPE events for diagrams

#include <wx/xml/xml.h>
#include <iostream>
#include <sstream>

#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "grape_listbox.h"
#include "grape_logpanel.h"

#include "event_select.h"

#include "event_diagram.h"

#include "dialogs/parameterdialog.h"

#include "libgrape/xml.h"
#include "mcrl2gen/mcrl2gen.h"



using namespace grape::libgrape;
using namespace grape::grapeapp;
using namespace grape::mcrl2gen;

using namespace std;


grape_event_select_diagram::grape_event_select_diagram(grape_frame *p_main_frame, const wxString &p_diagram_name)
: grape_event_base(p_main_frame, false, _T("diagram select"))
{
  m_diagram_name = p_diagram_name;
  diagram* dia_ptr = find_diagram( p_diagram_name );

  m_diagram = 0;
  m_diagram_type = GRAPE_NO_DIAGRAM;
  if ( dia_ptr != 0 )
  {
    m_diagram = dia_ptr->get_id();
    // perform a dynamic cast to determine the diagram type
    process_diagram* proc_dia_ptr = dynamic_cast<process_diagram*> ( dia_ptr );
    if ( proc_dia_ptr != 0 )
    {
      m_diagram_type = GRAPE_PROCESS_DIAGRAM;
    }
    else
    {
      assert(  dynamic_cast<architecture_diagram*> ( dia_ptr ) != 0 );
      m_diagram_type = GRAPE_ARCHITECTURE_DIAGRAM;
    }
  }
}

grape_event_select_diagram::~grape_event_select_diagram(void)
{
}

bool grape_event_select_diagram::Do(void)
{
  switch ( m_diagram_type )
  {
    case GRAPE_PROCESS_DIAGRAM:
    {
      diagram* dia_ptr = find_diagram( m_diagram );
      m_main_frame->get_glcanvas()->set_diagram( dia_ptr );
      m_main_frame->set_mode( GRAPE_MODE_PROC );
      break;
    }
    case GRAPE_ARCHITECTURE_DIAGRAM:
    {
      diagram* arch_ptr = find_diagram( m_diagram );
      m_main_frame->get_glcanvas()->set_diagram( arch_ptr );
      m_main_frame->set_mode( GRAPE_MODE_ARCH );
      break;
    }
    default:
    {
      m_main_frame->get_glcanvas()->set_diagram( 0 );
      m_main_frame->set_mode( GRAPE_MODE_SPEC );
      break;
    }
  }
  // deselect all objects if a diagram is selected
  if ( m_main_frame->get_glcanvas()->get_diagram() )
  {
    grape_event_deselect_all* event = new grape_event_deselect_all( m_main_frame );
    m_main_frame->get_event_handler()->Submit(event, false );
  }

  // refresh visuals
  m_main_frame->get_glcanvas()->reload_visual_objects();
  return true;
}

bool grape_event_select_diagram::Undo(void)
{
  // cannot be undone
  return true;
}

grape_event_rename_diagram::grape_event_rename_diagram(grape_frame *p_main_frame, const wxString &p_new_name, diagram* p_dia)
: grape_event_base(p_main_frame, true, _T("rename"))
{
  m_diagram = p_dia->get_id();
  m_orig_name = p_dia->get_name();
  m_new_name = p_new_name;
  m_diagram = p_dia->get_id();

  m_diagram_type = GRAPE_NO_DIAGRAM;
  if ( p_dia != 0 )
  {
    // perform a dynamic cast to determine the diagram type
    process_diagram* proc_dia_ptr = dynamic_cast<process_diagram*> ( p_dia );
    if ( proc_dia_ptr != 0 )
    {
      m_diagram_type = GRAPE_PROCESS_DIAGRAM;
    }
    else
    {
      assert(  dynamic_cast<architecture_diagram*> ( p_dia ) != 0 );
      m_diagram_type = GRAPE_ARCHITECTURE_DIAGRAM;
    }
  }
}

grape_event_rename_diagram::~grape_event_rename_diagram( void )
{
}

bool grape_event_rename_diagram::Do( void )
{
  grape_specification *spec = m_main_frame->get_grape_specification();
  diagram* dia_ptr = find_diagram( m_diagram );
  // update grapespecification
  dia_ptr->set_name( m_new_name );

  switch ( m_diagram_type )
  {
    case GRAPE_PROCESS_DIAGRAM:
    {
      m_main_frame->set_mode( GRAPE_MODE_PROC );
      // check whether any architecturereferences exist that should no longer point to the old name.
      spec->check_references( m_new_name, static_cast<process_diagram*> ( dia_ptr ) );
      grape_listbox *proc_list = m_main_frame->get_process_diagram_listbox();
      proc_list->SetString( proc_list->FindString( m_orig_name ), m_new_name );
      proc_list->SetStringSelection( m_new_name );
      // show selected diagram
      grape_event_select_diagram *event = new grape_event_select_diagram(m_main_frame, m_new_name);
      m_main_frame->get_event_handler()->Submit(event, false);
      break;
    }
    case GRAPE_ARCHITECTURE_DIAGRAM:
    {
      m_main_frame->set_mode( GRAPE_MODE_ARCH );
      // check whether any  architecturereferences exist that should point to the new name.
      spec->check_references( m_new_name, static_cast<architecture_diagram*> ( dia_ptr ) );
      grape_listbox *arch_list = m_main_frame->get_architecture_diagram_listbox();
      arch_list->SetString( arch_list->FindString( m_orig_name ), m_new_name );
      arch_list->SetStringSelection( m_new_name );
      // show selected diagram
      grape_event_select_diagram *event = new grape_event_select_diagram(m_main_frame, m_new_name);
      m_main_frame->get_event_handler()->Submit(event, false);
      break;
    }
    default: m_main_frame->set_mode( GRAPE_MODE_SPEC ); break;
  }

  finish_modification();
  return true;
}

bool grape_event_rename_diagram::Undo( void )
{
  grape_specification *spec = m_main_frame->get_grape_specification();
  diagram* dia_ptr = find_diagram( m_diagram );

  // update grapespecification
  dia_ptr->set_name( m_orig_name );

  switch ( m_diagram_type )
  {
    case GRAPE_PROCESS_DIAGRAM:
    {
      m_main_frame->set_mode( GRAPE_MODE_PROC );
      // check whether any architecturereferences exist that should no longer point to the old name.
      spec->check_references( m_new_name, static_cast<process_diagram*> ( dia_ptr ) );
      grape_listbox *proc_list = m_main_frame->get_process_diagram_listbox();
      proc_list->SetString( proc_list->FindString( m_new_name ), m_orig_name );
      proc_list->SetStringSelection( m_orig_name );
      break;
    }
    case GRAPE_ARCHITECTURE_DIAGRAM:
    {
      m_main_frame->set_mode( GRAPE_MODE_ARCH );
      // check whether any  architecturereferences exist that should point to the old name.
      spec->check_references( m_new_name, static_cast<architecture_diagram*> ( dia_ptr ) );
      grape_listbox *arch_list = m_main_frame->get_architecture_diagram_listbox();
      arch_list->SetString( arch_list->FindString( m_new_name ), m_orig_name );
      arch_list->SetStringSelection( m_orig_name );
      break;
    }
    default: m_main_frame->set_mode( GRAPE_MODE_SPEC ); break;
  }

  finish_modification();
  return true;
}

grape_event_dialog_rename_diagram::grape_event_dialog_rename_diagram(grape_frame *p_main_frame )
: grape_event_base(p_main_frame, false, _T("rename diagram"))
{
  m_old_name = wxEmptyString;
  m_new_name = wxEmptyString;
}

grape_event_dialog_rename_diagram::~grape_event_dialog_rename_diagram(void)
{
}

bool grape_event_dialog_rename_diagram::Do(void)
{
  bool retry = false;
  m_old_name = m_main_frame->get_architecture_diagram_listbox()->GetStringSelection();
  if ( m_old_name.IsEmpty() )
  {
    m_old_name = m_main_frame->get_process_diagram_listbox()->GetStringSelection();
  }

  arr_architecture_diagram *arr_arch_dia = m_main_frame->get_grape_specification()->get_architecture_diagram_list();
  arr_process_diagram *arr_proc_dia = m_main_frame->get_grape_specification()->get_process_diagram_list();

  wxString p_new_name = wxGetTextFromUser( _T("Please enter a name for the diagram."), _T("Rename diagram"), m_old_name, m_main_frame );
  if ( p_new_name.IsEmpty() || ( p_new_name == m_old_name ) )
  {
    return false;
  }
  else
  {
    for ( unsigned int i = 0; i < arr_arch_dia->GetCount(); ++i )
    {
      if ( ( arr_arch_dia->Item( i ).get_name() == p_new_name ) && ( arr_arch_dia->Item( i ).get_name() != m_old_name ) )
      {
        wxMessageBox( _T("The name is already in the specification."), _T("Notification"), wxOK | wxICON_INFORMATION, m_main_frame );
        retry = true;
        break;
      }
    }
    for ( unsigned int i = 0; i < arr_proc_dia->GetCount(); ++i )
    {
      if ( ( arr_proc_dia->Item( i ).get_name() == p_new_name ) && ( arr_proc_dia->Item( i ).get_name() != m_old_name ) )
      {
        wxMessageBox( _T("The name is already in the specification."), _T("Notification"), wxOK | wxICON_INFORMATION, m_main_frame );
        retry = true;
        break;
      }
    }

    if ( retry )
    {
      grape_event_dialog_rename_diagram *event = new grape_event_dialog_rename_diagram( m_main_frame );
      return m_main_frame->get_event_handler()->Submit( event, false );
    }
    else
    {
      // retrieve the to be renamed diagram
      diagram* dia_ptr = find_diagram( m_old_name );
      assert( dia_ptr != 0 );

      // save the rename operation
      m_new_name = p_new_name;

      grape_event_rename_diagram *event = new grape_event_rename_diagram(m_main_frame, m_new_name, dia_ptr);
      return m_main_frame->get_event_handler()->Submit( event, true );
    }
  }
  return true;
}

bool grape_event_dialog_rename_diagram::Undo(void)
{
  // cannot be undone
  return true;
}

grape_event_remove_diagram::grape_event_remove_diagram(grape_frame *p_main_frame, diagram* p_dia_ptr, bool p_normal )
: grape_event_base(p_main_frame, true, _T("remove diagram"))
{
  m_comments.Empty();
  m_reference_states.Empty();
  m_states.Empty();
  m_terminating_transitions.Empty();
  m_nonterminating_transitions.Empty();
  m_initial_designators.Empty();
  m_channels.Empty();
  m_channel_communications.Empty();
  m_architecture_references.Empty();
  m_process_references.Empty();
  m_blockeds.Empty();
  m_visibles.Empty();

  // retrieve the current diagram.
  assert( p_dia_ptr != 0 );
  m_diagram = p_dia_ptr->get_id();
  m_name = p_dia_ptr->get_name();
  architecture_diagram* arch_dia_ptr = dynamic_cast<architecture_diagram*> ( p_dia_ptr );
  if ( arch_dia_ptr != 0 )
  {
    m_type = GRAPE_ARCHITECTURE_DIAGRAM;

    // Fill the corresponding remove event arrays.
    for ( unsigned int i = 0; i < arch_dia_ptr->count_comment(); ++i )
    {
      comment* comm_ptr = arch_dia_ptr->get_comment( i );
      grape_event_remove_comment* event = new grape_event_remove_comment( m_main_frame, comm_ptr, arch_dia_ptr );
      m_comments.Add( event );
    }
    for ( unsigned int i = 0; i < arch_dia_ptr->count_architecture_reference(); ++i )
    {
      architecture_reference* arch_ref_ptr = arch_dia_ptr->get_architecture_reference( i );
      grape_event_remove_architecture_reference* event = new
        grape_event_remove_architecture_reference( m_main_frame, arch_ref_ptr, arch_dia_ptr, p_normal );
      m_architecture_references.Add( event );
    }
    for ( unsigned int i = 0; i < arch_dia_ptr->count_process_reference(); ++i )
    {
      process_reference* proc_ref_ptr = arch_dia_ptr->get_process_reference( i );
      grape_event_remove_process_reference* event = new
        grape_event_remove_process_reference( m_main_frame, proc_ref_ptr, arch_dia_ptr, p_normal );
      m_process_references.Add( event );
    }
    if ( !p_normal )
    {
      for ( unsigned int i = 0; i < arch_dia_ptr->count_channel(); ++i )
      {
        channel* chan_ptr = arch_dia_ptr->get_channel( i );
        grape_event_remove_channel* event = new
          grape_event_remove_channel( m_main_frame, chan_ptr, arch_dia_ptr, p_normal );
        m_channels.Add( event );
      }
      for ( unsigned int i = 0; i < arch_dia_ptr->count_channel_communication(); ++i )
      {
        channel_communication* comm_ptr = arch_dia_ptr->get_channel_communication( i );
        grape_event_remove_channel_communication* event = new
          grape_event_remove_channel_communication( m_main_frame, comm_ptr, arch_dia_ptr, p_normal );
        m_channel_communications.Add( event );
      }
    }
    for ( unsigned int i = 0; i < arch_dia_ptr->count_blocked(); ++i )
    {
      blocked* block_ptr = arch_dia_ptr->get_blocked( i );
      if ( p_normal )
      {
        // Only create remove events for blocked that are not connected to a channel.
        if ( block_ptr->get_attached_connection() == 0 )
        {
          grape_event_remove_blocked* event = new
            grape_event_remove_blocked( m_main_frame, block_ptr, arch_dia_ptr );
          m_blockeds.Add( event );
        }
      }
      else
      {
        // create remove for all
        grape_event_remove_blocked* event = new
          grape_event_remove_blocked( m_main_frame, block_ptr, arch_dia_ptr );
        m_blockeds.Add( event );
      }
    }
    for ( unsigned int i = 0; i < arch_dia_ptr->count_visible(); ++i )
    {
      // Only create remove events for visible that are not connected to a channel.
      visible* vis_ptr = arch_dia_ptr->get_visible( i );
      if ( p_normal )
      {
        // Only create remove events for visible that are not connected to a channel.
        if ( vis_ptr->get_attached_connection() == 0 )
        {
          grape_event_remove_visible* event = new
            grape_event_remove_visible( m_main_frame, vis_ptr, arch_dia_ptr );
          m_visibles.Add( event );
        }
      }
      else
      {
        grape_event_remove_visible* event = new
          grape_event_remove_visible( m_main_frame, vis_ptr, arch_dia_ptr );
        m_visibles.Add( event );
      }
    }
  }
  else
  {
    process_diagram* proc_dia_ptr = dynamic_cast<process_diagram*> ( p_dia_ptr );
    assert( proc_dia_ptr != 0 );
    m_type = GRAPE_PROCESS_DIAGRAM;

    m_preamble = *(proc_dia_ptr->get_preamble());

    // Fill the corresponding remove event arrays.
    for ( unsigned int i = 0; i < proc_dia_ptr->count_comment(); ++i )
    {
      comment* comm_ptr = proc_dia_ptr->get_comment( i );
      grape_event_remove_comment* event = new grape_event_remove_comment( m_main_frame, comm_ptr, proc_dia_ptr );
      m_comments.Add( event );
    }
    for ( unsigned int i = 0; i < proc_dia_ptr->count_terminating_transition(); ++i )
    {
      terminating_transition* tt_ptr = proc_dia_ptr->get_terminating_transition( i );
      grape_event_remove_terminating_transition* event = new grape_event_remove_terminating_transition( m_main_frame, tt_ptr, proc_dia_ptr );
      m_terminating_transitions.Add( event );
    }
    for ( unsigned int i = 0; i < proc_dia_ptr->count_nonterminating_transition(); ++i )
    {
      nonterminating_transition* ntt_ptr = proc_dia_ptr->get_nonterminating_transition( i );
      grape_event_remove_nonterminating_transition* event = new grape_event_remove_nonterminating_transition( m_main_frame, ntt_ptr, proc_dia_ptr );
      m_nonterminating_transitions.Add( event );
    }
    for ( unsigned int i = 0; i < proc_dia_ptr->count_initial_designator(); ++i )
    {
      initial_designator* init_ptr = proc_dia_ptr->get_initial_designator( i );
      grape_event_remove_initial_designator* event = new grape_event_remove_initial_designator( m_main_frame, init_ptr, proc_dia_ptr );
      m_initial_designators.Add( event );
    }
    for ( unsigned int i = 0; i < proc_dia_ptr->count_reference_state(); ++i )
    {
      reference_state* ref_state_ptr = proc_dia_ptr->get_reference_state( i );
      // boolean flag set to p_normal to indicate if it's a normal event. See documentation of m_normal of this event.
      grape_event_remove_reference_state* event = new grape_event_remove_reference_state( m_main_frame, ref_state_ptr, proc_dia_ptr, p_normal );
      m_reference_states.Add( event );
    }
    for ( unsigned int i = 0; i < proc_dia_ptr->count_state(); ++i )
    {
      state* state_ptr = proc_dia_ptr->get_state( i );
      // boolean flag set to p_normal to indicate if it's a normal event. See documentation of m_normal of this event.
      grape_event_remove_state* event = new grape_event_remove_state( m_main_frame, state_ptr, proc_dia_ptr, p_normal );
      m_states.Add( event );
    }
  }
}

grape_event_remove_diagram::~grape_event_remove_diagram( void )
{
  m_comments.Clear();
  m_reference_states.Clear();
  m_states.Clear();
  m_terminating_transitions.Clear();
  m_nonterminating_transitions.Clear();
  m_initial_designators.Clear();
  m_channels.Clear();
  m_architecture_references.Clear();
  m_process_references.Clear();
  m_blockeds.Clear();
  m_visibles.Clear();
}

bool grape_event_remove_diagram::Do( void )
{
  grape_specification* spec = m_main_frame->get_grape_specification();
  if ( m_type == GRAPE_ARCHITECTURE_DIAGRAM )
  {
    architecture_diagram* del_arch_dia_ptr = static_cast<architecture_diagram*> ( find_diagram( m_diagram, m_type ) );
    for ( unsigned int i = 0; i < m_blockeds.GetCount(); ++i )
    {
      grape_event_remove_blocked event = m_blockeds.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_visibles.GetCount(); ++i )
    {
      grape_event_remove_visible event = m_visibles.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_channel_communications.GetCount(); ++i )
    {
      grape_event_remove_channel_communication event = m_channel_communications.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_channels.GetCount(); ++i )
    {
      grape_event_remove_channel event = m_channels.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_architecture_references.GetCount(); ++i )
    {
      grape_event_remove_architecture_reference event = m_architecture_references.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_process_references.GetCount(); ++i )
    {
      grape_event_remove_process_reference event = m_process_references.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
    {
      grape_event_remove_comment event = m_comments.Item( i );
      event.Do();
    }

    // check whether any architecturereferences exist that should no longer point to the diagram.
    architecture_diagram* dummy = 0;
    spec->check_references( del_arch_dia_ptr->get_name(), dummy );

    // update architecture listbox
    grape_listbox *arch_listbox = m_main_frame->get_architecture_diagram_listbox();
    int pos = arch_listbox->FindString( del_arch_dia_ptr->get_name() );
    if ( pos != wxNOT_FOUND )
    {
      arch_listbox->Delete( pos );
      if ( pos > 0 )
      {
        arch_listbox->Select( pos - 1 );
      }
      else
      {
	if (!arch_listbox->IsEmpty())
        {
          arch_listbox->Select( 0 );
        }
        else
        {
          // if the architecture diagram listbox is empty, there will be no selection, try process diagram listbox
          if (!m_main_frame->get_process_diagram_listbox()->IsEmpty())
          {
          	m_main_frame->get_process_diagram_listbox()->Select( 0 );
      	  }
        }
      }
    }

    // update grape mode
    wxString m_selected_diagram = arch_listbox->GetStringSelection();
    if ( m_selected_diagram == wxEmptyString )
    {
      // if this is also empty there will be no selection; grape_event_select_diagram will process according to that.
      m_selected_diagram = m_main_frame->get_process_diagram_listbox()->GetStringSelection();
    }

    // select new diagram
    grape_event_select_diagram *event = new grape_event_select_diagram(m_main_frame, m_selected_diagram);
    m_main_frame->get_event_handler()->Submit(event, false);

    // remove diagram from grapespecification
    spec->remove_architecture_diagram( del_arch_dia_ptr );
  }
  else
  {
    assert( m_type == GRAPE_PROCESS_DIAGRAM); // If this assertion fails, then the diagram was of type GRAPE_NO_DIAGRAM ( a serious error );
    process_diagram* proc_dia_ptr = static_cast<process_diagram*> ( find_diagram( m_diagram, m_type ) );
    for ( unsigned int i = 0; i < m_terminating_transitions.GetCount(); ++i )
    {
      grape_event_remove_terminating_transition event = m_terminating_transitions.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_nonterminating_transitions.GetCount(); ++i )
    {
      grape_event_remove_nonterminating_transition event = m_nonterminating_transitions.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_initial_designators.GetCount(); ++i )
    {
      grape_event_remove_initial_designator event = m_initial_designators.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_reference_states.GetCount(); ++i )
    {
      grape_event_remove_reference_state event = m_reference_states.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_states.GetCount(); ++i )
    {
      grape_event_remove_state event = m_states.Item( i );
      event.Do();
    }
    for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
    {
      grape_event_remove_comment event = m_comments.Item( i );
      event.Do();
    }
    // check whether any processreferences exist that should no longer point to the diagram.
    process_diagram* dummy = 0;
    spec->check_references( proc_dia_ptr->get_name(), dummy );

    // update process listbox
    grape_listbox *proc_listbox = m_main_frame->get_process_diagram_listbox();
    int pos = proc_listbox->FindString( proc_dia_ptr->get_name() );
    proc_listbox->Delete( pos );
    if ( pos != 0 )
    {
      proc_listbox->Select( pos - 1 );
    }
    else
    {
      if (!proc_listbox->IsEmpty())
      {
         proc_listbox->Select( 0 );
      }
      else
      {
        // if the architecture diagram listbox is empty, there will be no selection, try process diagram listbox
        if (!m_main_frame->get_architecture_diagram_listbox()->IsEmpty())
        {
	      m_main_frame->get_architecture_diagram_listbox()->Select( 0 );
        }
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

    // remove diagram from grapespecification
    spec->remove_process_diagram( proc_dia_ptr );
  }

  finish_modification();
  return true;
}

bool grape_event_remove_diagram::Undo( void )
{
  grape_specification* spec = m_main_frame->get_grape_specification();
  if ( m_type == GRAPE_ARCHITECTURE_DIAGRAM )
  {
    architecture_diagram* arch_dia_ptr = spec->add_architecture_diagram( m_diagram, m_name );
    for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
    {
      grape_event_remove_comment event = m_comments.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_process_references.GetCount(); ++i )
    {
      grape_event_remove_process_reference event = m_process_references.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_architecture_references.GetCount(); ++i )
    {
      grape_event_remove_architecture_reference event = m_architecture_references.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_channels.GetCount(); ++i )
    {
      grape_event_remove_channel event = m_channels.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_channel_communications.GetCount(); ++i )
    {
      grape_event_remove_channel_communication event = m_channel_communications.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_visibles.GetCount(); ++i )
    {
      grape_event_remove_visible event = m_visibles.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_blockeds.GetCount(); ++i )
    {
      grape_event_remove_blocked event = m_blockeds.Item( i );
      event.Undo();
    }
    // check whether any architecturereferences exist that should point to the diagram.
    spec->check_references( arch_dia_ptr->get_name(), arch_dia_ptr );

    m_main_frame->get_glcanvas()->set_diagram( arch_dia_ptr );

    // update grape mode
    m_main_frame->set_mode( GRAPE_MODE_ARCH );

    // update architecture diagram listbox
    grape_listbox *arch_listbox = m_main_frame->get_architecture_diagram_listbox();
    int pos = arch_listbox->Append( arch_dia_ptr->get_name() );
    arch_listbox->Select( pos );
  }
  else
  {
    process_diagram* proc_dia_ptr = spec->add_process_diagram( m_diagram, m_name );
    proc_dia_ptr->set_preamble( m_preamble );
    for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
    {
      grape_event_remove_comment event = m_comments.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_states.GetCount(); ++i )
    {
      grape_event_remove_state event = m_states.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_reference_states.GetCount(); ++i )
    {
      grape_event_remove_reference_state event = m_reference_states.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_initial_designators.GetCount(); ++i )
    {
      grape_event_remove_initial_designator event = m_initial_designators.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_nonterminating_transitions.GetCount(); ++i )
    {
      grape_event_remove_nonterminating_transition event = m_nonterminating_transitions.Item( i );
      event.Undo();
    }
    for ( unsigned int i = 0; i < m_terminating_transitions.GetCount(); ++i )
    {
      grape_event_remove_terminating_transition event = m_terminating_transitions.Item( i );
      event.Undo();
    }
    // check whether any architecturereferences exist that should point to the diagram.
    spec->check_references( proc_dia_ptr->get_name(), proc_dia_ptr );

    m_main_frame->get_glcanvas()->set_diagram( proc_dia_ptr );

    // update grape mode
    m_main_frame->set_mode( GRAPE_MODE_PROC );

    // update process diagram listbox
    grape_listbox *proc_listbox = m_main_frame->get_process_diagram_listbox();
    int pos = proc_listbox->Append( proc_dia_ptr->get_name() );
    proc_listbox->Select( pos );
  }

  finish_modification();
  return true;
}

grape_event_export_current_diagram_image::grape_event_export_current_diagram_image( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, false, _T( "export current diagram to image" ) )
{
}

grape_event_export_current_diagram_image::~grape_event_export_current_diagram_image( void )
{
}

bool grape_event_export_current_diagram_image::Do( void )
{

  // display save dialog
  wxFileDialog save_dialog( m_main_frame, _T( "Export to image..." ),  m_main_frame->get_filename().GetPath(), _T( "" ), _T( "PNG files ( *.png )|*.png|BMP files ( *.bmp )|*.bmp" ), wxFD_SAVE | wxFD_OVERWRITE_PROMPT );
  int result = save_dialog.ShowModal();

  if ( result == wxID_OK )
  {
    // get filename
    wxFileName filename( save_dialog.GetPath() );
    if ( ( filename.GetExt().Lower() != _T("png") ) && ( filename.GetExt().Lower() != _T("bmp") ) )
    {
      wxString fullname = filename.GetFullName();
      filename.SetName( fullname );
      if ( save_dialog.GetFilterIndex() == 1 )
      {
        filename.SetExt( _T("bmp") );
      }
      else
      {
        filename.SetExt( _T("png") );
      }
      if ( filename.FileExists() )
      {
        wxString s = _T("The file ");
        s += filename.GetFullPath();
        s += _T(" already exists, do you wish to overwrite this file?" );
        int result = wxMessageBox( s, _T("Question"), wxYES_NO | wxICON_QUESTION, m_main_frame );
        if ( result == wxNO )
        {
          cerr << "Exporting Aborted" << endl;
          return false;
        }
      }
    }

    //calculate max (x,y) position of objects
    unsigned int count = m_main_frame->get_glcanvas()->count_visual_object();
    int maxx = 0;
    int maxy = 0;
    for ( unsigned int i = 0; i < count; ++i )
    {
      visual_object* vis_obj = m_main_frame->get_glcanvas()->get_visual_object(i);
      if (vis_obj != 0)
      {
        object* obj = vis_obj->get_selectable_object();
        if (obj != 0)
        {
          int coordx = int ((obj->get_coordinate().m_x+obj->get_width())*300);
          int coordy = int ((-obj->get_coordinate().m_y+obj->get_height())*300);
          if (maxx < coordx)
          {
            maxx = coordx;
          }
          if (maxy < coordy)
          {
            maxy = coordy;
          }
        }
      }
    }

    //create image for saving
    wxImage save_image(maxx, maxy);

    //get width of sub image
    int sub_image_width = m_main_frame->get_glcanvas()->get_image().GetWidth();
    //get height of sub image
    int sub_image_height = m_main_frame->get_glcanvas()->get_image().GetHeight();
    //don't export if the opengl canvas size is zero
    if ((sub_image_width == 0) || (sub_image_height == 0))
    {
      wxMessageBox( _T("Image could not be exported, because the canvas is too small."), _T("Notification"), wxOK | wxICON_EXCLAMATION, m_main_frame );

    }
    else
    {
      //clear log
      m_main_frame->get_logpanel()->Clear();
      //display message
      cerr << "Exporting Image..." << endl;
      //update application
      wxSafeYield();

      grape_event_deselect_all *event = new grape_event_deselect_all(m_main_frame);
      m_main_frame->get_event_handler()->Submit(event, false);

      for (int x = 0; x < maxx; x+=sub_image_width)
      {
        //update application
        wxSafeYield();

        for (int y = 0; y < maxy; y+=sub_image_height)
        {
          //set translation coordinate
          coordinate translation_coordinate = {-(float)x/300.0f, (float)y/300.0f};
          //paint translated canvas
          m_main_frame->get_glcanvas()->paint_coordinate(translation_coordinate);
          //get sub image
          wxImage sub_image = m_main_frame->get_glcanvas()->get_image();
          //paste sub images into base image
          save_image.Paste(sub_image, x, y);
        }
      }

      //display message
      cerr << "Saving image" << endl;
      //update application
      wxSafeYield();

      bool save_success;

      if ( filename.GetExt().Lower() == _T("bmp") )
      {
        save_success = save_image.SaveFile( filename.GetFullPath(), wxBITMAP_TYPE_BMP);
      }
      else
      {
        save_success = save_image.SaveFile( filename.GetFullPath(), wxBITMAP_TYPE_PNG);
      }
      //display message
      cerr << "Done" << endl;

      return save_success;
    }
  }
  return false;
}

bool grape_event_export_current_diagram_image::Undo( void )
{
  // cannot be undone
  return true;
}

grape_event_export_current_diagram_mcrl2::grape_event_export_current_diagram_mcrl2(grape_frame *p_main_frame)
: grape_event_base(p_main_frame, false, _T("export current diagram to mCRL2"))
{
}

grape_event_export_current_diagram_mcrl2::~grape_event_export_current_diagram_mcrl2(void)
{
}

bool grape_event_export_current_diagram_mcrl2::Do(void)
{
  // clear logpanel and catch cout
  m_main_frame->get_logpanel()->Clear();
  //m_main_frame->get_logpanel()->enable_catch_cout();

  // display save dialog
  wxFileDialog save_dialog(m_main_frame, _T("Export to mCRL2..."), m_main_frame->get_filename().GetPath(), _T(""), _T("mCRL2 files ( *.mcrl2 )|*.mcrl2"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  int result = save_dialog.ShowModal();
  if(result == wxID_OK)
  {
    // get filename
    wxFileName filename( save_dialog.GetPath() );
    if ( filename.GetExt().Lower() != _T("mcrl2") )
    {
      wxString fullname = filename.GetFullName();
      filename.SetName( fullname );
      filename.SetExt( _T("mcrl2") );
      if ( filename.FileExists() )
      {
        wxString s = _T("The file ");
        s += filename.GetFullPath();
        s += _T(" already exists, do you wish to overwrite this file?" );
        int result = wxMessageBox( s, _T("Question"), wxYES_NO | wxICON_QUESTION, m_main_frame );
        if ( result == wxNO )
        {
          return false;
        }
      }
    }

    // convert specification to XML
    wxString empty_filename = wxEmptyString;
    grape_specification *export_spec = m_main_frame->get_grape_specification();
    wxXmlDocument export_doc = xml_convert(*export_spec, empty_filename, 2, false);
    convert_spaces(export_doc);

    // determine current diagram and ask for a parameter initialisation in case it is a process diagram
    diagram *export_diagram= m_main_frame->get_glcanvas()->get_diagram();
    wxString export_name = filename.GetFullPath();
    wxString diagram_id;
    diagram_id.Printf(_T("%d"), export_diagram->get_id());
    architecture_diagram *arch_diag = dynamic_cast<architecture_diagram*>(export_diagram);
    if(arch_diag != 0)
    {
      // export architecture diagram
      if(!export_architecture_diagram_to_mcrl2(export_doc, export_name, diagram_id, true))
      {
        //m_main_frame->get_logpanel()->disable_catch_cout();
        return false;
      }
    }
    else
    {
      // export process diagram
      process_diagram *proc_diag = dynamic_cast<process_diagram*>(export_diagram);
      if(proc_diag != 0)
      {
        // get parameter initialisation
        list_of_decl_init param_init;
        param_init.Empty();
        preamble *export_preamble = proc_diag->get_preamble();
//        if(export_preamble->check_parameter_declarations_syntax())
//        {
        list_of_decl params = export_preamble->get_parameter_declarations_list();
        if(params.GetCount() > 0)
        {
          grape_parameter_dialog *dialog = new grape_parameter_dialog(params);
          dialog->show_modal();
          param_init = dialog->get_initialisations();
          delete dialog;
        }
//        }
//        else
//        {
//          std::cerr << "mCRL2 conversion failed: the syntax of the parameter declaration in the preamble of the exported diagram is invalid." << std::endl;
//          //m_main_frame->get_logpanel()->disable_catch_cout();
//          return false;
//        }
        if(!export_process_diagram_to_mcrl2(export_doc, export_name, diagram_id, param_init, true))
        {
          //m_main_frame->get_logpanel()->disable_catch_cout();
          return false;
        }
      }
    }
  }

  //m_main_frame->get_logpanel()->disable_catch_cout();
  return true;
}

bool grape_event_export_current_diagram_mcrl2::Undo(void)
{
  // cannot be undone
  return true;
}

grape_event_validate_specification::grape_event_validate_specification(grape_frame *p_main_frame)
: grape_event_base(p_main_frame, false, _T("validate specification"))
{
}

grape_event_validate_specification::~grape_event_validate_specification(void)
{
}

bool grape_event_validate_specification::Do(void)
{
  // clear logpanel
  m_main_frame->get_logpanel()->Clear();

  // convert specification to XML
  wxString empty_filename = wxEmptyString;
  grape_specification *validate_spec = m_main_frame->get_grape_specification();
  wxXmlDocument validate_doc = xml_convert(*validate_spec, empty_filename, 2, false);
  convert_spaces(validate_doc);

  validate(validate_doc);

  return true;
}

bool grape_event_validate_specification::Undo(void)
{
  return true;
}

grape_event_validate_diagram::grape_event_validate_diagram(grape_frame *p_main_frame)
: grape_event_base(p_main_frame, false, _T("validate current diagram"))
{
}

grape_event_validate_diagram::~grape_event_validate_diagram(void)
{
}

bool grape_event_validate_diagram::Do(void)
{
  // clear logpanel
  m_main_frame->get_logpanel()->Clear();

  // convert specification to XML
  wxString empty_filename = wxEmptyString;
  grape_specification *validate_spec = m_main_frame->get_grape_specification();
  wxXmlDocument validate_doc = xml_convert(*validate_spec, empty_filename, 2, false);
  convert_spaces(validate_doc);

  diagram *dia = m_main_frame->get_glcanvas()->get_diagram();
  int diagram_id = dia->get_id();
  wxString d_id;
  d_id.Printf(_T("%u"), diagram_id);

  // determine type of diagram to validate
  architecture_diagram *arch_dia = dynamic_cast<architecture_diagram*>(dia);
  process_diagram *proc_dia = dynamic_cast<process_diagram*>(dia);
  if(arch_dia != 0)
  {
    validate_architecture_diagram(validate_doc, d_id);
  }
  else if(proc_dia != 0)
  {
    validate_process_diagram(validate_doc, d_id);
  }

  return true;
}

bool grape_event_validate_diagram::Undo(void)
{
  return true;
}

grape_event_delete_selected_objects::grape_event_delete_selected_objects( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, true, _T( "delete selected objects" ) )
{
  m_comment.Empty();
  m_state.Empty();
  m_ref_state.Empty();
  m_ntt.Empty();
  m_tt.Empty();
  m_init.Empty();
  m_channel.Empty();
  m_visible.Empty();
  m_blocked.Empty();
  m_c_comm.Empty();
  m_proc_ref.Empty();
  m_arch_ref.Empty();

  // fill the remove event arrays
  // determine the type of diagram.
  diagram* dia = m_main_frame->get_glcanvas()->get_diagram();
  architecture_diagram* arch_dia = dynamic_cast<architecture_diagram*> ( dia );
  if ( arch_dia != 0 )
  {
    // walk through the list of selected objects.
    for ( unsigned int i = 0; i < arch_dia->count_selected_objects(); ++i )
    {
      object* obj_ptr = arch_dia->get_selected_object( i );
      object_type obj_type = obj_ptr->get_type();
      switch ( obj_type )
      {
        case COMMENT:
        {
          grape_event_remove_comment* event = new grape_event_remove_comment( m_main_frame,
                                                  static_cast<comment*> ( obj_ptr ), dia );
          m_comment.Add( event );
          break;
        }
        case CHANNEL:
        {
          grape_event_remove_channel* event = new grape_event_remove_channel( m_main_frame,
                                                  static_cast<channel*> ( obj_ptr ), arch_dia );
          m_channel.Add( event );
          break;
        }
        case CHANNEL_COMMUNICATION:
        {
          grape_event_remove_channel_communication* event = new grape_event_remove_channel_communication( m_main_frame,
                                                  static_cast<channel_communication*> ( obj_ptr ), arch_dia );
          m_c_comm.Add( event );
          break;
        }
        case PROCESS_REFERENCE:
        {
          grape_event_remove_process_reference* event = new grape_event_remove_process_reference( m_main_frame,
                                                  static_cast<process_reference*> ( obj_ptr ), arch_dia );
          m_proc_ref.Add( event );
          break;
        }
        case ARCHITECTURE_REFERENCE:
        {
          grape_event_remove_architecture_reference* event = new grape_event_remove_architecture_reference
                                    ( m_main_frame, static_cast<architecture_reference*> ( obj_ptr ), arch_dia );
          m_arch_ref.Add( event );
          break;
        }
        case VISIBLE:
        {
          grape_event_remove_visible* event = new grape_event_remove_visible( m_main_frame,
                                                  static_cast<visible*> ( obj_ptr ), arch_dia );
          m_visible.Add( event );
          break;
        }
        case BLOCKED:
        {
          grape_event_remove_blocked* event = new grape_event_remove_blocked( m_main_frame,
                                                  static_cast<blocked*> ( obj_ptr ), arch_dia );
          m_blocked.Add( event );
          break;
        }
        case INITIAL_DESIGNATOR:break;
        case STATE:break;
        case REFERENCE_STATE:break;
        case NONTERMINATING_TRANSITION:break;
        case TERMINATING_TRANSITION:break;
        case NONE: break;
        default: /* assert( false ); */ break;
      }
    }
  }
  else
  {
    process_diagram* proc_dia = dynamic_cast<process_diagram*> ( dia );
    assert( proc_dia != 0 );
    // walk through the list of selected objects.
    for ( unsigned int i = 0; i < proc_dia->count_selected_objects(); ++i )
    {
      object* obj_ptr = proc_dia->get_selected_object( i );
      object_type obj_type = obj_ptr->get_type();
      switch ( obj_type )
      {
        case COMMENT:
        {
          grape_event_remove_comment* event = new grape_event_remove_comment( m_main_frame,
                                                  static_cast<comment*> ( obj_ptr ), dia );
          m_comment.Add( event );
          break;
        }
        case INITIAL_DESIGNATOR:
        {
          grape_event_remove_initial_designator* event = new grape_event_remove_initial_designator( m_main_frame,
                                                  static_cast<initial_designator*> ( obj_ptr ), proc_dia );
          m_init.Add( event );
          break;
        }
        case STATE:
        {
          grape_event_remove_state* event = new grape_event_remove_state( m_main_frame,
                                                  static_cast<state*> ( obj_ptr ), proc_dia );
          m_state.Add( event );
          break;
        }
        case REFERENCE_STATE:
        {
          grape_event_remove_reference_state* event = new grape_event_remove_reference_state( m_main_frame,
                                                  static_cast<reference_state*> ( obj_ptr ), proc_dia );
          m_ref_state.Add( event );
          break;
        }
        case NONTERMINATING_TRANSITION:
        {
          grape_event_remove_nonterminating_transition* event = new grape_event_remove_nonterminating_transition( m_main_frame, static_cast<nonterminating_transition*> ( obj_ptr ), proc_dia );
          m_ntt.Add( event );
          break;
        }
        case TERMINATING_TRANSITION:
        {
          grape_event_remove_terminating_transition* event = new grape_event_remove_terminating_transition( m_main_frame, static_cast<terminating_transition*> ( obj_ptr ), proc_dia );
          m_tt.Add( event );
          break;
        }
        case CHANNEL:break;
        case CHANNEL_COMMUNICATION: break;
        case PROCESS_REFERENCE: break;
        case ARCHITECTURE_REFERENCE: break;
        case VISIBLE: break;
        case BLOCKED:break;
        case NONE:break;
        default: /* assert( false ); */ break;
      }
    }
  }
}

grape_event_delete_selected_objects::~grape_event_delete_selected_objects( void )
{
  m_comment.Clear();
  m_state.Clear();
  m_ref_state.Clear();
  m_ntt.Clear();
  m_tt.Clear();
  m_init.Clear();
  m_channel.Clear();
  m_visible.Clear();
  m_blocked.Clear();
  m_c_comm.Clear();
  m_proc_ref.Clear();
  m_arch_ref.Clear();
}

bool grape_event_delete_selected_objects::Do( void )
{
  // Note: Perform the remove Do events in the correct order, or there will be trouble.
  for ( unsigned int i = 0; i < m_comment.GetCount(); ++i )
  {
    grape_event_remove_comment event = m_comment.Item( i );
    event.Do();
  }
  for ( unsigned int i = 0; i < m_ntt.GetCount(); ++i )
  {
    grape_event_remove_nonterminating_transition event = m_ntt.Item( i );
    event.Do();
  }
  for ( unsigned int i = 0; i < m_tt.GetCount(); ++i )
  {
    grape_event_remove_terminating_transition event = m_tt.Item( i );
    event.Do();
  }
  for ( unsigned int i = 0; i < m_init.GetCount(); ++i )
  {
    grape_event_remove_initial_designator event = m_init.Item( i );
    event.Do();
  }
  // Perform remove of state and reference state after the remove of transitions and initial designators
  for ( unsigned int i = 0; i < m_state.GetCount(); ++i )
  {
    grape_event_remove_state event = m_state.Item( i );
    event.Do();
  }
  for ( unsigned int i = 0; i < m_ref_state.GetCount(); ++i )
  {
    grape_event_remove_reference_state event = m_ref_state.Item( i );
    event.Do();
  }

  // Perform property deletions first.
  for ( unsigned int i = 0; i < m_visible.GetCount(); ++i )
  {
    grape_event_remove_visible event = m_visible.Item( i );
    event.Do();
  }
  for ( unsigned int i = 0; i < m_blocked.GetCount(); ++i )
  {
    grape_event_remove_blocked event = m_blocked.Item( i );
    event.Do();
  }
  // Then channel communications
  for ( unsigned int i = 0; i < m_c_comm.GetCount(); ++i )
  {
    grape_event_remove_channel_communication event = m_c_comm.Item( i );
    event.Do();
  }
  // Then channels
  for ( unsigned int i = 0; i < m_channel.GetCount(); ++i )
  {
    grape_event_remove_channel event = m_channel.Item( i );
    event.Do();
  }
  // And lastly references
  for ( unsigned int i = 0; i < m_proc_ref.GetCount(); ++i )
  {
    grape_event_remove_process_reference event = m_proc_ref.Item( i );
    event.Do();
  }
  for ( unsigned int i = 0; i < m_arch_ref.GetCount(); ++i )
  {
    grape_event_remove_architecture_reference event = m_arch_ref.Item( i );
    event.Do();
  }

  m_main_frame->get_glcanvas()->SetFocus();
  return true;
}

bool grape_event_delete_selected_objects::Undo( void )
{
  // Note: Perform the remove Undo events in the correct order, or there will be trouble.
  for ( unsigned int i = 0; i < m_comment.GetCount(); ++i )
  {
    grape_event_remove_comment event = m_comment.Item( i );
    event.Undo();
  }

  // Perform put back state and reference state before transitions and initial designators
  for ( unsigned int i = 0; i < m_state.GetCount(); ++i )
  {
    grape_event_remove_state event = m_state.Item( i );
    event.Undo();
  }
  for ( unsigned int i = 0; i < m_ref_state.GetCount(); ++i )
  {
    grape_event_remove_reference_state event = m_ref_state.Item( i );
    event.Undo();
  }
  for ( unsigned int i = 0; i < m_ntt.GetCount(); ++i )
  {
    grape_event_remove_nonterminating_transition event = m_ntt.Item( i );
    event.Undo();
  }
  for ( unsigned int i = 0; i < m_tt.GetCount(); ++i )
  {
    grape_event_remove_terminating_transition event = m_tt.Item( i );
    event.Undo();
  }
  for ( unsigned int i = 0; i < m_init.GetCount(); ++i )
  {
    grape_event_remove_initial_designator event = m_init.Item( i );
    event.Undo();
  }

  // Begin with references
  for ( unsigned int i = 0; i < m_proc_ref.GetCount(); ++i )
  {
    grape_event_remove_process_reference event = m_proc_ref.Item( i );
    event.Undo();
  }
  for ( unsigned int i = 0; i < m_arch_ref.GetCount(); ++i )
  {
    grape_event_remove_architecture_reference event = m_arch_ref.Item( i );
    event.Undo();
  }
  // Then channels
  for ( unsigned int i = 0; i < m_channel.GetCount(); ++i )
  {
    grape_event_remove_channel event = m_channel.Item( i );
    event.Undo();
  }
  // Then channel communications
  for ( unsigned int i = 0; i < m_c_comm.GetCount(); ++i )
  {
    grape_event_remove_channel_communication event = m_c_comm.Item( i );
    event.Undo();
  }
  // And lastly the properties.
  for ( unsigned int i = 0; i < m_visible.GetCount(); ++i )
  {
    grape_event_remove_visible event = m_visible.Item( i );
    event.Undo();
  }
  for ( unsigned int i = 0; i < m_blocked.GetCount(); ++i )
  {
    grape_event_remove_blocked event = m_blocked.Item( i );
    event.Undo();
  }

  m_main_frame->get_glcanvas()->SetFocus();
  return true;
}
