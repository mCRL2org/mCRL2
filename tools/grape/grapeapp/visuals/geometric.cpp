// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file geometric.cpp
//
// Implements the visualcomment class.

#include "wx.hpp" // precompiled headers

#include <algorithm>
#include <string>
#include <cmath>

#include "grape_glcanvas.h"
#include "geometric.h"
#include "mcrl2/utilities/font_renderer.h"
#include "state.h"
#include "referencestate.h"
#include "workarounds.h" 

using namespace grape::grapeapp;
using namespace mcrl2::utilities::wx;

const float DEG2RAD = static_cast<float> ( M_PI/180 );
const unsigned int ELLIPSE_POINTS = 360; //pre: shouldn't be zero


coordinate grape::grapeapp::get_coordinate_from_controlpoints(coordinate p_startp, coordinate p_cp, coordinate p_endp, float pos)
{
  float neg_pos = 1-pos;
  coordinate point = {pos*pos*p_endp.m_x + 2*pos*neg_pos*p_cp.m_x + neg_pos*neg_pos*p_startp.m_x, pos*pos*p_endp.m_y + 2*pos*neg_pos*p_cp.m_y + neg_pos*neg_pos*p_startp.m_y};
  return point;
}

coordinate grape::grapeapp::get_coordinate_on_edge_ellipse(coordinate p_start, float p_ex, float p_ey, float p_width, float p_height)
{
    // calculate x, y around begin state
    float px = p_start.m_x;
    float py = p_start.m_y;

    float npx;
    float npy;
    float s = (py-p_ey)/(px-p_ex);

    if (px != p_ex) {
      if (px > py) {
        npx = p_width*sqrt(1/(1+pow(s*p_width/p_height,2)));
      } else {
        npx = -p_width*sqrt(1/(1+pow(s*p_width/p_height,2)));
      }
     npy = s*npx;
    } else {
      npx = 0;
      if (py > p_ex) {
        npy = p_height;
      } else {
        npy = -p_height;
      }
    }

    coordinate coord;
    if (px > p_ex) {
      coord.m_x = p_ex+npx;
      coord.m_y = p_ey+npy;
    } else {
      coord.m_x = p_ex-npx;
      coord.m_y = p_ey-npy;
    }
    return coord;
}

coordinate grape::grapeapp::get_coordinate_on_edge(coordinate p_start, compound_state* p_compound_state)
{
  coordinate center = p_compound_state->get_coordinate();
  // Try state
  state* astate = dynamic_cast<state*> ( p_compound_state );
  if ( astate != 0 ) // Cast succesful
  {
    return get_coordinate_on_edge_ellipse(p_start, astate->get_coordinate().m_x, astate->get_coordinate().m_y, astate->get_width()/2, astate->get_height()/2);
  }
  else // Cast failed
  {
    reference_state* areference_state = dynamic_cast<reference_state*> ( p_compound_state );
    assert( areference_state != 0 ); // Cast should be succesful

    float half_width = 0.5 * areference_state->get_width();
    float half_height = 0.5 * areference_state->get_height();

  // get radius of corner ellipse depending on percentage of rounding in x-axis direction
  float corner_radius_x = ( areference_state->get_width() * g_rounded_corner_percentage ) / 2;

  // get radius of corner ellipse depending on percentage of rounding in y-axis direction.
  float corner_radius_y = ( areference_state->get_height() * g_rounded_corner_percentage ) / 2;


    coordinate result;
    //  Determine which of the nine cases applies
    if ( p_start.m_x < center.m_x - half_width + corner_radius_x ) // point is to the left of the rectangle
    {
      if ( p_start.m_y < center.m_y - half_height + corner_radius_y )
      // point is to the left of and below the rectangle
      // _|_|_
      // _|_|_
      // x| |
      {
        result = get_coordinate_on_edge_ellipse(p_start, center.m_x - half_width + corner_radius_x, center.m_y - half_height + corner_radius_y, corner_radius_x, corner_radius_y);
      }
      else if ( p_start.m_y > center.m_y + half_height - corner_radius_y )
      // point is to the left of and above the rectangle
      // x|_|_
      // _|_|_
      //  | |
      {
        result = get_coordinate_on_edge_ellipse(p_start, center.m_x - half_width + corner_radius_x, center.m_y + half_height - corner_radius_y, corner_radius_x, corner_radius_y);
      }
      else
      // point is to the left the rectangle at the same height of the rectangle
      // _|_|_
      // x|_|_
      //  | |
      {
        result.m_x = center.m_x - half_width;
        result.m_y = p_start.m_y;
      }
    }
    else if ( p_start.m_x > center.m_x + half_width - corner_radius_x ) // point is to the rigth of the rectangle
    {
      if ( p_start.m_y < center.m_y - half_height )
      // point is to the right of and below the rectangle
      // _|_|_
      // _|_|_
      //  | |x
      {
        result = get_coordinate_on_edge_ellipse(p_start, center.m_x + half_width - corner_radius_x, center.m_y - half_height + corner_radius_y, corner_radius_x, corner_radius_y);
      }
      else if ( p_start.m_y > center.m_y + half_height - corner_radius_y )
      // point is to the right of and above the rectangle
      // _|_|x
      // _|_|_
      //  | |
      {
        result = get_coordinate_on_edge_ellipse(p_start, center.m_x + half_width - corner_radius_x, center.m_y + half_height - corner_radius_y, corner_radius_x, corner_radius_y);
      }
      else
      // point is to the right of the rectangle at the same height of the rectangle
      // _|_|_
      // _|_|x
      //  | |
      {
        result.m_x = center.m_x + half_width;
        result.m_y = p_start.m_y;
      }
    }
    else // points is between left and right edge of the rectangle
    {
      result.m_x = p_start.m_x;
      if ( p_start.m_y < center.m_y - half_height )
      // _|_|_
      // _|_|_
      //  |x|
      {
        result.m_y = center.m_y - half_height;
      }
      else if ( p_start.m_y > center.m_y + half_height )
      // _|x|_
      // _|_|_
      //  | |
      {
        result.m_y = center.m_y + half_height;
      }
      else // point is in the center of the rectangle
      // _|_|_
      // _|x|_
      //  | |
      {
        result.m_y = p_start.m_y;
      }
    }
    return result;
  }
}

