#include <wx/wx.h>
#include <wx/filename.h>

/* Include definition of the studio overview window */
#include "studio_overview.h"

#include "tool_manager.h"
#include "settings_manager.h"

/* Global settings manager component */
SettingsManager* settings;

/* Studio class declaration */
class Studio : public wxApp {
  DECLARE_CLASS(Studio)
  
  private:
    ToolManager     tool_manager;

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


  /* Export the settings_manager */
  settings = new SettingsManager(wxFileName::GetHomeDir().fn_str());

  /* Load tool configuration from storage */
  tool_manager.Load();

  StudioOverview* window = new StudioOverview(tool_manager, NULL, 1000);

  /* Make sure the main window is visible */
  return (window->Show(true));
}
  
int Studio::OnExit() {
  delete settings;

  return (wxApp::OnExit());
}


