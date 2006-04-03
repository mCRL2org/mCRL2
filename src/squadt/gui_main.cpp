#include "gui_main.h"
#include "gui_project.h"
#include "gui_dialog_project.h"
#include "tool_manager.h"
#include "core.h"

#include <boost/ref.hpp>

#include <wx/menu.h>
#include <wx/notebook.h>

/* Some custum identifiers for use with event handlers */
#define cmID_START_ANALYSIS    (wxID_HIGHEST + 1)
#define cmID_RESTART_ANALYSIS  (wxID_HIGHEST + 2)
#define cmID_STOP_ANALYSIS     (wxID_HIGHEST + 3)

namespace squadt {
  namespace GUI {

    wxString main::default_title = wxT("deskSQuADT");

    void main::about() {
    }

    void main::build() {
      /* Connect event handlers */
      Connect(wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_new));
      Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_open));
      Connect(wxID_CLOSE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_close));
      Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_about));
      Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_quit));
      Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(main::on_window_close));

      /* Generate and attach the menu bar */
      set_menu_bar();

      CreateStatusBar();

      SetSize(wxSize(800, 600));
    }

    /**
     * Contacts the local tool manager to ask for the current list of tools,
     * and stores this information in the tool information cache (tool_information_cache).
     **/
    void main::build_tool_cache() {
      const tool_manager::tool_list& tools = global_tool_manager->get_tools();

      for (tool_manager::tool_list::const_iterator i = tools.begin(); i != tools.end(); ++i) {
        sip::tool::capabilities c = (*i)->get_capabilities();
      }
    }

    void main::set_menu_bar() {
      wxMenuBar* menu = new wxMenuBar();

      /* File menu */
      wxMenu* file_menu = new wxMenu();

      file_menu->Append(wxID_NEW, wxT("&New...\tCTRL-n"));
      file_menu->Append(wxID_OPEN, wxT("&Open...\tCTRL-o"));
      file_menu->Append(wxID_CLOSE, wxT("&Close\tCTRL-F4"))->Enable(false);
      file_menu->AppendSeparator();
      file_menu->Append(wxID_EXIT, wxT("&Quit"));
      menu->Append(file_menu, wxT("&File"));

      wxMenu* edit_menu  = new wxMenu();

      edit_menu->Append(wxID_PREFERENCES, wxT("&Preferences"));
      menu->Append(edit_menu, wxT("&Edit"));

      wxMenu* analysis_menu  = new wxMenu();

      analysis_menu->Append(cmID_START_ANALYSIS, wxT("&Start\tCTRL-s"), wxT("Updates all outputs to complete the analysis"))->Enable(false);
      analysis_menu->Append(cmID_RESTART_ANALYSIS, wxT("&Restart\tCTRL-r"), wxT("Starts analysis from scratch"))->Enable(false);
      analysis_menu->Append(cmID_STOP_ANALYSIS, wxT("S&top"), wxT("Cancels generation of unfinished outputs"))->Enable(false);

      menu->Append(analysis_menu, wxT("&Analysis"));

      wxMenu* help_menu  = new wxMenu();

      help_menu->Append(wxID_HELP, wxT("&User Manual"));
      help_menu->AppendSeparator();
      help_menu->Append(wxID_ABOUT, wxT("&About"));
      menu->Append(help_menu, wxT("&Help"));

      SetMenuBar(menu);
    }

    void main::project_new() {
      using namespace boost::filesystem;

      dialog::new_project dialog(this);

      if (dialog.ShowModal()) {
        /* Create the new project */
        project* p = new GUI::project(boost::cref(path(dialog.get_location())));
         
        enable_project_level_functionality(p);
      }
    }

    void main::project_open() {
      using namespace boost::filesystem;

      dialog::open_project dialog(this);

      if (dialog.ShowModal()) {
        project* p = new GUI::project(boost::cref(path(dialog.get_location())));

        enable_project_level_functionality(p);
      }
    }

    /**
     * Closes the currently active project and switches to another project if
     * it exists and otherwise disables the project close menu option.
     **/
    void main::project_close() {
      disable_project_level_functionality();
    }

    /**
     * @param p a pointer to the project object for which this is requested
     **/
    void main::enable_project_level_functionality(project* p) {

      /* Adjust title */
      SetTitle(default_title + wxT(" - ") + p->get_name());

      /* Disable or enable controls, if appropriate */
      wxMenuBar& menu_bar = *GetMenuBar();

      menu_bar.Enable(wxID_NEW, false);
      menu_bar.Enable(wxID_OPEN, false);
      menu_bar.Enable(wxID_CLOSE, true);
      menu_bar.Enable(cmID_START_ANALYSIS, true);
      menu_bar.Enable(cmID_RESTART_ANALYSIS, true);
      menu_bar.Enable(cmID_STOP_ANALYSIS, true);
    }

    void main::disable_project_level_functionality() {

      /* Adjust title */
      SetTitle(default_title);

      /* Disable or enable controls, if appropriate */
      wxMenuBar& menu_bar = *GetMenuBar();

      menu_bar.Enable(wxID_NEW, true);
      menu_bar.Enable(wxID_OPEN, true);
      menu_bar.Enable(wxID_CLOSE, false);
      menu_bar.Enable(cmID_START_ANALYSIS, false);
      menu_bar.Enable(cmID_RESTART_ANALYSIS, false);
      menu_bar.Enable(cmID_STOP_ANALYSIS, false);
    }
  }
}

