// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nonterminatingtransition.cpp
//
// Implements the nonterminating_transition class.

#include "nonterminatingtransition.h"
#include "compoundstate.h"
#include "visuals/geometric.h"

using namespace grape::libgrape;

nonterminating_transition::nonterminating_transition( compound_state* p_beginstate, compound_state* p_endstate )
: transition( NONTERMINATING_TRANSITION )
{
  m_linetype = straight;
  m_breakpoints.Empty();
  m_beginstate = p_beginstate;
  m_endstate = p_endstate;
}

nonterminating_transition::nonterminating_transition( const nonterminating_transition &p_ntt )
: transition( p_ntt )
{
  m_endstate = p_ntt.m_endstate;
}

nonterminating_transition::~nonterminating_transition( void )
{
  if ( m_beginstate != 0 )
  {
    // Remove the reference from the state that is the beginstate of this transition
    m_beginstate->detach_transition_beginstate( this );
  }

  if ( m_endstate != 0 )
  {
    // Remove the reference from the state that is the endstate of this transition
    m_endstate->detach_transition_endstate( this );
  }

  // Free all space used by this object
  m_breakpoints.Clear();
}

void nonterminating_transition::attach_endstate( compound_state* p_endstate )
{
  m_endstate = p_endstate;
}

compound_state * nonterminating_transition::get_endstate( void )
{
  return m_endstate;
}

void nonterminating_transition::detach_endstate( void )
{
  m_endstate = 0;
}

coordinate nonterminating_transition::get_end_coordinate( void )
{
  coordinate head_coordinate;

  compound_state *beginstate = get_beginstate();
  compound_state *endstate = get_endstate();

  // if the transition has a beginstate and an endstate
  if ( ( beginstate != 0 ) && ( endstate != 0 ) )
  {
    if ( beginstate != endstate )
    {
     head_coordinate = grape::grapeapp::get_coordinate_on_edge( get_coordinate(), endstate ) - get_coordinate();
    }
    else
    {
      coordinate controlpoint = get_coordinate();

      //position of transition head
      float angle = -atan2(controlpoint.m_x - beginstate->get_coordinate().m_x, controlpoint.m_y - beginstate->get_coordinate().m_y)+M_PI*0.6;
      coordinate head_corner = { cos(angle)*(beginstate->get_width()*0.5+0.1) + beginstate->get_coordinate().m_x, sin(angle)*(beginstate->get_height()*0.5+0.1) + beginstate->get_coordinate().m_y };

      //intersection position with compound state
      head_coordinate = grape::grapeapp::get_coordinate_on_edge( head_corner, endstate ) - get_coordinate();
    }
  }

  // if the transition only has a endstate.
  if ( endstate != 0 )
  {
    head_coordinate = grape::grapeapp::get_coordinate_on_edge( get_coordinate(), endstate) - get_coordinate();
  }

  if ( endstate == 0 )
  {
    head_coordinate.m_x = get_width();
    head_coordinate.m_y = get_height();
  }

  return head_coordinate;
}


coordinate nonterminating_transition::get_begin_coordinate( void )
{
  compound_state *beginstate = get_beginstate();
  compound_state *endstate = get_endstate();

  //get coordinate from transition class
  coordinate tail_coordinate = transition::get_begin_coordinate();

  // if the transition has a beginstate and an endstate
  if ( ( beginstate != 0 ) && ( endstate != 0 ) )
  {
    if ( beginstate != endstate )
    {
      tail_coordinate = grape::grapeapp::get_coordinate_on_edge( get_coordinate(), beginstate ) - get_coordinate();
    }
    else
    {
      coordinate controlpoint = get_coordinate();

      //position of transition base
      float angle = -atan2(controlpoint.m_x - beginstate->get_coordinate().m_x, controlpoint.m_y - beginstate->get_coordinate().m_y)+M_PI*0.4;
      coordinate base_corner = { cos(angle)*(beginstate->get_width()*0.5+0.1) + beginstate->get_coordinate().m_x, sin(angle)*(beginstate->get_height()*0.5+0.1) + beginstate->get_coordinate().m_y };

      //intersection position with compound state
      tail_coordinate = grape::grapeapp::get_coordinate_on_edge( base_corner, beginstate ) - get_coordinate();
    }
  }

  return tail_coordinate;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_nonterminating_transition );
