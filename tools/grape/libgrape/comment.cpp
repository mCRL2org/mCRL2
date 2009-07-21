// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file comment.cpp
//
// Implements the comment class.

#include "wx.hpp" // precompiled headers

#include "wx/wx.h"

#include "comment.h"

using namespace grape::libgrape;

comment::comment( void )
: object( COMMENT )
{
  m_text = wxEmptyString;
  m_connected_to = 0;
}

comment::comment( const comment &p_comment )
: object( p_comment )
{
  m_text = p_comment.m_text;
  m_connected_to = p_comment.m_connected_to;
}

comment::~comment( void )
{
}

wxString comment::get_text( void ) const
{
  return m_text;
}

void comment::set_text( wxString &p_text )
{
  m_text = p_text;
}

void comment::attach_to_object( object* p_object )
{
  m_connected_to = p_object;
  m_connected_to->attach_comment( this );
}

void comment::detach_from_object( void )
{
  m_connected_to = 0;
}

object* comment::get_attached_object( void )
{
  return m_connected_to;
}

// WxWidgets dynamic array implementation.
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY( arr_comment )
