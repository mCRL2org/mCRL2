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
  if ( coldlg->ShowModal() == wxID_OK )
  {
    coldat = coldlg->GetColourData();
    SetBackgroundColour( coldat.GetColour() );
    ClearBackground();
    
    wxCommandEvent cmdEvent( wxEVT_COMMAND_BUTTON_CLICKED, GetId() );
    cmdEvent.SetEventObject( this );
    GetParent()->ProcessEvent( cmdEvent );
  }
}
