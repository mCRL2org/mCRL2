// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_comment.cpp
//
// Defines GraPE events for comments.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"
#include "dialogs/textdialog.h"
#include "../visuals/visualcomment.h"

#include "event_comment.h"

const float DEFAULT_COMMENT_WIDTH = 0.5f;
const float DEFAULT_COMMENT_HEIGHT = 0.5f;

using namespace grape::grapeapp;

grape_event_add_comment::grape_event_add_comment( grape_frame *p_main_frame, coordinate &p_coord )
: grape_event_base( p_main_frame, true, _T( "add comment" ) )
{
  m_comm = m_main_frame->get_new_id();
  m_coord = p_coord;
  m_def_comm_width = DEFAULT_COMMENT_WIDTH;
  m_def_comm_height = DEFAULT_COMMENT_HEIGHT;
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_in_diagram = dia_ptr->get_id();
}

grape_event_add_comment::~grape_event_add_comment( void )
{
}

bool grape_event_add_comment::Do( void )
{
  diagram* dia_ptr = find_diagram( m_in_diagram );
  dia_ptr->add_comment( m_comm, m_coord, m_def_comm_width, m_def_comm_height );

  finish_modification();
  return true;
}

bool grape_event_add_comment::Undo( void )
{
  // find the diagram the comment was added to
  diagram* dia_ptr = find_diagram( m_in_diagram );
  comment* comm_ptr = static_cast<comment*> ( find_object( m_comm, COMMENT, dia_ptr->get_id() ) );
  dia_ptr->remove_comment( comm_ptr );

  finish_modification();
  return true;
}

grape_event_remove_comment::grape_event_remove_comment( grape_frame *p_main_frame, comment* p_comm, diagram* p_dia_ptr )
: grape_event_base( p_main_frame, true, _T( "remove comment" ) )
{
  m_comm = p_comm->get_id();
  m_text = p_comm->get_text();
  object* obj_ptr = p_comm->get_attached_object();
  if ( obj_ptr != 0 )
  {
    m_object = obj_ptr->get_id();
  }
  else
  {
    m_object = -1;
  }
  m_coordinate = p_comm->get_coordinate();
  m_width = p_comm->get_width();
  m_height = p_comm->get_height();
  m_comments.Empty();
  for ( unsigned int i = 0; i < p_comm->count_comment(); ++i )
  {
    comment* comm_ptr = p_comm->get_comment( i );
    m_comments.Add( comm_ptr->get_id() );
  }
  m_in_diagram = p_dia_ptr->get_id();
}

grape_event_remove_comment::~grape_event_remove_comment(  void  )
{
  m_comments.Clear();
}

bool grape_event_remove_comment::Do(  void  )
{
  diagram* dia_ptr = find_diagram( m_in_diagram );
  comment* comm_ptr = static_cast<comment*> ( find_object( m_comm, COMMENT, dia_ptr->get_id() ) );
  
  if ( comm_ptr )
  {
    visualcomment* vis_comm_ptr = static_cast<visualcomment*> (m_main_frame->get_glcanvas()->get_visual_object( comm_ptr ) );
    
    bool reference_selected = vis_comm_ptr->get_reference_selected();
    
    // if there is no valid selected communication
    if (reference_selected)
    {
      // remove the selected channel
      comm_ptr->detach_from_object();
    } else {
      // remove the entire comment
      dia_ptr->remove_comment( comm_ptr );
    }
  }

  finish_modification();
  return true;
}

bool grape_event_remove_comment::Undo(  void  )
{
  // find the diagram the comment was removed from
  diagram* dia_ptr = find_diagram( m_in_diagram );
  
  comment* new_comm = static_cast<comment*> ( find_object( m_comm, COMMENT, dia_ptr->get_id() ) );
  // only recreate the comment if it was removed
  if (new_comm == 0) 
  {
    new_comm = dia_ptr->add_comment( m_comm, m_coordinate, m_width, m_height );

    //re-attach all detached objects
    new_comm->set_text( m_text );
    if ( m_object )
    {
      object* obj_ptr = find_object( m_object );
      if ( obj_ptr )
      {
        dia_ptr->attach_comment_to_object( new_comm, obj_ptr );
      }
    }
    for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
    {
      unsigned int identifier = m_comments.Item( i );
      comment* comm_ptr = static_cast<comment*> ( find_object( identifier, COMMENT, dia_ptr->get_id() ) );
      dia_ptr->attach_comment_to_object( comm_ptr, new_comm );
    }
  }
  else
  {
    object* obj_ptr = find_object( m_object );
    if ( obj_ptr )
    {
      dia_ptr->attach_comment_to_object( new_comm, obj_ptr );
    }    
  }

  finish_modification();
  return true;
}

