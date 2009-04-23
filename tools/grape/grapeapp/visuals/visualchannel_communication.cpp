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
  for ( int i = 0; i < comm->count_channel(); ++i )
  {
    channel* chan = comm->get_attached_channel( i );

    // draw communication line
    draw_line( m_object->get_coordinate(), chan->get_coordinate(), m_communication_selected == i);
  }

  channel_communication *cc = static_cast<channel_communication *>(m_object);

  float x = m_object->get_coordinate().m_x;
  float y = m_object->get_coordinate().m_y;
  wxString rename_to = cc->get_rename_to();

  // draw channel
  draw_channel( m_object->get_coordinate(), static_cast<float>(0.02), m_object->get_selected(), comm->get_channel_communication_type() );

  //draw text 
  if (rename_to != _T("")) grape_glcanvas::get_font_renderer()->draw_text("-> " + std::string(rename_to.fn_str()), x + 0.5 * m_object->get_width(), y, 0.0015f, al_right, al_top);

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

