// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_edit.cpp
//
// Defines GraPE events for editting operations.

#include "wx.hpp" // precompiled headers

#include "grape_clipboard.h"
#include "grape_frame.h"
#include "grape_glcanvas.h"

#include "event_edit.h"

using namespace grape::grapeapp;

grape_event_cut::grape_event_cut( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, true, _T( "cut" ) )
{
  // Low priority; not implemented
}

grape_event_cut::~grape_event_cut( void )
{
  // Low priority; not implemented
}

bool grape_event_cut::Do( void )
{
  // Low priority; not implemented
  return true;
}

bool grape_event_cut::Undo( void )
{
  // Low priority; not implemented
  return true;
}

grape_event_copy::grape_event_copy( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, false, _T( "copy" ) )
{
  // Low priority; not implemented
}

grape_event_copy::~grape_event_copy( void )
{
  // Low priority; not implemented
}

bool grape_event_copy::Do( void )
{
  // Low priority; not implemented
  return true;
}

bool grape_event_copy::Undo( void )
{
  // cannot be undone
  return true;
}

grape_event_paste::grape_event_paste( grape_frame *p_main_frame )
: grape_event_base( p_main_frame, true, _T( "paste" ) )
{
  // Low priority; not implemented
}

grape_event_paste::~grape_event_paste( void )
{
  // Low priority; not implemented
}

bool grape_event_paste::Do( void )
{
  // Low priority; not implemented
  return true;
}

bool grape_event_paste::Undo( void )
{
  // Low priority; not implemented
  return true;
}
