// Author(s): VitaminB100
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualchannel_communication.cpp
//
// Implements the visualchannel_communication class.

#include "channelcommunication.h"
#include "visualchannel_communication.h"
#include "geometric.h"

namespace grape {
	
using namespace grape::grapeapp;

visualchannel_communication::visualchannel_communication( channel_communication* p_channel_communication )
{
  m_object = p_channel_communication;
}

visualchannel_communication::visualchannel_communication( const visualchannel_communication &p_channel_communication )
: visual_object( p_channel_communication )
{
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
    draw_line( m_object->get_coordinate(), chan->get_coordinate(), m_object->get_selected());
  }

  // draw bounding box; only drawn if the object is selected
  // draw_bounding_box( m_object->get_coordinate(), m_object->get_width(), m_object->get_height(), m_object->get_selected() );
  // disabled because not useful for channel communication; cannot be resized anyway.
}

bool visualchannel_communication::is_inside( libgrape::coordinate &p_coord )
{
  bool result = false;
  channel_communication* comm = static_cast<channel_communication*>( m_object );

  //for all communications
  for ( unsigned int i = 0; i < comm->count_channel(); ++i )
  {
    channel* chan = comm->get_attached_channel( i );
    // test is inside line
    result = result || is_inside_line( m_object->get_coordinate(), chan->get_coordinate(), p_coord );
  }

  // return inside test
  return result || is_inside_ellipse( m_object->get_coordinate(), 0.05f, 0.05f, p_coord);
}

grape_direction visualchannel_communication::is_on_border( libgrape::coordinate &p_coord )
{
  return GRAPE_DIR_NONE;
}

}

