// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/dialog/preferences.cpp
/// \brief Add your file description here.

#include "wx.hpp" // precompiled headers

#include "gui/dialog/preferences.hpp"
#include "type_registry.hpp"
#include "build_system.hpp"
#include "tool_manager.hpp"
#include "executor.hpp"

#include <boost/foreach.hpp>

#include <tipi/controller.hpp>
#include <tipi/detail/utility/logger.hpp>

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/textdlg.h>
#include <wx/version.h>
#include <wx/slider.h>
#include <wx/sizer.h>

// Compatibility with wx 2.6.*
#if (wxMAJOR_VERSION <= 2) && (wxMINOR_VERSION < 8)
#define wxID_EDIT      (wxID_HIGHEST + 1)
#endif

namespace squadt {
  namespace GUI {

    class execution_preferences : public wxPanel {
      friend class preferences;

      private:

        /** \brief The maximum number of tool instances that are not active for a configuration operation */
        wxSlider* maximum_concurrent;

      private:

        /** \brief Event handler for changes to the maximum */
        void maximum_changed(wxCommandEvent&);

      public:

        execution_preferences(wxWindow*);
    };

    class edit_preferences : public wxPanel {
      friend class preferences;

      private:

        wxListView* formats_and_actions;

      private:

        static wxString no_action;

      private:

        /** \brief Function that is used for getting columns with decent widths */
        void activate();

        /** \brief Event handler for when the new button is pressed */
        void new_association(wxCommandEvent&);

        /** \brief Event handler for when the delete button is pressed */
        void remove_association(wxCommandEvent&);

        /** \brief Event handler for command changes */
        void edit_command(wxCommandEvent&);

        /** \brief Event handler for selection changes */
        void item_selected(wxCommandEvent&);

      public:

        /** \brief Constructor */
        edit_preferences(wxWindow*);
    };

    class debug_preferences : public wxPanel {
      friend class preferences;

      private:

        /* The level above which diagnostic messages are filtered and removed */
        wxSlider*   filter_level;

        /* The level above which diagnostic messages are filtered and removed (tool specific) */
        wxSlider*   default_filter_level;

        /* Toggles more verbose output (tools) */
        wxCheckBox* toggle_verbose;

        /* Toggles more verbose output (tools) */
        wxCheckBox* toggle_debug;

      private:

        /* Event handler for filter level changes */
        void filter_level_changed(wxCommandEvent&);

      public:

        /* Constructor */
        debug_preferences(wxWindow*);
    };

    wxString edit_preferences::no_action = wxT("");

    void execution_preferences::maximum_changed(wxCommandEvent&) {
      global_build_system.get_executor().set_maximum_instance_count(maximum_concurrent->GetValue());
    }

    execution_preferences::execution_preferences(wxWindow* w) : wxPanel(w, wxID_ANY) {
      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);

      maximum_concurrent = new wxSlider(this, wxID_ANY, global_build_system.get_executor().get_maximum_instance_count(),
                                    1, 25, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS|wxSL_BOTTOM);

      current_sizer->AddStretchSpacer();
      current_sizer->Add(maximum_concurrent, 0, wxEXPAND|wxLEFT|wxRIGHT, 14);
      current_sizer->Add(new wxStaticText(this, wxID_ANY,
              wxT("Maximum number of concurrent tool instances that are not running for configuration purposes.")),
                  0, wxEXPAND|wxLEFT|wxRIGHT, 10);
      current_sizer->AddStretchSpacer();

      SetSizer(current_sizer);

      Connect(wxEVT_SCROLL_CHANGED, wxCommandEventHandler(execution_preferences::maximum_changed));
    }

    /** \cond HELPER_FUNCTIONS
     * Convenience function
     **/
    inline void get_wxlist_value(wxListItem& s, wxListCtrl* l, size_t r, size_t c) {
       s.SetId(r);
       s.SetColumn(c);

       l->GetItem(s);
    }
    /// \endcond

