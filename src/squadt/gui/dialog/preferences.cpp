#include "preferences.h"
#include "../../build_system.h"
#include "../../tool_manager.h"
#include "../../executor.h"

#include <boost/foreach.hpp>

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

        /* \brief The maximum number of tool instances that are not active for a configuration operation */
        wxSlider* maximum_concurrent;

      private:

        /* \brief Event handler for changes to the maximum */
        void maximum_changed(wxCommandEvent&);

      public:

        execution_preferences(wxWindow*);
    };

    class edit_preferences : public wxPanel {
      friend class preferences;

      private:

        wxListView*                        formats_and_actions;

        wxTextCtrl*                        command_text;

      private:

        static wxString no_action;

      private:

        /* \brief Function that is used for getting columns with decent widths */
        void activate();

        /* \brief Event handler for when the new button is pressed */
        void apply_changes(wxCommandEvent&);

        /* \brief Event handler for when the new button is pressed */
        void apply_changes_for_row(long const&);

        /* \brief Event handler for when the new button is pressed */
        void new_association(wxCommandEvent&);

        /* \brief Event handler for when the delete button is pressed */
        void remove_association(wxCommandEvent&);

        /* \brief Event handler for changes after a new association operation */
        void after_label_edit(wxListEvent&);

        /* \brief Event handler for selection changes in the list control */
        void list_item_selected(wxListEvent&);

        /* \brief Event handler for command changes */
        void command_changed(wxCommandEvent&);

      public:

        edit_preferences(wxWindow*);
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

    wxString edit_preferences::no_action = wxT("");

    void execution_preferences::maximum_changed(wxCommandEvent&) {
      global_build_system.get_executor()->set_maximum_instance_count(maximum_concurrent->GetValue());
    }

    execution_preferences::execution_preferences(wxWindow* w) : wxPanel(w, wxID_ANY) {
      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);

      SetSizer(current_sizer);

      current_sizer->AddSpacer(30);
      current_sizer->Add(new wxStaticText(this, wxID_ANY,
              wxT("Maximum number of concurrent tool instances that are not running for configuration purposes.")),
                  0, wxEXPAND|wxLEFT|wxRIGHT, 10);

      maximum_concurrent = new wxSlider(this, wxID_ANY, global_build_system.get_executor()->get_maximum_instance_count(),
                                    1, 25, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS|wxSL_BOTTOM);

      current_sizer->Add(maximum_concurrent, 0, wxEXPAND|wxLEFT|wxRIGHT, 14);

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

    void edit_preferences::command_changed(wxCommandEvent&) {
      long selected = formats_and_actions->GetFirstSelected();

      if (0 <= selected) {
        wxListItem s;

        get_wxlist_value(s, formats_and_actions, selected, 1);

        s.SetText(command_text->GetValue());

        formats_and_actions->SetItem(s);

        // On Mac OS X selection/focus is lost at this point
        formats_and_actions->Select(selected);
      }
    }

    void edit_preferences::new_association(wxCommandEvent&) {
      long row = formats_and_actions->InsertItem(formats_and_actions->GetItemCount(), wxEmptyString);

      formats_and_actions->EditLabel(row);
      formats_and_actions->SetItem(row, 1, wxEmptyString);
    }

    void edit_preferences::remove_association(wxCommandEvent&) {
      long       selected = formats_and_actions->GetFirstSelected();
      wxListItem s;
        
      get_wxlist_value(s, formats_and_actions, selected, 0);

      global_build_system.get_type_registry()->register_command(mime_type(std::string(s.GetText().fn_str())), type_registry::command_none);

      formats_and_actions->DeleteItem(selected);
    }

    void edit_preferences::after_label_edit(wxListEvent& e) {
      // Assumes that the wxListCtrl only allows edit of items in the first
      // columns, and deleting item i deletes all items on row i
      long row = e.GetIndex();

      if (e.GetLabel().IsEmpty()) {
        formats_and_actions->DeleteItem(row);
      }
    }

    void edit_preferences::activate() {
      long width = formats_and_actions->GetClientSize().GetWidth();

      formats_and_actions->SetColumnWidth(0, (width + 2) / 3);
      formats_and_actions->SetColumnWidth(1, (width * 2 + 2) / 3);
    }

    void edit_preferences::apply_changes(wxCommandEvent&) {
      apply_changes_for_row(formats_and_actions->GetFirstSelected());
    }

    void edit_preferences::apply_changes_for_row(long const& selected) {
      if (0 <= selected) {
        wxListItem s;
        
        get_wxlist_value(s, formats_and_actions, selected, 0);

        if (s.GetText().IsEmpty()) {
          formats_and_actions->DeleteItem(selected);
        }
        else {
          mime_type   type(std::string(s.GetText().fn_str()));
          wxString    new_command = command_text->GetValue();
         
          get_wxlist_value(s, formats_and_actions, selected, 1);
         
          type_registry* registry = global_build_system.get_type_registry();
         
          if (new_command.IsEmpty()) {
            registry->register_command(type, type_registry::command_none);

            s.SetText(no_action);
          }
          else if (new_command.Lower() == wxT("system")) {
         
            registry->register_command(type, type_registry::command_system);
         
            if (registry->has_registered_command(type)) {
              std::auto_ptr < command > command_line = registry->get_registered_command(type, "$");
         
              s.SetText(wxString(command_line->argument_string().c_str(), wxConvLocal));
            }
            else {
              s.SetText(no_action);
            }
          }
          else if (new_command != no_action) {
            s.SetText(new_command);
         
            registry->register_command(type, std::string(new_command.fn_str()));
          }
         
          formats_and_actions->SetItem(s);
        }
      }
    }

    void edit_preferences::list_item_selected(wxListEvent&) {
      wxListItem s;

      get_wxlist_value(s, formats_and_actions, formats_and_actions->GetFirstSelected(), 1);

      if (s.GetText() == no_action) {
        command_text->SetValue(wxEmptyString);
      }
      else {
        command_text->SetValue(s.GetText());
      }
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

      type_registry* registry = global_build_system.get_type_registry();

      std::set < build_system::mime_type > types(registry->get_mime_types());

      BOOST_FOREACH(build_system::mime_type f, types) {
        std::auto_ptr < command > command_line = registry->get_registered_command(f, "$");

        formats_and_actions->InsertItem(row, wxString(f.as_string().c_str(), wxConvLocal));

        formats_and_actions->SetItem(row++, 1, command_line.get() ? wxString(command_line->argument_string().c_str(), wxConvLocal) : no_action);
      }

      known_formats->AddSpacer(5);
      known_formats->Add(formats_and_actions, 1, wxEXPAND|wxLEFT|wxRIGHT, 3);

      command_text = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

      current_sizer = new wxBoxSizer(wxHORIZONTAL);
      current_sizer->Add(new wxButton(this, wxID_NEW), 0, wxRIGHT, 5);
      current_sizer->Add(new wxButton(this, wxID_DELETE), 0, wxRIGHT, 5);
      current_sizer->Add(command_text, 1, wxEXPAND);

      GetSizer()->Add(current_sizer, 0, wxALL|wxALIGN_LEFT|wxEXPAND, 3);

      Connect(wxID_NEW, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(edit_preferences::new_association));
      Connect(wxID_DELETE, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(edit_preferences::remove_association));
      Connect(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(edit_preferences::apply_changes));
      Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(edit_preferences::command_changed));
      Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(edit_preferences::list_item_selected));
      Connect(wxEVT_COMMAND_LIST_END_LABEL_EDIT, wxListEventHandler(edit_preferences::after_label_edit));
      Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler(edit_preferences::after_label_edit));
    }

    void debug_preferences::filter_level_changed(wxCommandEvent&) {
      sip::controller::communicator::get_standard_error_logger()->
                     set_filter_level(static_cast < utility::logger::log_level > (filter_level->GetValue()));
    }

    debug_preferences::debug_preferences(wxWindow* w) : wxPanel(w, wxID_ANY) {
      wxSizer* current_sizer = new wxBoxSizer(wxVERTICAL);

      SetSizer(current_sizer);

      current_sizer->AddSpacer(30);
      current_sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Filter level for diagnostic messages and warnings")), 0, wxEXPAND|wxLEFT|wxRIGHT, 4);

      filter_level = new wxSlider(this, wxID_ANY, std::max(sip::controller::communicator::get_standard_error_logger()->get_filter_level(),
                                  static_cast < utility::logger::log_level > (1)), 1, 5, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_LABELS|wxSL_BOTTOM);

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
