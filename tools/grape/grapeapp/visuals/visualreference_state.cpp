// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualreference_state.cpp
//
// Implements the visualreference_state class.

#include "referencestate.h"
#include "visualreference_state.h"
#include "geometric.h"
#include "font_renderer.h"

using namespace grape::grapeapp;

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
  render_text(ref_state->get_name(), x + width*0.5, y + height - g_text_space, width, height*0.25 + g_text_space, true);

  // draw reference state text
  render_text(ref_state->get_text(), x, y + height*0.75 - g_text_space, width, height*0.75 + g_text_space);

  // draw bounding box; only drawn if the object is selected
  draw_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), m_object->get_selected() );
}

bool visualreference_state::is_inside( libgrape::coordinate &p_coord )
{
  return is_inside_rectangle( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord ) || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );

}

grape_direction visualreference_state::is_on_border( libgrape::coordinate &p_coord )
{
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}