bool grape::grapeapp::is_inside(coordinate p_poly[], int p_count, const coordinate& p_coord)
{
  coordinate pnt = {p_coord.m_x, p_coord.m_y};
  int k=0;
  int j=0;
  bool inside = false;
  j = p_count-1;

  // for each point
  for (k = 0; k < p_count; ++k)
  {
    if (((p_poly[k].m_y <= pnt.m_y) && (pnt.m_y < p_poly[j].m_y)) || ((p_poly[j].m_y <= pnt.m_y) && (pnt.m_y < p_poly[k].m_y)))
    {
      if ((pnt.m_x < (p_poly[j].m_x - p_poly[k].m_x) * (pnt.m_y - p_poly[k].m_y) / (p_poly[j].m_y - p_poly[k].m_y) + p_poly[k].m_x))
      {
        inside = !inside;
      }
    }
    j = k;
  }
  return inside;
}

void grape::grapeapp::set_color(const color p_color, bool p_selected)
{
  if (p_selected)
  {
    // set selected color
    glColor3f( p_color.r, p_color.g, p_color.b );

    // set bold line width
    glLineWidth( 2.0f );
  } else
  {
    /*                        1 + color
      set unselected color = -----------
                                  2
    */
    glColor3f( 0.5 + 0.5 * p_color.r, 0.5 + 0.5 * p_color.g, 0.5 + 0.5 * p_color.b );

    // set normal line width
    glLineWidth( 1.0f );
  }
}

bool grape::grapeapp::is_inside_line( const coordinate &p_begin, const coordinate &p_end, const coordinate &p_coord )
{
  // Remember, the y axis is negative.
  // Create a bounding box for the line
  float low_x = (std::min)( p_begin.m_x, p_end.m_x );
  float low_y = (std::max)( p_begin.m_y, p_end.m_y );
  float high_x = (std::max)( p_begin.m_x, p_end.m_x );
  float high_y = (std::min)( p_begin.m_y, p_end.m_y );

  if ( p_coord.m_x > low_x  - g_cursor_margin && p_coord.m_x < high_x + g_cursor_margin && p_coord.m_y < low_y  + g_cursor_margin && p_coord.m_y > high_y - g_cursor_margin )
  {
    // (nearly) vertical line, the if above is true, so p_coord is in the bounding box
    if ( fabs(p_begin.m_x - p_end.m_x) < g_cursor_margin )
    {
      return true;
    }

    // Determine the direction of the arrow ( from which corner to which corner of the bounding box? )
    if ( p_begin.m_x == low_x ) // The arrow starts at the 'left'
    {
        float angle = (p_end.m_y - p_begin.m_y) / (p_end.m_x - p_begin.m_x) ;
        float target_y = p_begin.m_y + ( p_coord.m_x - p_begin.m_x ) * angle;
        return ( p_coord.m_y < target_y + g_cursor_margin && p_coord.m_y > target_y - g_cursor_margin);
    }
    else // The arrow starts at the 'right'
    {
        // interchange begin and end coordinate.
        float angle = (p_begin.m_y - p_end.m_y) / (p_begin.m_x - p_end.m_x) ;
        float target_y = p_end.m_y + ( p_coord.m_x - p_end.m_x ) * angle;
        return ( p_coord.m_y < target_y + g_cursor_margin && p_coord.m_y > target_y - g_cursor_margin);
    }
  }
  return false;
}

bool grape::grapeapp::is_nearest_beginpoint( const coordinate &p_begin, const coordinate &p_end, const coordinate &p_coord )
{
  float distance_to_beginpoint = sqrt( pow( p_coord.m_y - p_begin.m_y, 2 ) + pow( p_coord.m_x - p_begin.m_x, 2 ) );
  float distance_to_endpoint = sqrt( pow( p_coord.m_y - p_end.m_y, 2 ) + pow( p_coord.m_x - p_end.m_x, 2 ) );
  return ( distance_to_beginpoint < distance_to_endpoint );
}

float grape::grapeapp::distance( const coordinate &p_begin, const coordinate &p_end )
{
  return sqrt( pow( p_end.m_y - p_begin.m_y, 2 ) + pow( p_end.m_x - p_begin.m_x, 2 ) );
}

bool grape::grapeapp::is_inside_ellipse( const coordinate &p_center, float p_radius_x, float p_radius_y, const coordinate &p_coord, bool p_minus_margin )
{
  // Determine the difference between the clicked and the center coordinate (treat as if center of ellipse is {0,0} and all axes are positive. )
  coordinate translated_coord;
  translated_coord.m_x = p_coord.m_x - p_center.m_x;
  translated_coord.m_y = p_coord.m_y - p_center.m_y;

  // scale the x and y coordinates of the translation, so that the width and height of the ellipse are treated as 1.
  translated_coord.m_x = translated_coord.m_x / p_radius_x;
  translated_coord.m_y = translated_coord.m_y / p_radius_y;

  float upper_bound = 1;
  if ( p_minus_margin )
  {
    upper_bound -= g_cursor_margin;
  }

  return ( sqrt( pow( translated_coord.m_x, 2 ) + pow( translated_coord.m_y, 2 ) ) <= upper_bound );
}

bool grape::grapeapp::is_on_border_ellipse( const coordinate &p_center, float p_radius_x, float p_radius_y, const coordinate &p_coord )
{
  bool inside_larger_ellipse = is_inside_ellipse( p_center, p_radius_x + g_cursor_margin, p_radius_y + g_cursor_margin, p_coord, false );
  bool inside_smaller_ellipse = is_inside_ellipse( p_center, p_radius_x, p_radius_y, p_coord );

  // test if a coordinate is on the border of the rectangle
  return ( inside_larger_ellipse && !inside_smaller_ellipse );
}

bool grape::grapeapp::is_inside_rectangle( const coordinate &p_rect_coord, float p_rect_width, float p_rect_height, const coordinate &p_coord, bool p_minus_margin )
{
  // calculate the x and y coordinates of the rectangle. { low_x, low_y } from the center coordinate of the rectangle.
  float low_x = p_rect_coord.m_x - 0.5 * p_rect_width;
  float high_x = low_x + p_rect_width;
  float low_y = p_rect_coord.m_y + 0.5 * p_rect_height;
  float high_y = low_y - p_rect_height;
  if ( p_minus_margin )
  {
    low_x -= g_cursor_margin;
    low_y += g_cursor_margin;
    high_x += g_cursor_margin;
    high_y -= g_cursor_margin;
  }

  // test inside rectangle
  bool between_x_coordinates = ( p_coord.m_x > low_x ) && ( p_coord.m_x < high_x );
  bool between_y_coordinates = ( p_coord.m_y < low_y ) && ( p_coord.m_y > high_y );
  return ( between_x_coordinates && between_y_coordinates );
}

