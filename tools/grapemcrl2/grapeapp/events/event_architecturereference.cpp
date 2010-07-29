// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_architecturereference.cpp
//
// Defines GraPE events for architecture references.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "dialogs/referencedialog.h"

#include "event_architecturereference.h"

const float DEFAULT_ARCHITECTURE_REFERENCE_WIDTH = 0.3f;
const float DEFAULT_ARCHITECTURE_REFERENCE_HEIGHT = 0.3f;

using namespace grape::grapeapp;

grape_event_add_architecture_reference::grape_event_add_architecture_reference( grape_frame *p_main_frame, coordinate &p_coord )
: grape_event_base( p_main_frame, true, _T( "add architecture reference" ) )
{
  m_arch_ref = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_def_arch_ref_width = DEFAULT_ARCHITECTURE_REFERENCE_WIDTH;
  m_def_arch_ref_height = DEFAULT_ARCHITECTURE_REFERENCE_HEIGHT;

  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( m_main_frame->get_glcanvas()->get_diagram() );
  assert( dia_ptr != 0 );// The diagram has to exist and be of the specified type, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();
}

grape_event_add_architecture_reference::~grape_event_add_architecture_reference( void )
{
}

bool grape_event_add_architecture_reference::Do( void )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // Has to be the case or the event wouldn't have been generated.
  architecture_reference* new_arch_ref = dia_ptr->add_architecture_reference( m_arch_ref, m_coord, m_def_arch_ref_width, m_def_arch_ref_height );

  // Check if a diagram exists that has the same name as the reference.
  grape_specification* spec = m_main_frame->get_grape_specification();
  for ( unsigned int i = 0; i < spec->count_architecture_diagram(); ++i)
  {
    architecture_diagram* arch_dia = spec->get_architecture_diagram( i );
    if ( arch_dia->get_name() == new_arch_ref->get_name() )
    {
      new_arch_ref->set_relationship_refers_to( arch_dia );
    }
  }

  finish_modification();
  return true;
}

bool grape_event_add_architecture_reference::Undo( void )
{
  // find the diagram the architecture reference was added to
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 ); // The diagram should exist.

  // Find the architecture reference that is to be removed
  architecture_reference* arch_ref_ptr = static_cast<architecture_reference*> ( find_object( m_arch_ref, ARCHITECTURE_REFERENCE, dia_ptr->get_id() ) );
  dia_ptr->remove_architecture_reference( arch_ref_ptr );

  finish_modification();
  return true;
}

grape_event_remove_architecture_reference::grape_event_remove_architecture_reference(  grape_frame *p_main_frame, architecture_reference* p_arch_ref, architecture_diagram* p_arch_dia_ptr, bool p_normal )
: grape_event_base( p_main_frame, true, _T( "remove architecture reference" ) )
{
  m_arch_ref = p_arch_ref->get_id();
  m_name = p_arch_ref->get_name();
  architecture_diagram* arch_dia = p_arch_ref->get_relationship_refers_to();
  m_property_of = -1;
  if (arch_dia != 0)
  {
    m_property_of = arch_dia->get_id();
  }
  m_coordinate = p_arch_ref->get_coordinate();
  m_width = p_arch_ref->get_width();
  m_height = p_arch_ref->get_height();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_arch_ref->count_comment(); ++i )
  {
    comment* comm_ptr = p_arch_ref->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_arch_dia_ptr->get_id();

  m_channels.Empty();
  if ( p_normal )
  {
    // Create remove event for all channels that are to be deleted.
    for ( unsigned int i = 0; i < p_arch_ref->count_channel(); ++i )
    {
      channel* chan_ptr = p_arch_ref->get_channel( i );
      // pass the flag to the channels
      grape_event_remove_channel* event = new grape_event_remove_channel( m_main_frame, chan_ptr, p_arch_dia_ptr, p_normal );
      m_channels.Add( event );
    }
  }
}

grape_event_remove_architecture_reference::~grape_event_remove_architecture_reference( void )
{
  m_comments.Clear();

  m_channels.Clear();
}

bool grape_event_remove_architecture_reference::Do( void )
{
  // Perform remove event Do for channels
  for ( unsigned int i = 0; i < m_channels.GetCount(); ++i )
  {
    grape_event_remove_channel event = m_channels.Item( i );
    event.Do();
  }

  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  architecture_reference* arch_ref_ptr = dynamic_cast<architecture_reference*> ( find_object( m_arch_ref, ARCHITECTURE_REFERENCE, dia_ptr->get_id() ) );
  assert( arch_ref_ptr != 0 );

  dia_ptr->remove_architecture_reference( arch_ref_ptr );

  finish_modification();
  return true;
}

bool grape_event_remove_architecture_reference::Undo( void )
{
  architecture_diagram* dia_ptr = dynamic_cast<architecture_diagram*> ( find_diagram( m_in_diagram ) );
  assert( dia_ptr != 0 );
  architecture_reference* new_arch_ref = dia_ptr->add_architecture_reference( m_arch_ref, m_coordinate, m_width, m_height );

  new_arch_ref->set_name( m_name );
  architecture_diagram* arch_dia = dynamic_cast<architecture_diagram*> ( find_diagram( m_property_of ) );
  new_arch_ref->set_relationship_refers_to( arch_dia );
  new_arch_ref->set_diagram( dia_ptr );
  // Restore comment connections.
  for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
  {
    unsigned int identifier = m_comments.Item( i );
    comment* comm_ptr = static_cast<comment*> ( find_object( identifier, COMMENT, dia_ptr->get_id() ) );
    dia_ptr->attach_comment_to_object( comm_ptr, new_arch_ref );
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

grape_event_change_archref::grape_event_change_archref( grape_frame *p_main_frame, architecture_reference* p_arch_ref )
: grape_event_base( p_main_frame, true, _T( "change architecture reference" ) )
{
  m_arch_ref_id = p_arch_ref->get_id();
  m_old_arch_name = p_arch_ref->get_name();

  if ( p_arch_ref->get_relationship_refers_to() )
  {
    m_old_arch_id = p_arch_ref->get_relationship_refers_to()->get_id();
  }
  else
  {
    m_old_arch_id = -1;
  }

  grape_reference_dialog dialog( p_main_frame, p_arch_ref, m_main_frame->get_grape_specification() );
  m_ok_pressed = dialog.show_modal();
  if ( m_ok_pressed )
  {
    m_new_arch_name = dialog.get_diagram_name();
    m_new_arch_id = dialog.get_diagram_id();
  }
}

grape_event_change_archref::~grape_event_change_archref( void )
{
}

bool grape_event_change_archref::Do( void )
{
  if ( !m_ok_pressed )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  architecture_diagram *diagram_ptr = static_cast< architecture_diagram* >( find_diagram( m_new_arch_id ) );
  architecture_reference *arch_ref = static_cast< architecture_reference* >( find_object( m_arch_ref_id, ARCHITECTURE_REFERENCE ) );
  arch_ref->set_name( m_new_arch_name );
  arch_ref->set_relationship_refers_to( diagram_ptr );

  finish_modification();
  return true;
}

bool grape_event_change_archref::Undo( void )
{
  architecture_diagram *diagram_ptr = static_cast<architecture_diagram*>(find_diagram( m_old_arch_id ));
  architecture_reference *arch_ref = static_cast< architecture_reference* >( find_object( m_arch_ref_id, ARCHITECTURE_REFERENCE ) );

  arch_ref->set_name( m_old_arch_name );
  arch_ref->set_relationship_refers_to( diagram_ptr );

  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_arch_ref )
