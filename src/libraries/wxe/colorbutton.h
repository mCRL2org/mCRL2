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