bool grape::grapeapp::is_inside_designator( const coordinate &p_begin, float p_width, float p_height, const coordinate &p_coord )
{
  coordinate begin = p_begin;
  coordinate end = { p_begin.m_x + p_width, p_begin.m_y + p_height };
  coordinate halfway = end;
  // make sure the base stops halfway to make room for the arrow head.
  halfway.m_x = 0.5 * ( halfway.m_x + begin.m_x );
  halfway.m_y = 0.5 * ( halfway.m_y + begin.m_y );

  float angle = atan2 ((end.m_y - p_begin.m_y), (end.m_x - p_begin.m_x));

  // Determine the base of the initial designator; the base is made up of second, first, fourth, third, in that order.
  // perform translations of the begin coordinate to two coordinates on the short edge side of the arrow
  coordinate first = { begin.m_x + sin( angle ) * 0.025, begin.m_y - cos( angle ) * 0.025 };
  coordinate second = { begin.m_x - sin( angle ) * 0.025, begin.m_y + cos( angle ) * 0.025 };
  // perform translations of the end coordinate to two coordinates on the short edge side of the arrow
  coordinate third = { halfway.m_x - sin( angle ) * 0.025, halfway.m_y + cos( angle ) * 0.025 };
  coordinate fourth = { halfway.m_x + sin( angle ) * 0.025, halfway.m_y - cos( angle ) * 0.025 };

  // The head of the arrow is made up of the coordinates fifth, end and sixth, in that order.
  coordinate fifth = { halfway.m_x + sin( angle ) * 0.05, halfway.m_y - cos( angle ) * 0.05 };
  coordinate sixth = { halfway.m_x - sin( angle ) * 0.05, halfway.m_y + cos( angle ) * 0.05 };

  // assign test points for arrow base
  coordinate base_coordinates[] = { first, second, third, fourth };

  //assign test points for arrow head
  coordinate head_coordinates[] = { fifth, end, sixth };

  //test if coordinate is inside test points
  return is_inside( base_coordinates, 4, p_coord ) || is_inside( head_coordinates, 3, p_coord );
}

bool grape::grapeapp::is_inside_designator( const coordinate &p_begin, const coordinate &p_end, const coordinate &p_coord )
{
  float width = p_end.m_x - p_begin.m_x;
  float height = p_end.m_y - p_begin.m_y;
  return is_inside_designator( p_begin, width, height, p_coord );
}

bool grape::grapeapp::is_on_border_designator( const coordinate &p_begin, const coordinate &p_end, float p_width, float p_height, const coordinate &p_coord )
{
  coordinate begin = p_begin;
  coordinate end = p_end;
  coordinate halfway = p_end;
  // make sure the base stops halfway to make room for the arrow head.
  halfway.m_x = 0.5 * ( halfway.m_x + begin.m_x );
  halfway.m_y = 0.5 * ( halfway.m_y + begin.m_y );

  float angle = atan2 ((p_end.m_y - p_begin.m_y), (p_end.m_x - p_begin.m_x));

  // Determine the base of the initial designator; the base is made up of second, first, fourth, third, in that order.
  // perform translations of the begin coordinate to two coordinates on the short edge side of the arrow
  coordinate first = { begin.m_x + sin( angle ) * 0.25 * p_width, begin.m_y - cos( angle ) * 0.25 * p_width };
  coordinate second = { begin.m_x - sin( angle ) * 0.25 * p_width, begin.m_y + cos( angle ) * 0.25 * p_width };
  // perform translations of the end coordinate to two coordinates on the short edge side of the arrow
  coordinate third = { halfway.m_x - sin( angle ) * 0.25 * p_width, halfway.m_y + cos( angle ) * 0.25 * p_width };
  coordinate fourth = { halfway.m_x + sin( angle ) * 0.25 * p_width, halfway.m_y - cos( angle ) * 0.25 * p_width };

  // The head of the arrow is made up of the coordinates fifth, end and sixth, in that order.
  coordinate fifth = { halfway.m_x + sin( angle ) * 0.5 * p_width, halfway.m_y - cos( angle ) * 0.5 * p_width };
  coordinate sixth = { halfway.m_x - sin( angle ) * 0.5 * p_width, halfway.m_y + cos( angle ) * 0.5 * p_width };

  // lines are traversed in the same way that they are traced in draw_designator (when drawing the outline)
  bool result = is_inside_line( second, first, p_coord );
  result = result || is_inside_line( first, fourth, p_coord );
  result = result || is_inside_line( fourth, fifth, p_coord );
  result = result || is_inside_line( fifth, end, p_coord );
  result = result || is_inside_line( end, sixth, p_coord );
  result = result || is_inside_line( sixth, third, p_coord );
  result = result || is_inside_line( third, second, p_coord );
  return result;
}

