// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file specification.cpp
//
// Implements the GrapeSpecification class.

#include "specification.h"

using namespace grape::libgrape;

// public
grape_specification::grape_specification( void )
{
  m_datatype_specification.set_declarations( wxEmptyString );
  m_process_diagrams.Empty();
  m_architecture_diagrams.Empty();
}

grape_specification::grape_specification( const grape_specification &p_spec )
{
  m_datatype_specification = p_spec.m_datatype_specification;
  m_process_diagrams = p_spec.m_process_diagrams;
  m_architecture_diagrams = p_spec.m_architecture_diagrams;
}

grape_specification::~grape_specification(void)
{
  for ( unsigned int i = 0; i < m_process_diagrams.GetCount(); ++i )
  {
    process_diagram& del_proc_dia = m_process_diagrams.Item( i );
    remove_process_diagram( &del_proc_dia );
  }
  m_process_diagrams.Clear();
  for ( unsigned int i = 0; i < m_architecture_diagrams.GetCount(); ++i )
  {
    architecture_diagram &del_arch_dia = m_architecture_diagrams.Item( i );
    remove_architecture_diagram( &del_arch_dia );
  }
  m_architecture_diagrams.Clear();
}


datatype_specification* grape_specification::get_datatype_specification( void )
{
  return &( m_datatype_specification );
}

arr_process_diagram* grape_specification::get_process_diagram_list( void )
{
  return &( m_process_diagrams );
}

arr_architecture_diagram* grape_specification::get_architecture_diagram_list( void )
{
  return &( m_architecture_diagrams );
}

architecture_diagram* grape_specification::add_architecture_diagram( unsigned int p_id, const wxString &p_name, int p_index )
{
  int index = p_index;
  while ( exists_diagram( p_name, index ) )
  {
    ++index;
  }

  // if no diagram with that name exists yet
  architecture_diagram* new_dia = new architecture_diagram;
  new_dia->set_name( p_name + wxString::Format( _T( "%d" ), index ) );
  new_dia->set_id( p_id );
  m_architecture_diagrams.Add( new_dia );
  return new_dia;
}

void grape_specification::remove_architecture_diagram( architecture_diagram* p_arch_dia )
{
  int n = m_architecture_diagrams.Index( *p_arch_dia );
  if ( n != wxNOT_FOUND )
  {
    architecture_diagram* del_diagram = m_architecture_diagrams.Detach( n );
    del_diagram->select_all_objects();
    // remove all comments from the architecture diagram
    for ( unsigned int i = 0; i < p_arch_dia->count_comment(); ++i )
    {
      comment* del_comment = p_arch_dia->get_comment( i );
      p_arch_dia->remove_comment( del_comment );
    }
    // remove all channel communications from the architecture diagram
    for ( unsigned int i = 0; i < p_arch_dia->count_channel_communication(); ++i )
    {
      channel_communication* del_c_comm = p_arch_dia->get_channel_communication( i );
      p_arch_dia->remove_channel_communication( del_c_comm );
    }
    // remove all channels from the architecture diagram
    for ( unsigned int i = 0; i < p_arch_dia->count_channel(); ++i )
    {
      channel* del_chan = p_arch_dia->get_channel( i );
      p_arch_dia->remove_channel( del_chan );
    }
    // remove all architecture references from the architecture diagram
    for ( unsigned int i = 0; i < p_arch_dia->count_architecture_reference(); ++i )
    {
      architecture_reference* del_arch_ref = p_arch_dia->get_architecture_reference( i );
      p_arch_dia->remove_architecture_reference( del_arch_ref );
    }
    // remove all process references from the architecture diagram
    for ( unsigned int i = 0; i < p_arch_dia->count_process_reference(); ++i )
    {
      process_reference* del_proc_ref = p_arch_dia->get_process_reference( i );
      p_arch_dia->remove_process_reference( del_proc_ref );
    }

    delete del_diagram;
  }
}