    void edit_preferences::edit_command(wxCommandEvent&) {
      wxTextEntryDialog command_dialog(this, wxT("Enter a command ($ is a place holder for the input file)"));

      try {
        long selected = formats_and_actions->GetFirstSelected();

        if (0 <= formats_and_actions) {
          wxListItem s;

          get_wxlist_value(s, formats_and_actions, selected, 1);

          command_dialog.SetValue(s.GetText());

          if (command_dialog.ShowModal() == wxID_OK) {
            formats_and_actions->SetItem(selected, 1, command_dialog.GetValue());

            get_wxlist_value(s, formats_and_actions, selected, 0);

            global_build_system.get_type_registry().register_command(
                            mime_type(std::string(formats_and_actions->GetItemText(selected).fn_str())), std::string(command_dialog.GetValue().fn_str()));
          }
        }
        else {
          wxMessageDialog(this, wxT("No format line selected"), wxT("Warning: ignoring request"), wxICON_WARNING|wxOK).ShowModal();
        }
      }
      catch (...) {
        wxMessageDialog(this, wxT("Invalid command specification; ignoring"), wxT("Error"), wxICON_ERROR|wxOK).ShowModal();
      }
    }

    void edit_preferences::new_association(wxCommandEvent&) {
      wxTextEntryDialog format_dialog(this, wxT("Enter format or MIME-type"));

      try {
        if (format_dialog.ShowModal() == wxID_OK) {
          long row = formats_and_actions->InsertItem(formats_and_actions->GetItemCount(), wxEmptyString);

          mime_type mt(std::string(format_dialog.GetValue().fn_str()));

          formats_and_actions->SetItem(row, 0, wxString(mt.string().c_str(), wxConvLocal));

          std::auto_ptr< command > c = global_build_system.get_type_registry().get_registered_command(mt, "$");

          if (c.get()) {
            formats_and_actions->SetItem(row, 1, wxString(c->string().c_str(), wxConvLocal));
          }
          else {
            formats_and_actions->SetItem(row, 1, wxEmptyString);
          }

          return;
        }
      }
      catch (...) {
        wxMessageDialog error(this, wxT("Invalid format or MIME-type; ignoring"), wxT("Error"), wxICON_ERROR|wxOK);

        error.ShowModal();
      }
    }

    void edit_preferences::remove_association(wxCommandEvent&) {
      long       selected = formats_and_actions->GetFirstSelected();
      wxListItem s;

      get_wxlist_value(s, formats_and_actions, selected, 0);

      global_build_system.get_type_registry().register_command(
                mime_type(std::string(s.GetText().fn_str())), type_registry::command_none);

      formats_and_actions->DeleteItem(selected);

      wxWindow::FindWindowById(wxID_EDIT, this)->Disable();
      wxWindow::FindWindowById(wxID_DELETE, this)->Disable();
    }

    void edit_preferences::activate() {
      long width = formats_and_actions->GetClientSize().GetWidth();

      formats_and_actions->SetColumnWidth(0, (width + 2) / 3);
      formats_and_actions->SetColumnWidth(1, (width * 2 + 2) / 3);
    }

    void edit_preferences::item_selected(wxCommandEvent&) {
      wxWindow::FindWindowById(wxID_EDIT, this)->Enable();
      wxWindow::FindWindowById(wxID_DELETE, this)->Enable();
    }

    edit_preferences::edit_preferences(wxWindow* w) : wxPanel(w, wxID_ANY) {
      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);

      SetSizer(current_sizer);

      wxStaticBoxSizer* known_formats = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Known formats and actions"));

      current_sizer->AddSpacer(30);
      current_sizer->Add(known_formats, 1, wxEXPAND|wxLEFT|wxRIGHT, 3);
      current_sizer->AddSpacer(5);

