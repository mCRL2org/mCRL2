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
