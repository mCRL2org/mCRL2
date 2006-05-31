#include "edge.h"
#include <string>
#include <iostream>
#include <math.h>

#define ARROW_WIDTH 1
#define PI 3.14159265

const double triangle_base = 4.0;
const double triangle_height = 8.0;

const wxColour color = "BLACK";

Edge::Edge(Node* _N1, Node* _N2, wxString _lbl) : lbl(_lbl) 
{ 
	N1=_N1;
  N2=_N2;
	lbl.Replace(wxT("\""), wxT(""), true);
}



void Edge::OnPaint(wxDC * ptrDC) 
{
    //Calculate triangle coord
    //angle expressed in radians
    double alpha = atan((GetYpos2()-GetYpos1()) / (GetXpos2()-GetXpos1()));

    wxCoord newX = (int) round((GetXpos2() + (GetXpos2()-GetXpos1()>0?
                             -CIRCLE_RADIUS * cos(alpha):
                              CIRCLE_RADIUS * cos(alpha)) ));
    wxCoord newY =  (int) round(( GetYpos2() - (GetXpos2()-GetXpos1()>=0?
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
    if (GetXpos2() > GetXpos1()) 
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
    ptrDC->DrawLine((wxCoord)GetXpos1(),(wxCoord)GetYpos1(),(wxCoord)GetXpos2(),(wxCoord)GetYpos2());

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


Node* Edge::Get_N1() 
{
  return N1;
}

Node* Edge::Get_N2() {
  return N2;
}

double Edge::GetXpos1() {
  return N1->GetX();
}

double Edge::GetYpos1() {
  return N1->GetY();
}

double Edge::GetXpos2() {
  return N2->GetX();
}

double Edge::GetYpos2() {
  return N2->GetY();
}

void Edge::ShowLabels() {
		labelsVisible = true;
} 

void Edge::HideLabels() {
		labelsVisible = false;
} 

