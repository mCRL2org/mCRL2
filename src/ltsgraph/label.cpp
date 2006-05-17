#include "label.h"
#include <string>

#define FONT_SIZE 9

Label::Label(wxString _name, double _posX, double _posY) : name(_name), posX(_posX), posY(_posY) {}


void Label::OnPaint(wxPaintDC * ptrDC) {


    wxFont MyFont(FONT_SIZE, wxSWISS , wxNORMAL, wxNORMAL, false, wxT("Arial"));
    ptrDC->SetFont(MyFont);
    ptrDC->DrawRotatedText(name,(int) round(posX),(int) round(posY),0);
 
}

void Label::SetXY(double _posX, double _posY) {
	posX = _posX;
	posY = _posY;
}






