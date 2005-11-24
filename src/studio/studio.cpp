#include <wx/wx.h>
#include <wx/splash.h>
#include <wx/filename.h>

/* Include definition of the project overview window */
#include "gui_project_overview.h"

#include "tool_executor.h"
#include "tool_manager.h"
#include "settings_manager.h"
#include "logger.h"

unsigned int svn_revision = UINT_MAX;

/* Global Settings Manager component */
SettingsManager  settings_manager(wxFileName::GetHomeDir().fn_str());

/* Global Tool Manager component */
ToolManager      tool_manager;

/* Global Tool Executor component (TODO run a single instance per machine) */
ToolExecutor     tool_executor;

Logger           logger;

/* Studio class declaration */
class Studio : public wxApp {
  DECLARE_CLASS(Studio)
  
  private:

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
  /* Load tool configuration from storage */
  ProjectOverview* window = new ProjectOverview(tool_manager, NULL, 1000);

  /* Show a splash */
  wxBitmap        splash_image;
  wxSplashScreen* splash;
  
  if (splash_image.LoadFile(wxT("pixmaps/TUElogo.xpm"), wxBITMAP_TYPE_XPM)) {
    splash = new wxSplashScreen(splash_image, wxSPLASH_CENTRE_ON_PARENT|wxSPLASH_TIMEOUT, 600, window, wxID_ANY);
  }

  /* Make sure the main window is visible */
  window->Show(true);

  tool_manager.Load();

  return (true);
}
  
int Studio::OnExit() {
  return (wxApp::OnExit());
}


