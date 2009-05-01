// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file geometric.h
//
// Declares functions for visual classes

#ifndef LIBGRAPE_GRAPE_FUNC_H
#define LIBGRAPE_GRAPE_FUNC_H

#include <cmath>
#ifdef __APPLE__
# include <GLUT/glut.h>
#else
# ifdef WIN32
#  include <windows.h>
# endif
# include <GL/glu.h>
#endif
#include <wx/string.h>
#include "nonterminatingtransition.h"
#include "compoundstate.h"
#include "channel.h"
#include "channelcommunication.h"

struct color
{
  float r;
  float g;
  float b;
};

//color definitions
const color g_color_blue = {0.0f, 0.0f, 1.0f};
const color g_color_black = {0.0f, 0.0f, 0.0f};
const color g_color_white = {1.0f, 1.0f, 1.0f};
const color g_color_red = {1.0f, 0.0f, 0.0f};
const color g_color_gray = {0.5f, 0.5f, 0.5f};
const color g_color_state = {(float)120/255, (float)255/255, (float)120/255};
const color g_color_comment = {(float)255/255, (float)255/255, (float)110/255};
const color g_color_terminating_transition = g_color_black;
const color g_color_nonterminating_transition = g_color_black;
const color g_color_architecture_diagram = {1.0f, 0.0f, 0.0f};
const color g_color_process_diagram = {1.0f, 0.0f, 0.0f};
const color g_color_architecture_reference = {(float)255/255, (float)160/255, (float)148/255};
const color g_color_process_reference = {(float)120/255, (float)255/255, (float)120/255};
const color g_color_channel_visible = g_color_white;
const color g_color_channel_blocked = g_color_red;
const color g_color_channel_hidden = g_color_gray;
const color g_color_channel_communication = g_color_black;
const color g_color_designator = {0.0f, 0.0f, 1.0f};
const color g_color_preamble = {(float)228/255, (float)182/255, (float)255/255};
const color g_color_blocked = g_color_black;
const color g_color_visible = g_color_black;

const float g_cursor_margin = 0.03f; // margin for clicking on the edge of an object.
// NOTE: tested cursor margins below:
// 0.01f is way too small, you still have to be accurate on one or maybe two pixels
// 0.05f is too large, it becomes difficult to add channels because the center of the object has to be clicked
// 0.03f is being tested now.

const float g_rounded_corner_percentage = 0.3f; // percentage used to determine the amount of roundness of corners for rounded rectangles

using namespace grape::libgrape;

namespace grape
{
  namespace grapeapp
  {
    /** Describes the  */
    enum grape_direction
    {
      GRAPE_DIR_NONE = 0, /**< No match. */
      GRAPE_DIR_NORTHWEST,/**< Topleft */
      GRAPE_DIR_NORTHEAST,/**< Topright */
      GRAPE_DIR_SOUTHEAST,/**< Bottom right */
      GRAPE_DIR_SOUTHWEST,/**< Bottom left */
      GRAPE_DIR_NORTH,    /**< Top */
      GRAPE_DIR_EAST,     /**< Right */
      GRAPE_DIR_SOUTH,    /**< Bottom */
      GRAPE_DIR_WEST,     /**< Left */
      GRAPE_DIR_ANY       /**< Any direction. Handy for lines or ellipses.*/
    };

    /**
     * Get coordinate of a bezier curve from controlpoints
     * @param p_startp start coordinate of the bezier
     * @param p_cp controlpoint 1 of the bezier
     * @param p_endp end coordinate of the bezier
     * @return Returns coordinate on the bezier curve
     */
    coordinate get_coordinate_from_controlpoints(coordinate p_startp, coordinate p_cp, coordinate p_endp, float pos);

    /**
     * Get intersection coordinate of ellipse
     * @param p_start start coordinate of the transition
     * @param p_ex ellipse x coordinate
     * @param p_ey ellipse y coordinate
     * @param p_width ellipse width
     * @param p_height ellipse height
     * @return Returns coordinate on the border of ellipse
     */
    coordinate get_coordinate_on_edge_ellipse(coordinate p_start, float p_ex, float p_ey, float p_width, float p_height);

    /**
     * Get intersection coordinate depending on the compound_state type
     * @param p_start start coordinate of the transition
     * @param p_compound_state begin or end state of the transition
     * @return Returns coordinate on the border of compound_state
     */
    coordinate get_coordinate_on_edge(coordinate p_start, compound_state* p_compound_state);

    /**
     * Test if coordinate is inside a polygon
     * @param A list of coordinates of which the polygon is built up.
     * @param The number of coordinates in the list
     * @param The to be checked coordinate
     * @return Returns whether the specified coordinate is inside the polygon
     */
    bool is_inside(coordinate p_poly[], int p_count, const coordinate& p_coord);

