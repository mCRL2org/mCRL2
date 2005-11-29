#include <wx/wx.h>
#include <wx/splash.h>
#include <wx/filename.h>
#include <wx/image.h>

#include <fstream>

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

std::ofstream    log_stream(settings_manager.GetLogFileName().c_str(), std::ios::app);

Logger*          logger;

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

/* Studio class implementation */
Studio::Studio() {
}

bool Studio::OnInit() {
  wxInitAllImageHandlers();

  logger = new Logger((log_stream) ? log_stream : std::cerr);

  /* Load tool configuration from storage */
  ProjectOverview* window = new ProjectOverview(NULL, 1000);

  /* Show a splash */
  wxBitmap        splash_image;
  wxSplashScreen* splash;
  
  if (splash_image.LoadFile(wxT("pixmaps/TUElogo.xpm"), wxBITMAP_TYPE_XPM)) {
    splash = new wxSplashScreen(splash_image, wxSPLASH_CENTRE_ON_PARENT|wxSPLASH_TIMEOUT, 600, window, wxID_ANY);
  }

  /* Make sure the main window is visible */
  window->Show(true);

  tool_manager.Load();

  window->GenerateToolContextMenus();

  /* Connect log display to logger */
  if (window->GetLogDisplay()) {
    logger->SetLogWindow(window->GetLogDisplay());
  }

  return (true);
}
  
int Studio::OnExit() {
  return (wxApp::OnExit());
}


