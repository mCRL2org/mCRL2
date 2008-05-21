// Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./edge.cpp

#include "edge.h"
#include <string>
#include <iostream>
#include <cmath>
#include <workarounds.h>

#define ARROW_WIDTH 1
#define PI 3.14159265
#define CONTROL_RADIUS 2
const double triangle_base = 2.0;
const double triangle_height = 7.0;

const wxString color = wxT("BLACK");
const wxString selected_colour = wxT("BLUE");
const wxString default_label_colour = wxT("BLACK");

edge::edge(Node* _N1, Node* _N2, wxString _lbl) : N1(_N1), N2(_N2), lbl(_lbl) 
{ 
  lbl.Replace(wxT("\""), wxT(""), true);
  labelsVisible = true;
  label_colour = default_label_colour;



  control_point_visible = false;
  control_selected = false;		
  label_selected = false;	

  double x1 = N1->GetX();
  double x2 = N2->GetX();
  double y1 = N1->GetY();
  double y2 = N2->GetY();  
  double radius = N1->GetRadius();
  // Initial position of control points is exactly between the nodes, if the nodes are not in the same place.
  // If they are in the same place, we put it diagonally above the node
  if (x1 == x2 && y1 == y2) {
    set_control(x1 + radius * 2, y1 - radius * 2);
    set_label_x(x1 + radius * 2 + POS_EDGE_LBL_X);
    set_label_y(y1 - radius * 2 + POS_EDGE_LBL_Y);
  }
  else {
    set_control((x1 + x2) / 2, (y1 + y2) / 2);
    set_label_x((x1 + x2) / 2 + POS_EDGE_LBL_X);
    set_label_y((y1 + y2) / 2 + POS_EDGE_LBL_Y);
  }
}

void edge::get_spline_control_points(wxPoint *points)
{
  /* First, we calculate the spline control point fromt the control point
   */
  double start_x = get_x_pos1();
  double start_y = get_y_pos1();
  
  double end_x = get_x_pos2();
  double end_y = get_y_pos2();

  double node_radius = N1->GetRadius();

  double control_x = get_x_control();
  double control_y = get_y_control();

  /* We calculate the actual spline control points by taking two points
   * on the line l through c=(control_x,control_y) that is parallel with
   * the line through s=(start_x,start_y) and e=(end_x,end_y). We put them
   * halfway c and the projection of s resp. e on l.
   */
  // Line s-e is a+lambda*b where we take a=s and b=s-e. Then l(lambda) =
  // c+lambda*b.
  double se_bx = start_x-end_x;
  double se_by = start_y-end_y;
  // If s = e, then we take for b a vector perpendicular to c-s
  double normalise; // We normalise b such that for lambda=1 resp. lambda=-1
                    // we get the desired spline control points.
  if ( se_bx == 0.0 && se_by == 0.0 ) // self loop
  {
    // assumption: !(start_x == control_x && start_y == control_y)
    if ( control_x == start_x )
    {
      se_bx = 1.0;
      se_by = 0;
      normalise = 1.0/10.0;
    } else {
      se_bx = -(control_y-start_y)/(control_x-start_x);
      se_by = 1.0;
      if ( control_x > start_x )
      {
        se_bx = -se_bx;
        se_by = -se_by;
      }
      // for a self loop we put the spline control points about 10 pixels
      // from c
      normalise = sqrt(se_bx*se_bx+1.0)/10.0;
    }
  } else {
    normalise = 4.0;
  }
  se_bx /= normalise;
  se_by /= normalise;
  // We now have the following spline control points
  p1x = start_x;
  p1y = start_y;
  p2x = control_x+se_bx;
  p2y = control_y+se_by;
  p3x = control_x-se_bx;
  p3y = control_y-se_by;
  p4x = end_x;
  p4y = end_y;
  // In case of a self loop, we put a bit of distance between p1 and p4
  if (start_x == end_x && start_y == end_y) {
    p1x -= (node_radius/2)*sin(-control_point_alpha);
    p1y -= (node_radius/2)*cos(control_point_alpha);
    p4x += (node_radius/2)*sin(-control_point_alpha);
    p4y += (node_radius/2)*cos(control_point_alpha);
/*  } else {
    double normalise_to_radius;
    normalise_to_radius = sqrt((p2x-p1x)*(p2x-p1x)+(p2y-p1y)*(p2y-p1y))/node_radius;
    p1x -= (p2x-p1x)/normalise_to_radius;
    p1y -= (p2y-p1y)/normalise_to_radius;
    normalise_to_radius = sqrt((p3x-p4x)*(p3x-p4x)+(p3y-p4y)*(p3y-p4y))/node_radius;
    p4x -= (p4x-p3x)/normalise_to_radius;
    p4y -= (p4y-p3y)/normalise_to_radius;*/
  }

  points[0].x = (int) p1x;
  points[0].y = (int) p1y;
  points[1].x = (int) p2x;
  points[1].y = (int) p2y;
  points[2].x = (int) p3x;
  points[2].y = (int) p3y;
  points[3].x = (int) p4x;
  points[3].y = (int) p4y;
}