unsigned int grape_specification::count_architecture_diagram( void )
{
  return m_architecture_diagrams.GetCount();
}

architecture_diagram* grape_specification::get_architecture_diagram( int p_index )
{
  return &( m_architecture_diagrams.Item( p_index ) );
}

process_diagram* grape_specification::add_process_diagram( unsigned int p_id, const wxString &p_name, int p_index )
{
  int index = p_index;
  while ( exists_diagram( p_name, index ) )
  {
    ++index;
  }

  // if no diagram with that name exists yet
  process_diagram* new_proc = new process_diagram;
  new_proc->set_name( p_name + wxString::Format( _T( "%d" ), index ) );
  new_proc->set_id( p_id );
  m_process_diagrams.Add( new_proc );
  return new_proc;
}

void grape_specification::remove_process_diagram( process_diagram* p_proc_dia )
{
  int n = m_process_diagrams.Index( *p_proc_dia );
  if ( n != wxNOT_FOUND )
  {
    process_diagram* del_diagram = m_process_diagrams.Detach( n );
    del_diagram->select_all_objects();

    // remove all comments from the process diagram
    for ( unsigned int i = 0; i < p_proc_dia->count_comment(); ++i )
    {
      comment* del_comment = p_proc_dia->get_comment( i );
      p_proc_dia->remove_comment( del_comment );
    }
    // remove all initial designators from the process diagram
    for ( unsigned int i = 0; i < p_proc_dia->count_initial_designator(); ++i )
    {
      initial_designator* del_init = p_proc_dia->get_initial_designator( i );
      p_proc_dia->remove_initial_designator( del_init );
    }
    // remove all reference states from the process diagram
    for ( unsigned int i = 0; i < p_proc_dia->count_reference_state(); ++i )
    {
      reference_state* del_ref_state = p_proc_dia->get_reference_state( i );
      p_proc_dia->remove_reference_state( del_ref_state );
    }
    // remove all states from the process diagram
    for ( unsigned int i = 0; i < p_proc_dia->count_state(); ++i )
    {
      state* del_state = p_proc_dia->get_state( i );
      p_proc_dia->remove_state( del_state );
    }
    // remove all terminating transitions from the process diagram
    for ( unsigned int i = 0; i < p_proc_dia->count_terminating_transition(); ++i )
    {
      terminating_transition* del_tt = p_proc_dia->get_terminating_transition( i );
      p_proc_dia->remove_terminating_transition( del_tt );
    }
    // remove all nonterminating transitions from the process diagram
    for ( unsigned int i = 0; i < p_proc_dia->count_nonterminating_transition(); ++i )
    {
      nonterminating_transition* del_ntt = p_proc_dia->get_nonterminating_transition( i );
      p_proc_dia->remove_nonterminating_transition( del_ntt );
    }

    delete del_diagram;
  }
}

unsigned int grape_specification::count_process_diagram( void )
{
  return m_process_diagrams.GetCount();
}

process_diagram* grape_specification::get_process_diagram( int p_index )
{
  return &( m_process_diagrams.Item( p_index ) );
}

// private

bool grape_specification::exists_architecture_diagram( const wxString &p_name, int p_index )
{
  int count = m_architecture_diagrams.GetCount();
  for ( int j = 0; j < count; ++j )
  {
    architecture_diagram* arch_dia = &( m_architecture_diagrams.Item( j ) );
    wxString concat_name = p_name + wxString::Format( _T( "%d" ), p_index );
    if ( arch_dia->get_name() == concat_name )
    {
      return true;
    } // end if
  } // end for
  return false;
}

bool grape_specification::exists_process_diagram( const wxString &p_name, int p_index )
{
  int count = m_process_diagrams.GetCount();
  for ( int j = 0; j < count; ++j )
  {
    process_diagram* proc_dia = & ( m_process_diagrams.Item( j ) );
    wxString concat_name = p_name + wxString::Format( _T( "%d" ), p_index );
    if ( proc_dia->get_name() == concat_name )
    {
      return true;
    } // end if
  } // end for
  return false;
}

