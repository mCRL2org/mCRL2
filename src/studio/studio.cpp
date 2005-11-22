#include <wx/wx.h>
#include <wx/filename.h>

/* Include definition of the project overview window */
#include "gui_project_overview.h"

#include "tool_executor.h"
#include "tool_manager.h"
#include "settings_manager.h"

unsigned int svn_revision = UINT_MAX;

/* Global Settings Manager component */
SettingsManager  settings_manager(wxFileName::GetHomeDir().fn_str());

/* Global Tool Manager component */
ToolManager      tool_manager;

/* Global Tool Executor component (TODO run a single instance per machine) */
ToolExecutor     tool_executor;

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
  /* TODO show a splash here */

  /* Load tool configuration from storage */
  tool_manager.Load();

  ProjectOverview* window = new ProjectOverview(tool_manager, NULL, 1000);

  /* Make sure the main window is visible */
  window->Show(true);

  return (true);
}
  
int Studio::OnExit() {
  return (wxApp::OnExit());
}


