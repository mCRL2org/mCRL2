// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file diagram.cpp
//
// Implements the Diagram class.

#include "diagram.h"

using namespace grape::libgrape;

diagram::diagram(void)
{
  m_id = -1;
  m_name = wxEmptyString;
  m_comments.Empty();
  m_selected_objects.Empty();
}

diagram::diagram( const diagram &p_diagram )
{
  m_id = p_diagram.m_id;
  m_name = p_diagram.m_name;
  m_comments = p_diagram.m_comments;
  m_selected_objects = p_diagram.m_selected_objects;
}

diagram::~diagram( void )
{
  m_comments.Clear();
  m_selected_objects.Clear();
}

unsigned int diagram::get_id( void ) const
{
  return m_id;
}

void diagram::set_id( unsigned int p_id )
{
  m_id = p_id;
}

wxString diagram::get_name( void ) const
{
  return m_name;
}

void diagram::set_name( const wxString &p_name )
{
  m_name = p_name;
}

comment* diagram::add_comment( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height )
{
  // deselect all objects
  deselect_all_objects();

  // Create new comment.
  comment* new_comment = new comment;
  new_comment->set_id( p_id );
  new_comment->set_coordinate( p_coord );
  new_comment->set_width( p_def_width );
  new_comment->set_height( p_def_height );
  new_comment->set_diagram( this );
  select_object( new_comment );

  // Establish relationships
  m_comments.Add( new_comment );
  return new_comment;
}

void diagram::remove_comment( comment* p_comment )
{
  deselect_object( p_comment );

  // Remove relationships
  object* new_object = p_comment->get_attached_object();
  if ( new_object != 0 )
  {
    new_object->detach_comment( p_comment );
  }

  // Delete the comment
  int n = m_comments.Index( *p_comment );
  if ( n != wxNOT_FOUND )
  {
    comment* del_comment = m_comments.Detach( n );
    delete del_comment;
  }

}

unsigned int diagram::count_comment( void )
{
  return m_comments.GetCount();
}

comment* diagram::get_comment( int p_i )
{
  return &( m_comments.Item( p_i ) );
}

arr_comment* diagram::get_comment_list( void )
{
  return &m_comments;
}

void diagram::attach_comment_to_object( comment* p_comm, object* p_object )
{
  assert( ( p_comm->get_diagram() == p_object->get_diagram() ) && ( p_object->get_diagram() == this ) );//, _T( "diagram::attach_comment_to_object pre ( comm_diag_ptr == obj_diag_ptr ) failed \n" ) );
  // Detach the comment first, if necessary
  object* object_ptr = p_comm->get_attached_object();
  if ( object_ptr != 0 )
  {
    detach_comment_from_object( p_comm );
  }
  // Attach the comment to the new object
  p_object->attach_comment( p_comm );
  p_comm->attach_to_object( p_object );
}

void diagram::detach_comment_from_object( comment* p_comm )
{
  object* object_ptr = p_comm->get_attached_object();
  if (object_ptr)
  {
    object_ptr->detach_comment( p_comm );
  }
  if (p_comm)
  {
    p_comm->detach_from_object();
  }
}

void diagram::select_object( object* p_object )
{
  deselect_all_objects();
  plus_select_object( p_object );
}

void diagram::plus_select_object( object* p_object )
{
  p_object->select();
  m_selected_objects.Add( p_object );
}

void diagram::deselect_object( object* p_object )
{
  p_object->deselect();
  if ( m_selected_objects.Index( p_object ) != wxNOT_FOUND )
  {
    m_selected_objects.Remove( p_object );
  }
}

unsigned int diagram::count_selected_objects( void )
{
  return m_selected_objects.GetCount();
}

object* diagram::get_selected_object( int p_index )
{
  return m_selected_objects.Item( p_index );
}
