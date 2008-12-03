// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_clipboard.cpp
//
// Defines the clipboard of the application.

#include "grape_clipboard.h"
#include "grape_frame.h"
#include "specification.h"

namespace grape {
	
using namespace grape::grapeapp;
using namespace grape::libgrape;

grape_clipboard::grape_clipboard( grape_frame* p_main_frame )
{
  m_main_frame = p_main_frame;
  m_objects.Empty();
}

grape_clipboard::~grape_clipboard( void )
{
  m_objects.Clear();
}

void grape_clipboard::add( libgrape::arr_object_ptr p_objects )
{
  m_objects = p_objects;
}

arr_object_ptr grape_clipboard::get( void )
{
  return m_objects;
}

}