#include "edge.h"
#include <string>
#include <iostream>
#include <math.h>

#define ARROW_WIDTH 1
#define PI 3.14159265

const double triangle_base = 4.0;
const double triangle_height = 8.0;

const wxColour color = "BLACK";

Edge::Edge(unsigned int _numN1, unsigned int _numN2, wxPoint _pos1, wxPoint _pos2, wxString _Slbl) 
	: numN1(_numN1), numN2(_numN2), pos1(_pos1), pos2(_pos2) {

    wxPoint * posLbl = new wxPoint( pos1.x + ((pos2.x-pos1.x)/2)-10 , pos1.y + ((pos2.y-pos1.y)/2)-10 );
    lbl = new Label(_Slbl, *posLbl);
    //cout << "EDGE pos1.x " << pos1.x << " pos1.y " << pos1.y << " pos2.x " << pos2.x << " pos2.y " << pos2.y << endl;
}


void Edge::OnPaint(wxPaintDC * ptrDC) {


    //Calculate triangle coord
    //angle expressed in radians
    double alpha = atan( (( double )(pos2.y-pos1.y)) / (( double ) (pos2.x-pos1.x)) );

    wxCoord newX =  pos2.x + (pos2.x-pos1.x>0?
                             -(int) round(10 * cos(alpha)):
                              (int) round(10 * cos(alpha)));
    wxCoord newY =  pos2.y - (pos2.x-pos1.x>=0?
                              (int) round(10 * sin(alpha)):
                             -(int) round(10 * sin(alpha)));
    
    
    //Calculate triangle points coord
    //angles expressed in radians
    double beta = atan( triangle_base/(triangle_height*2) );
    
    double sqrtCalc = sqrt( (triangle_height*triangle_height) + (triangle_base*triangle_base)/4 );
    
    double gamma_p1 = 1/2 * PI - alpha - beta;
    double lenX1 = cos(gamma_p1) * sqrtCalc;
    double lenY1 = sin(gamma_p1) * sqrtCalc;
    
    double gamma_p3 = alpha - beta;
    double lenY3 = sin(gamma_p3) * sqrtCalc;
    double lenX3 = cos(gamma_p3) * sqrtCalc; 
    
    //coord correcting depend on position 
    if (pos2.x < pos1.x) {
        lenY1 = -lenY1;
    }
    else {
        double lenTMP = lenY1;
        lenY1 = lenY3;
        lenY3 = lenTMP;
        lenX1 = -lenX1;
        lenX3 = -lenX3;
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
    ptrDC->DrawLine(pos1.x,pos1.y,pos2.x,pos2.y);

    //Label
    lbl->OnPaint(ptrDC);
 
}

unsigned int Edge::Get_numN1() {
	return numN1;
}

unsigned int Edge::Get_numN2() {
	return numN2;
}

void Edge::Set_pos1(wxCoord _x, wxCoord _y){
	if (!locked1) {
		pos1.x = _x;
		pos1.y = _y;
		lbl->SetXY(pos1.x + ((pos2.x-pos1.x)/2)-10 , pos1.y + ((pos2.y-pos1.y)/2)-10);
	}
	else 
		wxBell();
}

void Edge::Set_pos2(wxCoord _x, wxCoord _y) {
	if (!locked2) {
		pos2.x = _x;
		pos2.y = _y;
		//Change position of label
		lbl->SetXY(pos1.x + ((pos2.x-pos1.x)/2)-10 , pos1.y + ((pos2.y-pos1.y)/2)-10);
	}
	else
		wxBell();
}

int Edge::GetXpos1() {
	return pos1.x;
}

int Edge::GetYpos1() {
	return pos1.y;
}

int Edge::GetXpos2() {
	return pos2.x;
}

int Edge::GetYpos2() {
	return pos2.y;
}

void Edge::Lock1() {
	locked1 = true;
}

void Edge::Unlock1() {
	locked1 = false;
}

void Edge::Lock2() {
	locked2 = true;
}

void Edge::Unlock2() {
	locked2 = false;
}



