// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualvisible.cpp
//
// Implements the visualvisible class.

#include <string>

#include "grape_glcanvas.h"
#include "visible.h"
#include "visualvisible.h"
#include "geometric.h"
#include "connection.h"
#include "math.h"
#include "mcrl2/utilities/font_renderer.h"

namespace grape {

using namespace grape::grapeapp;
using namespace mcrl2::utilities::wx;

visualvisible::visualvisible( visible* p_visible, coordinate &p_coord, float &p_width, float &p_height )
{
  m_object = p_visible;
  m_coord = p_coord;
  m_width = p_width;
  m_height = p_height;
}

visualvisible::visualvisible( const visualvisible &p_visible )
: visual_object( p_visible )
{
  m_object = p_visible.m_object;
}

visualvisible::~visualvisible( void )
{
}

void visualvisible::draw( void )
{
  visible *vis_ptr = static_cast<visible *>(m_object);
  connection *conn_ptr = vis_ptr->get_attached_connection();
  coordinate on_visibility_frame;
  coordinate in_visibility_frame;

  if ( conn_ptr )
  {
    // get coordinate this
    in_visibility_frame = conn_ptr->get_coordinate();
  }
  else
  {
    // get attached object coordinate
    in_visibility_frame = m_object->get_coordinate();
  }

  on_visibility_frame = move_to_border_rectangle( m_coord, m_width, m_height, in_visibility_frame );

  // draw visible line
  draw_line( in_visibility_frame, on_visibility_frame, m_object->get_selected() );

  coordinate text_coordinate;
//  bool draw_on_side = false;
  Alignment horizontal_alignment;
  Alignment vertical_alignment;
  // Determine place to put the text
  // If it is to the left of the visibility frame.
  if ( on_visibility_frame.m_x <= m_coord.m_x )
  {
//    text_coordinate.m_x = on_visibility_frame.m_x - 3 * g_text_space;
    text_coordinate.m_x = on_visibility_frame.m_x - CHARWIDTH*0.0015f;
//    draw_on_side = true;
    horizontal_alignment = al_left;
  }
  else
  {
//    text_coordinate.m_x = on_visibility_frame.m_x + 3 * g_text_space;
    text_coordinate.m_x = on_visibility_frame.m_x;
//    draw_on_side = true;
    horizontal_alignment = al_right;
  }

  // if it is above or below the visibility frame
  // it is below
  if ( on_visibility_frame.m_y <= m_coord.m_y )
  {
//    text_coordinate.m_y = on_visibility_frame.m_y - g_text_space;
    text_coordinate.m_y = on_visibility_frame.m_y;
    vertical_alignment = al_bottom;
  }
  else
  {
//    text_coordinate.m_y = on_visibility_frame.m_y + g_text_space;
    text_coordinate.m_y = on_visibility_frame.m_y + CHARHEIGHT*0.0015f;
    vertical_alignment = al_top;
  }

//  render_text( vis_ptr->get_name(), text_coordinate.m_x, text_coordinate.m_y, 999, 999, draw_on_side );
  set_color(g_color_black, true);
  grape_glcanvas::get_font_renderer()->draw_text( std::string(vis_ptr->get_name().fn_str()), text_coordinate.m_x, text_coordinate.m_y, 0.0015f, horizontal_alignment, vertical_alignment );

  // do not draw a bounding box, visibles cannot be resized (they're always attached to the visibility frame and use their coordinate for the other endpoint, that's why)
}

bool visualvisible::is_inside( libgrape::coordinate &p_coord )
{
  visible *vis_ptr = static_cast<visible *>(m_object);
  connection *conn_ptr = vis_ptr->get_attached_connection();
  coordinate on_visibility_frame;
  coordinate in_visibility_frame;

  if ( conn_ptr )
  {
    // get coordinate this
    in_visibility_frame = conn_ptr->get_coordinate();
  }
  else
  {
    // get attached object coordinate
    in_visibility_frame = m_object->get_coordinate();
  }

  on_visibility_frame = move_to_border_rectangle( m_coord, m_width, m_height, in_visibility_frame );

  // test is inside line
  return is_inside_line( in_visibility_frame, on_visibility_frame, p_coord );
}

grape_direction visualvisible::is_on_border( libgrape::coordinate &p_coord )
{
  return GRAPE_DIR_NONE;
}

}

