// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//p_main_frame
/// \file event_processreference.cpp
//
// Defines GraPE events for process references.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "dialogs/referencedialog.h"

#include "event_channel.h"

#include "event_processreference.h"

const float DEFAULT_PROCESS_REFERENCE_WIDTH = 0.3f;
const float DEFAULT_PROCESS_REFERENCE_HEIGHT = 0.3f;

using namespace grape::grapeapp;

grape_event_add_process_reference::grape_event_add_process_reference( grape_frame *p_main_frame, coordinate &p_coord )
: grape_event_base( p_main_frame, true, _T( "add process reference" ) )
{
  m_proc_ref = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_def_proc_ref_width = DEFAULT_PROCESS_REFERENCE_WIDTH;
  m_def_proc_ref_height = DEFAULT_PROCESS_REFERENCE_HEIGHT;

  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( m_main_frame->get_glcanvas()->get_diagram() );
  assert( dia_ptr != 0 );// The diagram has to exist and be of the specified type, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();
}

grape_event_add_process_reference::~grape_event_add_process_reference( void )
{
}

bool grape_event_add_process_reference::Do( void )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Has to be the case or the event wouldn't have been generated.
  process_reference* new_proc_ref = dia_ptr->add_process_reference( m_proc_ref, m_coord, m_def_proc_ref_width, m_def_proc_ref_height );

  // Check if a diagram exists that has the same name as the reference.
  grape_specification* spec = m_main_frame->get_grape_specification();
  for ( unsigned int i = 0; i < spec->count_process_diagram(); ++i)
  {
    process_diagram* proc_dia = spec->get_process_diagram( i );
    if ( proc_dia->get_name() == new_proc_ref->get_name() )
    {
      new_proc_ref->set_relationship_refers_to( proc_dia );
    }
  }

  finish_modification();
  return true;
}

bool grape_event_add_process_reference::Undo( void )
{
  // find the diagram the process reference was added to
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the state that is to be removed
  process_reference* proc_ref_ptr = static_cast<process_reference*> ( find_object( m_proc_ref, PROCESS_REFERENCE, dia_ptr->get_id() ) );
  dia_ptr->remove_process_reference( proc_ref_ptr );

  finish_modification();
  return true;
}

grape_event_remove_process_reference::grape_event_remove_process_reference( grape_frame *p_main_frame, process_reference* p_proc_ref, architecture_diagram* p_arch_dia_ptr, bool p_normal )
: grape_event_base( p_main_frame, true, _T( "remove process reference" ) )
{
  m_proc_ref = p_proc_ref->get_id();
  m_name = p_proc_ref->get_name();
  process_diagram* proc_dia = p_proc_ref->get_relationship_refers_to();
  m_property_of = -1;
  if (proc_dia != 0)
  {
    m_property_of = proc_dia->get_id();
  }
  m_parameter_assignments = p_proc_ref->get_parameter_updates();
  m_coordinate = p_proc_ref->get_coordinate();
  m_width = p_proc_ref->get_width();
  m_height = p_proc_ref->get_height();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_proc_ref->count_comment(); ++i )
  {
    comment* comm_ptr = p_proc_ref->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_arch_dia_ptr->get_id();

  m_channels.Empty();

  if ( p_normal )
  {
  // Create remove event for all channels that are to be deleted.
    for ( unsigned int i = 0; i < p_proc_ref->count_channel(); ++i )
    {
      channel* chan_ptr = p_proc_ref->get_channel( i );
      // pass the flag to the channels
      grape_event_remove_channel* event = new grape_event_remove_channel( m_main_frame, chan_ptr, p_arch_dia_ptr, p_normal );
      m_channels.Add( event );
    }
  }
}

grape_event_remove_process_reference::~grape_event_remove_process_reference( void )
{
  m_parameter_assignments.Clear();
  m_comments.Clear();

  m_channels.Clear();
}

bool grape_event_remove_process_reference::Do( void )
{
  // Perform remove event Do for channels
  for ( unsigned int i = 0; i < m_channels.GetCount(); ++i )
  {
    grape_event_remove_channel event = m_channels.Item( i );
    event.Do();
  }

  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  process_reference* proc_ref_ptr = dynamic_cast<process_reference*> ( find_object( m_proc_ref, PROCESS_REFERENCE, dia_ptr->get_id() ) );
  assert( proc_ref_ptr != 0 );

  dia_ptr->remove_process_reference( proc_ref_ptr );

  finish_modification();
  return true;
}

bool grape_event_remove_process_reference::Undo( void )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  process_diagram* proc_dia = dynamic_cast<process_diagram*> ( find_diagram( m_property_of ) );
  process_reference* new_proc_ref = dia_ptr->add_process_reference( m_proc_ref, m_coordinate, m_width, m_height );

  new_proc_ref->set_name( m_name );
  new_proc_ref->set_diagram( dia_ptr );
  new_proc_ref->set_relationship_refers_to( proc_dia );

  // Restore parameter assignments
  new_proc_ref->set_parameter_updates( m_parameter_assignments );
  // Restore comment connections.
  for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
  {
    unsigned int identifier = m_comments.Item( i );
    comment* comm_ptr = static_cast<comment*> ( find_object( identifier, COMMENT, dia_ptr->get_id() ) );
    dia_ptr->attach_comment_to_object( comm_ptr, new_proc_ref );
  }

  // Perform remove event Undo for channels
  for ( unsigned int i = 0; i < m_channels.GetCount(); ++i )
  {
    grape_event_remove_channel event = m_channels.Item( i );
    event.Undo();
  }

  finish_modification();
  return true;
}

grape_event_change_procref::grape_event_change_procref( grape_frame *p_main_frame, process_reference* p_proc_ref )
: grape_event_base( p_main_frame, true, _T( "change process reference" ) )
{
  m_proc_ref_id = p_proc_ref->get_id();
  m_old_proc_name = p_proc_ref->get_name();
  m_old_text = p_proc_ref->get_text();

  if ( p_proc_ref->get_relationship_refers_to() )
  {
    m_old_proc_id = p_proc_ref->get_relationship_refers_to()->get_id();
  }
  else
  {
    m_old_proc_id = -1;
  }

  grape_reference_dialog dialog( p_main_frame, p_proc_ref, m_main_frame->get_grape_specification() );

  m_ok_pressed = dialog.show_modal();
  if ( m_ok_pressed )
  {
    m_new_proc_id = dialog.get_diagram_id();
    m_new_proc_name = dialog.get_diagram_name();
    m_new_text = dialog.get_initializations();
  }
}

grape_event_change_procref::~grape_event_change_procref( void )
{
}

bool grape_event_change_procref::Do( void )
{
  if ( !m_ok_pressed )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  process_diagram *diagram_ptr = static_cast<process_diagram*>(find_diagram( m_new_proc_id ) );
  process_reference *proc_ref = static_cast<process_reference*>(find_object( m_proc_ref_id, PROCESS_REFERENCE ) );

  proc_ref->set_relationship_refers_to( diagram_ptr );
  proc_ref->set_name( m_new_proc_name );
  proc_ref->set_text( m_new_text );

  finish_modification();
  return true;
}

bool grape_event_change_procref::Undo( void )
{
  process_diagram *diagram_ptr = static_cast<process_diagram*>(find_diagram( m_old_proc_id ));
  process_reference *proc_ref = static_cast<process_reference*>(find_object( m_proc_ref_id, PROCESS_REFERENCE ) );

  proc_ref->set_relationship_refers_to( diagram_ptr );
  proc_ref->set_name( m_old_proc_name );
  proc_ref->set_text( m_old_text );

  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_proc_ref );
