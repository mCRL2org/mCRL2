#ifndef SQUADT_MAIN_H
#define SQUADT_MAIN_H

#include <wx/wx.h>
#include <wx/frame.h>

class wxMenu;

namespace squadt {
  namespace GUI {

    class project;

    /**
     * \brief The main window that contains the project views are placed
     *
     * For the time being there can be only one active project. But all
     * components are constructed with modularity in mind. The idea is that if
     * desired, somewhere in the future, simultaneous multiple project views
     * can be added with little effort.
     **/
    class main : public wxFrame {

      private:

        /** \brief The default title for the main window */
        static wxString default_title;

      private:

        /** \brief Helper function that creates widgets and adds them to the window */
        void build();

        /** \brief Helper function that gathers the tool information that is used to on-the-fly build context menus */
        void build_tool_cache();

        /** \brief Shows a project creation dialog and switches the active view to the new project */
        inline void on_menu_new(wxCommandEvent& /* event */);

        /** \brief Shows a project open dialog and switches the active view to the new project */
        inline void on_menu_open(wxCommandEvent& /* event */);

        /** \brief Closes the active project view */
        inline void on_menu_close(wxCommandEvent& /* event */);

        /** \brief Creates, shows and eventually removes the about window */
        inline void on_menu_about(wxCommandEvent&);

        /** \brief Cleans up and closes the window */
        inline void on_menu_quit(wxCommandEvent&);

        /** \brief Cleans up and closes the window */
        inline void on_window_close(wxCloseEvent&);

        /** \brief Shows a project creation dialog and switches the active view to the new project */
        void project_new();

        /** \brief Closes the active project view */
        void project_open();

        /** \brief Shows a project open dialog and switches the active view to the new project */
        void project_close();

        /** \brief Creates, shows and eventually removes the about window */
        void about();

        /** \brief Helper function for setting the menu bar */
        void set_menu_bar();

        /** \brief Cleans up and closes the window */
        inline void quit();

      public:

        /** \brief Constructor */
        main();
    };

    inline main::main() : wxFrame(0, wxID_ANY, default_title, wxDefaultPosition, wxDefaultSize) {
      build();

      /* Reposition the window */
      CentreOnScreen();

      /* Now show the window in all its marvel */
      Show(true);
    }

    inline void main::on_menu_new(wxCommandEvent&) {
      project_new();
    }

    inline void main::on_menu_open(wxCommandEvent&) {
      project_open();
    }

    inline void main::on_menu_close(wxCommandEvent&) {
      project_close();
    }

    inline void main::on_menu_about(wxCommandEvent&) {
      about();
    }

    inline void main::on_menu_quit(wxCommandEvent&) {
      quit();
    }

    inline void main::on_window_close(wxCloseEvent&) {
      quit();
    }

    inline void main::quit() {
      Destroy();
    }
  }
}

#endif
