// Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// LICENSE_1_0.txt)
//
/// \file ./node.cpp

#include "node.h"
#include "workarounds.h"
#include "images/lock.xpm"

static const wxColour &border_color      = *wxBLACK;
static const wxColour &border_color_init = *wxRED;

Node::Node(unsigned int _num, double _posX, double _posY, wxString const& _lbl, bool _initState) : 
             posX(_posX), posY(_posY), initState(_initState), lbl(_lbl), num(_num)
{
  locked = false;
  labelsVisible = true;
  node_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
  radius = CIRCLE_RADIUS;

  border_colour = (initState) ? border_color_init : border_color;
}

void Node::OnPaint(wxDC * ptrDC) 
{
  //Circle
  int pen_size = (initState) ? 2 : 1;
		
  wxPen pen = wxPen(border_colour, pen_size, wxSOLID);
  ptrDC->SetPen(pen);
  wxBrush myBrush(node_colour,wxSOLID );
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

  if (locked) {
    static const wxBitmap lock(lock_xpm);
    ptrDC->DrawBitmap(lock, (int)posX - radius, (int)posY - radius, true);
  }
}

void Node::set_node_colour(wxColour colour) {
  node_colour = colour;
}

void Node::set_border_colour(wxColour colour) {
  border_colour = colour;
}

wxColour Node::get_node_colour() {
  return node_colour;
}

wxColour Node::get_border_colour() {
  return border_colour;
}

void Node::reset_border_colour() {
  border_colour = (initState) ? border_color_init : border_color;
}
