// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualpreamble.cpp
//
// Implements the visualpreamble class.

#include "preamble.h"
#include "visualpreamble.h"
#include "geometric.h"
#include "font_renderer.h"

const float preamble_x_coordinate = 0.51f;
const float preamble_y_coordinate = -0.16f;
const float preamble_width = 1.0f;
const float preamble_height = 0.3f;

using namespace grape::grapeapp;

visualpreamble::visualpreamble( preamble* p_preamble )
{
  m_object = 0;
  m_preamble = p_preamble;
}

visualpreamble::visualpreamble( const visualpreamble &p_preamble )
: visual_object( p_preamble )
{
}

visualpreamble::~visualpreamble( void )
{
}

void visualpreamble::draw( void )
{
  wxString var = m_preamble->get_local_variable_declarations();
  wxString par = m_preamble->get_parameter_declarations();
  float x = preamble_x_coordinate - 0.5 * preamble_width;
  float y = preamble_y_coordinate - 0.5 * preamble_height;

  // draw preamble
  coordinate center = { preamble_x_coordinate, preamble_y_coordinate };
  draw_filled_rectangle( center, preamble_width, preamble_height, false, g_color_preamble );

  // draw preamble variable declarations
  render_text(_T("Parameters"), x, y+preamble_height-g_text_space, preamble_width*0.5, preamble_height);
  render_text(par, x+0.05f, y+preamble_height-g_text_space*3, preamble_width*0.5-0.05f, preamble_height+g_text_space);

  // draw preamble parameter declarations
  render_text(_T("Local Variables"), x+preamble_width*0.5, y+preamble_height-g_text_space*1, preamble_width*0.5, preamble_height);
  render_text(var, x+preamble_width*0.5+0.05f, y+preamble_height-g_text_space*3, preamble_width*0.5-0.05f, preamble_height+g_text_space);

  // draw horizontal line
  coordinate p_middle_top = {preamble_x_coordinate, y+0.025f};
  coordinate p_middle_bottom = {preamble_x_coordinate, y+preamble_height-0.025f};
  draw_line( p_middle_top, p_middle_bottom, false);
 
  // draw vertical line
  coordinate p_top_top = {preamble_x_coordinate-preamble_width*0.5+0.025f, y+preamble_height-g_text_space*2};
  coordinate p_top_bottom = {preamble_x_coordinate+preamble_width*0.5-0.025f, y+preamble_height-g_text_space*2};
  draw_line( p_top_top, p_top_bottom, false);
}

bool visualpreamble::get_selected( void ) const
{
  return false;
}

preamble* visualpreamble::get_preamble( void )
{
  return m_preamble;
}
bool visualpreamble::is_inside( libgrape::coordinate &p_coord )
{
  float low_x = preamble_x_coordinate - 0.5 * preamble_width;
  float low_y = preamble_y_coordinate + 0.5 * preamble_height;
  float high_x = low_x + preamble_width;
  float high_y = low_y - preamble_height;

  // test inside architecture reference
  bool between_x_coordinates = ( p_coord.m_x > low_x ) && ( p_coord.m_x < high_x );
  bool between_y_coordinates = ( p_coord.m_y < low_y ) && ( p_coord.m_y > high_y );
  return ( between_x_coordinates && between_y_coordinates );
}

grape_direction visualpreamble::is_on_border( libgrape::coordinate &p_coord )
{
  // it doesn't matter for a preamble where it was clicked
  return GRAPE_DIR_NONE;
}
