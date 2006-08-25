#ifndef SQUADT_MAIN_H
#define SQUADT_MAIN_H

#include <map>

#include <wx/wx.h>
#include <wx/menu.h>
#include <wx/frame.h>

#include "gui_project.h"
#include "gui_miscellaneous.h"
#include "tool.h"
#include "core.h"

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
      friend void squadt::GUI::project::spawn_context_menu(project::object_data&);

      private:

        /** \brief The default title for the main window */
        static wxString                                   default_title;

        /** \brief The currently opened project, or 0 */
        squadt::GUI::project*                             project_view;

      private:

        /** \brief Helper function that creates widgets and adds them to the window */
        void build();

        /** \brief Shows a project creation dialog and switches the active view to the new project */
        inline void on_menu_new(wxCommandEvent& /* event */);

        /** \brief Shows a project open dialog and switches the active view to the new project */
        inline void on_menu_open(wxCommandEvent& /* event */);

        /** \brief Closes the active project view */
        inline void on_menu_close(wxCommandEvent& /* event */);

        /** \brief Triggers project update */
        inline void on_menu_update(wxCommandEvent& /* event */);

        /** \brief Closes the active project view */
        inline void on_menu_add_file(wxCommandEvent& /* event */);

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

        /** \brief Updates the active project */
        void project_update();

        /** \brief Shows a project open dialog and switches the active view to the new project */
        void project_close();

        /** \brief Add a file to the project */
        void project_add_file();

        /** \brief Adds a project view and sets widget properties to enable access to project level functionality */
        void add_project_view(project*);

        /** \brief Removes a project view and sets widget properties to disable access to project level functionality */
        void remove_project_view(project*);

        /** \brief Creates, shows and eventually removes the about window */
        void about();

        /** \brief Helper function for setting the menu bar */
        void set_menu_bar();

        /** \brief Cleans up and closes the window */
        inline void quit();

      public:

        /** \brief Tool selection helper that indexes the tools known by the global tool manager */
        static miscellaneous::tool_selection_helper::sptr tool_registry;

      public:

        /** \brief Constructor */
        main();
    };

    inline main::main() : wxFrame(0, wxID_ANY, default_title, wxDefaultPosition, wxDefaultSize), project_view(0) {
      /* Add widgets */
      build();

      /* Build the index (on the tool database) that is used to populate the context menus */
      if (tool_registry.get() == 0) {
        tool_registry = miscellaneous::tool_selection_helper::sptr(new miscellaneous::tool_selection_helper());
      }

      /* Default size is the minimum size */
      SetMinSize(wxDefaultSize);

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

    inline void main::on_menu_add_file(wxCommandEvent&) {
      project_add_file();
    }

    inline void main::on_menu_update(wxCommandEvent&) {
      project_update();
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
      if (project_view != 0) {
        project_view->store();
      }

      global_tool_manager.reset();

      /* Destroy the tool manager first; it will terminate all tools */
      Destroy();
    }
  }
}

#endif
