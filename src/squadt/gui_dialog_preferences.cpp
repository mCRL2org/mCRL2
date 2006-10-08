#include "gui_dialog_preferences.h"
#include "gui_miscellaneous.h"

#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>
#include <wx/slider.h>
#include <wx/sizer.h>

#include <sip/controller.h>
#include <utility/logger.h>

#define cmID_EDIT      (wxID_HIGHEST + 1)

namespace squadt {
  namespace GUI {

    class execution_preferences : public wxPanel {
      friend class preferences;

      private:

        /* The maximum number of tool instances that are not active for a configuration operation */
        wxSlider* maximum_concurrent;

      public:

        execution_preferences(wxWindow*);
    };

    class edit_preferences : public wxPanel {
      friend class preferences;

      private:

        miscellaneous::tool_selection_helper::sptr tool_registry;

        wxListCtrl*                                formats_and_actions;

      private:

        /* \brief Function that is used for getting columns with decent widths */
        void activate();

      public:

        edit_preferences(miscellaneous::tool_selection_helper::sptr const&, wxWindow*);
    };

    class debug_preferences : public wxPanel {
      friend class preferences;

      private:

        /* The level above which diagnostic messages are filtered and removed */
        wxSlider*   filter_level;

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

    execution_preferences::execution_preferences(wxWindow* w) : wxPanel(w, wxID_ANY) {
      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);

      SetSizer(current_sizer);

      current_sizer->AddSpacer(30);
      current_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Maximum number of concurrent tool instances that are not running for configuration purposes.")), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);

      maximum_concurrent = new wxSlider(this, wxID_ANY, 1, 1, 25, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS|wxSL_BOTTOM);

      current_sizer->Add(maximum_concurrent, 0, wxEXPAND|wxLEFT|wxRIGHT, 14);
    }

    void edit_preferences::activate() {
      long width = formats_and_actions->GetClientSize().GetWidth();

      formats_and_actions->SetColumnWidth(0, (width + 2) / 3);
      formats_and_actions->SetColumnWidth(1, (width * 2 + 2) / 3);
    }

    edit_preferences::edit_preferences(miscellaneous::tool_selection_helper::sptr const& h, wxWindow* w) : wxPanel(w, wxID_ANY), tool_registry(h) {
      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);

      SetSizer(current_sizer);

      wxStaticBoxSizer* known_formats = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Known formats and actions"));

      current_sizer->AddSpacer(30);
      current_sizer->Add(known_formats, 1, wxEXPAND|wxLEFT|wxRIGHT, 3);
      current_sizer->AddSpacer(5);

      formats_and_actions = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_VRULES|wxLC_HRULES);

      formats_and_actions->InsertColumn(0, wxT("Format"), wxLIST_FORMAT_CENTRE);
      formats_and_actions->InsertColumn(1, wxT("Edit action"), wxLIST_FORMAT_LEFT);

      long row = 0;

      BOOST_FOREACH(storage_format f, tool_registry->get_storage_formats()) {
        formats_and_actions->InsertItem(row, wxString(f.c_str(), wxConvLocal));
        formats_and_actions->SetItem(row++, 1, wxT("No action"));
      }

      known_formats->AddSpacer(5);
      known_formats->Add(formats_and_actions, 1, wxEXPAND|wxLEFT|wxRIGHT, 3);

      current_sizer->Add(new wxButton(this, cmID_EDIT, wxT("Edit")), 0, wxALL|wxALIGN_LEFT, 3);

      current_sizer->RecalcSizes();
    }

    void debug_preferences::filter_level_changed(wxCommandEvent&) {
      sip::controller::communicator::get_standard_error_logger()->set_filter_level(static_cast < utility::logger::log_level > (filter_level->GetValue()));
    }

    debug_preferences::debug_preferences(wxWindow* w) : wxPanel(w, wxID_ANY) {
      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);

      SetSizer(current_sizer);

      current_sizer->AddSpacer(30);
      current_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Filter level for diagnostic messages and warnings")), 0, wxEXPAND|wxLEFT|wxRIGHT, 4);

      filter_level = new wxSlider(this, wxID_ANY, std::max(sip::controller::communicator::get_standard_error_logger()->get_filter_level(), static_cast < utility::logger::log_level > (1)),
                                                                        1, 5, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS|wxSL_BOTTOM);

      Connect(wxEVT_SCROLL_CHANGED, wxCommandEventHandler(debug_preferences::filter_level_changed));

      current_sizer->AddSpacer(3);
      current_sizer->Add(filter_level, 0, wxEXPAND|wxLEFT|wxRIGHT, 4);
      current_sizer->AddSpacer(8);
      current_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Notice that lower filter levels are more restrictive")), 0, wxEXPAND|wxLEFT|wxRIGHT, 4);
      current_sizer->AddSpacer(15);

      wxStaticBoxSizer* tool_group = new wxStaticBoxSizer(wxVERTICAL, this, wxT("Tool specific"));

      toggle_verbose = new wxCheckBox(this, wxID_ANY, wxT("verbose messages"));
      toggle_debug   = new wxCheckBox(this, wxID_ANY, wxT("debug messages"));

      tool_group->Add(toggle_verbose, 0, wxEXPAND|wxLEFT|wxRIGHT, 3);
      tool_group->Add(toggle_debug, 0, wxEXPAND|wxLEFT|wxRIGHT, 3);

      current_sizer->Add(tool_group, 0, wxEXPAND|wxLEFT|wxRIGHT, 4);
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
      tab_manager->AddPage(new edit_preferences(p->tool_registry, tab_manager), wxT("Editing"));

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