void edge::get_arrow_points(wxPoint *points)
{
  double start_x = get_x_pos1();
  double start_y = get_y_pos1();
  
  double end_x = get_x_pos2();
  double end_y = get_y_pos2();

  double node_radius = N1->GetRadius();

  // We calculate the distance from control point until the end of the 
  // edge. 
  double dist_cp_ed = sqrt( (p3x - p4x) * (p3x - p4x) + (p3y - p4y) * (p3y - p4y));

  // Calculate the ratio between the radius and dist_cp_ed
  double arrow_ratio = 0.0;

  if ( dist_cp_ed != 0) {
    if ( start_x == end_x && start_y == end_y )
      arrow_ratio = (node_radius*0.71) / dist_cp_ed;
    else
      arrow_ratio = node_radius / dist_cp_ed;
  }
  else {
    arrow_ratio = 0;
  }

  // Calculate the x and y the arrow starts on through the ratio
  double triangle_x = p4x - (p4x - p3x) * arrow_ratio;
  double triangle_y = p4y - (p4y - p3y) * arrow_ratio;

  // Also, we calculate the ratio for triangle A -- defined  by the center of the triangle base, the center of the node, and the point 
  // (base.x, node.y)--, with respect to triangle B -- defined by the spline control point, the center of the node and the point 
  // (spline_control.x, node.y)
  double base_ratio = 0.0;
  
  if ( dist_cp_ed != 0) {
    base_ratio = (node_radius + triangle_height) / dist_cp_ed;
    //base_ratio = (triangle_height) / dist_cp_ed;
  }
  else {
    base_ratio = 0;
  }

  // We calculate the location of the center of the triangle base
  double base_x = p4x - base_ratio * (p4x - p3x);
  double base_y = p4y - base_ratio * (p4y - p3y);

  // Now, we can calculate the cosinus and the sinus of the angle the arrow's center line makes with the basis of our grid.
  double sinus_alpha = p4y;
  double cosinus_alpha = p4x;

  if (triangle_height != 0) {
    sinus_alpha = (base_y - p4y) / triangle_height;
    cosinus_alpha = (base_x - p4x) / triangle_height;
  }
  
  // with sinus of alpha and cosinus of alpha, we can calculate the rest of the points as follows:
  // 1) Rotate the point (triangle_height, 1/2 * triangle_base) through angle alpha, w.r.t. to the origin.
  // 2) Translate the point gained through 1) over the vector (end_x, end_y)
  // Together, this can be summarised in the calculation shown in the assignment below. 
  // For the other point, substitue 1/2 * triangle_base with - 1/2 * triangle_base in calculation).
  int arrow_base_high_x = (int) round(p4x + (cosinus_alpha * triangle_height -  .5 * triangle_base * sinus_alpha));
  int arrow_base_high_y = (int) round(p4y + (sinus_alpha * triangle_height + .5 * triangle_base * cosinus_alpha));
  int arrow_base_low_x  = (int) round(p4x + (cosinus_alpha * triangle_height +  .5 * triangle_base * sinus_alpha));
  int arrow_base_low_y  = (int) round(p4y + (sinus_alpha * triangle_height - .5 * triangle_base * cosinus_alpha));
  
  //Edge head (polygone)
  points[0].x = (int) triangle_x;
  points[0].y = (int) triangle_y;
  points[1].x = arrow_base_high_x;
  points[1].y = arrow_base_high_y;
  points[2].x = arrow_base_low_x;
  points[2].y = arrow_base_low_y;
}

void edge::on_paint(wxDC * ptrDC) 
{
    wxPoint spline_points[4];
    get_spline_control_points(spline_points);

    wxPoint arrow_points[3];
    get_arrow_points(arrow_points);

    //Prepare the filling
    wxBrush myBrush(color,wxSOLID );
    ptrDC->SetBrush(myBrush);
    
    //Draw the triangle
    ptrDC->DrawPolygon(3,arrow_points, 0, 0, wxWINDING_RULE);

    //Edge body (spline)
    wxPen myPen(color,ARROW_WIDTH,wxSOLID);
    ptrDC->SetPen(myPen);

    ptrDC->DrawSpline(4,spline_points);
    

    //Label
		if (labelsVisible) {
			wxFont MyFont(FONT_SIZE, wxSWISS , wxNORMAL, wxNORMAL, false, wxT("Arial"));
			ptrDC->SetFont(MyFont);

                        if (label_selected) {
                          ptrDC->SetTextForeground(selected_colour);
                        }
                        else {
                          ptrDC->SetTextForeground(label_colour);
                        }

			ptrDC->DrawRotatedText(lbl,static_cast<int>(get_label_lower_x()),static_cast<int>(get_label_lower_y()),0);
		}
  // Store label higher coordinates, now that we have a Device Context
  wxCoord w, h;
  ptrDC->GetTextExtent(lbl, &w, &h);
  label_higher_x = label_x + w;
  label_higher_y = label_y + h;
  
  // Control point
  if (control_point_visible) {
    if (control_selected) {
       myBrush.SetColour(selected_colour);
       ptrDC->SetBrush(myBrush);
    }

    ptrDC->DrawCircle((wxCoord)get_x_control(), (wxCoord)get_y_control(),CONTROL_RADIUS); 
    
  }
}


