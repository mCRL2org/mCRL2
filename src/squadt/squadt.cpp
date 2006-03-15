#include <fstream>

#include <boost/shared_ptr.hpp>

#include <wx/wx.h>
#include <wx/splash.h>
#include <wx/filename.h>
#include <wx/image.h>

/* Include definition of the project overview window */
#include "gui_project_overview.h"

#include "tool_manager.h"
#include "settings_manager.tcc"
#include "ui_core.h"

namespace squadt {

  /* Global Settings Manager component */
  settings_manager_ptr global_settings_manager;

  /* Global Tool Manager component */
  ToolManager          tool_manager;
 
  std::ofstream        log_stream;
 
}

/* Squadt class declaration */
class Squadt : public wxApp {
  public:

    virtual bool OnInit();
    virtual int  OnExit();
};

IMPLEMENT_APP(Squadt)

/* Squadt class implementation */
bool Squadt::OnInit() {
  using namespace squadt;
  using namespace squadt::GUI;

  global_settings_manager = settings_manager_ptr(new settings_manager(wxFileName::GetHomeDir().fn_str()));

  wxInitAllImageHandlers();

  log_stream.open(global_settings_manager->path_to_system_settings("squadt.log").c_str(), std::ios::app);

  /* Load tool configuration from storage */
  ProjectOverview* window = new ProjectOverview(NULL, 1000);

  /* Show a splash */
  wxBitmap        splash_image;
  wxSplashScreen* splash;
  wxString        logo(global_settings_manager->path_to_images("logo.jpg").c_str(), wxConvLocal);

  if (splash_image.LoadFile(logo, wxBITMAP_TYPE_JPEG)) {
    splash = new wxSplashScreen(splash_image, wxSPLASH_CENTRE_ON_PARENT|wxSPLASH_TIMEOUT, 600, window, wxID_ANY);
  }

  /* Make sure the main window is visible */
  window->Show(true);

  tool_manager.Load();

  window->GenerateToolContextMenus();

  return (true);
}

int Squadt::OnExit() {
  return (wxApp::OnExit());
}

