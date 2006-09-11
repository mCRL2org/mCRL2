#include "edge.h"
#include <string>
#include <iostream>
#include <math.h>

#define ARROW_WIDTH 1
#define PI 3.14159265
#define CONTROL_RADIUS 2
const double triangle_base = 4.0;
const double triangle_height = 8.0;

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
  double radius = N1->get_radius();
  // Initial position of control points is exactly between the nodes, if the nodes are not in the same place.
  // If they are in the same place, we put it diagonally above the node
  if (x1 == x2 && y1 == y2) {
    set_control(x1 + radius * 2, y1 + radius * 2);
  }
  else {
    set_control((x1 + x2) / 2, (y1 + y2) / 2);
  }
  label_x= (x1 + x2) / 2 + POS_EDGE_LBL_X;
  label_y= (y1 + y2) / 2 + POS_EDGE_LBL_Y;
}


void edge::on_paint(wxDC * ptrDC) 
{
    //Calculate triangle coord
    //angle expressed in radians
    /* First, we calculate the spline control point fromt the control point
     */
    double start_x = get_x_pos1();
    double start_y = get_y_pos1();
    
    double end_x = get_x_pos2();
    double end_y = get_y_pos2();

    /* Adjust the start and end point of the arrow to allow self-referring loops */
    double node_radius = N1->get_radius();

    if (start_x == end_x && start_y == end_y) {
      start_x += node_radius;
      end_y += node_radius;
    }
    double control_x = get_x_control();
    double control_y = get_y_control();

    double spline_control_x = (8 * control_x - (start_x + end_x)) / 6;
    double spline_control_y = (8 * control_y - (start_y + end_y)) / 6;

    // To make sure division by zero does not occur
    if ( end_x == spline_control_x ) {
      spline_control_x += 0.001;
    }

    double alpha = atan((end_y - spline_control_y) / (end_x - spline_control_x));

    wxCoord newX = (int) round((end_x + (end_x - spline_control_x >0?
                             -node_radius * cos(alpha):
                              node_radius * cos(alpha)) ));
    wxCoord newY =  (int) round(( end_y - (end_x - spline_control_x >=0?
                              node_radius * sin(alpha):
                             - node_radius * sin(alpha))));
    
    
    //Calculate triangle points coord
    //angles expressed in radians
    double beta = atan(triangle_base/(triangle_height*2));
    
    double ArrowSideLength = sqrt( (triangle_height*triangle_height) + 
                    (triangle_base*triangle_base)/4 );
    
    double gamma_p1 = alpha - beta;
    double lenX1 = cos(gamma_p1) * ArrowSideLength;
    double lenY1 = sin(gamma_p1) * ArrowSideLength;
    
    double gamma_p3 = alpha + beta;
    double lenX3 = cos(gamma_p3) * ArrowSideLength; 
    double lenY3 = sin(gamma_p3) * ArrowSideLength;
    
    // coord correcting depend on position 
    if (get_x_pos2() >= get_x_control()) 
    {
      lenX1 = -lenX1;
      lenY1 = -lenY1;
      lenX3 = -lenX3;
      lenY3 = -lenY3;
    }
   
    //Edge head (polygone)
    wxPoint * p1 = new wxPoint((int) round(lenX1), (int) round(lenY1));
    wxPoint * p2 = new wxPoint(0, 0);
    wxPoint * p3 = new wxPoint((int) round(lenX3), (int) round(lenY3));

    wxPoint points[3] = {*p1,*p2,*p3};

    //Prepare the filling
    wxBrush myBrush(color,wxSOLID );
    ptrDC->SetBrush(myBrush);
    
    //Draw the triangle
    ptrDC->DrawPolygon(3,points, newX, newY, wxWINDING_RULE);

    //Edge body (spline)
    wxPen myPen(color,ARROW_WIDTH,wxSOLID);
    ptrDC->SetPen(myPen);

    ptrDC->DrawSpline((wxCoord)start_x,(wxCoord)start_y,
                      (wxCoord)spline_control_x, (wxCoord)spline_control_y,
                      (wxCoord)end_x,(wxCoord)end_y);

    

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

			ptrDC->DrawRotatedText(lbl,static_cast<int>(label_x),static_cast<int>(label_y),0);
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

    ptrDC->DrawCircle((wxCoord)control_x, (wxCoord)control_y,CONTROL_RADIUS); 
    
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

  double beta = 0.0;

  if (x_1 == x_2) {
    beta = PI / 2;
  }
  else {
    beta = atan ( (y_2 - y_1) / (x_2 - x_1) );
  }

  double gamma = beta + control_point_alpha;

  return x_1 + control_point_dist * cos(gamma);

}

