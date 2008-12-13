// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_help.cpp
//
// Defines GraPE events for help actions.

#include <wx/aboutdlg.h>
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

extern const char *g_tool_name;
extern const char *g_tool_author;
extern const char *g_mcrl2_version;
extern const char *g_mcrl2_copyright_year;

extern wxAboutDialogInfo get_about_info();

bool grape_event_about::Do( void )
{
  wxAboutDialogInfo info(get_about_info());

  wxAboutBox( info );
  return true;
}

bool grape_event_about::Undo( void )
{
  // cannot be undone
  return true;
}