    /**
     * Set opengl color, performs a set color, depending on whether the object is selected.
     * @param p_color draw color
     * @param p_selected selection property of the object
     */
    void set_color(const color p_c, bool p_selected);

    /**
     * Function to see if a nearest the endpoint of a line or (if false is returned) nearest the endpoint of the line.
     * @param p_begin The begin coordinate of the line.
     * @param p_end The end coordinate of the line.
     * @param p_coord The to be checked coordinate.
     * @return Returns whether the coordinate is nearest the beginpoint (true) or nearest the endpoint (false)
     */
    bool is_nearest_beginpoint( const coordinate &p_begin, const coordinate &p_end, const coordinate &p_coord );

    /**
     * Distance function of two coordinates.
     * @param p_begin The begin coordinate
     * @param p_end The end coordinate
     * @return Returns the distance between the begin and end coordinate.
     */
    float distance( const coordinate &p_begin, const coordinate &p_end );

    /**
     * Function to see if a coordinate is on a line, taking into account the cursor margin.
     * @param p_begin The begin coordinate of the line.
     * @param p_end The end coordinate of the line.
     * @param p_coord The to be checked coordinate.
     * @return Returns whether the coordinate is on the line.
     */
    bool is_inside_line( const coordinate &p_begin, const coordinate &p_end, const coordinate &p_coord );

    /**
     * Function to see if a coordinate is inside an ellipse with the center at the specified coordinate and with the specified radii. Note that the y-axis is negative and the origin is the upper left corner.
     * @param p_center The center coordinate of the ellipse.
     * @param p_radius_x The radius of the ellipse in the direction of the x-axis.
     * @param p_radius_y The radius of the ellipse in the direction of the y-axis.
     * @param p_coord The to be checked coordinate.
     * @param p_minus_margin A flag indicating whether the cursor margin should be subtracted from the size of the rectangle.
     * @return Returns whether the coordinate is inside the ellipse.
     */
    bool is_inside_ellipse( const coordinate &p_center, float p_radius_x, float p_radius_y, const coordinate &p_coord, bool p_minus_margin = true );

    /**
     * Function to see if a coordinate is on the border of an ellipse with the center at the specified coordinate, and with the specified width and height. Note that the y-axis is negative and the origin is the upper left corner.
     * @param p_center The center cordinate of the ellipse
     * @param p_radius_x The radius of the ellipse in the direction of the x-axis.
     * @param p_radius_y The radius of the ellipse in the direction of the y-axis.
     * @param p_coord The to be checked coordinate.
     * @return Returns whether the coordinate is on the edge of the ellipse.
     */
    bool is_on_border_ellipse( const coordinate &p_center, float p_radius_x, float p_radius_y, const coordinate &p_coord );

    /**
     * Function to see if a coordinate is on the border of an ellipse with the center at the specified coordinate, and with the specified width and height. Note that the y-axis is negative and the origin is the upper left corner.
     * @param p_center The center cordinate of the reference.
     * @param p_width Width of the reference.
     * @param p_height Height of the reference.
     * @param p_coord The to be checked coordinate.
     * @return Returns whether the coordinate is on the edge of the ellipse.
     */
    grape_direction is_on_border_reference(const coordinate &p_center, float p_width, float p_height, coordinate p_coord);

    /**
     * Function to see if a coordinate is inside a rectangle with the upper left corner at the specified coordinate, and with the specified width and height. Note that the y-axis is negative and the origin is the upper left corner.
     * @param p_rect_coord The center coordinate of the rectangle.
     * @param p_rect_width The width of the rectangle.
     * @param p_rect_height The height of the rectangle.
     * @param p_coord The to be checked coordinate.
     * @param p_minus_margin A flag indicating whether the cursor margin should be subtracted from the size of the rectangle.
     * @return Returns whether the coordinate is inside the rectangle.
     */
    bool is_inside_rectangle( const coordinate &p_rect_coord, float p_rect_width, float p_rect_height, const coordinate &p_coord, bool p_minus_margin = true );

    /**
     * Function to see if a coordinate is on the border of a rectangle with the upper left corner at the specified
     * coordinate, and with the specified width and height.
     * By default, the allowed margin of the cursor is taken into account and therefore subtracted from the size of the rectangle at each side.
     * @param p_rect_coord The center coordinate of the rectangle.
     * @param p_rect_width The width of the rectangle.
     * @param p_rect_height The height of the rectangle.
     * @param p_coord The to be checked coordinate.
     * @return If no border was clicked, @c GRAPE_DIR_NONE is returned. Otherwise one of directions defined in grape_direction.
     */
    grape_direction is_on_border_rectangle( const coordinate &p_rect_coord, float p_rect_width, float p_rect_height, coordinate &p_coord );

