// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file initialdesignator.cpp
//
// Implements the initial_designator class.

#include "initialdesignator.h"
#include "compoundstate.h"

using namespace grape::libgrape;

initial_designator::initial_designator( void )
: object( INITIAL_DESIGNATOR )
{
  m_designates = 0;
}

initial_designator::initial_designator( const initial_designator &p_init_designator )
: object( p_init_designator )
{
  m_designates = p_init_designator.m_designates;
}

initial_designator::~initial_designator( void )
{
  if ( m_designates != 0 )
  {
    // Remove all references from compoundstates to this initial designator.
    m_designates->detach_initial_designator( this );
  }
}

void initial_designator::attach( compound_state* p_compound_state )
{
  m_designates = p_compound_state;
}

void initial_designator::detach( void )
{
  m_designates = 0;
}

compound_state * initial_designator::get_attached_state( void )
{
  return m_designates;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_initial_designator );
