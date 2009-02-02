// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualblocked.cpp
//
// Implements the visualblocked class.

#include "blocked.h"
#include "visualblocked.h"
#include "geometric.h"
#include "connection.h"

using namespace grape::grapeapp;

visualblocked::visualblocked( blocked* p_blocked )
{
  m_object = p_blocked;
}

visualblocked::visualblocked( const visualblocked &p_blocked )
: visual_object( p_blocked )
{
}

visualblocked::~visualblocked( void )
{
}

void visualblocked::draw( void )
{
  blocked *block_ptr = static_cast<blocked *>(m_object);
  connection *conn_ptr = block_ptr->get_attached_connection();
  coordinate line_end_coordinate;
  if ( conn_ptr )
  {
    line_end_coordinate = conn_ptr->get_coordinate();
  }
  else
  {
    coordinate blocked_coordinate = m_object->get_coordinate();
    line_end_coordinate.m_x = blocked_coordinate.m_x + m_object->get_width();
    line_end_coordinate.m_y = blocked_coordinate.m_y + m_object->get_height();
  }

  // draw line
  draw_line( m_object->get_coordinate(), line_end_coordinate, m_object->get_selected());

  // draw white rectangle
  draw_filled_rectangle( m_object->get_coordinate(), 0.1f, 0.1f, false, g_color_white);

  // draw blocked
  draw_cross( m_object->get_coordinate(), 0.1f, 0.1f, m_object->get_selected());
  draw_line_rectangle( m_object->get_coordinate(), 0.1f, 0.1f, m_object->get_selected(), g_color_blocked);

  // draw bounding box; only drawn if the object is selected
  draw_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), m_object->get_selected() );
}

bool visualblocked::is_inside( coordinate &p_coord )
{
  blocked *block_ptr = static_cast<blocked *>(m_object);
  connection *conn_ptr = block_ptr->get_attached_connection();
  coordinate line_end_coordinate;
  if ( conn_ptr )
  {
    line_end_coordinate = conn_ptr->get_coordinate();
  }
  else
  {
    coordinate blocked_coordinate = m_object->get_coordinate();
    line_end_coordinate.m_x = blocked_coordinate.m_x + m_object->get_width();
    line_end_coordinate.m_y = blocked_coordinate.m_y + m_object->get_height();
  }

  bool result = is_inside_rectangle( m_object->get_coordinate(), 0.1f, 0.1f, p_coord, false);
  return result || is_inside_line( m_object->get_coordinate(), line_end_coordinate, p_coord)  || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );
}

grape_direction visualblocked::is_on_border( coordinate &p_coord )
{
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}
