#include <wx/wx.h>
#include <wx/splash.h>
#include <wx/filename.h>
#include <wx/image.h>

#include <fstream>

/* Include definition of the project overview window */
#include "gui_project_overview.h"

#include "tool_manager.h"
#include "settings_manager.tcc"
#include "logger.h"

namespace squadt {

  /* Global Settings Manager component */
  settings_manager _settings_manager(static_cast < const char* > (wxFileName::GetHomeDir().fn_str()));

  /* Global Tool Manager component */
  ToolManager      tool_manager;
 
  std::ofstream    log_stream(_settings_manager.path_to_system_settings("squadt.log").c_str(), std::ios::app);
 
  Logger*          logger;
 
  namespace GUI {

    /* Squadt class declaration */
    class Squadt : public wxApp {
      private:
 
      public:
 
        Squadt();
    
        virtual bool OnInit();
        virtual int  OnExit();
    };
 
    IMPLEMENT_APP(Squadt)
 
    /* Squadt class implementation */
    Squadt::Squadt() {
    }
 
    bool Squadt::OnInit() {
      wxInitAllImageHandlers();

      logger = new Logger((log_stream.good()) ? log_stream : std::cerr);
 
      /* Load tool configuration from storage */
      ProjectOverview* window = new ProjectOverview(NULL, 1000);
 
      /* Show a splash */
      wxBitmap        splash_image;
      wxSplashScreen* splash;
      wxString        logo(_settings_manager.path_to_images("logo.jpg").c_str(), wxConvLocal);
 
      if (splash_image.LoadFile(logo, wxBITMAP_TYPE_JPEG)) {
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
    
    int Squadt::OnExit() {
      return (wxApp::OnExit());
    }
  }
}

IMPLEMENT_APP(squadt::GUI::Squadt)
 
