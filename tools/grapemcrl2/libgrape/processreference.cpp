// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file processreference.cpp
//
// Implements the process_reference class.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include <wx/tokenzr.h>

#include "processreference.h"

using namespace grape::libgrape;

process_reference::process_reference( void )
: compound_reference( PROCESS_REFERENCE )
{
  m_parameter_assignments.Clear();
  m_refers_to_process = 0;
}

process_reference::process_reference( const process_reference &p_process_ref )
: compound_reference( p_process_ref )
{
  m_parameter_assignments = p_process_ref.m_parameter_assignments;
  m_refers_to_process = p_process_ref.m_refers_to_process;
}

process_reference::~process_reference( void )
{

  // Remove all references to this object.
  for ( unsigned int i = 0; i < m_has_channel.GetCount(); ++i )
  {
    channel* channel_ptr = m_has_channel.Item(i);
    channel_ptr->detach_reference();
  }

  // Free all used resources.
  m_has_channel.Clear();
  m_parameter_assignments.Clear();
}

process_diagram* process_reference::get_relationship_refers_to( void )
{
  return m_refers_to_process;
}

void process_reference::set_relationship_refers_to( process_diagram* p_proc_diagram )
{
  m_refers_to_process = p_proc_diagram;
}

list_of_varupdate process_reference::get_parameter_updates( void ) const
{
  return m_parameter_assignments;
}

void process_reference::set_parameter_updates( const list_of_varupdate& p_parameter_assignments )
{
  m_parameter_assignments = p_parameter_assignments;
}

bool process_reference::set_text( const wxString &p_text )
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
  return valid;
}

wxString process_reference::get_text() const
{
  wxString result;
  for ( unsigned int i = 0; i < m_parameter_assignments.GetCount(); ++i )
  {
    varupdate parameter_assignment = m_parameter_assignments.Item( i );
    result += parameter_assignment.get_varupdate() + _T( ";\n" );
  }
  return result;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_process_reference )

