// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualcomment.cpp
//
// Implements the visualcomment class.

#include "wx.hpp" // precompiled headers

#include <string>

#include "grape_glcanvas.h"
#include "comment.h"
#include "visualcomment.h"
#include "geometric.h"
#include "mcrl2/utilities/font_renderer.h"

namespace grape {

using namespace grape::grapeapp;
using namespace mcrl2::utilities::wx;

visualcomment::visualcomment( comment* p_comment )
{
  m_object = p_comment;
  m_reference_selected = false;
}

visualcomment::visualcomment( const visualcomment &p_visualcomment )
: visual_object( p_visualcomment )
{
  m_reference_selected = p_visualcomment.m_reference_selected;
}

visualcomment::~visualcomment( void )
{
}

void visualcomment::draw( void )
{
  comment *c = static_cast<comment *>(m_object);

  float width = m_object->get_width();
  float height = m_object->get_height();
  float x = m_object->get_coordinate().m_x-width/2;
  float y = m_object->get_coordinate().m_y-height/2;
  wxString text = c->get_text();
  bool selected = m_object->get_selected();
  object* attached_object = c->get_attached_object();

  if (attached_object != 0)
  {
    // draw dashed attached object line
    draw_line( m_object->get_coordinate(), attached_object->get_coordinate(), m_reference_selected, g_color_black, true);
  }

  // draw comment
  draw_filled_rectangle( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), selected, g_color_comment);

  // draw comment text
  grape_glcanvas::get_font_renderer()->draw_wrapped_text( std::string(text.fn_str()), x+0.01f, x+width-0.01f, y+height-0.01f, y+0.01f, 0.0015f, al_left, al_top );

  // draw comment reference rectangle
  if (m_object->get_selected()) draw_filled_rectangle( get_reference_coordinate(), 0.04f, 0.04f, selected, g_color_white);

  // draw bounding box
  draw_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), m_object->get_selected() );
}

bool visualcomment::is_inside( libgrape::coordinate &p_coord )
{  
  comment *c = static_cast<comment *>(m_object);
  object* attached_object = c->get_attached_object();
  
  // test if the mouse is on the reference line  
  if (attached_object != 0)
  {
    m_reference_selected = is_inside_line( m_object->get_coordinate(), attached_object->get_coordinate(), p_coord);
  }   
  
  return m_reference_selected || is_inside_rectangle( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord ) || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );
}

grape_direction visualcomment::is_on_border( libgrape::coordinate &p_coord )
{
  // test if a coordinate is on the border of a architecture refernece
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}

bool visualcomment::is_inside_reference( libgrape::coordinate &p_coord )
{
  return is_inside_rectangle(get_reference_coordinate(), 0.04f, 0.04f, p_coord);
}

coordinate visualcomment::get_reference_coordinate()
{
  coordinate ref_coord = {m_object->get_coordinate().m_x + m_object->get_width() * 0.5 - 0.03f, m_object->get_coordinate().m_y - m_object->get_height() * 0.5 + 0.03f};
  return ref_coord;
}

bool visualcomment::get_reference_selected()
{
  return m_reference_selected;
}

}
