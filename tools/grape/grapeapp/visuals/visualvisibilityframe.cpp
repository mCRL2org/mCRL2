// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualvisibilityframe.cpp
//
// Implements the visualvisibility_frame class.

#include "visualvisibilityframe.h"
#include "geometric.h"

using namespace grape::grapeapp;

visualvisibility_frame::visualvisibility_frame( coordinate& p_coordinate, float p_width, float p_height )
{
  m_object = 0;
  m_coordinate = p_coordinate;
  m_width = p_width;
  m_height = p_height;
}

visualvisibility_frame::visualvisibility_frame( const visualvisibility_frame &p_visibility_frame )
: visual_object( p_visibility_frame )
{
  m_coordinate = p_visibility_frame.m_coordinate;
  m_width = p_visibility_frame.m_width;
  m_height = p_visibility_frame.m_height;
}

visualvisibility_frame::~visualvisibility_frame( void )
{
}

void visualvisibility_frame::draw( void )
{
  //draw visibility
  draw_line_rectangle( m_coordinate, m_width, m_height, false, g_color_black);
}

bool visualvisibility_frame::is_inside( coordinate &p_coord )
{
  return is_inside_rectangle( m_coordinate, m_width, m_height, p_coord, false );
}

coordinate visualvisibility_frame::get_coordinate( void )
{
  return m_coordinate;
}

float visualvisibility_frame::get_width( void )
{
  return m_width;
}

float visualvisibility_frame::get_height( void )
{
  return m_height;
}

grape_direction visualvisibility_frame::is_on_border( coordinate & )
{
  return GRAPE_DIR_NONE;
}
