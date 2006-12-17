#include "main.h"
#include "about.h"
#include "project.h"
#include "resources.h"
#include "dialog/project_settings.h"
#include "dialog/preferences.h"
#include "../tool_manager.h"

#include <wx/menu.h>

/* Some custom identifiers for use with event handlers */
#define cmID_UPDATE      (wxID_HIGHEST)
#define cmID_PREFERENCES (wxID_HIGHEST + 1)

namespace squadt {
  namespace GUI {

    wxString main::default_title = wxT("deskSQuADT");

    void main::about() {
      squadt::GUI::about about_dialog(this);
      
      about_dialog.ShowModal();
    }

    main::main() : wxFrame(0, wxID_ANY, default_title, wxDefaultPosition, wxDefaultSize),
                                       project_view(0) {

      /* Add widgets */
      build();

      /* Default size is the minimum size */
      SetMinSize(wxDefaultSize);

      /* Reposition the window */
      CentreOnScreen();

      /* Now show the window in all its marvel */
      Show(true);
    }

    void main::build() {
      /* Connect event handlers */
      Connect(wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_new));
      Connect(wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_open));
      Connect(cmID_UPDATE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_update));
      Connect(wxID_ADD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_add_file));
      Connect(wxID_CLOSE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_close));
      Connect(wxID_PREFERENCES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_preferences));
      Connect(cmID_PREFERENCES, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_preferences));
      Connect(wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_about));
      Connect(wxID_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(main::on_menu_quit));
      Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(main::on_window_close));

      /* Load main icons */
      main_icon_list = load_main_icons();

      SetIcon(*gui_icon);

      /* Load default icons for file formats */
      format_icon_list = load_format_icons();

      /* Load default small icons for file formats */
      format_small_icon_list = load_small_format_icons();

      /* Generate and attach the menu bar */
      set_menu_bar();

      CreateStatusBar();

      SetSizer(new wxBoxSizer(wxVERTICAL));

      SetSize(wxSize(1024, 768));

      Raise();
    }

    void main::set_menu_bar() {
      wxMenuBar* menu = new wxMenuBar();

      /* File menu */
      wxMenu* project_menu = new wxMenu();

      project_menu->Append(wxID_NEW, wxT("&New...\tCTRL-n"));
      project_menu->Append(wxID_OPEN, wxT("&Open...\tCTRL-o"));
      project_menu->Append(cmID_UPDATE, wxT("&Update...\tCTRL-f"))->Enable(false);
      project_menu->Append(wxID_CLOSE, wxT("&Close\tCTRL-F4"))->Enable(false);
      project_menu->AppendSeparator();
      project_menu->Append(wxID_ADD, wxT("&Add file...\tCTRL-f"))->Enable(false);
      project_menu->AppendSeparator();
      project_menu->Append(wxID_EXIT, wxT("&Quit"));
      menu->Append(project_menu, wxT("&Project"));

      wxMenu* edit_menu  = new wxMenu();

      edit_menu->Append(wxID_PREFERENCES, wxT("&Preferences"));
#ifdef __WXMAC__
      edit_menu->Append(cmID_PREFERENCES, wxT("&Preferences"));
#endif
      menu->Append(edit_menu, wxT("&Edit"));

      wxMenu* help_menu  = new wxMenu();

      help_menu->Append(wxID_HELP, wxT("&User Manual"));
      help_menu->AppendSeparator();
      help_menu->Append(wxID_ABOUT, wxT("&About"));

      menu->Append(help_menu, wxT("&Help"));

      SetMenuBar(menu);
    }

    void main::project_new() {

      dialog::new_project dialog(this);

      switch (dialog.ShowModal()) {
        case 1:
          /* Create the new project */
          project_new(dialog.get_location(), dialog.get_description());
          break;
        case 2:
          project_open(dialog.get_location());
          break;
        default: /* Cancelled by user */
          break;
      }
    }

    /**
     * @param[in] s a path to a project store
     * @param[in] d an optional description
     **/
    void main::project_new(std::string const& s, std::string const& d) {
      add_project_view(new GUI::project(this, boost::filesystem::path(s), d, true));
    }

    void main::project_open() {
      dialog::open_project dialog(this);

      if (dialog.ShowModal() == wxOK) {
        project_open(dialog.get_location());
      }
    }

    /**
     * @param[in] s a path to a project store
     **/
    void main::project_open(std::string const& s) {
      try {
        add_project_view(new GUI::project(this, boost::filesystem::path(s)));
      }
      catch (...) {
        wxMessageDialog(0, wxT("Unable to load project possible reasons are corruption or an incompatible description file."),
                                  wxT("Fatal: project file corrupt"), wxOK).ShowModal();
      }
    }

    void main::project_add_file() {
      project_view->add();
    }

    /**
     * Closes the currently active project and switches to another project if
     * it exists and otherwise disables the project close menu option.
     **/
    void main::project_close() {
      remove_project_view(project_view);
    }

    void main::project_update() {
      project_view->update();
    }

    /**
     * @param p a pointer to the project object for which this is requested
     **/
    void main::add_project_view(project* p) {

      /* Only one project view supported at this time ... */
      assert(project_view == 0);

      project_view = p;

      GetSizer()->Add(project_view, 1, wxEXPAND);

      Layout();

      p->load_views();

      /* Adjust title */
      SetTitle(default_title + wxT(" - ") + p->get_name());

      /* Disable or enable controls, if appropriate */
      wxMenuBar& menu_bar = *GetMenuBar();

      menu_bar.Enable(wxID_NEW, false);
      menu_bar.Enable(wxID_OPEN, false);
      menu_bar.Enable(cmID_UPDATE, true);
      menu_bar.Enable(wxID_CLOSE, true);
      menu_bar.Enable(wxID_ADD, true);
    }

    /**
     * @param p a pointer to the project object for which this is requested
     *
     * \pre project_view is not 0
     **/
    void main::remove_project_view(project* p) {

      assert(project_view != 0);

      GetSizer()->Detach(project_view);
      GetSizer()->RecalcSizes();

      Layout();

      project_view->Destroy();
      project_view = 0;

      /* Adjust title */
      SetTitle(default_title);

      /* Disable or enable controls, if appropriate */
      wxMenuBar& menu_bar = *GetMenuBar();

      menu_bar.Enable(wxID_NEW, true);
      menu_bar.Enable(wxID_OPEN, true);
      menu_bar.Enable(cmID_UPDATE, false);
      menu_bar.Enable(wxID_CLOSE, false);
      menu_bar.Enable(wxID_ADD, false);
    }

    void main::show_preferences() {
      GUI::preferences dialog(this);

      dialog.ShowModal();
    }

    void main::quit() {
      if (project_view != 0) {
        project_view->store();
      }

      global_build_system.shutdown();

      Destroy();
    }
  }
}

