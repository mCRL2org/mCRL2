#include "gui_dialog_project.h"
#include "settings_manager.tcc"
#include "project_manager.h"

#include <wx/panel.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/msgdlg.h>
#include <wx/textctrl.h>

namespace squadt {
  namespace GUI {
    namespace dialog {

      wxString project::default_directory = wxFileName::GetHomeDir();

      /**
       * @param p the parent window
       * @param t the title for the window
       **/
      project::project(wxWindow* p, wxString t) : dialog::basic(p, t, wxSize(450, 350)) {
      }

      /**
       * @param[in] p should be a valid path that identifies a project store
       **/
      bool project::is_project_directory(wxString p) {
        wxString f(settings_manager::path_concatenate(std::string(p.fn_str()),
                                settings_manager::project_definition_base_name).c_str(), wxConvLocal);

        return (wxFileName::FileExists(f));
      }

      /**
       * @param[in] p should be a valid path that identifies a project store
       **/
      wxString project::get_project_description(wxString p) {
        std::string f(p.fn_str());

        project_manager::ptr m = project_manager::read(f);

        return (wxString(m->get_description().c_str(), wxConvLocal));
      }

      project::~project() {
      }

      /**
       * @param p the parent window
       **/
      new_project::new_project(wxWindow* p) : dialog::project(p, wxT("Project name and description")) {
        build();

        Connect(wxEVT_COMMAND_TEXT_UPDATED, wxTextEventHandler(dialog::new_project::on_text_updated));
        Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dialog::new_project::on_button_clicked));
      }

      void new_project::build() {
        wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);
        wxTreeEvent e;

        /* Prepare sizers */
        screen0                   = new wxBoxSizer(wxVERTICAL);
        screen1                   = new wxBoxSizer(wxVERTICAL);
        already_project_directory = new wxBoxSizer(wxVERTICAL);
        open_project_instead      = new wxBoxSizer(wxVERTICAL);

