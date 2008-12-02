// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualarchitecture_reference.cpp
//
// Implements the visualarchitecture_reference class.

#include <string>
#include <math.h>
#include "grape_glcanvas.h"
#include "architecturereference.h"
#include "visualarchitecture_reference.h"
#include "geometric.h"
#include "mcrl2/utilities/font_renderer.h"

using namespace grape::grapeapp;
using namespace mcrl2::utilities;

visualarchitecture_reference::visualarchitecture_reference( architecture_reference* p_architecture_reference )
{
  m_object = p_architecture_reference;
}

visualarchitecture_reference::visualarchitecture_reference( const visualarchitecture_reference &p_architecture_reference )
: visual_object( p_architecture_reference )
{
}

visualarchitecture_reference::~visualarchitecture_reference( void )
{
}

void visualarchitecture_reference::draw( void )
{
  architecture_reference *archref = static_cast<architecture_reference*>(m_object);

  float width = m_object->get_width();
  float height = m_object->get_height();
  float x = m_object->get_coordinate().m_x-width/2;
  float y = m_object->get_coordinate().m_y-height/2;
  wxString name = archref->get_name();
  bool selected = m_object->get_selected();

  //draw architecture reference
  draw_filled_rectangle( m_object->get_coordinate(), width, height, selected, g_color_architecture_reference);

  //draw architecture reference name
  grape_glcanvas::get_font_renderer()->draw_wrapped_text( std::string(name.fn_str()), x, x+width, y+height, y, 0.0015f, al_center, al_center );

  // draw bounding box; only drawn if the object is selected
  draw_bounding_box( m_object->get_coordinate(), width, height, selected);
}

bool visualarchitecture_reference::is_inside( libgrape::coordinate &p_coord )
{
  return is_inside_rectangle( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord ) || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );
}

grape_direction visualarchitecture_reference::is_on_border( libgrape::coordinate &p_coord )
{
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}

coordinate visualarchitecture_reference::move_to_border( libgrape::coordinate &p_coord )
{
  return move_to_border_rectangle( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord );
}
