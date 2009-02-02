// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file processdiagram.cpp
//
// Implements the process_diagram class.

#include "processdiagram.h"

using namespace grape::libgrape;

// private
process_diagram::process_diagram(void)
: diagram()
{
  m_initial_designators.Empty();
  m_reference_states.Empty();
  m_states.Empty();
  m_terminating_transitions.Empty();
  m_nonterminating_transitions.Empty();
}

process_diagram::process_diagram( const process_diagram &p_process_diagram )
: diagram( p_process_diagram )
{
  m_preamble = p_process_diagram.m_preamble;
  m_initial_designators = p_process_diagram.m_initial_designators;
  m_reference_states = p_process_diagram.m_reference_states;
  m_states = p_process_diagram.m_states;
  m_terminating_transitions = p_process_diagram.m_terminating_transitions;
  m_nonterminating_transitions = p_process_diagram.m_nonterminating_transitions;
}

process_diagram::~process_diagram(void)
{
  m_initial_designators.Clear();
  m_reference_states.Clear();
  m_states.Clear();
  m_terminating_transitions.Clear();
  m_nonterminating_transitions.Clear();
}

preamble * process_diagram::get_preamble( void )
{
  return &( m_preamble );
}

void process_diagram::set_preamble( const preamble &p_preamble )
{
  m_preamble = p_preamble;
}

state* process_diagram::add_state( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height )
{
  // deselect all objects
  deselect_all_objects();

  // Create new state
  state* new_state = new state;
  new_state->set_id( p_id );
  new_state->set_coordinate( p_coord );
  new_state->set_width( p_def_width );
  new_state->set_height( p_def_height );
  new_state->set_diagram( this );
  wxString name;
  name.Printf( _T("State%d" ), p_id );
  new_state->set_name( name );
  select_object( new_state );

  // Establish relationships
  m_states.Add( new_state );
  return new_state;
}

void process_diagram::remove_state( state* p_state )
{
  deselect_object( p_state );

  // Remove all transitions that have this state as beginstate
  while ( p_state->count_transition_beginstate() )
  {
    transition* trans_ptr = p_state->get_transition_beginstate( 0 );
    p_state->detach_transition_beginstate( trans_ptr );
    delete_transition( trans_ptr );
  } // end while

  // Remove all transitions that have this state as endstate
  while ( p_state->count_transition_endstate() )
  {
    nonterminating_transition* ntt_ptr = p_state->get_transition_endstate( 0 );
    p_state->detach_transition_endstate( ntt_ptr );
    delete_transition( ntt_ptr );
  } // end while

  // Remove all initial designators that designate this state
  while ( p_state->count_initial_designator() )
  {
    initial_designator* init_ptr = p_state->get_initial_designator( 0 );
    p_state->detach_initial_designator( init_ptr );
    int m = m_initial_designators.Index( *init_ptr );
    if ( m != wxNOT_FOUND )
    {
      m_initial_designators.Detach( m );
    }
    delete init_ptr;
  } // end while

  // Remove the state itself
  int i = m_states.Index( *p_state );
  if ( i != wxNOT_FOUND )
  {
    state* del_state = m_states.Detach( i );
    delete del_state;
  }
}

unsigned int process_diagram::count_state( void )
{
  return m_states.GetCount();
}

state* process_diagram::get_state( int p_i )
{
  return &( m_states.Item( p_i ) );
}

arr_state* process_diagram::get_state_list( void )
{
  return &m_states;
}



reference_state* process_diagram::add_reference_state( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height )
{
  // deselect all objects
  deselect_all_objects();

  // Create new reference_state
  reference_state* new_ref_state = new reference_state;
  new_ref_state->set_id( p_id );
  new_ref_state->set_coordinate( p_coord );
  new_ref_state->set_width( p_def_width );
  new_ref_state->set_height( p_def_height );
  new_ref_state->set_diagram( this );
  select_object( new_ref_state );

  // Establish relationships
  m_reference_states.Add( new_ref_state );
  return new_ref_state;
}

