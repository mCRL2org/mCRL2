// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_help.cpp
//
// Defines GraPE events for help actions.

#include <wx/html/helpctrl.h>

#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "event_help.h"

namespace grape {
	
using namespace grape::grapeapp;

grape_event_help::grape_event_help( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, false, _T( "show help" ) )
{
}

grape_event_help::~grape_event_help( void )
{
}

bool grape_event_help::Do( void )
{
  m_main_frame->get_help_controller()->DisplayContents();
  return true;
}

bool grape_event_help::Undo( void )
{
  // cannot be undone
  return true;
}

}

using namespace grape::grapeapp;

grape_event_about::grape_event_about( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, false, _T( "show about box" ) )
{
}

grape_event_about::~grape_event_about( void )
{
}

bool grape_event_about::Do( void )
{
  return true;
}

bool grape_event_about::Undo( void )
{
  // cannot be undone
  return true;
}

