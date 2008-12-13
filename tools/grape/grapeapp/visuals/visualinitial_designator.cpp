// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualinitial_designator.cpp
//
// Implements the visualinitial_designator class.

#include "initialdesignator.h"
#include "visualinitial_designator.h"
#include "geometric.h"
#include "compoundstate.h" 

namespace grape {
	
using namespace grape::grapeapp;

visualinitial_designator::visualinitial_designator( initial_designator* p_initial_designator )
{
  m_object = p_initial_designator;
}

visualinitial_designator::visualinitial_designator( const visualinitial_designator &p_initial_designator )
: visual_object( p_initial_designator )
{
}

visualinitial_designator::~visualinitial_designator( void )
{
}

void visualinitial_designator::draw( void )
{
  initial_designator *id = static_cast<initial_designator*>(m_object);

  bool selected = m_object->get_selected();

  compound_state * state_ptr = id->get_attached_state();

  if ( state_ptr )
  {
    // get intersection coordinate
    coordinate end_coordinate = get_coordinate_on_edge(m_object->get_coordinate(), state_ptr);

    // draw initial designator arrow
    draw_designator( m_object->get_coordinate(), end_coordinate, selected);
  }
  else
  {
    // draw initial designator arrow
    draw_designator( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), selected );
  }

  // draw bounding box; only drawn if the object is selected
  draw_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), m_object->get_selected() );
}

bool visualinitial_designator::is_inside( libgrape::coordinate &p_coord )
{
  initial_designator *init_ptr = static_cast<initial_designator*>(m_object);
  compound_state * state_ptr = init_ptr->get_attached_state();
  if ( state_ptr )
  {
    coordinate state_coord = get_coordinate_on_edge( m_object->get_coordinate(), state_ptr);
    // test if inside arrow
    return is_inside_designator( m_object->get_coordinate(), state_coord, p_coord) || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );
  }

  // test if inside arrow
  return is_inside_designator( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord)  || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );
}

grape_direction visualinitial_designator::is_on_border( libgrape::coordinate &p_coord )
{
  // test if a coordinate is on the border of an arrow
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}

}

