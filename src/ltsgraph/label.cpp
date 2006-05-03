#include "label.h"
#include <string>

#define FONT_SIZE 9

Label::Label(wxString _name, wxPoint _pos) : name(_name), pos(_pos) {}


void Label::OnPaint(wxPaintDC * ptrDC) {


    wxFont MyFont(FONT_SIZE, wxSWISS , wxNORMAL, wxNORMAL, false, wxT("Arial"));
    ptrDC->SetFont(MyFont);
    ptrDC->DrawRotatedText(name,pos.x,pos.y,0);
 
}

void Label::SetXY(wxCoord _x, wxCoord _y) {
	pos.x = _x;
	pos.y = _y;
}