        /* Create widgets for screen0 */
        wxStaticText* name_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("Select a name for this project, the name will be used to name "
                            "the directory under which all project related files will be stored."));

        name_text->Wrap(GetSize().GetWidth() - 40);

        wxStaticText* description_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("An optional description:"));

        description_text->Wrap(GetSize().GetWidth() - 40);

        name               = new wxTextCtrl(main_panel, wxID_ANY, wxT(""));
        description        = new wxTextCtrl(main_panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);

        /* Add widgets to screen0 */
        screen0->Add(name_text);
        screen0->AddSpacer(10);
        screen0->Add(name, 0, wxEXPAND);
        screen0->AddSpacer(20);
        screen0->Add(description_text);
        screen0->AddSpacer(10);
        screen0->Add(description, 0, wxEXPAND);

        /* Add widgets for warning on screen1 */
        wxStaticText* already_project_directory_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("The selected directory is recognised as another project store; we "
                        "strongly recommend against nesting project directories."));

        already_project_directory_text->Wrap(GetSize().GetWidth() - 40);
        already_project_directory->Add(already_project_directory_text);

        /* Add widgets for warning on screen1 */
        wxStaticText* open_project_instead_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("The selected directory already contains a project store for a "
                        "project with the same name. "));

        open_project_instead_text->Wrap(GetSize().GetWidth() - 40);

        wxButton*     open_instead_button = new wxButton(main_panel, wxID_OPEN);

        open_project_instead->Add(open_project_instead_text);
        open_project_instead->Add(open_instead_button, 0, wxALIGN_RIGHT);

        /* Create widgets for screen1 */
        wxStaticText* directory_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("Select the directory in which a new directory for the project files"
                        " is to be created."));

        directory_text->Wrap(GetSize().GetWidth() - 40);

        directory_selector = new wxGenericDirCtrl(main_panel, wxID_ANY, default_directory,
                        wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER|wxDIRCTRL_DIR_ONLY);

        /* Add widgets to screen1 */
        screen1->Add(directory_text);
        screen1->AddSpacer(10);
        screen1->Add(directory_selector, 1, wxEXPAND|wxALIGN_CENTER);
        screen1->AddSpacer(10);
        screen1->Add(already_project_directory);
        screen1->Add(open_project_instead);
         
        /* Add sizers to the main sizer */
        s->AddSpacer(10);
        s->Add(screen0, 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
        s->Add(screen1, 0, wxEXPAND|wxLEFT|wxRIGHT, 20);

        /* Hide everything in screen1 */
        s->Show(screen1, false, true);

        main_panel->SetSizer(s);

        Layout();

        /* Trigger event to set the buttons right */
        on_text_updated(e);
      }

      /**
       * @param[in] e the event object passed by wxWidgets
       **/
      void new_project::on_button_clicked(wxCommandEvent& e) {
        if (e.GetId() == wxID_CANCEL) {
          EndModal(0);
        }
        else {
          wxTreeEvent e;
          wxSizer*    s = main_panel->GetSizer();
         
          s->Show(screen0, false, true);
          s->Show(screen1, true);

          SetTitle(wxT("Project storage"));

          Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(dialog::new_project::on_selection_changed));
          Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dialog::new_project::on_button_clicked1));

          Layout();

          /* Trigger event to set the widget enabledness/visibility right */
          on_selection_changed(e);
        }
      }

      /**
       * @param[in] e the event object passed by wxWidgets
       **/
      void new_project::on_button_clicked1(wxCommandEvent& e) {
        switch (e.GetId()) {
          case wxID_CANCEL:
            EndModal(0);
            break;
          case wxID_OPEN:
            directory_selector->SetPath(
                            wxFileName(directory_selector->GetPath(), name->GetValue()).GetFullPath());

            EndModal(2);
            break;
          default: /* wxID_OK */
            if (wxFileName(directory_selector->GetPath(), name->GetValue()).DirExists()) {
              if (wxMessageDialog(0, wxT("The directory already exists; any files it contains will be imported into the project."),
                                    wxT("Warning"), wxOK|wxCANCEL).ShowModal() == wxID_OK) {
                EndModal(1);
              }
            }
            else if (wxFileName(directory_selector->GetPath(), name->GetValue()).FileExists()) {
              wxString message = wxT("Unable to create project store, a file `");

              message.Append(name->GetValue());
              message.Append(wxT("' already exists in the selected directory."));

              wxMessageDialog(0, message,wxT("Error"), wxOK).ShowModal();
            }
            else {
              EndModal(1);
            }
            break;
        }
      }

      void new_project::on_text_updated(wxCommandEvent&) {
        button_accept->Enable(!name->GetValue().IsEmpty());
      }

      void new_project::on_selection_changed(wxTreeEvent&) {
        screen1->Show(open_project_instead, false);
        screen1->Show(already_project_directory, false);

        button_accept->Enable(false);

        if (dialog::project::is_project_directory(directory_selector->GetPath())) {
          /* Directory seems to be a project store */
          screen1->Show(already_project_directory, true);
        }
        else {
          wxFileName f(directory_selector->GetPath(), name->GetValue());

          if (dialog::project::is_project_directory(f.GetFullPath())) {
            /* Directory contains a directory with the name of this project */
            screen1->Show(open_project_instead, true);
          }
          else {
            button_accept->Enable(true);
          }
        }

        screen1->Layout();
      }

      /** \brief Gets the name of the project */
      std::string new_project::get_name() const {
        return (static_cast < const char* > (name->GetValue().fn_str()));
      }

      /** \brief Gets the directory reserved from project files */
      std::string new_project::get_location() const {
        return (static_cast < const char* > (wxFileName(directory_selector->GetPath(), name->GetValue()).GetFullPath().fn_str()));
      }

      /** \brief Gets the name of the project */
      std::string new_project::get_description() const {
        return (static_cast < const char* > (description->GetValue().fn_str()));
      }

      new_project::~new_project() {
      }

      /**
       * @param p the parent window
       **/
      open_project::open_project(wxWindow* p) : dialog::project(p, wxT("Select an existing project")) {
        build();

        Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(dialog::open_project::on_selection_changed));
      }

      void open_project::build() {
        wxBoxSizer*       s = new wxBoxSizer(wxVERTICAL);
        wxTreeEvent       e;

        /* Prepare sizers */
        project_description = new wxStaticBoxSizer(wxVERTICAL, main_panel, wxT("Project description"));
        not_store           = new wxBoxSizer(wxVERTICAL);

        description = new wxStaticText(main_panel, wxID_ANY, wxT(""));

        description->Wrap(GetSize().GetWidth() - 40);

        project_description->Add(description, 0, wxLEFT|wxRIGHT);

        wxStaticText* not_store_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("The currently selected directory is not recognised as a project store."));

        not_store_text->Wrap(GetSize().GetWidth() - 40);

        not_store->Add(not_store_text);

        directory_selector = new wxGenericDirCtrl(main_panel, wxID_ANY, default_directory,
                        wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER|wxDIRCTRL_DIR_ONLY);

        s->AddSpacer(10);
        s->Add(directory_selector, 1, wxEXPAND|wxLEFT|wxRIGHT, 20);
        s->AddSpacer(10);
        s->Add(project_description, 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
        s->Add(not_store, 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
        s->AddSpacer(10);

        s->Show(project_description,false);
        s->Show(not_store, false);

        main_panel->SetSizer(s);

        Layout();

        /* Trigger event to set the buttons right */
        on_selection_changed(e);
      }

      /**
       * @param[in] e the event object passed by wxWidgets
       **/
      void open_project::on_button_clicked(wxCommandEvent& e) {
        switch (e.GetId()) {
          case wxID_CANCEL:
            EndModal(0);
            break;
          default: /* wxID_OK */
            if (!selection_is_valid) {
              wxMessageDialog(0, wxT("Could not read project description; project file corrupt or inaccessible?"),
                              wxT("Error"), wxOK).ShowModal();

              button_accept->Enable(false);
            }
            else {
              EndModal(1);
            }
            break;
        }
      }

      void open_project::on_selection_changed(wxTreeEvent&) {
        wxSizer* s = main_panel->GetSizer();

        description->SetLabel(wxT(""));

        button_accept->Enable(false);

        if (dialog::project::is_project_directory(directory_selector->GetPath())) {

          selection_is_valid = true;

          try {
            wxString label = dialog::project::get_project_description(directory_selector->GetPath());

            description->SetLabel(label);

            s->Show(not_store, false);
            s->Show(project_description, !label.IsEmpty());
          }
          catch (...) {
            /* Project description could not be extracted */
            s->Show(not_store, false);
          }
        }
        else {
          s->Show(not_store, true);
          s->Show(project_description, false);

          selection_is_valid = false;
        }

        button_accept->Enable(selection_is_valid);

        Layout();
      }

      /** \brief Gets the name of the project */
      std::string open_project::get_name() const {
        return (std::string(wxFileName(directory_selector->GetPath()).GetFullName().fn_str()));
      }

      /** \brief Gets the directory reserved from project files */
      std::string open_project::get_location() const {
        return (std::string(directory_selector->GetPath().fn_str()));
      }

      /** \brief Gets the name of the project */
      std::string open_project::get_description() const {
        return (std::string(description->GetLabel().fn_str()));
      }

      open_project::~open_project() {
      }

      /**
       * @param p the parent window
       * @param s path of the project store
       **/
      add_to_project::add_to_project(wxWindow* p, wxString s) : dialog::project(p, wxT("Select the file to add...")),
                                                                project_store(s) {
        build();

        Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(dialog::add_to_project::on_selection_changed));
        Connect(wxEVT_COMMAND_TEXT_UPDATED, wxTextEventHandler(dialog::add_to_project::on_name_updated));
      }

      void add_to_project::build() {
        wxBoxSizer*       s = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer*       t = new wxBoxSizer(wxHORIZONTAL);
        wxTreeEvent       e;

        name = new wxTextCtrl(main_panel, wxID_ANY, wxT(""));

        t->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Add as :")));
        t->AddSpacer(5);
        t->Add(name, 1, wxEXPAND);

        file_selector = new wxGenericDirCtrl(main_panel, wxID_ANY, default_directory,
                        wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER);

        s->AddSpacer(10);
        s->Add(file_selector, 1, wxEXPAND|wxLEFT|wxRIGHT, 20);
        s->AddSpacer(10);
        s->Add(t, 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
        s->AddSpacer(10);

        main_panel->SetSizer(s);

        Layout();

        /* Trigger event to set the buttons right */
        on_selection_changed(e);
      }

      /**
       * @param[in] e the event object passed by wxWidgets
       **/
      void add_to_project::on_button_clicked(wxCommandEvent& e) {
        switch (e.GetId()) {
          case wxID_CANCEL:
            EndModal(0);
            break;
          default: /* wxID_OK */
            if (wxFileName(project_store, name->GetValue()).DirExists() || wxFileName(project_store, name->GetValue()).DirExists()) {
              wxMessageDialog(0, wxT("A file with this name is already part of the project!"), wxT("Error"), wxOK).ShowModal();

              button_accept->Enable(false);
            }
            else {
              EndModal(1);
            }
            break;
        }
      }

      void add_to_project::on_name_updated(wxCommandEvent&) {
        button_accept->Enable(false);

        if (!name->GetValue().IsEmpty()) {
          wxFileName n(project_store, name->GetValue());
         
          if (n.DirExists() || n.FileExists()) {
            wxMessageDialog(0, wxT("A file with this name is already part of the project!"), wxT("Error"), wxOK).ShowModal();
          }
          else {
            button_accept->Enable(true);
          }
        }
      }

      void add_to_project::on_selection_changed(wxTreeEvent&) {
        name->SetValue(wxFileName::FileExists(file_selector->GetPath()) ?
                wxFileName(file_selector->GetPath()).GetFullName() : wxT(""));
      }

      /** \brief Gets the name under which to add the file to the project */
      std::string add_to_project::get_name() const {
        return (std::string(name->GetValue().fn_str()));
      }

      /** \brief Gets the selected file that is to be added the the project */
      std::string add_to_project::get_source() const {
        return (std::string(file_selector->GetPath().fn_str()));
      }

      /** \brief Gets the selected file that is to be added the the project */
      std::string add_to_project::get_destination() const {
        return (std::string(wxFileName(project_store, name->GetValue()).GetFullPath().fn_str()));
      }

      add_to_project::~add_to_project() {
      }
    }
  }
}
