// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file architecturereference.cpp
//
// Implements the architecture_reference class.

#include "architecturereference.h"
#include "architecturediagram.h"

using namespace grape::libgrape;

architecture_reference::architecture_reference(void)
: compound_reference( ARCHITECTURE_REFERENCE )
{
  m_refers_to_architecture = 0;
}

architecture_reference::architecture_reference( const architecture_reference &p_arch_ref )
: compound_reference( p_arch_ref )
{
  m_refers_to_architecture = p_arch_ref.m_refers_to_architecture;
}

architecture_reference::~architecture_reference(void)
{
  // Remove all references to this object.
  for ( unsigned int i = 0; i < m_has_channel.GetCount(); ++i )
  {
    channel* channel_ptr = m_has_channel.Item(i);
    channel_ptr->detach_reference();
  }

  // Free all used resources.
  m_has_channel.Clear();
}

architecture_diagram* architecture_reference::get_relationship_refers_to( void )
{
  return m_refers_to_architecture;
}

void architecture_reference::set_relationship_refers_to( architecture_diagram* p_arch_diagram )
{
  m_refers_to_architecture = p_arch_diagram;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(arr_architecture_reference);
