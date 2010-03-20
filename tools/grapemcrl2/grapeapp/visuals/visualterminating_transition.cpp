// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualterminating_transition.cpp
//
// Implements the visualterminating_transition class.

#include "wx.hpp" // precompiled headers

#include "terminatingtransition.h"
#include "visualterminating_transition.h"
#include "compoundstate.h"
#include "transition.h"
#include "label.h"
#include "geometric.h"
#include "pics/termtransition2.xpm"

#ifdef __APPLE__
  #include <OpenGL/glu.h>
#else
  #include <GL/glu.h>
#endif

namespace grape {

using namespace grape::grapeapp;

visualterminating_transition::visualterminating_transition( terminating_transition* p_terminating_transition )
{
  m_object = p_terminating_transition;
  
  // Enable texture mapping
  glEnable(GL_TEXTURE_2D);
  
  // Allocate texture memory
  glGenTextures(1, &texture_id);

  // Bind texture
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Create a bitmap from xpm format
  wxBitmap bitmap = wxBitmap( termtransition2_xpm );
  
  // Create an image from the bitmap
	wxImage image = ( bitmap.ConvertToImage() );
	
	// Build the texture
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 28, 28, GL_RGB, GL_UNSIGNED_BYTE, image.GetData());

}

visualterminating_transition::visualterminating_transition( const visualterminating_transition &p_terminating_transition )
: visual_object( p_terminating_transition )
{
}

visualterminating_transition::~visualterminating_transition( void )
{
}

void visualterminating_transition::draw( void )
{
  terminating_transition *tt = static_cast<terminating_transition*>(m_object);
  compound_state *begin_state = tt->get_beginstate();

  float width = m_object->get_width();
  float height = m_object->get_height();
  float x = m_object->get_coordinate().m_x;
  float y = m_object->get_coordinate().m_y;
  coordinate fc;
  coordinate tc = {x+width, y+height};

  bool selected = m_object->get_selected();

  //beginstate
  if (begin_state != 0)
  {
    // calculate x, y around begin state
    fc = get_coordinate_on_edge(tc, begin_state);
  }

  //no beginstate
  if (begin_state == 0)
  {
    // calculate x, y begin state
    fc.m_x = x;
    fc.m_y = y;
  }

  // draw text
  terminating_transition *transition = static_cast<terminating_transition *>(m_object);
  wxString text = transition->get_label()->get_text();

  // calculate point on border of tick
  coordinate end_arrow = move_to_border_rectangle( tc, 0.1f, 0.1f, fc );
  // draw line
  draw_terminating_transition( fc, end_arrow, selected, text );

  // draw tick
  glColor3f(1.0, 1.0, 1.0);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3f(x+width-0.05, y+height-0.05, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(x+width-0.05, y+height+0.05, 0.0);
    glTexCoord2f(1.0, 0.0); glVertex3f(x+width+0.05, y+height+0.05, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(x+width+0.05, y+height-0.05, 0.0);
  glEnd();

  // draw box
  draw_line_rectangle( tc, 0.1f, 0.1f, selected, g_color_black );     
}

bool visualterminating_transition::is_inside( libgrape::coordinate &p_coord )
{
  terminating_transition *tt = static_cast<terminating_transition*>(m_object);
  compound_state *begin_state = tt->get_beginstate();

  float width = m_object->get_width();
  float height = m_object->get_height();
  float x = m_object->get_coordinate().m_x;
  float y = m_object->get_coordinate().m_y;
  coordinate fc;
  coordinate tc = {x+width, y+height};

  bool inside = false;

  //beginstate
  if (begin_state != 0)
  {
    // calculate x, y around begin state
    fc = get_coordinate_on_edge(tc, begin_state);
  }

  //no beginstate
  if (begin_state == 0)
  {
    // calculate x, y begin state
    fc.m_x = x;
    fc.m_y = y;
  }

  // test is inside line
  inside = inside || is_inside_line( fc, tc, p_coord );

  // test is inside rectangle
  inside = inside || is_inside_rectangle( tc, 0.1f, 0.1f, p_coord, false);

  inside = inside || ( grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() ) != GRAPE_DIR_NONE );

  // return result
  return inside;
}

grape_direction visualterminating_transition::is_on_border( libgrape::coordinate &p_coord )
{
  return grab_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), p_coord, m_object->get_selected() );
}

}

