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
const wxString control_selected_colour = wxT("BLUE");

edge::edge(Node* _N1, Node* _N2, wxString _lbl) : N1(_N1), N2(_N2), lbl(_lbl) 
{ 
	lbl.Replace(wxT("\""), wxT(""), true);
	labelsVisible = true;

        // Initial position of control points is exactly between the nodes.
        control_point_x = (N1->GetX()+N2->GetX()) / 2;
        control_point_y = (N1->GetY()+N2->GetY()) / 2;

        //TODO (CT): Make false the default value.
        control_point_visible = false;
        control_selected = false;
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
                             -CIRCLE_RADIUS * cos(alpha):
                              CIRCLE_RADIUS * cos(alpha)) ));
    wxCoord newY =  (int) round(( end_y - (end_x - spline_control_x >=0?
                              CIRCLE_RADIUS * sin(alpha):
                             -CIRCLE_RADIUS * sin(alpha))));
    
    
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
	
			double x1=N1->GetX();
			double y1=N1->GetY();
			double x2=N2->GetX();
			double y2=N2->GetY();
			double posX=(x1+x2)/2+POS_EDGE_LBL_X;
			double posY=(y1+y2)/2+POS_EDGE_LBL_Y;
			ptrDC->DrawRotatedText(lbl,(int) round(posX),(int) round(posY),0);
		}
  // Control point
  if (control_point_visible) {
    if (control_selected) {
       myBrush.SetColour(control_selected_colour);
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
  return control_point_x;
}

double edge::get_y_control() {
  return control_point_y;
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

void edge::set_x_control(double new_x) {
  control_point_x = new_x;
}

void edge::set_y_control(double new_y) {
  control_point_y = new_y;
}

void edge:: reset_control() {
  control_point_x = (N1->GetX() + N2->GetX()) / 2;
  control_point_y = (N1->GetY() + N2->GetY()) / 2;
}
  
void edge::set_control_selected(bool selection_value) {
  control_selected = selection_value;
}

void edge::set_control_visible(bool selection_value) {
  control_point_visible = selection_value;
}
