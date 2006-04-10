#include <fstream>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "tool_manager.h"
#include "settings_manager.tcc"
#include "core.h"

#include "gui_splash.h"
#include "gui_main.h"

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/thread.h>

namespace squadt {

  /* Global Settings Manager component */
  boost::shared_ptr < settings_manager > global_settings_manager;

  /* Global Tool Manager component */
  boost::shared_ptr < tool_manager >     global_tool_manager;
}

using namespace squadt::GUI;

class initialisation : public wxThread {
  private:
    splash* splash_window;

  public:
    initialisation(splash*);

    void* Entry();
};

initialisation::initialisation(splash* s) : wxThread(wxTHREAD_JOINABLE), splash_window(s) {
  Create();
  Run();
}

void* initialisation::Entry() {
  squadt::global_tool_manager->query_tools(
                  boost::bind(&splash::set_operation, splash_window, std::string("processing"), _1));

  return (0);
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

  wxInitAllImageHandlers();

  wxImage logo(wxString(global_settings_manager->path_to_images("logo.jpg").c_str(), wxConvLocal));

  splash* splash_window = new splash(&logo, 3);

  splash_window->set_category("Loading components");

  global_tool_manager = tool_manager::read();

  splash_window->set_category("Querying tools", global_tool_manager->number_of_tools());

  /* Perform initialisation */
  initialisation ti(splash_window);

  /* Cannot just wait because the splash would not be updated */
  while (ti.IsAlive()) {
    splash_window->update();

    wxYield();
  }

  splash_window->set_category("Initialising components");

  /* Disable splash */
  splash_window->set_done();

  /* Initialise main application window */
  new squadt::GUI::main();

  return (true);
}

int Squadt::OnExit() {
  return (wxApp::OnExit());
}