void process_diagram::remove_reference_state( reference_state* p_state )
{
  deselect_object( p_state );

  // Remove all transitions that have this state as beginstate
  while ( p_state->count_transition_beginstate() )
  {
    transition* trans_ptr = p_state->get_transition_beginstate( 0 );
    p_state->detach_transition_beginstate( trans_ptr );
    delete_transition( trans_ptr );
  } // end while

  // Remove all transitions that have this state as endstate
  while ( p_state->count_transition_endstate() )
  {
    nonterminating_transition* ntt_ptr = p_state->get_transition_endstate( 0 );
    p_state->detach_transition_endstate( ntt_ptr );
    delete_transition( ntt_ptr );
  } // end while

  // Remove all initial designators that designate this state
  while ( p_state->count_initial_designator() )
  {
    initial_designator* init_ptr = p_state->get_initial_designator( 0 );
    p_state->detach_initial_designator( init_ptr );
    int m = m_initial_designators.Index( *init_ptr );
    if ( m != wxNOT_FOUND )
    {
      m_initial_designators.Detach( m );
      delete init_ptr;
    }
  } // end while

  // Remove the state itself
  int i = m_reference_states.Index( *p_state );
  if ( i != wxNOT_FOUND )
  {
    reference_state* del_state = m_reference_states.Detach( i );
    delete del_state;
  }
}

unsigned int process_diagram::count_reference_state( void )
{
  return m_reference_states.GetCount();
}

reference_state* process_diagram::get_reference_state( int p_i )
{
  return &( m_reference_states.Item( p_i ) );
}

arr_reference_state* process_diagram::get_reference_state_list( void )
{
  return &m_reference_states;
}


nonterminating_transition* process_diagram::add_nonterminating_transition( unsigned int p_id, compound_state* p_beginstate, compound_state* p_endstate )
{
  // deselect all objects
  deselect_all_objects();

  // Create new nonterminating_transition
  nonterminating_transition* new_ntt = new nonterminating_transition(p_beginstate, p_endstate);
  new_ntt->set_id( p_id );

  // Calculate coordinate of transition based on average between beginstate and endstate
  coordinate new_coord;
  // except when beginstate == endstate
  if ( p_beginstate == p_endstate )
  {
    if ( p_beginstate )
    {
      new_coord.m_x = p_beginstate->get_coordinate().m_x - p_beginstate->get_width();
      new_coord.m_y = p_beginstate->get_coordinate().m_y - p_beginstate->get_height();
    }
  }
  else
  {
    if ( ( p_beginstate ) && ( p_endstate ) )
    {
      new_coord.m_x = 0.5 * ( p_beginstate->get_coordinate().m_x + p_endstate->get_coordinate().m_x );
      new_coord.m_y = 0.5 * ( p_beginstate->get_coordinate().m_y + p_endstate->get_coordinate().m_y );
    }
  }
  new_ntt->set_coordinate( new_coord );
  new_ntt->set_diagram( this );
  select_object( new_ntt );

  // Establish relationships
  new_ntt->attach_beginstate( p_beginstate );
  if ( p_beginstate ) p_beginstate->attach_transition_beginstate( new_ntt );
  new_ntt->attach_endstate( p_endstate );
  if ( p_endstate ) p_endstate->attach_transition_endstate( new_ntt );
  m_nonterminating_transitions.Add( new_ntt );
  return new_ntt;
}

void process_diagram::remove_nonterminating_transition( nonterminating_transition* p_ntt )
{
  deselect_object( p_ntt );

 // Remove the nonterminating transition itself
  int i = m_nonterminating_transitions.Index( *p_ntt );
  if ( i != wxNOT_FOUND )
  {
    nonterminating_transition* del_ntt = m_nonterminating_transitions.Detach( i );
    delete del_ntt;
  }
}

unsigned int process_diagram::count_nonterminating_transition( void )
{
  return m_nonterminating_transitions.GetCount();
}

nonterminating_transition* process_diagram::get_nonterminating_transition( int p_i )
{
  return &( m_nonterminating_transitions.Item( p_i ) );
}

arr_nonterminating_transition* process_diagram::get_nonterminating_transition_list( void )
{
  return &m_nonterminating_transitions;
}


