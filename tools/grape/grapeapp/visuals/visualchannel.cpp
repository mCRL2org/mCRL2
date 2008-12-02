// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualchannel.cpp
//
// Implements the visualchannel class.

#include <string>

#include "grape_glcanvas.h"
#include "channel.h"
#include "visualchannel.h"
#include "geometric.h"
#include "mcrl2/utilities/font_renderer.h"
#include "compoundreference.h"

using namespace grape::grapeapp;
using namespace mcrl2::utilities;

visualchannel::visualchannel( channel* p_channel )
{
  m_object = p_channel;
}

visualchannel::visualchannel( const visualchannel &p_channel )
: visual_object( p_channel )
{
}

visualchannel::~visualchannel( void )
{
}

void visualchannel::draw( void )
{
  channel *c = static_cast<channel *>(m_object);

  float x = m_object->get_coordinate().m_x;
  float y = m_object->get_coordinate().m_y;
  wxString name = c->get_name();

  // draw channel
  draw_channel( m_object->get_coordinate(), 0.5 * m_object->get_width(),  m_object->get_selected() );

  // draw channel name
  compound_reference* cref = c->get_reference();

  float ox = cref->get_coordinate().m_x;
  float oy = cref->get_coordinate().m_y;
  Alignment horizontal_align;
  Alignment vertical_align;
  float textx;
  float texty;
  if (ox > x)
  {
    // set text left
//    textx = x-g_text_space*2 - name.Len() * (g_text_space);
    textx = x - 0.5 * m_object->get_width();
    horizontal_align = al_left;
  } 
  else
  {
    // set text right
//    textx = x+g_text_space;
    textx = x + 0.5 * m_object->get_width();
    horizontal_align = al_right;
  }

  if (oy > y)
  {
    // set text down
//    texty = y-g_text_space*2;
    texty = y;
    vertical_align = al_bottom;
  } 
  else
  {
    // set text up
//    texty = y+g_text_space;
    texty = y + m_object->get_width();
    vertical_align = al_top;
  }
  //draw text
  grape_glcanvas::get_font_renderer()->draw_text( std::string(name.fn_str()), textx, texty, 0.0015f, horizontal_align, vertical_align);
}

bool visualchannel::is_inside( libgrape::coordinate &p_coord )
{
  // test is inside ellipse
  return is_inside_ellipse( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord ) || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );
}

grape_direction visualchannel::is_on_border( libgrape::coordinate &p_coord )
{
  // test if a coordinate is on the border of a channel
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}
