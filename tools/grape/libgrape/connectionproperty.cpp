// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file connectionproperty.cpp
//
// Implements the connection_property class.

#include "connection.h"

using namespace grape::libgrape;

connection_property::connection_property( void )
: object( NONE )
{
  // shouldn't be called
}

connection_property::connection_property( object_type p_type, connection *p_connection )
: object( p_type )
{
  assert( p_type == BLOCKED || p_type == VISIBLE );
  m_linetype = straight;
  m_breakpoints.Empty();
  m_has_property = p_connection;
}

connection_property::connection_property( const connection_property &p_connection_property )
: object( p_connection_property )
{
  m_linetype = p_connection_property.m_linetype;
  m_breakpoints = p_connection_property.m_breakpoints;
  m_has_property = p_connection_property.m_has_property;
}

connection_property::~connection_property( void )
{
  if ( m_has_property != 0 )
  {
    m_has_property->detach_property();
  }
}

void connection_property::attach( connection* p_conn )
{
  m_has_property = p_conn;
}

void connection_property::detach( void )
{
  m_has_property = 0;
}

connection * connection_property::get_attached_connection( void )
{
  return m_has_property;
}

void connection_property::add_breakpoint ( coordinate &p_coordinate, int p_place )
{
  m_breakpoints.Insert( p_coordinate, p_place, 1 );
}

void connection_property::move_breakpoint ( coordinate &p_coordinate, int p_place )
{
  m_breakpoints.RemoveAt( p_place, 1 );
  m_breakpoints.Insert( p_coordinate, p_place, 1 );
}

void connection_property::remove_breakpoint ( int p_place )
{
  m_breakpoints.RemoveAt( p_place, 1 );
}


void connection_property::set_linetype( linetype p_linetype )
{
  m_linetype = p_linetype;
}

linetype connection_property::get_linetype( void )
{
  return m_linetype;
}

bool connection_property::movable( void )
{
  return ( m_has_property == 0 );
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_connection_property );
