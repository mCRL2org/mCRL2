//  Copyright 2007 A.j. (Hannes) pretorius. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./colorbutton.cpp

// --- colorbutton.cpp ----------------------------------------------
// (c) 2007  -  S.C.W. Ploeger  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------


#include "colorbutton.h"

BEGIN_EVENT_TABLE( wxColorButton, wxPanel )
  EVT_LEFT_DOWN( wxColorButton::OnMouseDown )
END_EVENT_TABLE()

wxColorButton::wxColorButton( wxWindow* parent, wxWindow* topframe, wxWindowID
    id, const wxPoint& pos, const wxSize& size )
  : wxPanel( parent, id, pos, size, wxSUNKEN_BORDER )
{
  appFrame = topframe;
}

void wxColorButton::OnMouseDown( wxMouseEvent& /*event*/ )
{
  wxColourData coldat;
  coldat.SetColour( GetBackgroundColour() );
  wxColourDialog* coldlg = new wxColourDialog( appFrame, &coldat );
  coldlg->CentreOnParent();
  if ( coldlg->ShowModal() == wxID_OK )
  {
    coldat = coldlg->GetColourData();
    SetBackgroundColour( coldat.GetColour() );
    ClearBackground();
    
    wxCommandEvent cmdEvent( wxEVT_COMMAND_BUTTON_CLICKED, GetId() );
    cmdEvent.SetEventObject( this );
    GetParent()->ProcessEvent( cmdEvent );
  }
  coldlg->Close();
  coldlg->Destroy();
}


// -- end -----------------------------------------------------------