      formats_and_actions = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                            wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_VRULES|wxLC_HRULES);

      formats_and_actions->InsertColumn(0, wxT("Format"), wxLIST_FORMAT_CENTRE);
      formats_and_actions->InsertColumn(1, wxT("Edit action"), wxLIST_FORMAT_LEFT);

      long row = 0;

      type_registry& registry(global_build_system.get_type_registry());

      std::set < tipi::mime_type > types(registry.get_mime_types());

      BOOST_FOREACH(tipi::mime_type f, types) {
        std::auto_ptr < command > command_line = registry.get_registered_command(f, "$");

        formats_and_actions->InsertItem(row, wxString(f.string().c_str(), wxConvLocal));
        formats_and_actions->SetItem(row++, 1, command_line.get() ? wxString(command_line->string().c_str(), wxConvLocal) : no_action);
      }

      known_formats->AddSpacer(5);
      known_formats->Add(formats_and_actions, 1, wxEXPAND|wxLEFT|wxRIGHT, 3);

      current_sizer = new wxBoxSizer(wxHORIZONTAL);
      current_sizer->Add(new wxButton(this, wxID_NEW), 0, wxRIGHT, 5);
      current_sizer->Add(new wxButton(this, wxID_EDIT, wxT("Edit")), 0, wxRIGHT, 5);
      current_sizer->Add(new wxButton(this, wxID_DELETE), 0, wxRIGHT, 5);

      GetSizer()->Add(current_sizer, 0, wxALL|wxALIGN_LEFT|wxEXPAND, 3);

      wxWindow::FindWindowById(wxID_EDIT, this)->Disable();
      wxWindow::FindWindowById(wxID_DELETE, this)->Disable();

      Connect(wxID_NEW, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(edit_preferences::new_association));
      Connect(wxID_EDIT, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(edit_preferences::edit_command));
      Connect(wxID_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(edit_preferences::remove_association));
      Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxCommandEventHandler(edit_preferences::item_selected));
    }

    void debug_preferences::filter_level_changed(wxCommandEvent& e) {
      if (static_cast < wxSlider* > (e.GetEventObject()) == filter_level) {
        tipi::controller::communicator::get_default_logger().
                     set_filter_level(static_cast < tipi::utility::logger::log_level > (filter_level->GetValue()));
      }
      else {
        tipi::controller::communicator::get_default_logger().
                     set_default_filter_level(static_cast < tipi::utility::logger::log_level > (default_filter_level->GetValue()));
      }
    }

    debug_preferences::debug_preferences(wxWindow* w) : wxPanel(w, wxID_ANY) {
      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);

      filter_level = new wxSlider(this, wxID_ANY, tipi::controller::communicator::get_default_logger().get_filter_level(),
                                  0, 5, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS|wxSL_BOTTOM);

      current_sizer->AddStretchSpacer();
      current_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Main log filter level for diagnostic messages and warnings.")), 0, wxEXPAND|wxLEFT|wxRIGHT, 4);
      current_sizer->AddSpacer(8);
      current_sizer->Add(filter_level, 0, wxEXPAND|wxLEFT|wxRIGHT, 4);

      default_filter_level = new wxSlider(this, wxID_ANY, tipi::controller::communicator::get_default_logger().get_default_filter_level(),
                                  0, 5, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS|wxSL_BOTTOM);

      current_sizer->AddSpacer(8);
      current_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Filter level communicated to tools.")), 0, wxEXPAND|wxLEFT|wxRIGHT, 4);
      current_sizer->AddSpacer(8);
      current_sizer->Add(default_filter_level, 0, wxEXPAND|wxLEFT|wxRIGHT, 4);
      current_sizer->AddSpacer(16);
      current_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Note: lower filter levels are more restrictive")), 0, wxEXPAND|wxLEFT|wxRIGHT, 4);
      current_sizer->AddStretchSpacer();

      SetSizer(current_sizer);

      Connect(wxEVT_COMMAND_SLIDER_UPDATED, wxCommandEventHandler(debug_preferences::filter_level_changed));
    }

    /**
     * \param[in] p the parent window
     **/
    preferences::preferences(main* p) : wxDialog(p, wxID_ANY, wxT("Preferences"), wxDefaultPosition, wxSize(475,350)) {
      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);

      tab_manager = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

      /* Tool execution settings */
      tab_manager->AddPage(new execution_preferences(tab_manager), wxT("Execution"));

      /* Edit settings */
      tab_manager->AddPage(new edit_preferences(tab_manager), wxT("Editing"));

      /* Debug settings */
      tab_manager->AddPage(new debug_preferences(tab_manager), wxT("Debug"));

      wxButton* okay_button = new wxButton(this, wxID_OK);

      current_sizer->Add(tab_manager, 1, wxEXPAND|wxALL, 3);
      current_sizer->Add(okay_button, 0, wxALIGN_RIGHT|wxRIGHT|wxBOTTOM, 3);

      SetSizer(current_sizer);

      Connect(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, wxNotebookEventHandler(preferences::change_tab));
    }

    void preferences::change_tab(wxNotebookEvent& e) {
      if (e.GetSelection() == 1) {
        static_cast < edit_preferences* > (tab_manager->GetPage(e.GetSelection()))->activate();
      }
    }
  }
}
