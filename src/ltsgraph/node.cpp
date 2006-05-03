#include "node.h"

#define POS_LBL_X 3
#define POS_LBL_Y 5

Node::Node(int _num, wxPoint _pos, wxString _Slbl) : num(_num), pos(_pos) {
    //cout << "NODE no: " << num << " pos.x " << pos.x << " pos.y " << pos.y << endl;
    wxPoint * posLbl = new wxPoint( pos.x-POS_LBL_X, pos.y-POS_LBL_Y );
    lbl = new Label(_Slbl, *posLbl);
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
    ptrDC->DrawCircle(pos.x,pos.y,CIRCLE_RADIUS);

    //Label
    lbl->OnPaint(ptrDC);
    
}

int Node::Get_num() {
    return num;
}

wxCoord Node::GetX() {
    return pos.x;
}

wxCoord Node::GetY() {
    return pos.y;
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

void Node::SetXY(wxCoord _x, wxCoord _y) {
	if (!locked) {
		pos.x = _x;
		pos.y = _y;
		//Change position of label
		lbl->SetXY(pos.x-POS_LBL_X, pos.y-POS_LBL_Y);
	}
	else 
		wxBell();
}







