// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file terminatingtransition.cpp
//
// Implements the TerminatingTransition class.

#include "terminatingtransition.h"
#include "compoundstate.h"

using namespace grape::libgrape;

terminating_transition::terminating_transition( compound_state* p_beginstate )
: transition( TERMINATING_TRANSITION, p_beginstate )
{
}

terminating_transition::terminating_transition( const terminating_transition &p_tt )
: transition( p_tt )
{
}

terminating_transition::~terminating_transition( void )
{
  if ( m_beginstate != 0 )
  {
    // Remove all references from the state that is the beginstate of this transition
    m_beginstate->detach_transition_beginstate( this );
  }

  // Free all space used by this state
  m_breakpoints.Clear();
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_terminating_transition );