terminating_transition* process_diagram::add_terminating_transition( unsigned int p_id, compound_state* p_beginstate, coordinate &p_endcoordinate )
{
  // deselect all objects
  deselect_all_objects();

  //Create new terminating transition
  terminating_transition* new_tt = new terminating_transition( p_beginstate );
  new_tt->set_id( p_id );
  select_object( new_tt );

  // Calculate coordinate and width of transition based on beginstate and endstate
  coordinate new_coord;
  if ( p_beginstate )
  {
    new_coord.m_x = p_beginstate->get_coordinate().m_x;
    new_coord.m_y = p_beginstate->get_coordinate().m_y;
    new_tt->set_coordinate( new_coord );
  }

  float new_width;
  new_width = p_endcoordinate.m_x - new_coord.m_x;
  new_tt->set_width( new_width );

  float new_height;
  new_height = p_endcoordinate.m_y - new_coord.m_y;
  new_tt->set_height( new_height );

  new_tt->set_diagram( this );

  // Establish relationships
  new_tt->attach_beginstate( p_beginstate );
  if ( p_beginstate ) p_beginstate->attach_transition_beginstate( new_tt );
  m_terminating_transitions.Add( new_tt );
  return new_tt;
}

void process_diagram::remove_terminating_transition( terminating_transition* p_trans )
{
  deselect_object( p_trans );

  // Remove the nonterminating transition itself
  int i = m_terminating_transitions.Index( *p_trans );
  if ( i != wxNOT_FOUND )
  {
    terminating_transition* del_trans = m_terminating_transitions.Detach( i );
    delete del_trans;
  }
}

unsigned int process_diagram::count_terminating_transition( void )
{
  return m_terminating_transitions.GetCount();
}

terminating_transition* process_diagram::get_terminating_transition( int p_i )
{
  return &( m_terminating_transitions.Item( p_i ));
}

arr_terminating_transition* process_diagram::get_terminating_transition_list( void )
{
  return &m_terminating_transitions;
}

void process_diagram::attach_transition_beginstate( transition* p_trans, compound_state* p_state )
{
  // Detach the transition first, if necessary
  compound_state* state_ptr = p_trans->get_beginstate();
  if ( state_ptr != 0 )
  {
    detach_transition_beginstate( p_trans );
  }
  // Establish relationships
  p_trans->attach_beginstate( p_state );
  p_state->attach_transition_beginstate( p_trans );
}

void process_diagram::detach_transition_beginstate( transition* p_trans )
{
  // Remove the reference from the state to the transition.
  compound_state* beginstate_ptr = p_trans->get_beginstate();
  if ( beginstate_ptr != 0 )
  {
    beginstate_ptr->detach_transition_beginstate( p_trans );
  }

  // Remove the reference from the transition to the state.
  p_trans->detach_beginstate();
}

void process_diagram::attach_nonterminating_transition_endstate( nonterminating_transition* p_ntt, compound_state* p_state )
{
  // Detach the transition first, if necessary
  compound_state* state_ptr = p_ntt->get_endstate();
  if ( state_ptr != 0 )
  {
    detach_nonterminating_transition_endstate( p_ntt );
  }
  // Establish relationships
  p_ntt->attach_endstate( p_state );
  p_state->attach_transition_endstate( p_ntt );
}

void process_diagram::detach_nonterminating_transition_endstate( nonterminating_transition* p_ntt )
{
  // Remove the reference from the state to the transition.
  compound_state * endstate_ptr = p_ntt->get_endstate();
  endstate_ptr->detach_transition_endstate( p_ntt );

  // Remove the reference from the transition to the state.
  p_ntt->detach_endstate();
}

initial_designator* process_diagram::add_initial_designator( unsigned int p_id, compound_state* p_state, float p_def_width, float p_def_height, coordinate &p_coord )
{
  // deselect all objects
  deselect_all_objects();

  // Create new initial designator
  initial_designator* new_init_d = new initial_designator;
  new_init_d->set_id( p_id );
  select_object( new_init_d );

  new_init_d->set_coordinate( p_coord );

  new_init_d->set_width( p_def_width );
  new_init_d->set_height( p_def_height );
  new_init_d->set_diagram( this );

  // Establish relationships
  new_init_d->attach( p_state );
  if ( p_state != 0 )
  {
    p_state->attach_initial_designator( new_init_d );
  }
  m_initial_designators.Add( new_init_d );
  return new_init_d;
}

