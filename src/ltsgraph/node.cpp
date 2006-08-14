#include "node.h"

const wxColour borderColor     = "BLACK";
const wxColour borderColorInit = "RED";

Node::Node(unsigned int _num, double _posX, double _posY, wxString _lbl, bool _initState) : 
             posX(_posX), posY(_posY), lbl(_lbl), num(_num), initState(_initState)
{
  locked = false;
	labelsVisible = true;
}

void Node::OnPaint(wxDC * ptrDC) 
{
  wxColour color;
  if (!locked)
    color = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
  else 
    color = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);

    //Circle
		if (initState) {
			wxPen pen = wxPen(borderColorInit, 2, wxSOLID);
			ptrDC->SetPen(pen);
		}
		else {
			wxPen pen = wxPen(borderColor, 1, wxSOLID);
			ptrDC->SetPen(pen);
		}
    wxBrush myBrush(color,wxSOLID );
    ptrDC->SetBrush(myBrush);
    ptrDC->DrawCircle((wxCoord)posX,(wxCoord)posY,radius);

    //Label
		if (labelsVisible) {
			wxFont MyFont(FONT_SIZE, wxSWISS , wxNORMAL, wxNORMAL, false, wxT("Arial"));
			ptrDC->SetFont(MyFont);
	
			double x= posX-POS_NODE_LBL_X;;
			//Adjust label x position according to the length
			switch (lbl.Length()) {
				case 1:  x = posX-POS_NODE_LBL_X;  break;
				case 2:  x = posX-POS_NODE_LBL_X-3;break;
				case 3:  x = posX-POS_NODE_LBL_X-6;break;
				default: x = posX-POS_NODE_LBL_X;  break;
			}
	
			double y=posY-POS_NODE_LBL_Y;
	
			ptrDC->DrawRotatedText(lbl,(int) round(x),(int) round(y),0.0);
		}
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

string Node::Get_lbl() {
	string st_lbl = string(lbl.fn_str());
	return st_lbl;
}

bool Node::IsLocked(){
  return locked;
}

bool Node::IsInitState(){
  return initState;
}

void Node::Lock() {
  locked = true;
}

void Node::Unlock() {
  locked = false;
}

bool Node::LabelVisible() {
	return labelsVisible;
}

void Node::ShowLabels() {
		labelsVisible = true;
} 

void Node::HideLabels() {
		labelsVisible = false;
} 

void Node::SetXY(double _x, double _y) {
  if (!locked) {
    posX = _x;
    posY = _y;
  }
  else 
    wxBell();
}

void Node::ForceSetXY(double _x, double _y) {
    posX = _x;
    posY = _y;
}

void Node::SetRadius(int newRadius) {
	radius = newRadius;
}

