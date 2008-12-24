// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file object.cpp
//
// Implements the abstract object class.

#include "object.h"
#include "diagram.h"

using namespace grape::libgrape;

object::object( void )
{
  // shouldn't be called
}

object::object( object_type p_type )
{
  m_type = p_type;
  init();
}

void object::init()
{
  m_id = -1;
  m_selected = false;
  m_width = 0.25f;
  m_height = 0.25f;
  m_comments.Clear();
  m_in_diagram = 0;
  m_coordinate.m_x = 0;
  m_coordinate.m_y = 0;
}

object::object( const object &p_object )
{
  m_id = p_object.m_id;
  m_type = p_object.m_type;
  m_selected = p_object.m_selected;
  m_width = p_object.m_width;
  m_height = p_object.m_height;
  m_comments = p_object.m_comments;
  m_in_diagram = p_object.m_in_diagram;
  m_coordinate = p_object.m_coordinate;
}

object::~object( void )
{
  // Detach the object from all comments it was attached to.
  for ( unsigned int i = 0; i < m_comments.GetCount(); ++i )
  {
    comment* comm_ptr = m_comments.Item( i );
    comm_ptr->detach_from_object();
  }
  m_comments.Empty();
}

object_type object::get_type( void )
{
  return m_type;
}


coordinate object::get_coordinate( void ) const
{
  return m_coordinate;
}

void object::set_coordinate( coordinate &p_coordinate )
{
  m_coordinate = p_coordinate;
}

unsigned int object::get_id( void ) const
{
  return m_id;
}

void object::set_id( unsigned int p_id )
{
  m_id = p_id;
}

void object::select( void )
{
  m_selected = true;
}

void object::deselect( void )
{
  m_selected = false;
}

bool object::get_selected( void ) const
{
  return m_selected;
}

float object::get_width( void ) const
{
  return m_width;
}

void object::set_width( float p_width )
{
  m_width = p_width;
}

float object::get_height( void ) const
{
  return m_height;
}

void object::set_height( float p_height )
{
  m_height = p_height;
}

void object::attach_comment( comment* p_comment )
{
  m_comments.Add( p_comment );
}

void object::detach_comment( comment *p_comment )
{
  m_comments.Remove( p_comment );
}

unsigned int object::count_comment( void )
{
  return m_comments.GetCount();
}

comment* object::get_comment( int p_i )
{
  return m_comments.Item( p_i );
}

diagram* object::get_diagram( void )
{
  return m_in_diagram;
}

void object::set_diagram( diagram* p_diag )
{
  m_in_diagram = p_diag;
}

