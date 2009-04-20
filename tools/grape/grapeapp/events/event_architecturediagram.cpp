// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_architecturediagram.cpp
//
// Defines GraPE events for architecture diagrams

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "grape_listbox.h"

#include "event_diagram.h"

#include "event_architecturediagram.h"

using namespace grape::grapeapp;

grape_event_add_architecture_diagram::grape_event_add_architecture_diagram( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, true, _T( "add architecture diagram" ) )
{
  m_arch = m_main_frame->get_new_id();
}

grape_event_add_architecture_diagram::~grape_event_add_architecture_diagram( void )
{
}

bool grape_event_add_architecture_diagram::Do( void )
{
  // add architecture diagram to grapespecification
  grape_specification* spec = m_main_frame->get_grape_specification();
  assert( spec != 0 );
  architecture_diagram* new_arch_ptr = spec->add_architecture_diagram( m_arch, _T("ArchitectureDiagram") );

  m_main_frame->get_glcanvas()->set_diagram( new_arch_ptr );

  m_main_frame->set_mode( GRAPE_MODE_ARCH );

  // update architecture diagram listbox
  grape_listbox *arch_listbox = m_main_frame->get_architecture_diagram_listbox();
  int pos = arch_listbox->Append( new_arch_ptr->get_name() );
  arch_listbox->Select( pos );

  // check whether any architecturereferences exist that should point to the diagram.
  spec->check_references( new_arch_ptr->get_name(), new_arch_ptr );

  finish_modification();
  return true;
}

bool grape_event_add_architecture_diagram::Undo( void )
{
  grape_specification *spec = m_main_frame->get_grape_specification();
  // Retrieve the to be deleted process diagram
  architecture_diagram* del_arch_ptr = static_cast<architecture_diagram*> ( find_diagram( m_arch , GRAPE_ARCHITECTURE_DIAGRAM ) );

  // check whether any architecturereferences exist that should no longer point to the diagram.
  architecture_diagram* dummy_ptr = 0;
  spec->check_references( del_arch_ptr->get_name(), dummy_ptr );

  // update architecture listbox
  grape_listbox *arch_listbox = m_main_frame->get_architecture_diagram_listbox();
  int pos = arch_listbox->FindString( del_arch_ptr->get_name() );
  arch_listbox->Delete( pos );
  if ( pos > 0 )
  {
    arch_listbox->Select( pos - 1 );
  }
  else
  {
    // if the architecture diagram listbox is empty, there will be no selection, try process diagram listbox
    grape_listbox *proc_listbox = m_main_frame->get_process_diagram_listbox();
    if ( !proc_listbox->IsEmpty() )
    {
      proc_listbox->Select( 0 );
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

  // remove process diagram from grapespecification
  spec->remove_architecture_diagram( del_arch_ptr );

  finish_modification();
  return true;
}
