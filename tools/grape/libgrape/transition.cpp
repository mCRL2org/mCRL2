// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transition.cpp
//
// Implements the Transition class.

#include "transition.h"
#include "compoundstate.h"
#include "visuals/geometric.h"

using namespace grape::libgrape;

transition::transition( void )
: object( NONE )
{
  // shouldn't be called
}

transition::transition( object_type p_type, compound_state* p_beginstate )
: object( p_type )
{
  assert( p_type == TERMINATING_TRANSITION || p_type == NONTERMINATING_TRANSITION );
  m_linetype = straight;
  m_breakpoints.Empty();
  m_beginstate = p_beginstate;
}

transition::transition( const transition &p_transition )
: object( p_transition )
{
  m_label = p_transition.m_label;
  m_linetype = p_transition.m_linetype;
  m_breakpoints = p_transition.m_breakpoints;
  m_beginstate = p_transition.m_beginstate;
}

transition::~transition( void )
{
}

void transition::set_label( const label& p_label )
{
  m_label = p_label;
}

label * transition::get_label( void )
{
  return &m_label;
}

void transition::attach_beginstate( compound_state* p_beginstate )
{
  m_beginstate = p_beginstate;
}

compound_state * transition::get_beginstate( void )
{
  return m_beginstate;
}

void transition::detach_beginstate( void )
{
  m_beginstate = 0;
}

linetype transition::get_linetype( void ) const
{
  return m_linetype;
}

void transition::set_linetype( const linetype &p_linetype )
{
  m_linetype = p_linetype;
}

void transition::add_breakpoint( const coordinate &p_coordinate )
{
  m_breakpoints.Add( &p_coordinate );
}

void transition::move_breakpoint( coordinate &p_breakpoint, const coordinate &p_coordinate )
{
  remove_breakpoint( p_breakpoint );
  add_breakpoint( p_coordinate );
}

void transition::remove_breakpoint( coordinate &p_breakpoint )
{
  int n = m_breakpoints.Index( p_breakpoint );
  if ( n != wxNOT_FOUND )
  {
    coordinate *c = m_breakpoints.Detach(n);
    delete c;
  }
}

list_of_coordinate *transition::get_breakpoints( void )
{
  return &m_breakpoints;
}


bool transition::movable( void ) const
{
  return m_beginstate == 0;
}

coordinate transition::get_begin_coordinate( void )
{ 
  coordinate tail_coordinate = get_coordinate();
  // get coordinate of begin state
  if (m_beginstate != 0) 
  {
    // if there is an endstate, find the intersection point with the end state
    tail_coordinate = grape::grapeapp::get_coordinate_on_edge( get_coordinate(), get_beginstate() );            
  }
  return tail_coordinate;
} 