    /**
     * Function to see if a coordinate is inside an initial designator with the specified begin *
      coordinate, and with the specified width and height.
     * By default, the allowed margin of the cursor is taken into account and therefore subtracted from the size of the designator at each side.
     * @param p_begin The begin coordinate of the designator.
     * @param p_width The width of the designator.
     * @param p_height The height of the designator.
     * @param p_coord The to be checked coordinate.
     * @return Returns whether the coordinate is on the border of the rectangle.
     */
    bool is_inside_designator( const coordinate &p_begin, float p_width, float p_height, const coordinate &p_coord );

    /**
     * Function to see if a coordinate is inside an initial designator with the specified end
     * coordinate, and with the specified width and height.
     * By default, the allowed margin of the cursor is taken into account and therefore subtracted from the size of the designator at each side.
     * @param p_width The width of the designator.
     * @param p_height The height of the designator.
     * @param p_end The begin coordinate of the designator.
     * @param p_coord The to be checked coordinate.
     * @return Returns whether the coordinate is on the border of the rectangle.
     */
    bool is_inside_designator( const coordinate &p_begin, const coordinate &p_end, const coordinate &p_coord );

    /**
     * Function to see if a coordinate is on the edge of an initial designator with the specified begin and end
     * coordinate, and with the specified width and height.
     * Note that the y-axis is negative and the origin is the upper left corner.
     * @param p_begin The begin coordinate of the designator.
     * @param p_end The end coordinate of the designator.
     * @param p_width The width of the designator.
     * @param p_height The height of the designator.
     * @param p_coord The to be checked coordinate.
     * @return Returns whether the coordinate is on the border of the designator.
     */
    bool is_on_border_designator( const coordinate &p_begin, const coordinate &p_end, float p_width, float p_height, const coordinate &p_coord );

   /**
     * Test if coordinate is on the border of the rectangle, if not a nearby coordinate that is on the border is returned.
     * @param p_rect_coord The center coordinate of the rectangle.
     * @param p_rect_width The width of the rectangle.
     * @param p_rect_height The height of the rectangle.
     * @param p_coord The to be checked coordinate.
     * @return Returns a coordinate on the border of the visualobject
     */
    coordinate move_to_border_rectangle( const coordinate &p_rect_coord, float p_rect_width, float p_rect_height, const coordinate &p_coord );

    /**
     * Line draw function.
     * @param p_begin The begin coordinate of the to be drawn line
     * @param p_end The end coordinate of the to be drawn line.
     * @param A flag indicating whether the object is selected.
     */
    void draw_line( const coordinate &p_begin, const coordinate &p_end, bool p_selected, const color p_color = g_color_black );

    /**
     * Initial designator (arrow) draw function.
     * @param p_begin The begin coordinate of the arrow.
     * @param p_width The width of the arrow.
     * @param p_height The width of the arrow.
     * @param p_selected A flag indicating whether the object is selected.
     */
    void draw_designator( const coordinate &p_begin, float p_width, float p_height, bool p_selected );

    /**
     * Initial designator (arrow) draw function.
     * @param p_begin The begin coordinate of the arrow
     * @param p_end The end coordinate of the arrow.
     * @param p_selected A flag indicating whether the object is selected.
     */
    void draw_designator( const coordinate &p_begin, const coordinate &p_end, bool p_selected );

    /**
     * State (ellipse) draw function.
     * @param p_center The center coordinate of the ellipse.
     * @param p_radius_x The radius of the ellipse, in the direction of the x-axis.
     * @param p_radius_y The radius of the ellipse, in the direction of the y-axis.
     * @param p_selected A flag indicating whether the object is selected.
     */
    void draw_state( const coordinate &p_center, float p_radius_x, float p_radius_y, bool p_selected  );

    /**
     * Function to see if a coordinate is inside a nonterminating transition.
     * @param p_ntt_coord The coordinate of the nonterminating transtition.
     * @param p_base_coordinate The coordinate where the arrow should start.
     * @param p_head_coordinate The coordinate where the arrow should end.
     * @param p_coord The to be checked coordinate.
     */
    bool is_inside_nonterminating_transition( const coordinate &p_ntt_coord, const coordinate &p_base_coordinate, const coordinate &p_head_coordinate, const coordinate &p_coord );

    /**
     * Function to see if a coordinate is inside a nonterminating transition with same begin and endstate.
     * @param p_center The coordinate center coordinate of the reference.
     * @param p_width Width of the reference.
     * @param p_height Height of the reference.
     * @param p_coord The to be checked coordinate.
     */
    bool is_inside_reference(const coordinate &p_center, float p_width, float p_height, coordinate p_coord);

    /**
     * Nonterminating transition (arrow) draw function.
     * @param p_begin The begin coordinate of the arrow.
     * @param p_control The control coordinate of the arrow.
     * @param p_end The end coordinate of the arrow.
     * @param p_selected A flag indicating whether the object is selected.
     * @param p_label_text The label of the object.
     */
    void draw_nonterminating_transition( const coordinate p_begin, const coordinate p_control, const coordinate p_end, const bool p_selected, const wxString &p_label_text );

