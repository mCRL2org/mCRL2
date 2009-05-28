// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualchannel_communication.cpp
//
// Implements the visualchannel_communication class.

#include "wx.hpp" // precompiled headers

#include "channelcommunication.h"
#include "visualchannel_communication.h"
#include "geometric.h"
#include "grape_glcanvas.h"
#include "mcrl2/utilities/font_renderer.h"

#include <algorithm>

namespace grape {

using namespace grape::grapeapp;
using namespace mcrl2::utilities::wx;

visualchannel_communication::visualchannel_communication( channel_communication* p_channel_communication )
{
  m_object = p_channel_communication;
  m_communication_selected = -1;
}

visualchannel_communication::visualchannel_communication( const visualchannel_communication &p_channel_communication )
: visual_object( p_channel_communication )
{
  m_communication_selected = p_channel_communication.m_communication_selected;
}

visualchannel_communication::~visualchannel_communication( void )
{
}

void visualchannel_communication::draw( void )
{
  channel_communication* comm = static_cast<channel_communication*>( m_object );

  //for all communications
  for ( unsigned int i = 0; i < comm->count_channel(); ++i )
  {
    channel* chan = comm->get_attached_channel( i );

    // draw communication line
    draw_line( m_object->get_coordinate(), chan->get_coordinate(), static_cast<unsigned>(m_communication_selected) == i);
  }

  channel_communication *cc = static_cast<channel_communication *>(m_object);

  float x = m_object->get_coordinate().m_x;
  float y = m_object->get_coordinate().m_y;
  wxString name_to = cc->get_name_to();

  // draw channel
  draw_channel( m_object->get_coordinate(), static_cast<float>(0.02), m_object->get_selected(), comm->get_channel_communication_type() );

  
  // only find empty position if there is text
  if (name_to != _T("")) 
  {
    static float* rotation = 0;
    rotation = new float[(comm->count_channel()+1)];
    
    //store all angles in the array
    for ( unsigned int i = 0; i < comm->count_channel(); ++i )
    {
      channel* chan = comm->get_attached_channel( i );
      coordinate new_coordinate = m_object->get_coordinate()-chan->get_coordinate();
      
      rotation[i] = M_PI+atan2(new_coordinate.m_y, new_coordinate.m_x);
    }
    std::sort(rotation, rotation+comm->count_channel());
    rotation[comm->count_channel()] = rotation[0]+2.0f*M_PI;
     
    float biggest_gap = 0;
    int gap_index = 0;
    
    // find biggest gap between communication lines
    for ( unsigned int i = 0; i < comm->count_channel(); ++i )
    {
      if (biggest_gap < rotation[i+1]-rotation[i])
      {
        biggest_gap = rotation[i+1]-rotation[i];
        gap_index = i;
      }
    }
    // calculate the actual angle
    float empty_rotation = (rotation[gap_index+1]+rotation[gap_index])*0.5;    
    while (empty_rotation < 0.0f) empty_rotation += static_cast<float> ( 2.0f*M_PI );
    while (empty_rotation >= 2.0f*M_PI) empty_rotation -= static_cast<float> ( 2.0f*M_PI );
    
    
    // align text
    Alignment align_horizontal = al_center;
    Alignment align_vertical = al_center;

    if ((empty_rotation >= 0.0f*M_PI) && (empty_rotation < 0.25f*M_PI)) align_horizontal = al_right;
    if ((empty_rotation >= 1.75f*M_PI) && (empty_rotation < 2.0f*M_PI)) align_horizontal = al_right;    
    if ((empty_rotation >= 0.75f*M_PI) && (empty_rotation < 1.25f*M_PI)) align_horizontal = al_left;    
    
    if ((empty_rotation >= 0.25f*M_PI) && (empty_rotation < 0.75f*M_PI)) align_vertical = al_top;
    if ((empty_rotation >= 1.25f*M_PI) && (empty_rotation < 1.75f*M_PI)) align_vertical = al_bottom;
    
    // draw text 
    grape_glcanvas::get_font_renderer()->draw_text(std::string(name_to.fn_str()), x + 0.05f*cos(empty_rotation), y + 0.025f + 0.05f*sin(empty_rotation), 0.0015f, align_horizontal, align_vertical);   
    if (rotation != 0) {
      delete [] rotation;
    }
  }
}

bool visualchannel_communication::is_inside( libgrape::coordinate &p_coord )
{
  m_communication_selected = -1;
  channel_communication* comm = static_cast<channel_communication*>( m_object );

  if (!is_inside_ellipse( m_object->get_coordinate(), 0.05f, 0.05f, p_coord))
  {
    //for all communications
    for ( unsigned int i = 0; i < comm->count_channel(); ++i )
    {
      channel* chan = comm->get_attached_channel( i );

      // test if the mouse is on the communication line
      if (is_inside_line( m_object->get_coordinate(), chan->get_coordinate(), p_coord)) 
      {
        // store the last selected communication line
        m_communication_selected = i;
        return true;
      }
    }
    return false;    
  } else {
    return true;
  }
}

grape_direction visualchannel_communication::is_on_border( libgrape::coordinate &p_coord )
{  
  return GRAPE_DIR_NONE;
}

int visualchannel_communication::get_communication_selected()
{
  return m_communication_selected;
}

}