grape_direction grape::grapeapp::is_on_border_rectangle( const coordinate &p_rect_coord, float p_rect_width, float p_rect_height, coordinate &p_coord )
{
  coordinate center;
  // check northwest
  center.m_x = p_rect_coord.m_x - 0.5 * p_rect_width;
  center.m_y = p_rect_coord.m_y + 0.5 * p_rect_height;
  if ( is_inside_rectangle( center, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )
  {
    return GRAPE_DIR_NORTHWEST;
  }

  center.m_x = p_rect_coord.m_x + 0.5 * p_rect_width;
  center.m_y = p_rect_coord.m_y + 0.5 * p_rect_height;
  if ( is_inside_rectangle( center, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )
  {
    return GRAPE_DIR_NORTHEAST;
  }

  center.m_x = p_rect_coord.m_x + 0.5 * p_rect_width;
  center.m_y = p_rect_coord.m_y - 0.5 * p_rect_height;
  if ( is_inside_rectangle( center, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )
  {
    return GRAPE_DIR_SOUTHEAST;
  }

  center.m_x = p_rect_coord.m_x - 0.5 * p_rect_width;
  center.m_y = p_rect_coord.m_y - 0.5 * p_rect_height;
  if ( is_inside_rectangle( center, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )
  {
    return GRAPE_DIR_SOUTHWEST;
  }

  // check north
  center.m_x = p_rect_coord.m_x;
  center.m_y = p_rect_coord.m_y + 0.5 * p_rect_height;
  if ( is_inside_rectangle( center, p_rect_width + 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )
  {
    return GRAPE_DIR_NORTH;
  }

  // check east
  center.m_x = p_rect_coord.m_x + 0.5 * p_rect_width;
  center.m_y = p_rect_coord.m_y;
  if ( is_inside_rectangle( center, 2 * g_cursor_margin, p_rect_height + 2 * g_cursor_margin, p_coord, false ) )
  {
    return GRAPE_DIR_EAST;
  }

  center.m_x = p_rect_coord.m_x;
  center.m_y = p_rect_coord.m_y - 0.5 * p_rect_height;
  if ( is_inside_rectangle( center, p_rect_width + 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )
  {
    return GRAPE_DIR_SOUTH;
  }

  center.m_x = p_rect_coord.m_x - 0.5 * p_rect_width;
  center.m_y = p_rect_coord.m_y;
  if ( is_inside_rectangle( center, 2 * g_cursor_margin, p_rect_height + 2 * g_cursor_margin, p_coord, false ) )
  {
    return GRAPE_DIR_WEST;
  }

  return GRAPE_DIR_NONE;
}

coordinate grape::grapeapp::move_to_border_rectangle( const coordinate &p_rect_coord, float p_rect_width, float p_rect_height, const coordinate &p_coord )
{
  coordinate result = p_coord;
  // calculate the x and y coordinates of the rectangle. { low_x, low_y } form the upper left coordinate of the rectangle.
  float lower_x = p_rect_coord.m_x -  0.5 * p_rect_width;
  float upper_x = lower_x + p_rect_width;
  float lower_y = p_rect_coord.m_y + 0.5 * p_rect_height;
  float upper_y = lower_y - p_rect_height;

  // determine the distance to each of the four edges.
  float distance_lower_x = result.m_x - lower_x; // subtract x-coordinate of edge from x-coordinate of click-event.
  float distance_upper_x = upper_x - result.m_x;
  float distance_lower_y = lower_y - result.m_y;
  float distance_upper_y = result.m_y - upper_y;
  // find out which distance is the lowest.
  float lowest = (std::min)( distance_lower_x, (std::min)( distance_upper_x, (std::min)( distance_lower_y, distance_upper_y ) ) );
  if ( lowest == distance_lower_x )
  {
    result.m_x = lower_x;
    if ( result.m_y < upper_y )
    {
      result.m_y = upper_y;
    }
    else if ( result.m_y > lower_y )
    {
      result.m_y = lower_y;
    }
  }
  else if ( lowest == distance_upper_x )
  {
    result.m_x = upper_x;
    if ( result.m_y < upper_y )
    {
      result.m_y = upper_y;
    }
    else if ( result.m_y > lower_y )
    {
      result.m_y = lower_y;
    }
  }
  else if ( lowest == distance_lower_y )
  {
    result.m_y = lower_y;
    if ( result.m_x < lower_x )
    {
      result.m_x = lower_x;
    }
    else if ( result.m_x > upper_x )
    {
      result.m_x = upper_x;
    }
  }
  else if ( lowest == distance_upper_y )
  {
    result.m_y = upper_y;
    if ( result.m_x < lower_x )
    {
      result.m_x = lower_x;
    }
    else if ( result.m_x > upper_x )
    {
      result.m_x = upper_x;
    }
  }

  return result;
}

void grape::grapeapp::draw_line( const coordinate &p_begin, const coordinate &p_end, bool p_selected, const color p_color )
{
  // set color of line
  set_color( p_color, p_selected );

  // draw line
  glBegin(GL_LINE_STRIP);
    glVertex3f( p_begin.m_x, p_begin.m_y, 0.0f);
    glVertex3f( p_end.m_x, p_end.m_y, 0.0f);
    glVertex3f( p_end.m_x, p_end.m_y, 0.0f);
    glVertex3f( p_begin.m_x, p_begin.m_y, 0.0f);
  glEnd();
}

void grape::grapeapp::draw_designator( const coordinate &p_begin, float p_width, float p_height, bool p_selected )
{
  // set color of the designator
  set_color(g_color_designator, p_selected);

  coordinate begin = p_begin;
  coordinate end = { p_begin.m_x + p_width, p_begin.m_y + p_height };
  coordinate halfway = end;
  // make sure the base stops halfway to make room for the arrow head.
  halfway.m_x = 0.5 * ( halfway.m_x + begin.m_x );
  halfway.m_y = 0.5 * ( halfway.m_y + begin.m_y );

  float angle = atan2 ( (end.m_y - p_begin.m_y), (end.m_x - p_begin.m_x));

  // perform translations of the begin coordinate to two coordinates on the short edge side of the arrow
  coordinate first = { begin.m_x + sin( angle ) * 0.025, begin.m_y - cos( angle ) * 0.025 };
  coordinate second = { begin.m_x - sin( angle ) * 0.025, begin.m_y + cos( angle ) * 0.025 };
  // perform translations of the end coordinate to two coordinates on the short edge side of the arrow
  coordinate third = { halfway.m_x - sin( angle ) * 0.025, halfway.m_y + cos( angle ) * 0.025 };
  coordinate fourth = { halfway.m_x + sin( angle ) * 0.025, halfway.m_y - cos( angle ) * 0.025 };

  // the head and base of the arrow need to be drawn separately due to the workings of GL_POLYGON
  // Draw base of the arrow
  glBegin(GL_QUADS);
    glVertex3f( first.m_x, first.m_y, 0.0f );
    glVertex3f( second.m_x, second.m_y, 0.0f );
    glVertex3f( third.m_x, third.m_y, 0.0f );
    glVertex3f( fourth.m_x, fourth.m_y, 0.0f );
  glEnd();

  coordinate fifth = {  halfway.m_x + sin( angle ) * 0.05, halfway.m_y - cos( angle ) * 0.05 };
  coordinate sixth = { halfway.m_x - sin( angle ) * 0.05, halfway.m_y + cos( angle ) * 0.05 };
  // draw the head of the arrow
  glBegin(GL_TRIANGLES);
    glVertex3f( fifth.m_x, fifth.m_y, 0.0f );
    glVertex3f( end.m_x, end.m_y, 0.0f );
    glVertex3f( sixth.m_x, sixth.m_y, 0.0f );
  glEnd();

  // set black color
  glColor3f(0.0f, 0.0f, 0.0f);

  // draw line around the designator. The coordinates used here are numbered by moment of declaration and order of use in the above methods; their order is different here.
  glBegin(GL_LINE_LOOP);
    glVertex3f( second.m_x, second.m_y, 0.0f );
    glVertex3f( first.m_x, first.m_y, 0.0f );
    glVertex3f( fourth.m_x, fourth.m_y, 0.0f );
    glVertex3f( fifth.m_x, fifth.m_y, 0.0f );
    glVertex3f( end.m_x, end.m_y, 0.0f );
    glVertex3f( sixth.m_x, sixth.m_y, 0.0f );
    glVertex3f( third.m_x, third.m_y, 0.0f );
  glEnd();
}

void grape::grapeapp::draw_designator( const coordinate &p_begin, const coordinate &p_end, bool p_selected )
{
  float width = p_end.m_x - p_begin.m_x;
  float height = p_end.m_y - p_begin.m_y;
  draw_designator( p_begin, width, height, p_selected );
}

void grape::grapeapp::draw_state( const coordinate &p_center, float p_radius_x, float p_radius_y, bool p_selected  )
{
  glPushMatrix();

  // set the color for the fill of the ellipse
  set_color(g_color_state, p_selected);

  // perform necessary translation
  glTranslatef( p_center.m_x, p_center.m_y, 0.0f);
  // fill ellipse
  glBegin(GL_POLYGON);
    // for each point in the ellipse
    for (unsigned int i=0; i < ELLIPSE_POINTS; ++i)
    {
      float radians = i * DEG2RAD * ( 360.0f / ELLIPSE_POINTS );
      glVertex3f( cos( radians ) * p_radius_x, sin( radians ) * p_radius_y , 0.0f );
    }
  glEnd();

  // set the color for the outer line of the ellipse; black
  glColor3f(0.0f, 0.0f, 0.0f);

  // draw outer line of ellipse
  glBegin(GL_LINE_LOOP);
    // for each point in the ellipse
    for (unsigned int i=0; i < ELLIPSE_POINTS; ++i)
    {
      float degInRad = i * DEG2RAD * ( 360.0f / ELLIPSE_POINTS );
      glVertex3f( cos( degInRad ) * p_radius_x, sin( degInRad ) * p_radius_y, 0.0f );
    }
  glEnd();

  glPopMatrix();
}

bool grape::grapeapp::is_inside_nonterminating_transition( const coordinate &p_ntt_coord, const coordinate &p_base_coordinate, const coordinate &p_head_coordinate, const coordinate &p_coord )
{
  coordinate sum_base = { p_ntt_coord.m_x + p_base_coordinate.m_x, p_ntt_coord.m_y + p_base_coordinate.m_y };
  coordinate sum_head = { p_ntt_coord.m_x + p_head_coordinate.m_x, p_ntt_coord.m_y + p_head_coordinate.m_y };
  return is_inside_rectangle( p_ntt_coord, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) ||
         is_inside_rectangle( sum_base, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) ||
         is_inside_rectangle( sum_head, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false );
}

void grape::grapeapp::draw_nonterminating_transition( const coordinate p_begin, const coordinate p_control, const coordinate p_end, bool p_selected, const wxString &p_label_text )
{
  float distance_begin_to_control = sqrt( pow( p_control.m_y - p_begin.m_y, 2 ) + pow( p_control.m_x - p_begin.m_x, 2 ) );
  float distance_control_to_end = sqrt( pow( p_end.m_y - p_control.m_y, 2 ) + pow( p_end.m_x - p_control.m_x, 2 ) );
  float strength = distance_begin_to_control/(distance_begin_to_control + distance_control_to_end);
  float distance_left = distance_begin_to_control * 0.5;
  float distance_right = distance_control_to_end * 0.5;

  coordinate p_right = {(1-strength)*p_begin.m_x + strength*p_control.m_x, (1-strength)*p_begin.m_y + strength*p_control.m_y};
  coordinate p_left = {(1-strength)*p_control.m_x + strength*p_end.m_x, (1-strength)*p_control.m_y + strength*p_end.m_y};
  float angle_control = atan2(( p_left.m_x - p_right.m_x), ( p_left.m_y - p_right.m_y));

  coordinate p_control_left = {p_control.m_x + distance_left * sin( angle_control - M_PI ), p_control.m_y + distance_left * cos( angle_control - M_PI )};
  coordinate p_control_right = {p_control.m_x + distance_right * sin( angle_control - M_PI*2 ), p_control.m_y + distance_right * cos( angle_control - M_PI*2 )};

  //draw bezier
  coordinate pre_pnt;
  coordinate pnt = p_begin;
  float angle_middle;
  for(unsigned int i=1;i<=40;++i)
  {
    pre_pnt = pnt;
    if (i <= 20)
    {
      pnt = get_coordinate_from_controlpoints(p_begin, p_control_left, p_control, float(i*0.05f));  
    }
    else
    {
      pnt = get_coordinate_from_controlpoints(p_control, p_control_right, p_end, float((i-20)*0.05f));
      // calculate angle of midpoint
      if (i == 21) angle_middle = atan2((pre_pnt.m_y-pnt.m_y), (pre_pnt.m_x-pnt.m_x)) + M_PI*0.5f;
    };
    draw_line(pre_pnt, pnt, p_selected, g_color_black);
  }

  // calculate rotation of arrow
  float angle_arrow = atan2((pre_pnt.m_y-pnt.m_y), (pre_pnt.m_x-pnt.m_x));

  // draw arrow head based on calculated angle
  float one_side_x = pnt.m_x + 0.03f * cos( angle_arrow - M_PI/4 );
  float one_side_y = pnt.m_y + 0.03f * sin( angle_arrow - M_PI/4 );
  float other_side_x = pnt.m_x + 0.03f * cos( angle_arrow + M_PI/4 );
  float other_side_y = pnt.m_y + 0.03f * sin( angle_arrow + M_PI/4 );
    
  // draw transition arrow
  glBegin(GL_TRIANGLES);
    glVertex3f( pnt.m_x, pnt.m_y, 0.0f);
    glVertex3f( one_side_x, one_side_y, 0.0f);
    glVertex3f( other_side_x, other_side_y, 0.0f);
  glEnd();

  // keep angle between 0..pi
  while (angle_middle < 0.0f) angle_middle += static_cast<float> ( M_PI );
  while (angle_middle >= M_PI) angle_middle -= static_cast<float> ( M_PI );
  
  // align text
  Alignment align_horizontal = al_center;
  Alignment align_vertical = al_center;

  if ((angle_middle >= 0.0f*M_PI) && (angle_middle < 0.25f*M_PI)) align_horizontal = al_right;   
  if ((angle_middle >= 0.75f*M_PI) && (angle_middle < 1.25f*M_PI)) align_horizontal = al_left;    
    
  if ((angle_middle >= 0.25f*M_PI) && (angle_middle < 0.75f*M_PI)) align_vertical = al_top;
    
  set_color(g_color_black, true);
  // draw text 
  grape_glcanvas::get_font_renderer()->draw_text( std::string(p_label_text.fn_str()),p_control.m_x + 0.05f*cos(angle_middle), p_control.m_y + 0.025f + 0.05f*sin(angle_middle), 0.0015f, align_horizontal, align_vertical );
 
  //draw control point
  if (p_selected)
  {
    draw_line_rectangle(p_control, 0.03f, 0.03f, false, g_color_black);
  }
  else
  {
    draw_filled_rectangle(p_control, 0.015f, 0.015f, false, g_color_black);
  };

  draw_filled_rectangle(p_begin, 0.015f, 0.015f, false, g_color_black);
  draw_filled_rectangle(p_end, 0.015f, 0.015f, false, g_color_black);
}

void grape::grapeapp::draw_terminating_transition( const coordinate &p_begin, const coordinate &p_end, bool p_selected, const wxString &p_label_text )
{
  float width = p_end.m_x - p_begin.m_x;
  float height = p_end.m_y - p_begin.m_y;

  coordinate end_coord = { p_begin.m_x + width, p_begin.m_y + height };
  draw_line( p_begin, end_coord, p_selected, g_color_black );

  // calculate rotation of arrow
  // correction + get_coordinate not necessary as this results in 0
  float angle = atan2(( p_begin.m_y - end_coord.m_y), ( p_begin.m_x - end_coord.m_x));

  // draw arrow head based on calculated angle
  float one_side_x = end_coord.m_x + 0.03 * cos( angle - M_PI/4 );
  float one_side_y = end_coord.m_y + 0.03 * sin( angle - M_PI/4 );
  float other_side_x = end_coord.m_x + 0.03 * cos( angle + M_PI/4 );
  float other_side_y = end_coord.m_y + 0.03 * sin( angle + M_PI/4 );

  // draw transition arrow
  glBegin(GL_TRIANGLES);
    glVertex3f( end_coord.m_x, end_coord.m_y, 0.0f);
    glVertex3f( one_side_x, one_side_y, 0.0f);
    glVertex3f( other_side_x, other_side_y, 0.0f);
  glEnd();

  // calculate midpoint
  coordinate midpoint;
  midpoint.m_x = ( end_coord.m_x + p_begin.m_x ) * 0.5;
  midpoint.m_y = ( end_coord.m_y + p_begin.m_y ) * 0.5;
    
  angle += static_cast<float> ( M_PI*0.5f );
  while (angle < 0.0f) angle += static_cast<float> ( 2.0f*M_PI );
  while (angle >= 2.0f*M_PI) angle -= static_cast<float> ( 2.0f*M_PI );
  
  // align text
  Alignment align_horizontal = al_right;
  Alignment align_vertical = al_center;

  if ((angle >= 0.5f*M_PI) && (angle < 1.5f*M_PI)) align_horizontal = al_left;    
    
  if ((angle >= 0.25f*M_PI) && (angle < 0.75f*M_PI)) align_vertical = al_top;
  if ((angle >= 1.25f*M_PI) && (angle < 1.75f*M_PI)) align_vertical = al_bottom;
    
  set_color(g_color_black, true);
  // draw text 
  grape_glcanvas::get_font_renderer()->draw_text(std::string(p_label_text.fn_str()), midpoint.m_x + 0.05f*cos(angle), midpoint.m_y + 0.025f + 0.05f*sin(angle), 0.0015f, align_horizontal, align_vertical);   
  
  // do not draw the bounding box, this is already done in visualnonterminating transition
}

void grape::grapeapp::draw_channel( const coordinate &p_center, float p_radius, bool p_selected, const channel_type p_channel_type)
{
  glPushMatrix();

  // perform necessary translation
  glTranslatef( p_center.m_x, p_center.m_y, 0.0f);
  // set the color for the fill of the ellipse
  if (p_channel_type == VISIBLE_CHANNEL) set_color(g_color_channel_visible, p_selected);
  if (p_channel_type == HIDDEN_CHANNEL) set_color(g_color_channel_hidden, p_selected);
  if (p_channel_type == BLOCKED_CHANNEL) set_color(g_color_channel_blocked, p_selected);

  // fill ellipse
  glBegin(GL_POLYGON);
    // for each point in the ellipse
    for (unsigned int i=0; i < ELLIPSE_POINTS; ++i)
    {
      float radians = i * DEG2RAD * ( 360.0f / ELLIPSE_POINTS );
      glVertex3f( cos( radians ) * p_radius, sin( radians ) * p_radius , 0.0f );
    }
  glEnd();

  // set the color for the outer line of the ellipse; black
  glColor3f(0.0f, 0.0f, 0.0f);

  // draw outer line of ellipse
  glBegin(GL_LINE_LOOP);
    // for each point in the ellipse
    for (unsigned int i=0; i < ELLIPSE_POINTS; ++i)
    {
      float degInRad = i * DEG2RAD * ( 360.0f / ELLIPSE_POINTS );
      glVertex3f( cos( degInRad ) * p_radius, sin( degInRad ) * p_radius, 0.0f );
    }
  glEnd();
  glPopMatrix();
}

void grape::grapeapp::draw_channel( const coordinate &p_center, float p_radius, bool p_selected, const channel_communication_type p_channel_communication_type)
{
  channel_type p_channel_type = VISIBLE_CHANNEL;
  if (p_channel_communication_type == VISIBLE_CHANNEL_COMMUNICATION) p_channel_type = VISIBLE_CHANNEL;
  if (p_channel_communication_type == HIDDEN_CHANNEL_COMMUNICATION) p_channel_type = HIDDEN_CHANNEL;
  if (p_channel_communication_type == BLOCKED_CHANNEL_COMMUNICATION) p_channel_type = BLOCKED_CHANNEL;
  draw_channel( p_center, p_radius, p_selected, p_channel_type );
}

void grape::grapeapp::draw_filled_rectangle( const coordinate &p_center, float p_width, float p_height, bool p_selected, const color p_color )
{
  glPushMatrix();

  // set color of architecture reference
  set_color( p_color, p_selected);

  glTranslatef( p_center.m_x - 0.5 * p_width, p_center.m_y - 0.5 * p_height, 0.0f);

  //draw rectangle
  glBegin(GL_QUADS);
    glVertex3f(0,   0,   0.0f);
    glVertex3f(p_width, 0,   0.0f);
    glVertex3f(p_width, p_height, 0.0f);
    glVertex3f(0,   p_height, 0.0f);
  glEnd();

  // draw lines
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_LINE_LOOP);
    glVertex3f(0,   0,   0.0f);
    glVertex3f(p_width, 0,   0.0f);
    glVertex3f(p_width, p_height, 0.0f);
    glVertex3f(0,   p_height, 0.0f);
  glEnd();

  glPopMatrix();
}

void grape::grapeapp::draw_line_rectangle( const coordinate &p_center, float p_width, float p_height, bool p_selected, const color p_color )
{
  glPushMatrix();

  // set the color
  set_color(p_color, p_selected);
  glTranslatef( p_center.m_x - 0.5 * p_width, p_center.m_y - 0.5 * p_height, 0.0f);

  //draw line rectangle
  glBegin(GL_LINE_LOOP);
    glVertex3f(0,   0,   0.0f);
    glVertex3f(p_width, 0,   0.0f);
    glVertex3f(p_width, p_height, 0.0f);
    glVertex3f(0,   p_height, 0.0f);
  glEnd();

  glPopMatrix();
}

void grape::grapeapp::draw_cross( const coordinate &p_center, float p_width, float p_height, bool p_selected )
{
  glPushMatrix();
  set_color( g_color_black, p_selected );
  glTranslatef( p_center.m_x - 0.5 * p_width, p_center.m_y - 0.5 * p_height, 0.0f);

  //draw cross
  glBegin(GL_LINES);
    glVertex3f(0,   0,   0.0f);
    glVertex3f(p_width, p_height, 0.0f);
    glVertex3f(p_width, 0,   0.0f);
    glVertex3f(0,   p_height, 0.0f);
  glEnd();

  glPopMatrix();
}

void grape::grapeapp::draw_tick( const coordinate &p_center, float p_width, float p_height, bool p_selected )
{
  // draw line rectangle
  draw_line_rectangle( p_center, p_width, p_height, p_selected, g_color_black);

  // set color of tick
  set_color( g_color_black, p_selected);

  float x = p_center.m_x;
  float y = p_center.m_y;
  // draw tick
  glBegin( GL_QUADS );
    glVertex3f( x - 0.035f, y + 0.01f, 0.0f );
    glVertex3f( x - 0.005f, y - 0.04f, 0.0f );
    glVertex3f( x + 0.035f, y + 0.04f, 0.0f );
    glVertex3f( x - 0.005f, y - 0.025f, 0.0f );
  glEnd();
}

void grape::grapeapp::draw_reference(const coordinate &p_center, float p_width, float p_height, bool p_selected)
{
  // get radius of corner ellipse depending on percentage of rounding in x-axis direction
  float corner_radius_x = ( p_width * g_rounded_corner_percentage ) / 2;

  // get radius of corner ellipse depending on percentage of rounding in y-axis direction.
  float corner_radius_y = ( p_height * g_rounded_corner_percentage ) / 2;

  coordinate points[36];

  // top right corner
  for (unsigned int i=0; i < 90; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x + 0.5 * p_width + cos( degInRad ) * corner_radius_x - corner_radius_x;
    points[i/10].m_y = p_center.m_y + 0.5 * p_height + sin( degInRad ) * corner_radius_y - corner_radius_y;
  }
  // top left corner
  for (unsigned int i=90; i < 180; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x - 0.5 * p_width + cos( degInRad ) * corner_radius_x + corner_radius_x;
    points[i/10].m_y = p_center.m_y + 0.5 * p_height + sin( degInRad ) * corner_radius_y - corner_radius_y;
  }
  // bottom left corner
  for (unsigned int i=180; i < 270; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x - 0.5 * p_width + cos( degInRad ) * corner_radius_x + corner_radius_x;
    points[i/10].m_y = p_center.m_y - 0.5 * p_height + sin( degInRad ) * corner_radius_y + corner_radius_y;
  }
  // bottom right corner
  for (unsigned int i=270; i < 360; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x + 0.5 * p_width  + cos( degInRad ) * corner_radius_x - corner_radius_x;
    points[i/10].m_y = p_center.m_y - 0.5 * p_height + sin( degInRad ) * corner_radius_y + corner_radius_y;
  }

  // set color of rounded rectangle
  set_color( g_color_process_reference, p_selected );

  // draw process reference
  glBegin(GL_POLYGON);
    for (unsigned int i=0; i < 36; ++i)
    {
      glVertex3f(points[i].m_x, points[i].m_y, 0.0f);
    }
  glEnd();

  // draw process reference line
  glColor3f(0.0f, 0.0f, 0.0f);
  glBegin(GL_LINE_LOOP);
    for (unsigned int i=0; i < 36; ++i)
    {
      glVertex3f(points[i].m_x, points[i].m_y, 0.0f);
    }
  glEnd();

  // draw process reference separation line
  glBegin(GL_LINES);
    glVertex3f( p_center.m_x - 0.5 * p_width, p_center.m_y + 0.25 * p_height, 0.0f );
    glVertex3f( p_center.m_x + 0.5 * p_width, p_center.m_y + 0.25 * p_height, 0.0f );
  glEnd();

}

bool grape::grapeapp::is_inside_reference(const coordinate &p_center, float p_width, float p_height, coordinate p_coord)
{
  // get radius of corner ellipse depending on percentage of rounding in x-axis direction
  float corner_radius_x = ( p_width * g_rounded_corner_percentage ) / 2;

  // get radius of corner ellipse depending on percentage of rounding in y-axis direction.
  float corner_radius_y = ( p_height * g_rounded_corner_percentage ) / 2;

  coordinate points[36];

    // top right corner
  for (unsigned int i=0; i < 90; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x + 0.5 * p_width + cos( degInRad ) * corner_radius_x - corner_radius_x;
    points[i/10].m_y = p_center.m_y + 0.5 * p_height + sin( degInRad ) * corner_radius_y - corner_radius_y;
  }
  // top left corner
  for (unsigned int i=90; i < 180; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x - 0.5 * p_width + cos( degInRad ) * corner_radius_x + corner_radius_x;
    points[i/10].m_y = p_center.m_y + 0.5 * p_height + sin( degInRad ) * corner_radius_y - corner_radius_y;
  }
  // bottom left corner
  for (unsigned int i=180; i < 270; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x - 0.5 * p_width + cos( degInRad ) * corner_radius_x + corner_radius_x;
    points[i/10].m_y = p_center.m_y - 0.5 * p_height + sin( degInRad ) * corner_radius_y + corner_radius_y;
  }
  // bottom right corner
  for (unsigned int i=270; i < 360; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x + 0.5 * p_width  + cos( degInRad ) * corner_radius_x - corner_radius_x;
    points[i/10].m_y = p_center.m_y - 0.5 * p_height + sin( degInRad ) * corner_radius_y + corner_radius_y;
  }

  return is_inside(points, 36, p_coord);
}


grape_direction grape::grapeapp::is_on_border_reference(const coordinate &p_center, float p_width, float p_height, coordinate p_coord)
{
  // get radius of corner ellipse depending on percentage of rounding in x-axis direction
  float corner_radius_x = ( p_width * g_rounded_corner_percentage ) / 2;

  // get radius of corner ellipse depending on percentage of rounding in y-axis direction.
  float corner_radius_y = ( p_height * g_rounded_corner_percentage ) / 2;

  coordinate points[36];

  // top right corner
  for (unsigned int i=0; i < 90; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x + 0.5 * p_width + cos( degInRad ) * corner_radius_x - corner_radius_x;
    points[i/10].m_y = p_center.m_y + 0.5 * p_height + sin( degInRad ) * corner_radius_y - corner_radius_y;
  }
  // top left corner
  for (unsigned int i=90; i < 180; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x - 0.5 * p_width + cos( degInRad ) * corner_radius_x + corner_radius_x;
    points[i/10].m_y = p_center.m_y + 0.5 * p_height + sin( degInRad ) * corner_radius_y - corner_radius_y;
  }
  // bottom left corner
  for (unsigned int i=180; i < 270; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x - 0.5 * p_width + cos( degInRad ) * corner_radius_x + corner_radius_x;
    points[i/10].m_y = p_center.m_y - 0.5 * p_height + sin( degInRad ) * corner_radius_y + corner_radius_y;
  }
  // bottom right corner
  for (unsigned int i=270; i < 360; i+=10)
  {
    float degInRad = i * DEG2RAD;
    points[i/10].m_x = p_center.m_x + 0.5 * p_width  + cos( degInRad ) * corner_radius_x - corner_radius_x;
    points[i/10].m_y = p_center.m_y - 0.5 * p_height + sin( degInRad ) * corner_radius_y + corner_radius_y;
  }

  //bool is_on_border = false
  for (unsigned int i=1; i < 36; ++i)
  {
     //is_on_border = is_on_border ||
     if (is_inside_line(points[i-1], points[i], p_coord)) {
       if (i >= 1 && i < 9) return GRAPE_DIR_NORTHEAST;
       if (i == 9) return GRAPE_DIR_NORTH;
       if (i >= 10 && i < 18) return GRAPE_DIR_NORTHWEST;
       if (i == 18) return GRAPE_DIR_WEST;
       if (i >= 19 && i < 27) return GRAPE_DIR_SOUTHWEST;
       if (i == 27) return GRAPE_DIR_SOUTH;
       if (i >= 28 && i < 36) return GRAPE_DIR_SOUTHEAST;
     }
  }
  if (is_inside_line(points[0], points[36], p_coord)) return GRAPE_DIR_EAST;

  return GRAPE_DIR_NONE;
}

void grape::grapeapp::draw_bounding_box( const coordinate &p_center, float p_width, float p_height, bool p_selected)
{
  if ( p_selected )
  {
    // Draw the eight rectangles
    coordinate left_low = { p_center.m_x - 0.5 * p_width - 2 * g_cursor_margin, p_center.m_y - 0.5 * p_height - 2 * g_cursor_margin };
    draw_line_rectangle( left_low, g_cursor_margin, g_cursor_margin, false, g_color_black );
    coordinate left_center = { left_low.m_x, p_center.m_y };
    draw_line_rectangle( left_center, g_cursor_margin, g_cursor_margin,false, g_color_black );
    coordinate left_up = { left_low.m_x, p_center.m_y + 0.5 * p_height + 2 * g_cursor_margin };
    draw_line_rectangle( left_up, g_cursor_margin, g_cursor_margin,false, g_color_black );
    coordinate center_up = { p_center.m_x, left_up.m_y };
    draw_line_rectangle( center_up, g_cursor_margin, g_cursor_margin, false, g_color_black );

    coordinate right_low = { p_center.m_x + 0.5 * p_width + 2 * g_cursor_margin, left_low.m_y };
    draw_line_rectangle( right_low, g_cursor_margin, g_cursor_margin, false, g_color_black );
    coordinate right_center = { right_low.m_x, p_center.m_y };
    draw_line_rectangle( right_center, g_cursor_margin, g_cursor_margin, false, g_color_black );
    coordinate right_up = { right_low.m_x, left_up.m_y };
    draw_line_rectangle( right_up, g_cursor_margin, g_cursor_margin, false, g_color_black );
    coordinate center_low = { p_center.m_x, p_center.m_y - 0.5 * p_height - 2 * g_cursor_margin };
    draw_line_rectangle( center_low, g_cursor_margin, g_cursor_margin, false, g_color_black );

    //draw dashed rectangle
    glLineStipple(1, 0x3F07);
    glEnable(GL_LINE_STIPPLE);
    glColor3f( 0.5, 0.5, 0.5 );
    glBegin(GL_LINE_LOOP);
      glVertex3f(left_low.m_x,   left_low.m_y,   0.0f);
      glVertex3f(left_up.m_x, left_up.m_y,   0.0f);
      glVertex3f(right_up.m_x, right_up.m_y, 0.0f);
      glVertex3f(right_low.m_x, right_low.m_y, 0.0f);
    glEnd();
    glDisable(GL_LINE_STIPPLE);
  } // end if
}

grape_direction grape::grapeapp::grab_bounding_box( const coordinate &p_center, float p_width, float p_height, const coordinate &p_coord, bool p_selected )
{
  grape_direction result = GRAPE_DIR_NONE;

  if ( p_selected )
  {
    coordinate left_low = { p_center.m_x - 0.5 * p_width - 2 * g_cursor_margin, p_center.m_y - 0.5 * p_height - 2 * g_cursor_margin };
    if ( is_inside_rectangle( left_low, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) ) result = GRAPE_DIR_SOUTHWEST;

    coordinate left_center = { left_low.m_x, p_center.m_y };
    if ( is_inside_rectangle( left_center, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false )  ) result = GRAPE_DIR_WEST;

    coordinate left_up = { left_low.m_x, p_center.m_y + 0.5 * p_height + 2 * g_cursor_margin };
    if ( is_inside_rectangle( left_up, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )  result = GRAPE_DIR_NORTHWEST;

    coordinate center_up = { p_center.m_x, left_up.m_y };
    if ( is_inside_rectangle( center_up, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )  result = GRAPE_DIR_NORTH;

    coordinate right_low = { p_center.m_x + 0.5 * p_width + 2 * g_cursor_margin, left_low.m_y };
    if ( is_inside_rectangle( right_low, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )  result = GRAPE_DIR_SOUTHEAST;

    coordinate right_center = { right_low.m_x, p_center.m_y };
    if ( is_inside_rectangle( right_center, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )  result = GRAPE_DIR_EAST;

    coordinate right_up = { right_low.m_x, left_up.m_y };
    if ( is_inside_rectangle( right_up, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )  result = GRAPE_DIR_NORTHEAST;

    coordinate center_low = { p_center.m_x, p_center.m_y - 0.5 * p_height - 2 * g_cursor_margin };
    if ( is_inside_rectangle( center_low, 2 * g_cursor_margin, 2 * g_cursor_margin, p_coord, false ) )  result = GRAPE_DIR_SOUTH;
  }
  return result;
}

float grape::grapeapp::get_width_hint( const wxString &p_string )
{
  int length = p_string.Length();
  int num_chars = length < 20 ? length : 20;
  wxString truncated = p_string.Left( num_chars );
  return CHARWIDTH * 0.0030f * num_chars;
}
