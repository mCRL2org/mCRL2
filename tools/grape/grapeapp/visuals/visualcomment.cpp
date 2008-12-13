// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualcomment.cpp
//
// Implements the visualcomment class.

#include <string>

#include "grape_glcanvas.h"
#include "comment.h"
#include "visualcomment.h"
#include "geometric.h"
#include "mcrl2/utilities/font_renderer.h"

namespace grape {
	
using namespace grape::grapeapp;
using namespace mcrl2::utilities;

visualcomment::visualcomment( comment* p_comment )
{
  m_object = p_comment;
}

visualcomment::visualcomment( const visualcomment &p_visualcomment )
: visual_object( p_visualcomment )
{
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
    // draw attached object line
    draw_line( m_object->get_coordinate(), attached_object->get_coordinate(),selected);
  }

  // draw comment
  draw_filled_rectangle( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), selected, g_color_comment);

  // draw comment text
//  render_text(text, x, y+height-g_text_space, width, height+g_text_space);
  grape_glcanvas::get_font_renderer()->draw_wrapped_text( std::string(text.fn_str()), x, x+width, y+height, y, 0.0015f, al_left, al_top );

  // draw bounding box; only drawn if the object is selected
  draw_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), m_object->get_selected() );
}

bool visualcomment::is_inside( libgrape::coordinate &p_coord )
{
  return is_inside_rectangle( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord ) || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );
}

grape_direction visualcomment::is_on_border( libgrape::coordinate &p_coord )
{
  // test if a coordinate is on the border of a architecture refernece
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}

}