void process_diagram::remove_initial_designator( initial_designator* p_init )
{
  deselect_object( p_init );

  // Remove the nonterminating transition itself
  int i = m_initial_designators.Index( *p_init );
  if ( i != wxNOT_FOUND )
  {
    initial_designator* del_init = m_initial_designators.Detach( i );
    delete del_init;
  }
}

unsigned int process_diagram::count_initial_designator( void )
{
  return m_initial_designators.GetCount();
}

initial_designator* process_diagram::get_initial_designator( int p_i )
{
  return &( m_initial_designators.Item( p_i ) );
}

arr_initial_designator* process_diagram::get_initial_designator_list( void )
{
  return &m_initial_designators;
}

void process_diagram::attach_initial_designator( initial_designator* p_init, compound_state* p_state )
{
  // Detach the initial designator first, if necessary
  compound_state* state_ptr = p_init->get_attached_state();
  if ( state_ptr != 0 )
  {
    coordinate c = p_init->get_coordinate();
    detach_initial_designator( p_init, c );
  }
  // Establish relationships
  p_state->attach_initial_designator( p_init );
  p_init->attach( p_state );
}


void process_diagram::detach_initial_designator( initial_designator* p_init, coordinate &p_coord )
{
  // Remove the reference from the state to the initial designator.
  compound_state * state_ptr = p_init->get_attached_state();
  state_ptr->detach_initial_designator( p_init );

  // Remove the reference from the initial designator to the state.
  p_init->detach();

  // Set coordinate of initial designator.
  p_init->set_coordinate( p_coord );
}

void process_diagram::select_all_objects( void )
{
  deselect_all_objects();
  int count_comment = m_comments.GetCount();
  for ( int i = 0; i < count_comment; ++i )
  {
    comment* comm_ptr = &( m_comments.Item( i ) );
    plus_select_object( comm_ptr );
  }
  int count_init = m_initial_designators.GetCount();
  for ( int i = 0; i < count_init; ++i )
  {
    initial_designator* init_ptr = &( m_initial_designators.Item( i ) );
    plus_select_object( init_ptr );
  }
  int count_state = m_states.GetCount();
  for ( int i = 0; i < count_state; ++i )
  {
    state* state_ptr = &( m_states.Item( i ) );
    plus_select_object( state_ptr );
  }
  int count_ref_state = m_reference_states.GetCount();
  for ( int i = 0; i < count_ref_state; ++i )
  {
    reference_state* ref_state_ptr = &( m_reference_states.Item( i ) );
    plus_select_object( ref_state_ptr );
  }
  int count_tt = m_terminating_transitions.GetCount();
  for ( int i = 0; i < count_tt; ++i )
  {
    terminating_transition* tt_ptr = &( m_terminating_transitions.Item( i ) );
    plus_select_object( tt_ptr );
  }
  int count_ntt = m_nonterminating_transitions.GetCount();
  for ( int i = 0; i < count_ntt; ++i )
  {
    nonterminating_transition* ntt_ptr = &( m_nonterminating_transitions.Item( i ) );
    plus_select_object( ntt_ptr );
  }
}

