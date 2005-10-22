#include <wx/wx.h>

/* Include definition of the studio overview window */
#include "studio_overview.h"

/* Unique identification number for the main application window */
#define STUDIO_OVERVIEW 1000

/* Studio class declaration */
class Studio : public wxApp {
  DECLARE_CLASS(Studio)
  
  public:

    Studio();
  
    virtual bool OnInit();
    virtual int  OnExit();
};

IMPLEMENT_APP(Studio)

IMPLEMENT_CLASS(Studio, wxApp)

IMPLEMENT_WX_THEME_SUPPORT

/* Studio class implementation */
Studio::Studio() {
}
  
bool Studio::OnInit() {
  StudioOverview* window = new StudioOverview(NULL, STUDIO_OVERVIEW);
  
  /* Make sure the main window is visible */
  return (window->Show(true));
}
  
int Studio::OnExit() {
  return (wxApp::OnExit());
}


