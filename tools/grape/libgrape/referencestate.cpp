// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file referencestate.cpp
//
// Implements the ReferenceState class.

#include <wx/tokenzr.h>

#include "referencestate.h"

using namespace grape::libgrape;

reference_state::reference_state(void)
: compound_state( REFERENCE_STATE )
{
  m_parameter_assignments.Empty();
  m_refers_to_process = 0;
}

reference_state::reference_state( const reference_state &p_ref_state )
: compound_state( p_ref_state )
{
  m_parameter_assignments = p_ref_state.m_parameter_assignments;
  m_refers_to_process = p_ref_state.m_refers_to_process;
}

reference_state::~reference_state( void )
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
  m_parameter_assignments.Clear();
}

process_diagram* reference_state::get_relationship_refers_to( void )
{
  return m_refers_to_process;
}

void reference_state::set_relationship_refers_to( process_diagram* p_proc_diagram )
{
  m_refers_to_process = p_proc_diagram;
}

list_of_varupdate reference_state::get_parameter_updates( void ) const
{
  return m_parameter_assignments;
}

void reference_state::set_parameter_updates( const list_of_varupdate& p_parameter_assignments )
{
  m_parameter_assignments = p_parameter_assignments;
}

bool reference_state::set_text( const wxString &p_text )
{
  bool valid = true;
  m_parameter_assignments.Empty();
  wxStringTokenizer tkw( p_text, _T(";") );
  varupdate var_update;
  while( tkw.HasMoreTokens() )
  {
    wxString token = tkw.GetNextToken();
    valid &= var_update.set_varupdate( token );
    if (valid)
    {
      m_parameter_assignments.Add( var_update );
    }
  }
  return true;
}

wxString reference_state::get_text() const
{
  wxString result;
  for ( unsigned int i = 0; i < m_parameter_assignments.GetCount(); ++i )
  {
    varupdate parameter_assignment = m_parameter_assignments.Item( i );
    result += parameter_assignment.get_lhs() + _T( ":=" ) + parameter_assignment.get_rhs() + _T( ";\n" );
  }
  return result;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_reference_state );
