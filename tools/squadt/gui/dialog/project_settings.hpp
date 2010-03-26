// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/dialog/project_settings.h
/// \brief Add your file description here.

#ifndef GUI_DIALOG_PROJECT_H
#define GUI_DIALOG_PROJECT_H

#include <string>

#include "base.hpp"

namespace squadt {
  namespace GUI {
    namespace dialog {

      /** \brief Base class for project level dialogs */
      class project : public dialog::basic {

        protected:

          /** \brief The directory from which the directory selectors start */
          static wxString default_directory;

        protected:

          /** \brief Determines wether a directory is recognised as a project store */
          static bool is_project_directory(wxString);

          /** \brief Determines wether a directory is recognised as a project store */
          static wxString get_project_description(wxString);

        public:

          /** \brief Constructor */
          project(wxWindow*, wxString);

          /** \brief Gets the name of the project */
          virtual std::string get_name() const = 0;

          /** \brief Destructor */
          virtual ~project() = 0;
      };

      /** \brief Creates a dialog window to gather information about an existing project */
      class add_to_project : public project {

        private:

          /** \brief Control to select a project store */
          wxGenericDirCtrl* file_selector;

          /** \brief Text field that contains the name of the new project */
          wxTextCtrl*       name;

          /** \brief Warning that a file with the same name is part of the project */
          wxSizer*          cannot_add;

        private:

          /** \brief Helper function that places the widgets */
          void build(std::string const&);

          /** \brief Handler for the event that one of the buttons is clicked */
          void on_button_clicked(wxCommandEvent&);

          /** \brief Handler for the event that a directory is selected */
          void on_selection_changed(wxTreeEvent&);

          /** \brief Handler for the event that the name changes */
          void on_name_updated(wxCommandEvent&);

        public:

          /** \brief Constructor */
          add_to_project(wxWindow*, std::string const&);

          /** \brief Gets the name of the project */
          std::string get_name() const;

          /** \brief Gets the name of the project */
          std::string get_source() const;

          /** \brief Gets the name of the project */
          std::string get_destination() const;

          /** \brief Destructor */
          ~add_to_project();
      };
    }
  }
}

#endif

