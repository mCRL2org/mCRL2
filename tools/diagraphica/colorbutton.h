//  Copyright 2007 A.j. (Hannes) pretorius. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./colorbutton.h

// --- colorbutton.h ------------------------------------------------
// (c) 2007  -  S.C.W. Ploeger  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#ifndef COLORBUTTON_H
#define COLORBUTTON_H
#include <wx/colordlg.h>
#include <wx/wx.h>

class wxColorButton : public wxPanel
{
  public:
    wxColorButton( wxWindow* parent, wxWindow* topframe, wxWindowID id = -1,
	const wxPoint& pos = wxDefaultPosition, const wxSize& size =
	wxDefaultSize );
    void OnMouseDown( wxMouseEvent& event );

  private:
    wxWindow* appFrame;
  DECLARE_EVENT_TABLE()
};

#endif


// -- end -----------------------------------------------------------