    /**
     * Nonterminating transition (arrow) draw function.
     * @param p_begin The begin coordinate of the arrow
     * @param p_end The end coordinate of the arrow.
     * @param p_selected A flag indicating whether the object is selected.
     * @param p_label_text The label of the object.
     */
  //  void draw_nonterminating_transition( const coordinate &p_begin, const coordinate &p_end, bool p_selected, const wxString &p_label_text );

    /**
     * Terminating transition (arrow) draw function.
     * @param p_begin The begin coordinate of the arrow
     * @param p_end The end coordinate of the arrow.
     * @param p_selected A flag indicating whether the object is selected.
     * @param p_label_text The label of the object.
     */
    void draw_terminating_transition( const coordinate &p_begin, const coordinate &p_end, bool p_selected, const wxString &p_label_text );

    /**
     * Channel (circle) draw function.
     * @param p_center The center coordinate of the ellipse.
     * @param p_radius The radius of the circle.
     * @param p_selected A flag indicating whether the object is selected.
     * @param p_channel_type Type of the channel.
     */
    void draw_channel( const coordinate &p_center, float p_radius, bool p_selected, const channel_type p_channel_type);

    /**
     * Channel communication (circle) draw function.
     * @param p_center The center coordinate of the ellipse.
     * @param p_radius The radius of the circle.
     * @param p_selected A flag indicating whether the object is selected.
     * @param p_channel_communication_type Type of the channel communication.
     */
    void draw_channel( const coordinate &p_center, float p_radius, bool p_selected, const channel_communication_type p_channel_communication_type);

    /**
     * Filled rectangle draw function.
     * @param p_center The center coordinate of the rectangle.
     * @param p_width The width of the rectangle.
     * @param p_height The height of the rectangle.
     * @param p_selected A flag indicating whether the object is selected.
     * @param p_color The intended fill color of the rectangle.
     */
    void draw_filled_rectangle( const coordinate &p_center, float p_width, float p_height, bool p_selected, const color p_color );

    /**
     * Line rectangle draw function.
     * @param p_center The center coordinate of the rectangle.
     * @param p_width The width of the rectangle.
     * @param p_height The height of the rectangle.
     * @param p_selected A flag indicating whether the object is selected.
     * @param p_color The intended color of the edge of the rectangle.
     */
    void draw_line_rectangle( const coordinate &p_center, float p_width, float p_height, bool p_selected, const color p_color );

    /**
     * Cross draw function.
     * @param p_center The center coordinate of the cross.
     * @param p_width The width of the cross.
     * @param p_height The height of the cross.
     * @param p_selected A flag indicating whether the cross is selected.
     */
    void draw_cross( const coordinate &p_center, float p_width, float p_height, bool p_selected );

    /**
     * Tick draw function.
     * @param p_center The center coordinate of the tick.
     * @param p_width The width of the tick.
     * @param p_height The height of the tick.
     * @param p_selected A flag indicating whether the tick is selected.
     */
    void draw_tick( const coordinate &p_center, float p_width, float p_height, bool p_selected );

    /**
     * Process reference draw function.
     * @param p_center The center coordinate of the process reference.
     * @param p_width The width of the process reference.
     * @param p_height The height of the process reference.
     * @param p_selected A flag indicating whether the process reference is selected.
     */
    void draw_reference(const coordinate &p_center, float p_width, float p_height, bool p_selected);

    /**
     * Bounding box draw function.
     * @param p_center The center coordinate of the bounding box.
     * @param p_width The width of the bounding box.
     * @param p_height The height of the bounding box.
     * @param p_selected A flag indicating whether the bounding box should be drawn (true) or not (false)
     */
    void draw_bounding_box( const coordinate &p_center, float p_width, float p_height, bool p_selected);

    /**
     * Bounding box touch function. Returns whether one of the points ( squares ) of the bounding box is touched,
     * @param p_center The center coordinate of the bounding box.
     * @param p_width The width of the bounding box.
     * @param p_height The height of the bounding box.
     * @param p_coord The to be checked coordinate
     * @return If none of the squares on the bounding box was clicked, @c GRAPE_DIR_NONE is returned. Otherwise one of directions defined in grape_direction.
     */
    grape_direction grab_bounding_box( const coordinate &p_center, float p_width, float p_height, const coordinate &p_coord, bool p_selected );

    /**
     * Calculates a suitable width for an object given its name.
     * @param p_string The string to consider for calculating the width.
     * @return The calculated width.
     */
    float get_width_hint( const wxString &p_string );
  }
}

#endif // LIBGRAPE_GRAPE_FUNC_H
