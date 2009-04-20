// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualstate.cpp
//
// Implements the visualstate class.

#include "wx.hpp" // precompiled headers

#include <string>

#include "grape_glcanvas.h"
#include "state.h"
#include "visualstate.h"
#include "geometric.h"
#include "mcrl2/utilities/font_renderer.h"

namespace grape {

using namespace grape::grapeapp;
using namespace mcrl2::utilities::wx;

visualstate::visualstate( state* p_state )
{
  m_object = p_state;
}

visualstate::visualstate( const visualstate &p_state )
: visual_object( p_state )
{
}

visualstate::~visualstate( void )
{
}

void visualstate::draw( void )
{
  state *s = static_cast<state *>(m_object);

  float dwidth = m_object->get_width()/2;
  float dheight = m_object->get_height()/2;
  float x = m_object->get_coordinate().m_x-dwidth;
  float y = m_object->get_coordinate().m_y-dheight;
  wxString name = s->get_name();

  // draw state
  draw_state( m_object->get_coordinate(), 0.5f * m_object->get_width(), 0.5f * m_object->get_height(), m_object->get_selected());

  // draw state name
  grape_glcanvas::get_font_renderer()->draw_wrapped_text( std::string(name.fn_str()), x, x+m_object->get_width(), y+m_object->get_height(), y, 0.0015f, al_center, al_center );

  // draw bounding box; only drawn if the object is selected
  draw_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), m_object->get_selected() );
}

bool visualstate::is_inside( coordinate &p_coord )
{
  // test inside visual state
  return is_inside_ellipse( m_object->get_coordinate(), 0.5 * m_object->get_width(), 0.5 * m_object->get_height(), p_coord ) || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );
}

grape_direction visualstate::is_on_border( libgrape::coordinate &p_coord )
{
  // test if a coordinate is on the border of a state
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}

}

