#ifndef GUI_DIALOG_PROJECT_H
#define GUI_DIALOG_PROJECT_H

#include <string>

#include "base.h"

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

      /**
       * \brief Creates a dialog window to gather information about a new project
       *
       * The return code from EndModal is:
       *  - 0 in case the new project operation was cancelled
       *  - 1 in case the new project operation was successful
       *  - 2 in case another project was chosen to be opened instead
       **/
      class new_project : public project {

        private:

          /** \brief Text field that contains the name of the new project */
          wxTextCtrl*       location;

          /** \brief Text field that contains a description for the project */
          wxTextCtrl*       description;

          /** \brief Shows the first screen of widgets (before okay is clicked) */
          wxBoxSizer*       screen0;

          /** \brief Shows a message that a project with the same name was found and a button to open it */
          wxBoxSizer*       open_project_instead;

        private:

          /** \brief Helper function that places the widgets */
          void build();

          /** \brief Handler for the event that one of the buttons is clicked */
          void on_button_clicked(wxCommandEvent&);

          /** \brief Handler for the event that the name or description are updated */
          void on_text_updated(wxCommandEvent&);

        public:

          /** \brief Constructor */
          new_project(wxWindow*);

          /** \brief Gets the name of the project */
          std::string get_name() const;

          /** \brief Gets the name of the project */
          std::string get_location() const;

          /** \brief Gets the name of the project */
          std::string get_description() const;

          /** \brief Destructor */
          ~new_project();
      };

      /** \brief Creates a dialog window to gather information about an existing project */
      class open_project : public project {

        private:

          /** \brief Control to select a project store */
          wxGenericDirCtrl* directory_selector;

          /** \brief Text field that contains a description for the project */
          wxStaticText*     description;

          /** \brief Whether the currently selected directory is a valid project store */
          bool              selection_is_valid;

          /** \brief Shows a warning that the currently selected directory is not a project store */
          wxBoxSizer*       not_store;

          /** \brief Shows a text field with the description of the current project */
          wxStaticBoxSizer* project_description;

        private:

          /** \brief Helper function that places the widgets */
          void build();

          /** \brief Handler for the event that one of the buttons is clicked */
          void on_button_clicked(wxCommandEvent&);

          /** \brief Handler for the event that a directory is selected */
          void on_selection_changed(wxTreeEvent&);

        public:

          /** \brief Constructor */
          open_project(wxWindow*);

          /** \brief Gets the name of the project */
          std::string get_name() const;

          /** \brief Gets the name of the project */
          std::string get_location() const;

          /** \brief Gets the name of the project */
          std::string get_description() const;

          /** \brief Destructor */
          ~open_project();
      };

      /** \brief Creates a dialog window to gather information about an existing project */
      class add_to_project : public project {

        private:

          /** \brief Control to select a project store */
          wxGenericDirCtrl* file_selector;

          /** \brief Text field that contains the name of the new project */
          wxTextCtrl*       name;

          /** \brief Path to the project store */
          wxString          project_store;

        private:

          /** \brief Helper function that places the widgets */
          void build();

          /** \brief Handler for the event that one of the buttons is clicked */
          void on_button_clicked(wxCommandEvent&);

          /** \brief Handler for the event that a directory is selected */
          void on_selection_changed(wxTreeEvent&);

          /** \brief Handler for the event that the name changes */
          void on_name_updated(wxCommandEvent&);

        public:

          /** \brief Constructor */
          add_to_project(wxWindow*, wxString);

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

