#include "node.h"

const double POS_LBL_X = 3.0;
const double POS_LBL_Y = 5.0;

Node::Node(unsigned int _num, double _posX, double _posY, wxString _Slbl) : posX(_posX), posY(_posY), num(_num) {

    lbl = new Label(_Slbl, posX-POS_LBL_X, posY-POS_LBL_Y);
	locked = false;
}

void Node::OnPaint(wxPaintDC * ptrDC) {
	
	wxColour color;
	if (!locked)
		color = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	else 
		color = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);


    //Circle
    wxBrush myBrush(color,wxSOLID );
    ptrDC->SetBrush(myBrush);
    ptrDC->DrawCircle((wxCoord)posX,(wxCoord)posY,CIRCLE_RADIUS);

    //Label
    lbl->OnPaint(ptrDC);
    
}

unsigned int Node::Get_num() {
    return num;
}

double Node::GetX() {
    return posX;
}

double Node::GetY() {
    return posY;
}

bool Node::IsLocked(){
	return locked;
}

void Node::Lock() {
	locked = true;
}

void Node::Unlock() {
	locked = false;
}

void Node::SetXY(double _x, double _y) {
	if (!locked) {
		posX = _x;
		posY = _y;
		//Change position of label
		lbl->SetXY(posX-POS_LBL_X, posY-POS_LBL_Y);
	}
	else 
		wxBell();
}







