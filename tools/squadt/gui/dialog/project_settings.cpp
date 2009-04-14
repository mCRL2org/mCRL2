// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/dialog/project_settings.cpp
/// \brief Add your file description here.

#include "wx.hpp" // precompiled headers

#include <boost/filesystem/operations.hpp>

#include "gui/dialog/project_settings.hpp"
#include "gui/project.hpp"
#include "gui/resources.hpp"
#include "settings_manager.hpp"
#include "project_manager.hpp"

#include <wx/panel.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/msgdlg.h>
#include <wx/dirdlg.h>
#include <wx/dir.h>
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

        try {
          boost::shared_ptr < project_manager > m = project_manager::create(std::string(p.fn_str()), false);

          return m.get() != 0;
        }
        catch (...) {
        }

        return false;
      }

      /**
       * @param[in] p should be a valid path that identifies a project store
       **/
      wxString project::get_project_description(wxString p) {
        return (wxEmptyString);
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

        location = new wxTextCtrl(main_panel, wxID_ANY, default_directory, wxDefaultPosition, wxDefaultSize);

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
          default: { /* wxID_OK */
              using namespace boost::filesystem;

              try {
                path target(std::string(location->GetValue().fn_str()));

                if (exists(target)) {
                  if (is_directory(target)) {
                    if (directory_iterator(target) != directory_iterator()) {
                      if (wxMessageDialog(0, wxT("Convert the directory to project store and import any other files it contains."),
                                            wxT("Warning: directory exists"), wxOK|wxCANCEL).ShowModal() == wxID_OK) {
                        EndModal(1);
                      }
                    }
                    else {
                      EndModal(1);
                    }
                  }
                  else {
                    wxMessageDialog(0, wxT("Unable to create project store, a file is in the way.`"), wxT("Error"), wxOK).ShowModal();
                  }
                }
                else {
                  EndModal(1);
                }
              }
              catch (...) {
                wxMessageDialog(0, wxT("Invalid file or directory.`"), wxT("Error"), wxOK).ShowModal();
              }
            } break;
        }
      }

      void new_project::on_text_updated(wxCommandEvent&) {
        screen0->Show(open_project_instead, false);

        button_accept->Enable(false);

        try {
          if (!location->GetValue().IsEmpty()) {
            using namespace boost::filesystem;

            path target(std::string(location->GetValue().fn_str()));

            if (exists(target) && is_directory(target) && dialog::project::is_project_directory(location->GetValue())) {
              /* Directory contains a directory with the name of this project */
              screen0->Show(open_project_instead, true);
            } else {
              button_accept->Enable(true);
            }
          }
        }
        catch (...) {
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
        Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dialog::open_project::on_button_clicked));
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
            EndModal(wxCANCEL);
            break;
          default: /* wxID_OK */
            if (!selection_is_valid) {
              wxMessageDialog(0, wxT("Could not read project description; project file corrupt or inaccessible?"),
                              wxT("Error"), wxOK).ShowModal();

              button_accept->Enable(false);
            }
            else {
              EndModal(wxOK);
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
      add_to_project::add_to_project(wxWindow* p, std::string const& directory) : dialog::project(p, wxT("Select the file to add...")) {

        build(directory);

        Connect(wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler(dialog::add_to_project::on_selection_changed));
        Connect(wxEVT_COMMAND_TEXT_UPDATED, wxTextEventHandler(dialog::add_to_project::on_name_updated));
        Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(dialog::add_to_project::on_button_clicked));
      }

      void add_to_project::build(std::string const& directory) {
        wxBoxSizer*       s = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer*       t = new wxBoxSizer(wxHORIZONTAL);

        name = new wxTextCtrl(main_panel, wxID_ANY, wxT(""));

        t->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Add as :")), 0, wxALIGN_CENTRE_VERTICAL);
        t->AddSpacer(5);
        t->Add(name, 1, wxEXPAND);

        file_selector = new wxGenericDirCtrl(main_panel, wxID_ANY, wxString(directory.c_str(), wxConvLocal),
                        wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER);

        cannot_add = new wxBoxSizer(wxHORIZONTAL);
        cannot_add->Add(new wxStaticBitmap(main_panel, wxID_ANY, *alert_icon));
        cannot_add->Add(new wxStaticText(main_panel, wxID_ANY, wxT(" Sorry this name is already taken!")));

        s->AddSpacer(8);
        s->Add(file_selector, 2, wxEXPAND|wxLEFT|wxRIGHT, 20);
        s->AddSpacer(8);
        s->Add(t, 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
        s->AddSpacer(8);
        s->Add(cannot_add, 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
        s->AddSpacer(8);

        s->Show(cannot_add, false);
        button_accept->Enable(false);
        name->Enable(false);

        main_panel->SetSizer(s);

        Layout();
      }

      /**
       * @param[in] e the event object passed by wxWidgets
       **/
      void add_to_project::on_button_clicked(wxCommandEvent& e) {
        switch (e.GetId()) {
          case wxID_CANCEL:
            EndModal(0);
            break;
          default: { /* wxID_OK */
              wxSizer* sizer = main_panel->GetSizer();

              using namespace boost::filesystem;

              try {
                if (!static_cast < squadt::GUI::project* > (GetParent())->exists(std::string(name->GetValue().fn_str()))) {
                  EndModal(1);

                  return;
                }
              }
              catch(...) {
              }

              button_accept->Enable(false);
              sizer->Show(cannot_add, true);

              sizer->Layout();
            }
            break;
        }
      }

      void add_to_project::on_name_updated(wxCommandEvent&) {
        wxSizer* sizer = main_panel->GetSizer();

        if (!name->GetValue().IsEmpty()) {
          using namespace boost::filesystem;

          bool status = true;

          try {
            if (!static_cast < squadt::GUI::project* > (GetParent())->exists(std::string(name->GetValue().fn_str()))) {
              status = false;
            }
          }
          catch (...) {
          }

          button_accept->Enable(!status);
          sizer->Show(cannot_add, status);
        }
        else {
          sizer->Show(cannot_add, false);
          button_accept->Enable(false);
        }

        sizer->Layout();
      }

      void add_to_project::on_selection_changed(wxTreeEvent&) {
        if (wxFileName::FileExists(file_selector->GetPath())) {
          name->SetValue(wxFileName(file_selector->GetPath()).GetFullName());
          name->Enable(true);
        }
        else {
          name->SetValue(wxEmptyString);
          name->Enable(false);
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
        return boost::filesystem::path(name->GetValue().fn_str()).string();
      }

      add_to_project::~add_to_project() {
      }
    }
  }
}
