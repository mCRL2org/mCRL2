#include "edge.h"
#include <string>
#include <iostream>
#include <math.h>

#define ARROW_WIDTH 1
#define PI 3.14159265

const double triangle_base = 4.0;
const double triangle_height = 8.0;

const wxString color = wxT("BLACK");

edge::edge(Node* _N1, Node* _N2, wxString _lbl) : N1(_N1), N2(_N2), lbl(_lbl) 
{ 
	lbl.Replace(wxT("\""), wxT(""), true);
	labelsVisible = true;
}


void edge::on_paint(wxDC * ptrDC) 
{
    //Calculate triangle coord
    //angle expressed in radians
    double alpha = atan((get_y_pos2()-get_y_pos1()) / (get_x_pos2()-get_x_pos1()));

    wxCoord newX = (int) round((get_x_pos2() + (get_x_pos2()-get_x_pos1()>0?
                             -CIRCLE_RADIUS * cos(alpha):
                              CIRCLE_RADIUS * cos(alpha)) ));
    wxCoord newY =  (int) round(( get_y_pos2() - (get_x_pos2()-get_x_pos1()>=0?
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
    if (get_x_pos2() >= get_x_pos1()) 
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

    //Edge body (line)
    wxPen myPen(color,ARROW_WIDTH,wxSOLID);
    ptrDC->SetPen(myPen);
    ptrDC->DrawLine((wxCoord)get_x_pos1(),(wxCoord)get_y_pos1(),(wxCoord)get_x_pos2(),(wxCoord)get_y_pos2());

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

bool edge::LabelVisible() {
	return labelsVisible;
}

void edge::ShowLabels() {
		labelsVisible = true;
} 

void edge::HideLabels() {
		labelsVisible = false;
} 