bool grape_specification::exists_diagram( const wxString &p_name, int p_index )
{
  return ( ( exists_process_diagram( p_name, p_index ) ) || ( exists_architecture_diagram( p_name, p_index ) ) );
}

object* grape_specification::find_object( grape_specification *p_spec, unsigned int p_id, object_type p_type )
{
  object *result = 0;
  unsigned int count = p_spec->count_process_diagram();
  for ( unsigned int i = 0; !result && i < count; ++i )
  {
    result = process_diagram::find_object( p_spec->get_process_diagram(i), p_id, p_type );
  }

  count = p_spec->count_architecture_diagram();
  for ( unsigned int i = 0; !result && i < count; ++i )
  {
    result = architecture_diagram::find_object( p_spec->get_architecture_diagram(i), p_id, p_type );
  }

  return result;
}

void grape_specification::check_references( wxString p_name, architecture_diagram* p_arch_dia )
{
  // check whether any architecturereferences exist that point to the specified name.
  for ( unsigned int i = 0; i < count_architecture_diagram(); ++i )
  {
    architecture_diagram* arch_dia_ptr = get_architecture_diagram( i );
    for ( unsigned int j = 0; j < arch_dia_ptr->count_architecture_reference(); ++j )
    {
      architecture_reference* arch_ref_ptr = arch_dia_ptr->get_architecture_reference( j );
      bool samename = arch_ref_ptr->get_name() == p_name;
      if ( samename || arch_ref_ptr->get_relationship_refers_to() == p_arch_dia )
      {
        // only set a new name when p_arch_dia is not 0. Because 0 means
        // the diagram has been removed, but we want to keep the reference
        // name.
        if ( p_arch_dia )
        {
          arch_ref_ptr->set_name( p_arch_dia->get_name() );
        }
        if ( samename ) // if the name matches, also set the relation
        {
          arch_ref_ptr->set_relationship_refers_to( p_arch_dia );
        }
      }
    }
  }
}

void grape_specification::check_references( wxString p_name, process_diagram* p_proc_dia )
{
  // check whether any processreferences exist that point to the specified name.
  for ( unsigned int i = 0; i < count_architecture_diagram(); ++i )
  {
    architecture_diagram* arch_dia_ptr = get_architecture_diagram( i );
    for ( unsigned int j = 0; j < arch_dia_ptr->count_process_reference(); ++j )
    {
      process_reference* proc_ref_ptr = arch_dia_ptr->get_process_reference( j );
      bool samename = proc_ref_ptr->get_name() == p_name;
      if ( samename || proc_ref_ptr->get_relationship_refers_to() == p_proc_dia )
      {
        // see check_references above
        if ( p_proc_dia )
        {
          proc_ref_ptr->set_name( p_proc_dia->get_name() );
        }
        if ( samename )
        {
          proc_ref_ptr->set_relationship_refers_to( p_proc_dia );
        }
      }
    }
  }
  // check whether any referencestates exist that point to the specified name.
  for ( unsigned int i = 0; i < count_process_diagram(); ++i )
  {
    process_diagram* proc_dia_ptr = get_process_diagram( i );
    for ( unsigned int j = 0; j < proc_dia_ptr->count_reference_state(); ++j )
    {
      reference_state* ref_state_ptr = proc_dia_ptr->get_reference_state( j );
      bool samename = ref_state_ptr->get_name() == p_name;
      if ( samename || ref_state_ptr->get_relationship_refers_to() == p_proc_dia )
      {
        // see check_references above
        if ( p_proc_dia )
        {
          ref_state_ptr->set_name( p_proc_dia->get_name() );
        }
        if ( samename )
        {
          ref_state_ptr->set_relationship_refers_to( p_proc_dia );
        }
      }
    }
  }
}
