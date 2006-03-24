#include <fstream>

#include <boost/shared_ptr.hpp>

#include "tool_manager.h"
#include "settings_manager.tcc"
#include "core.h"
#include "gui_splash.h"

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/image.h>

/* Include definition of the project overview window */
#include "gui_project_overview.h"

namespace squadt {

  /* Global Settings Manager component */
  boost::shared_ptr < settings_manager > global_settings_manager;

  /* Global Tool Manager component */
  boost::shared_ptr < tool_manager >     global_tool_manager;
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

  global_settings_manager = settings_manager::ptr(new settings_manager(wxFileName::GetHomeDir().fn_str()));
  global_tool_manager     = tool_manager::read();

  wxInitAllImageHandlers();

  wxImage logo(wxString(global_settings_manager->path_to_images("logo.jpg").c_str(), wxConvLocal));

  new splash(&logo);

  wxYield();

  global_tool_manager->query_tools();

  /* Load tool configuration from storage */
//  ProjectOverview* window = new ProjectOverview(NULL, 1000);

  /* Show a splash */
//  wxBitmap        splash_image;
//  wxSplashScreen* splash;

//  if (splash_image.LoadFile(logo, wxBITMAP_TYPE_JPEG)) {
//    splash = new wxSplashScreen(splash_image, wxSPLASH_CENTRE_ON_PARENT|wxSPLASH_TIMEOUT, 600, window, wxID_ANY);
//  }

  /* Make sure the main window is visible */
//  window->Show(true);

//  window->GenerateToolContextMenus();

  return (true);
}

int Squadt::OnExit() {
  return (wxApp::OnExit());
}