void process_diagram::deselect_all_objects( void )
{
  int count_comment = m_comments.GetCount();
  for ( int i = 0; i < count_comment; ++i )
  {
    comment* comm_ptr = &( m_comments.Item( i ) );
    deselect_object( comm_ptr );
  }
  int count_init = m_initial_designators.GetCount();
  for ( int i = 0; i < count_init; ++i )
  {
    initial_designator* init_ptr = &( m_initial_designators.Item( i ) );
    deselect_object( init_ptr );
  }
  int count_state = m_states.GetCount();
  for ( int i = 0; i < count_state; ++i )
  {
    state* state_ptr = &( m_states.Item( i ) );
    deselect_object( state_ptr );
  }
  int count_ref_state = m_reference_states.GetCount();
  for ( int i = 0; i < count_ref_state; ++i )
  {
    reference_state* ref_state_ptr = &( m_reference_states.Item( i ) );
    deselect_object( ref_state_ptr );
  }
  int count_tt = m_terminating_transitions.GetCount();
  for ( int i = 0; i < count_tt; ++i )
  {
    terminating_transition* tt_ptr = &( m_terminating_transitions.Item( i ) );
    deselect_object( tt_ptr );
  }
  int count_ntt = m_nonterminating_transitions.GetCount();
  for ( int i = 0; i < count_ntt; ++i )
  {
    nonterminating_transition* ntt_ptr = &( m_nonterminating_transitions.Item( i ) );
    deselect_object( ntt_ptr );
  }
}

// public
void process_diagram::delete_transition( transition* p_trans )
{
  if ( p_trans != 0 )
  {
    // Try nonterminating transition
    nonterminating_transition* del_ntt = dynamic_cast<nonterminating_transition*> ( p_trans );
    if ( del_ntt != 0 ) // Cast succesfulr
    {
      int m = m_nonterminating_transitions.Index( *del_ntt );
      if ( m != wxNOT_FOUND )
      {
        m_nonterminating_transitions.Detach( m );
        delete del_ntt;
      } // end if
    }
    else // Cast failed
    {
      // Try terminating transition
      terminating_transition* del_tt = dynamic_cast<terminating_transition*> ( p_trans );
      if ( del_tt != 0 ) // Cast succesful
      {
        int m = m_terminating_transitions.Index( *del_tt );
        if ( m != wxNOT_FOUND )
        {
          m_terminating_transitions.Detach( m );
          delete del_tt;
        } // end if
      } // end if del_tt
    } // end if del_ntt
  } // end if p_trans
}

object* process_diagram::find_object( process_diagram* p_proc_dia, unsigned int p_id, object_type p_type )
{
  bool b = p_type == ANY || p_type == COMMENT;
  for ( unsigned int i = 0; b && i < p_proc_dia->count_comment(); ++i )
  {
    comment* comm_ptr = p_proc_dia->get_comment( i );
    if ( comm_ptr->get_id() == p_id )
    {
      return comm_ptr;
    }
  }

  b = p_type == ANY || p_type == STATE;
  for ( unsigned int i = 0; b && i < p_proc_dia->count_state(); ++i )
  {
    state* state_ptr = p_proc_dia->get_state( i );
    if ( state_ptr->get_id() == p_id )
    {
      return state_ptr;
    }
  }

  b = p_type == ANY || p_type == REFERENCE_STATE;
  for ( unsigned int i = 0; b && i < p_proc_dia->count_reference_state(); ++i )
  {
    reference_state* ref_state_ptr = p_proc_dia->get_reference_state( i );
    if ( ref_state_ptr->get_id() == p_id )
    {
      return ref_state_ptr;
    }
  }

  b = p_type == ANY || p_type == NONTERMINATING_TRANSITION;
  for ( unsigned int i = 0; b && i < p_proc_dia->count_nonterminating_transition(); ++i )
  {
    nonterminating_transition* ntt_ptr = p_proc_dia->get_nonterminating_transition( i );
    if ( ntt_ptr->get_id() == p_id )
    {
      return ntt_ptr;
    }
  }

  b = p_type == ANY || p_type == TERMINATING_TRANSITION;
  for ( unsigned int i = 0; b && i < p_proc_dia->count_terminating_transition(); ++i )
  {
    terminating_transition* tt_ptr = p_proc_dia->get_terminating_transition( i );
    if ( tt_ptr->get_id() == p_id )
    {
      return tt_ptr;
    }
  }

  b = p_type == ANY || p_type == INITIAL_DESIGNATOR;
  for ( unsigned int i = 0; b && i < p_proc_dia->count_initial_designator(); ++i )
  {
    initial_designator* init_ptr = p_proc_dia->get_initial_designator( i );
    if ( init_ptr->get_id() == p_id )
    {
      return init_ptr;
    }
  }
  return 0;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_process_diagram );
