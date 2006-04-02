#include "gui_dialog_project.h"
#include "settings_manager.tcc"
#include "project_manager.h"

#include <wx/panel.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/dirctrl.h>
#include <wx/textctrl.h>

namespace squadt {
  namespace GUI {
    namespace dialog {

      wxString project::default_directory = wxFileName::GetHomeDir();

      /**
       * @param p the parent window
       * @param t the title for the window
       **/
      project::project(wxWindow* p, wxString t) : wxDialog(p, wxID_ANY, t, wxDefaultPosition, wxSize(450, 350)) {
        build();

        Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(dialog::project::on_window_close));
        Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dialog::project::on_button_clicked));
      }

      /**
       * @param[in] e the event object passed by wxWidgets
       **/
      void project::on_button_clicked(wxCommandEvent& e) {
        EndModal((e.GetId() == wxID_CANCEL) ? 0 : 1);
      }

      void project::on_window_close(wxCloseEvent&) {
        EndModal(0);
      }

      void project::build() {
        wxBoxSizer* s  = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* ss = new wxBoxSizer(wxHORIZONTAL);

        main_panel    = new wxPanel(this, wxID_ANY);
        button_accept = new wxButton(this, wxID_OK);
        button_cancel = new wxButton(this, wxID_CANCEL);

        ss->Add(button_accept, 0, wxRIGHT, 5);
        ss->Add(button_cancel, 0, wxLEFT|wxRIGHT, 5);
        s->Add(main_panel, 1, wxEXPAND|wxALL, 4);
        s->Add(new wxStaticLine(this, wxID_ANY), 0, wxEXPAND|wxALL, 2);
        s->Add(ss, 0, wxALIGN_RIGHT|wxALL, 5);

        SetSizer(s);
      }

      /**
       * @param[in] p should be a valid path that identifies a directory
       **/
      bool project::is_project_directory(wxString p) {
        wxString f(settings_manager::path_concatenate(std::string(p.fn_str()),
                                settings_manager::project_definition_base_name).c_str(), wxConvLocal);

        return (wxFileName::FileExists(f));
      }

      /**
       * @param[in] p should be a valid path that identifies a directory
       **/
      wxString project::get_project_description(wxString p) {
        return (wxString(project_manager::read(std::string(p.fn_str()))->get_description().c_str(), wxConvLocal));
      }

      project::~project() {
      }

      /**
       * @param p the parent window
       **/
      new_project::new_project(wxWindow* p) : dialog::project(p, wxT("Specify new project")) {
        build();

        Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(dialog::new_project::on_selection_changed));
        Connect(wxEVT_COMMAND_TEXT_UPDATED, wxTextEventHandler(dialog::new_project::on_text_updated));
        Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dialog::new_project::on_button_clicked));
      }

      void new_project::build() {
        wxBoxSizer* s = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* t = new wxBoxSizer(wxVERTICAL);
        wxTreeEvent e;

        wxStaticText* name_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("Select a name for this project, the name will be used to name "
                            "the directory under which all project related files will be stored."));

        wxStaticText* description_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("An optional description:"));

        wxStaticText* directory_text = new wxStaticText(main_panel, wxID_ANY,
                        wxT("Select the directory in which a new directory for the project files is to be created."));

        name_text->Wrap(GetSize().GetWidth() - 10);
        description_text->Wrap(GetSize().GetWidth() - 10);
        directory_text->Wrap(GetSize().GetWidth() - 10);

        name               = new wxTextCtrl(main_panel, wxID_ANY, wxT(""));
        description        = new wxTextCtrl(main_panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
        directory_selector = new wxGenericDirCtrl(main_panel, wxID_ANY, default_directory,
                        wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER|wxDIRCTRL_DIR_ONLY);

        s->Add(directory_selector, 1, wxEXPAND|wxALIGN_CENTER|wxALL, 5);
        s->Add(directory_text, 0, wxALL, 6);
        t->AddSpacer(10);
        t->Add(name_text, 0, wxALL, 6);
        t->AddSpacer(10);
        t->Add(name, 0, wxEXPAND|wxALIGN_CENTER|wxLEFT|wxRIGHT, 20);
        t->AddSpacer(20);
        t->Add(description_text, 0, wxALL, 6);
        t->AddSpacer(10);
        t->Add(description, 0, wxEXPAND|wxALIGN_CENTER|wxLEFT|wxRIGHT, 20);
        s->Add(t, 0, wxEXPAND|wxTOP, 20);
         
        s->Show(directory_selector, false);
        s->Show(directory_text, false);

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
          wxSizer* s = main_panel->GetSizer();
         
          button_accept->Enable(false);
         
          s->Show(s->GetItem(2)->GetSizer(), false, true);
          s->Show(directory_selector, true);

          Layout();
        }
      }

      void new_project::on_text_updated(wxCommandEvent&) {
        button_accept->Enable(!name->GetValue().IsEmpty());
      }

      void new_project::on_selection_changed(wxTreeEvent&) {
        description->SetLabel(wxT(""));

        if (!dialog::project::is_project_directory(directory_selector->GetPath())) {
          button_accept->Enable(true);
        }
      }

      /** \brief Gets the name of the project */
      wxString new_project::get_name() const {
        return (wxFileName(directory_selector->GetPath()).GetFullName());
      }

      /** \brief Gets the directory reserved from project files */
      wxString new_project::get_location() const {
        return (directory_selector->GetPath());
      }

      /** \brief Gets the name of the project */
      wxString new_project::get_description() const {
        return (description->GetValue());
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
        wxStaticBoxSizer* t = new wxStaticBoxSizer(wxVERTICAL, main_panel, wxT("Project description"));
        wxTreeEvent       e;

        description        = new wxStaticText(main_panel, wxID_ANY, wxT(""));
        directory_selector = new wxGenericDirCtrl(main_panel, wxID_ANY, default_directory,
                        wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER|wxDIRCTRL_DIR_ONLY);

        t->Add(description, 0, wxALL, 0);
        s->Add(directory_selector, 4, wxEXPAND|wxALIGN_CENTER|wxALL, 0);
        s->Add(t, 2, wxEXPAND|wxALL, 0);

        main_panel->SetSizer(s);

        GetSizer()->Layout();

        /* Trigger event to set the buttons right */
        on_selection_changed(e);
      }

      void open_project::on_selection_changed(wxTreeEvent&) {
        button_accept->Enable(false);
        description->Enable(false);
        description->SetLabel(wxT(""));

        if (dialog::project::is_project_directory(
                                directory_selector->GetPath())) {
          try {
            wxString label = dialog::project::get_project_description(directory_selector->GetPath());

            button_accept->Enable(true);
            description->Enable(true);
            description->SetLabel(label);
          }
          catch (...) {
            /* Project description could not be extracted */
          }
        }
      }

      /** \brief Gets the name of the project */
      wxString open_project::get_name() const {
        return (wxFileName(directory_selector->GetPath()).GetFullName());
      }

      /** \brief Gets the directory reserved from project files */
      wxString open_project::get_location() const {
        return (directory_selector->GetPath());
      }

      /** \brief Gets the name of the project */
      wxString open_project::get_description() const {
        return (description->GetLabel());
      }

      open_project::~open_project() {
      }
    }
  }
}
