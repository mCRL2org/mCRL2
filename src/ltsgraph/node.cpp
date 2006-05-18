#include "node.h"

Node::Node(unsigned int _num, double _posX, double _posY, wxString _lbl) : 
             posX(_posX), posY(_posY), lbl(_lbl), num(_num)
{
  locked = false;
}

void Node::OnPaint(wxPaintDC * ptrDC) 
{
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

    wxFont MyFont(FONT_SIZE, wxSWISS , wxNORMAL, wxNORMAL, false, wxT("Arial"));
    ptrDC->SetFont(MyFont);

    double x=posX+POS_NODE_LBL_X;
    double y=posY+POS_NODE_LBL_Y;
    ptrDC->DrawRotatedText(lbl,(int) round(x),(int) round(y),0.0);
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
    // Change position of label; Not necessary, ought to
    // be automatic.
    // lbl->SetXY(posX-POS_LBL_X, posY-POS_LBL_Y);
  }
  else 
    wxBell();
}