Node* edge::get_n1() 
{
  return N1;
}

Node* edge::get_n2() {
  return N2;
}

double edge::get_x_pos1() {
  return N1->GetX();
}

string edge::get_lbl() {
	string st_lbl = string(lbl.fn_str());
	return st_lbl;
}

double edge::get_y_pos1() {
  return N1->GetY();
}

double edge::get_x_pos2() {
  return N2->GetX();
}

double edge::get_y_pos2() {
  return N2->GetY();
}

double edge::get_x_control() {
  double x_1 = N1 -> GetX();
  double y_1 = N1 -> GetY();
  double x_2 = N2 -> GetX();
  double y_2 = N2 -> GetY();

  double beta = atan2( y_2 - y_1 , x_2 - x_1 );
  double gamma = beta + control_point_alpha;
  double node_dist = sqrt( (x_1 - x_2) * (x_1 - x_2) + (y_1 - y_2) * (y_1 - y_2) );
  
  if (node_dist != 0) {
    return x_1 + node_dist * control_point_dist * cos(gamma);
  }
  else {
    return x_1 + control_point_dist * cos(gamma);
  }
}

double edge::get_y_control() {
  double x_1 = N1 -> GetX();
  double y_1 = N1 -> GetY();
  double x_2 = N2 -> GetX();
  double y_2 = N2 -> GetY();
  double beta = 0.0;

  beta = atan2 ( y_2 - y_1 , x_2 - x_1 );

  double gamma = beta + control_point_alpha;  
  
  double node_dist = sqrt( (x_1 - x_2) * (x_1 - x_2) + (y_1 - y_2) * (y_1 - y_2) );
  
  if (node_dist != 0) {
    return y_1 + node_dist * control_point_dist * sin(gamma);
  }
  else {
    return y_1 + control_point_dist * sin(gamma);
  }
}

bool edge::LabelVisible() {
  return labelsVisible;
}

void edge::ShowLabels() {
  labelsVisible = true;
} 

void edge::HideLabels() {
  labelsVisible = false;
}

void edge::set_control(double new_x, double new_y) {
  // Calculate polar coordinates alpha, dist from new_x, new_y
  
  // Calculate the angle beta, that the line N1, N2 makes with the basis of the 
  // DC (in radians). We do this in a procedural fashion, to avoid large if statements.
  double x_1 = N1->GetX();
  double y_1 = N1->GetY();
  double x_2 = N2->GetX();
  double y_2 = N2->GetY();

  double beta = 0.0;


  /*if (x_2 == x_1 && y_2 == y_1) {
    beta = PI / 2;
  }
  else {*/
    beta = atan2 ( y_2 - y_1 , x_2 - x_1 );
  /*}*/

  // Calculate angle of point new_x, new_y w.r.t x-axis. This is always correct, ctrl point 
  double gamma = 0.0;

  /*if (new_x == x_1) {
    gamma = PI / 2;
  }
  else {*/
    gamma = atan2 ( new_y - y_1 , new_x - x_1);
  /*}*/


  control_point_alpha = gamma - beta;


  // Calculate d(N1, (new_x, new_y))
  control_point_dist = sqrt( (x_1 - new_x) * (x_1 - new_x) + (y_1 - new_y) * (y_1 - new_y) );
  double node_dist = sqrt( ( x_1 - x_2) * (x_1 - x_2) + (y_1 - y_2) * (y_1 - y_2) );
  
  if ((node_dist == 0) && (control_point_dist < 2*N1->GetRadius())) {
    control_point_dist = N1->GetRadius() * 2;
  } else if ( node_dist != 0 )
  {
    control_point_dist = control_point_dist / node_dist;
  }

}

  
void edge::set_control_selected(bool selection_value) {
  control_selected = selection_value;
}

void edge::set_control_visible(bool selection_value) {
  control_point_visible = selection_value;
}

wxColour edge::get_label_colour() {
  return label_colour;
}

void edge::set_label_colour(wxColour new_colour) {
  label_colour = new_colour;
}

double edge::get_label_lower_x() {
  return label_x + get_x_control();
}

double edge::get_label_lower_y() {
  return label_y + get_y_control();
}

void edge::set_label_x(double new_value) {
  // Set the label value relative to the control point.
  label_x = new_value - get_x_control();
}

void edge::set_label_y(double new_value) {
  label_y = new_value - get_y_control();
}

void edge::set_label_selected(bool selection_value) {
  label_selected = selection_value;
}

double edge::get_label_higher_x() {
  return label_higher_x + get_x_control();
}

double edge::get_label_higher_y() {
  return label_higher_y + get_y_control();
}

void edge::set_label_text(wxString new_value) {
  lbl = new_value;
}