double edge::get_y_control() {
  double x_1 = N1 -> GetX();
  double y_1 = N1 -> GetY();
  double x_2 = N2 -> GetX();
  double y_2 = N2 -> GetY();

  double beta = 0.0;

  if (x_1 == x_2) {
    beta = PI / 2;
  }
  else {
    beta = atan ( (y_2 - y_1) / (x_2 - x_1) );
  }

  double gamma = beta + control_point_alpha;

  return y_1 + control_point_dist * sin(gamma);


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


  if (x_2 == x_1) {
    beta = PI / 2;
  }
  else {
    beta = atan ( (y_2 - y_1) / (x_2 - x_1) );
  }


/* 
  // Correct beta according to the coordinates 
  if ( x_2 < x_1 ) {
    beta += PI / 2;
  }

  if ( y_2 < y_1 ) {
    beta = -beta;
  }
*/

  // Calculate angle of point new_x, new_y w.r.t x-axis. This is always correct, ctrl point 
  double gamma = 0.0;

  if (new_x == x_1) {
    gamma = PI / 2;
  }
  else {
    gamma = atan ( (new_y - y_1) / (new_x - x_1));
  }


  control_point_alpha = gamma - beta;


  // Calculate d(N1, (new_x, new_y))
  if (x_1 < new_x) {
    if ( y_1 < new_y) {
      control_point_dist = sqrt( (x_1 - new_x) * (x_1 - new_x) + (y_1 - new_y) * (y_1 - new_y) );
    }
    else {
            control_point_dist = sqrt( (x_1 - new_x) * (x_1 - new_x) + (new_y - y_1) * (new_y - y_1));
    }
  }
  else {
    if ( y_1 < new_y) {
      control_point_dist = sqrt( (new_x - x_1) * (new_x - x_1) + (y_1 - new_y) * (y_1 - new_y) );
    }
    else {
      control_point_dist = sqrt(  (new_x - x_1) * (new_x - x_1)  + (new_y - y_1) * (new_y - y_1));
    }
  }
/*  
  std::cerr << "x_1 = " << x_1 << "\n";
  std::cerr << "y_1 = " << y_1 << "\n";
  std::cerr << "new_x = " << new_x << "\n";
  std::cerr << "new_y = " << new_y << "\n";
  std::cerr << "control_point_dist = " << control_point_dist << "\n";
  std::cerr << "=====================\n";
*/  
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
  return label_x;
}

double edge::get_label_lower_y() {
  return label_y;
}

void edge::set_label_x(double new_value) {
  label_x = new_value;
}

void edge::set_label_y(double new_value) {
  label_y = new_value;
}

void edge::set_label_selected(bool selection_value) {
  label_selected = selection_value;
}

double edge::get_label_higher_x() {
  return label_higher_x;
}

double edge::get_label_higher_y() {
  return label_higher_y;
}

void edge::reset_label() {
  double x1 = N1->GetX();
  double x2 = N2->GetX();
  double y1 = N1->GetY();
  double y2 = N2->GetY();
  label_x= (x1 + x2) / 2 + POS_EDGE_LBL_X;
  label_y= (y1 + y2) / 2 + POS_EDGE_LBL_Y;
}

void edge::set_label_text(wxString new_value) {
  lbl = new_value;
}
