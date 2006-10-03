#include "gui_dialog_project.h"
#include "settings_manager.tcc"
#include "project_manager.h"

#include <wx/panel.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/msgdlg.h>
#include <wx/dirdlg.h>
#include <wx/textctrl.h>

#define cmID_BROWSE (wxID_HIGHEST + 1)

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
        bool r = true;

        try {
          project_manager::ptr l = project_manager::create(std::string(p.fn_str()), false);

          r = l.get() != 0;

        } catch (...) {
          r = false;
        }

        return (r);
      }

      /**
       * @param[in] p should be a valid path that identifies a project store
       **/
      wxString project::get_project_description(wxString p) {
        std::string f(p.fn_str());

        project_manager::ptr m = project_manager::create(f, false);

        return (wxString(m->get_description().c_str(), wxConvLocal));
      }

      project::~project() {
      }

      /**
       * @param p the parent window
       **/
      new_project::new_project(wxWindow* p) : dialog::project(p, wxT("Specify project store location")) {
        build();

        Connect(wxEVT_COMMAND_TEXT_UPDATED, wxTextEventHandler(dialog::new_project::on_text_updated));
        Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dialog::new_project::on_button_clicked));
      }

      void new_project::build() {
        wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);
        wxTreeEvent e;

        /* Prepare sizers */
        screen0                   = new wxBoxSizer(wxVERTICAL);
        open_project_instead      = new wxBoxSizer(wxVERTICAL);

        /* Add widgets for warning on screen0 */
        wxStaticText* open_project_instead_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("The selected directory seems to already contain a project store press button to open"));

        open_project_instead_text->Wrap(GetSize().GetWidth() - 40);

        wxButton*     open_instead_button = new wxButton(main_panel, wxID_OPEN);
        wxButton*     browse_button       = new wxButton(main_panel, cmID_BROWSE, wxT("Browse"));

        open_project_instead->Add(open_project_instead_text);
        open_project_instead->Add(open_instead_button, 0, wxALIGN_RIGHT);

        /* Create widgets for screen0 */
        wxStaticText* directory_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("Select a directory where the files for the new project can be stored."));

        location = new wxTextCtrl(main_panel, wxID_ANY, default_directory);

        directory_text->Wrap(GetSize().GetWidth() - 40);

        /* Add widgets to screen0 */
        screen0->Add(directory_text);
        screen0->AddSpacer(10);
        screen0->Add(location, 0, wxEXPAND);
        screen0->AddSpacer(10);
        screen0->Add(browse_button, 0, wxALIGN_RIGHT);
        screen0->AddSpacer(20);
        screen0->Add(open_project_instead);

        screen0->Show(open_project_instead, false);
         
        /* Add sizers to the main sizer */
        s->AddSpacer(10);
        s->Add(screen0, 0, wxEXPAND|wxLEFT|wxRIGHT, 20);

        main_panel->SetSizer(s);

        Layout();
      }

      /**
       * @param[in] e the event object passed by wxWidgets
       **/
      void new_project::on_button_clicked(wxCommandEvent& e) {
        switch (e.GetId()) {
          case wxID_CANCEL:
            EndModal(0);
            break;
          case cmID_BROWSE: {
            wxDirDialog dialog(0, wxT("Select a directory"), default_directory);
           
            if (dialog.ShowModal() == wxID_OK) {
              location->SetValue(dialog.GetPath());
            }
            break;
          }
          case wxID_OPEN:
            EndModal(2);
            break;
          default: /* wxID_OK */
            if (wxFileName(location->GetValue()).DirExists()) {
              if (wxMessageDialog(0, wxT("Convert the directory to project store and import any other files it contains."),
                                    wxT("Warning: directory exists"), wxOK|wxCANCEL).ShowModal() == wxID_OK) {
                EndModal(1);
              }
            }
            else if (wxFileName(location->GetValue()).FileExists()) {
              wxMessageDialog(0, wxT("Unable to create project store, a file is in the way.`"),wxT("Error"), wxOK).ShowModal();
            }
            else {
              EndModal(1);
            }
            break;
        }
      }

      void new_project::on_text_updated(wxCommandEvent&) {
        screen0->Show(open_project_instead, false);

        button_accept->Enable(false);

        if (!location->GetValue().IsEmpty()) {
          if (wxFileName(location->GetValue()).FileExists()) {
            wxMessageDialog(0, wxT("Unable to create project store, a file is in the way.`"),wxT("Error"), wxOK).ShowModal();
          }
          else if (wxFileName(location->GetValue()).DirExists() && dialog::project::is_project_directory(location->GetValue())) {
            /* Directory contains a directory with the name of this project */
            screen0->Show(open_project_instead, true);
          } else {
            button_accept->Enable(true);
          }
        }
         
        screen0->Layout();
      }

      /** \brief Gets the name of the project */
      std::string new_project::get_name() const {
        return (static_cast < const char* > (wxFileName(location->GetValue()).GetDirs().Last().fn_str()));
      }

      /** \brief Gets the directory reserved from project files */
      std::string new_project::get_location() const {
        return (static_cast < const char* > (wxFileName(location->GetValue()).GetFullPath().fn_str()));
      }

      /** \brief Gets the name of the project */
      std::string new_project::get_description() const {
        return ("");
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

        s->Layout();

        /* Circumvent refresh bug in wxGTK by explicitly using refresh */
        directory_selector->Refresh();
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
              wxMessageDialog(0, wxT("A file with this name already exists in the project directory!"), wxT("Error"), wxOK).ShowModal();

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

          if (n.FileExists()) {
            wxMessageDialog(0, wxT("A file with this name is already part of the project!"), wxT("Error"), wxOK).ShowModal();
          }
          else {
            button_accept->Enable(true);
          }
        }
      }

      void add_to_project::on_selection_changed(wxTreeEvent&) {
        if (wxFileName::FileExists(file_selector->GetPath())) {
          /* To circumvent a bug in wxWidgets: sometimes the text updated event is not triggered */
          name->SetValue(wxT("n"));

          name->SetValue(wxFileName(file_selector->GetPath()).GetFullName());
        }
        else {
          name->SetValue(wxEmptyString);

          button_accept->Enable(false);
        }
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