grape_event_change_comment_text::grape_event_change_comment_text( grape_frame *p_main_frame, comment* p_comment )
: grape_event_base( p_main_frame, true, _T( "edit comment text" ) )
{
  m_comment_id = p_comment->get_id();
  m_old_text = p_comment->get_text();

  grape_text_dlg dialog( _T("Edit comment text"), _T("Enter the comment text:"), m_old_text, true /* multiline */ );

  m_ok_pressed = dialog.show_modal( m_new_text );
}

grape_event_change_comment_text::~grape_event_change_comment_text( void )
{
}

bool grape_event_change_comment_text::Do( void )
{
  if ( !m_ok_pressed )
  {
    // user cancelled, don't push it on the undo stack
    return false;
  }

  comment *comment_ptr = static_cast<comment *>( find_object( m_comment_id, COMMENT ) );
  comment_ptr->set_text( m_new_text );

  finish_modification();
  return true;
}

bool grape_event_change_comment_text::Undo( void )
{
  comment *comment_ptr = static_cast<comment *>( find_object( m_comment_id, COMMENT ) );
  comment_ptr->set_text( m_old_text );

  finish_modification();
  return true;
}

grape_event_attach_comment::grape_event_attach_comment( grape_frame *p_main_frame, comment* p_comment, object* p_object )
: grape_event_base( p_main_frame, true, _T( "attach comment" ) )
{
  m_comment = p_comment->get_id();
  m_object = p_object->get_id();
  object* obj_ptr = p_comment->get_attached_object();
  m_connected_to = obj_ptr ? obj_ptr->get_id() : -1;
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_diagram = dia_ptr->get_id();
}

grape_event_attach_comment::~grape_event_attach_comment( void )
{
}

bool grape_event_attach_comment::Do( void )
{
  diagram* dia_ptr = find_diagram( m_diagram );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  comment* comm_ptr = static_cast<comment*> ( find_object( m_comment, COMMENT ) );
  object* obj_ptr = find_object( m_object );
  dia_ptr->attach_comment_to_object( comm_ptr, obj_ptr );

  finish_modification();
  return true;
}

bool grape_event_attach_comment::Undo( void )
{
  diagram* dia_ptr = find_diagram( m_diagram );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  comment* comm_ptr = static_cast<comment*> ( find_object( m_comment, COMMENT ) );
  dia_ptr->detach_comment_from_object( comm_ptr );

  // re-attach if it was attached before
  if ( m_connected_to != -1 )
  {
    object* obj_ptr = find_object( m_connected_to );
    dia_ptr->attach_comment_to_object( comm_ptr, obj_ptr );
  }

  finish_modification();
  return true;
}


grape_event_detach_comment::grape_event_detach_comment( grape_frame *p_main_frame, comment* p_comment )
: grape_event_base( p_main_frame, true, _T( "detach comment" ) )
{
  m_comment = p_comment->get_id();
  object* obj_ptr = p_comment->get_attached_object();
  m_object = obj_ptr ? obj_ptr->get_id() : -1;
  diagram* dia_ptr = m_main_frame->get_glcanvas()->get_diagram();
  assert( dia_ptr != 0 );// The diagram has to exist, or else this event could not have been generated.
  m_diagram = dia_ptr->get_id();
}

grape_event_detach_comment::~grape_event_detach_comment( void )
{
}

bool grape_event_detach_comment::Do( void )
{
  if ( m_object == -1 )
  {
    return false;
  }

  diagram* dia_ptr = find_diagram( m_diagram );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  comment* comm_ptr = static_cast<comment*> ( find_object( m_comment, COMMENT ) );
  dia_ptr->detach_comment_from_object( comm_ptr );

  finish_modification();
  return true;
}

bool grape_event_detach_comment::Undo( void )
{
  diagram* dia_ptr = find_diagram( m_diagram );
  assert( dia_ptr != 0 ); // Should be the case or this event wouldn't be possible.

  comment* comm_ptr = static_cast<comment*> ( find_object( m_comment, COMMENT ) );
  object* obj_ptr = find_object( m_object );
  dia_ptr->attach_comment_to_object( comm_ptr, obj_ptr );

  finish_modification();
  return true;
}

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_event_remove_comment )
