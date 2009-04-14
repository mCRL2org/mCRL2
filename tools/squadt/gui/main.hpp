// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/main.h
/// \brief Add your file description here.

#ifndef SQUADT_MAIN_H
#define SQUADT_MAIN_H

#include "project.hpp"

#include <wx/wx.h>
#include <wx/frame.h>

namespace squadt {
  namespace GUI {

    /**
     * \brief The main window that contains the project views are placed
     *
     * For the time being there can be only one active project. But all
     * components are constructed with modularity in mind. The idea is that if
     * desired, somewhere in the future, simultaneous multiple project views
     * can be added with little effort.
     **/
    class main : public wxFrame {
      friend void squadt::GUI::project::spawn_context_menu(project::tool_data&);

      private:

        /** \brief The default title for the main window */
        static wxString                                   default_title;

        /** \brief The currently opened project, or 0 */
        squadt::GUI::project*                             project_view;

      private:

        /** \brief Helper function that creates widgets and adds them to the window */
        void build();

        /** \brief Shows a project creation dialog and switches the active view to the new project */
        void on_menu_new(wxCommandEvent&);

        /** \brief Shows a project open dialog and switches the active view to the new project */
        void on_menu_open(wxCommandEvent&);

        /** \brief Closes the active project view */
        void on_menu_close(wxCommandEvent&);

        /** \brief Triggers project update */
        void on_menu_update(wxCommandEvent&);

        /** \brief Closes the active project view */
        void on_menu_add_existing_file(wxCommandEvent&);

        /** \brief Closes the active project view */
        void on_menu_add_new_file(wxCommandEvent&);

        /** \brief Cleans up and closes the window */
        void on_menu_quit(wxCommandEvent&);

        /** \brief Shows the preferences dialog */
        void on_menu_preferences(wxCommandEvent&);

        /** \brief Cleans up and closes the window */
        void on_window_close(wxCloseEvent&);

      public:

        /** \brief Shows a project creation dialog and switches the active view to the new project */
        void project_new();

        /** \brief Open a new project view */
        void project_open();

        /** \brief Creates a new project view */
        void project_new(std::string const&, std::string const& = "");

        /** \brief Open a new project view from a path */
        void project_open(std::string const&);

        /** \brief Updates the active project */
        void project_update();

        /** \brief Shows a project open dialog and switches the active view to the new project */
        void project_close();

        /** \brief Add a file to the project */
        void project_add_existing_file();

        /** \brief Add a new file to the project */
        void project_add_new_file();

        /** \brief Adds a project view and sets widget properties to enable access to project level functionality */
        void add_project_view(project*);

        /** \brief Removes a project view and sets widget properties to disable access to project level functionality */
        void remove_project_view(project*);

        /** \brief Creates and shows the preferences window */
        void show_preferences();

        /** \brief Creates, shows and eventually removes the about window */
        void about();

        /** \brief Creates, shows and eventually removes the help window */
        void manual();

        /** \brief Helper function for setting the menu bar */
        void set_menu_bar();

        /** \brief Cleans up and closes the window */
        void quit();

        /** \brief Constructor */
        main();
    };

    inline void main::on_menu_new(wxCommandEvent&) {
      project_new();
    }

    inline void main::on_menu_open(wxCommandEvent&) {
      project_open();
    }

    inline void main::on_menu_add_existing_file(wxCommandEvent&) {
      project_add_existing_file();
    }

    inline void main::on_menu_add_new_file(wxCommandEvent&) {
      project_add_new_file();
    }

    inline void main::on_menu_update(wxCommandEvent&) {
      project_update();
    }

    inline void main::on_menu_close(wxCommandEvent&) {
      project_view->store();

      project_close();
    }

    inline void main::on_menu_quit(wxCommandEvent&) {
      quit();
    }

    inline void main::on_menu_preferences(wxCommandEvent&) {
      show_preferences();
    }

    inline void main::on_window_close(wxCloseEvent&) {
      quit();
    }
  }
}

#endif
