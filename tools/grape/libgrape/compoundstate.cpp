// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file compoundstate.cpp
//
// Implements the compound_state class.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include "compoundstate.h"

using namespace grape::libgrape;

compound_state::compound_state( void )
: object( NONE )
{
  // shouldn't be called
}

compound_state::compound_state( object_type p_type )
: object( p_type )
{
  assert( p_type == STATE || p_type == REFERENCE_STATE );
  m_name = wxEmptyString;
  m_current_state = false;
  m_designates.Empty();
  m_beginstate.Empty();
  m_endstate.Empty();
}

compound_state::compound_state( const compound_state &p_compound_state )
: object( p_compound_state )
{
  m_name = p_compound_state.m_name;
  m_current_state = p_compound_state.m_current_state;
  m_designates = p_compound_state.m_designates;
  m_beginstate = p_compound_state.m_beginstate;
  m_endstate = p_compound_state.m_endstate;
}

compound_state::~compound_state(void)
{
  // Remove all references from initial designators to this state.

  // for-loop declarations
  int count = m_designates.GetCount();
  for (int i = 0; i < count; ++i)
  {
    initial_designator* init_ptr = m_designates.Item( i );
    init_ptr->detach();
  }

  // Remove all references from transitions that have this state as beginstate.

  // for-loop declarations
  count = m_beginstate.GetCount();
  for (int i = 0; i < count; ++i)
  {
    transition* trans_ptr = m_beginstate.Item( i );
    trans_ptr->detach_beginstate();
  }

  // Remove all references from transitions that have this state as endstate.

  // for-loop declarations
  count = m_endstate.GetCount();
  for (int i = 0; i < count; ++i)
  {
    nonterminating_transition* trans_ptr = m_endstate.Item( i );
    trans_ptr->detach_endstate();
  }

  // Free all space used by this state
  m_designates.Clear();
  m_beginstate.Clear();
  m_endstate.Clear();
}

wxString compound_state::get_name( void )
{
  return m_name;
}

void compound_state::set_name( const wxString &p_name )
{
  m_name = p_name;
}

void compound_state::attach_transition_beginstate( transition* p_transition )
{
  m_beginstate.Add( p_transition );
}

transition* compound_state::get_transition_beginstate( int p_index )
{
  return m_beginstate.Item( p_index );
}

unsigned int compound_state::count_transition_beginstate( void )
{
  return m_beginstate.GetCount();
}

void compound_state::detach_transition_beginstate( transition* p_transition )
{
  int i = m_beginstate.Index(p_transition,false);
  if ( i != wxNOT_FOUND )
  {
    // Remove because it is an array of pointers.
    m_beginstate.Remove( m_beginstate.Item( i ) );
  }
}

void compound_state::attach_transition_endstate( nonterminating_transition* p_nonterminating_transition )
{
  m_endstate.Add( p_nonterminating_transition );
}

nonterminating_transition* compound_state::get_transition_endstate( int p_index )
{
  return m_endstate.Item( p_index );
}

unsigned int compound_state::count_transition_endstate( void )
{
  return m_endstate.GetCount();
}

void compound_state::detach_transition_endstate( nonterminating_transition* p_nonterminating_transition )
{
  int i = m_endstate.Index( p_nonterminating_transition, false );
  if ( i != wxNOT_FOUND )
  {
    // Note: Remove because it is an array of pointers.
    m_endstate.Remove( m_endstate.Item( i ) );
  }
}

void compound_state::attach_initial_designator( initial_designator* p_init )
{
  m_designates.Add( p_init );
}

void compound_state::detach_initial_designator( initial_designator* p_init )
{
  int i = m_designates.Index( p_init, false );
  if ( i != wxNOT_FOUND )
  {
    // Note: Remove because it is an array of pointers.
    m_designates.Remove( m_designates.Item( i ) );
  }
}

initial_designator* compound_state::get_initial_designator( int p_index )
{
  return m_designates.Item( p_index );
}

unsigned int compound_state::count_initial_designator( void )
{
  return m_designates.GetCount();
}

bool compound_state::get_current_state( void ) const
{
  return m_current_state;
}

void compound_state::set_current_state( bool p_current_state )
{
  m_current_state = p_current_state;
}
