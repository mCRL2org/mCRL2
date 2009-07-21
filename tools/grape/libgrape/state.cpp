
// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file state.cpp
//
// Implements the state class.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include "state.h"

using namespace grape::libgrape;

state::state( void )
: compound_state( STATE )
{
  m_name = wxEmptyString;
}

state::state( const state &p_state )
: compound_state( p_state )
{
  m_name = p_state.m_name;
}

state::~state( void )
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


// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_state )
