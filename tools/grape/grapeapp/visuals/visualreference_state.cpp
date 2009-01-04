// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualreference_state.cpp
//
// Implements the visualreference_state class.

#include <string>

#include "grape_glcanvas.h"
#include "referencestate.h"
#include "visualreference_state.h"
#include "geometric.h"
#include "mcrl2/utilities/font_renderer.h"

using namespace grape::grapeapp;
using namespace mcrl2::utilities::wx;

visualreference_state::visualreference_state( reference_state* p_reference_state )
{
  m_object = p_reference_state;
}

visualreference_state::visualreference_state( const visualreference_state &p_reference_state )
: visual_object( p_reference_state )
{
  m_object = p_reference_state.m_object;
}

visualreference_state::~visualreference_state( void )
{
}

void visualreference_state::draw( void )
{
  reference_state *ref_state = static_cast<reference_state *>(m_object);

  float width = m_object->get_width();
  float height = m_object->get_height();
  float x = m_object->get_coordinate().m_x-width/2;
  float y = m_object->get_coordinate().m_y-height/2;

  // draw reference state
  draw_process_reference( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), m_object->get_selected() );

  // draw reference state name 
  grape_glcanvas::get_font_renderer()->draw_wrapped_text( std::string(ref_state->get_name().fn_str()), x, x+width, y+height, y+height*0.75, 0.0015f, al_center, al_center );

  // draw reference state text
  grape_glcanvas::get_font_renderer()->draw_wrapped_text( std::string(ref_state->get_text().fn_str()), x, x+width, y+height*0.75, y, 0.0015f, al_left, al_top );

  // draw bounding box; only drawn if the object is selected
  draw_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), m_object->get_selected() );
}

bool visualreference_state::is_inside( coordinate &p_coord )
{
  return is_inside_rectangle( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord ) || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );

}

grape_direction visualreference_state::is_on_border( coordinate &p_coord )
{
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}
