// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file connection.cpp
//
// Implements the Connection class.

#include "connection.h"

using namespace grape::libgrape;

connection::connection( void )
: object( NONE )
{
  // shouldn't be called
}

connection::connection( object_type p_type )
: object( p_type )
{
  assert( p_type == CHANNEL || p_type == CHANNEL_COMMUNICATION );
  m_has_property = 0;
}

connection::connection( const connection &p_connection )
: object( p_connection )
{
  m_has_property = p_connection.m_has_property;
}

connection::~connection( void )
{
  if ( m_has_property != 0 )
  {
    // Remove all references to the connection.
    m_has_property->detach();
  }
}

void connection::attach_property( connection_property* p_prop )
{
  m_has_property = p_prop;
}

void connection::detach_property( void )
{
  m_has_property = 0;
}

connection_property* connection::get_property( void )
{
  return m_has_property;
}
