#ifndef GUI_DIALOG_PROJECT
#define GUI_DIALOG_PROJECT

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/sizer.h>

namespace squadt {
  namespace GUI {
    namespace dialog {

      /** Base class for project settings */
      class project : public wxDialog {

        protected:

          /** The panel that holds the custom controls for this dialog */
          wxPanel*        main_panel;

          /** \brief Button that, when pressed, accepts the current settings */
          wxButton*       button_accept;

          /** \brief Button that, when pressed, cancels and closes the window */
          wxButton*       button_cancel;

          /** \brief The directory from which the directory selectors start */
          static wxString default_directory;

        protected:

          /** \brief Determines wether a directory is recognised as a project store */
          static bool is_project_directory(wxString);

          /** \brief Determines wether a directory is recognised as a project store */
          static wxString get_project_description(wxString);

        private:

          /** \brief Helper function that places the widgets */
          void build();

          /** \brief Handler for when one of the buttons is clicked */
          void on_button_clicked(wxCommandEvent&);

          /** \brief Handler for the window close event */
          void on_window_close(wxCloseEvent&);

        public:

          /** \brief Constructor */
          inline project(wxWindow*, wxString);

          /** \brief Gets the name of the project */
          virtual wxString get_name() const = 0;

          /** \brief Gets the name of the project */
          virtual wxString get_location() const = 0;

          /** \brief Gets the name of the project */
          virtual wxString get_description() const = 0;

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

          /** \brief Control to select a directory that contains the new project store */
          wxGenericDirCtrl* directory_selector;

          /** \brief Text field that contains the name of the new project */
          wxTextCtrl*       name;

          /** \brief Text field that contains a description for the project */
          wxTextCtrl*       description;

          /** \brief Shows the first screen of widgets (before okay is clicked) */
          wxBoxSizer*       screen0;

          /** \brief Shows the second screen of widgets (after okay is first clicked) */
          wxBoxSizer*       screen1;

          /** \brief Shows a warning that the currently selected directory is a project store */
          wxBoxSizer*       already_project_directory;

          /** \brief Shows a message that a project with the same name was found and a button to open it */
          wxBoxSizer*       open_project_instead;

        private:

          /** \brief Helper function that places the widgets */
          void build();

          /** \brief Handler for the event that one of the buttons is clicked */
          void on_button_clicked(wxCommandEvent&);

          /** \brief Handler for the event that one of the buttons is clicked (alternative) */
          void on_button_clicked1(wxCommandEvent&);

          /** \brief Handler for the event that a directory is selected */
          void on_selection_changed(wxTreeEvent&);

          /** \brief Handler for the event that the name or description are updated */
          void on_text_updated(wxCommandEvent&);

        public:

          /** \brief Constructor */
          new_project(wxWindow*);

          /** \brief Gets the name of the project */
          wxString get_name() const;

          /** \brief Gets the name of the project */
          wxString get_location() const;

          /** \brief Gets the name of the project */
          wxString get_description() const;

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
          wxString get_name() const;

          /** \brief Gets the name of the project */
          wxString get_location() const;

          /** \brief Gets the name of the project */
          wxString get_description() const;

          /** \brief Destructor */
          ~open_project();
      };
    }
  }
}

#endif

