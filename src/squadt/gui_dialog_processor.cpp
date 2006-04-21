#include "gui_main.h"
#include "gui_dialog_processor.h"

namespace squadt {
  namespace GUI {
    namespace dialog {

      /**
       * @param o a pointer to the parent window
       * @param t the title for the dialog window 
       * @param n the value for the name field
       **/
      processor_details::processor_details(wxWindow* o, wxString s, squadt::processor* p) :
                                                dialog::processor(o, wxT("View and change details")), project_store(s), target_processor(p) {
        build();

        Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(processor_details::on_tool_selector_item_selected), 0, this);
        Connect(wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler(processor_details::on_tool_selector_item_select), 0, this);
      }

      /**
       * @param n reference to the name
       **/
      void processor_details::build() {
        wxBoxSizer*       s = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer*       t = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer*       u = new wxBoxSizer(wxHORIZONTAL);

        name = new wxTextCtrl(main_panel, wxID_ANY, wxT(""));

        u->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Name :")));
        u->AddSpacer(5);
        u->Add(name, 1, wxEXPAND);

        wxNotebook* notebook = new wxNotebook(main_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

        input_objects = new wxListCtrl(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                        wxLC_REPORT|wxLC_SMALL_ICON|wxLC_ALIGN_LEFT|wxLC_SINGLE_SEL|wxLC_VRULES);
        input_objects->InsertColumn(0, wxT("Name"));
        input_objects->InsertColumn(1, wxT("Size"));
        input_objects->InsertColumn(2, wxT("Date"));
        output_objects = new wxListCtrl(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                        wxLC_REPORT|wxLC_SMALL_ICON|wxLC_ALIGN_LEFT|wxLC_SINGLE_SEL|wxLC_VRULES);
        output_objects->InsertColumn(0, wxT("Name"));
        output_objects->InsertColumn(1, wxT("Size"));
        output_objects->InsertColumn(2, wxT("Date"));

        notebook->AddPage(input_objects, wxT("Input"));
        notebook->AddPage(output_objects, wxT("Output"));

        t->Add(u, 0, wxEXPAND);
        t->AddSpacer(5);
        t->Add(notebook, 1, wxEXPAND);

        tool_selector = new wxTreeCtrl(main_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                        wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_SINGLE|wxSUNKEN_BORDER);

        tool_selector->AddRoot(wxEmptyString);

        s->AddSpacer(20);
        s->Add(tool_selector, 3, wxEXPAND|wxTOP|wxBOTTOM, 10);
        s->AddSpacer(10);
        s->Add(t, 5, wxEXPAND|wxTOP|wxBOTTOM, 10);
        s->AddSpacer(20);

        main_panel->SetSizer(s);

        Layout();
      }

      void processor_details::on_tool_selector_item_selected(wxTreeEvent& e) {
        if (tool_selector->GetItemParent(e.GetItem()) == tool_selector->GetRootItem()) {
          tool_selector->Toggle(e.GetItem());

          tool_selector->UnselectItem(e.GetItem());
        }
      }

      void processor_details::on_tool_selector_item_select(wxTreeEvent& e) {
        if (tool_selector->GetItemParent(e.GetItem()) == tool_selector->GetRootItem()) {
          e.Veto();
        }
      }

      void processor_details::populate_tool_list(storage_format f) {
        using namespace squadt::miscellaneous;

        tool_selector->DeleteChildren(tool_selector->GetRootItem());

        main::tool_registry->by_format(f, boost::bind(&processor_details::add_to_tool_list, this, _1));
      }

      void processor_details::add_to_tool_list(const miscellaneous::tool_selection_helper::tools_by_category::value_type& p) {
        wxString     current_category_name = wxString(p.first.c_str(), wxConvLocal);

        wxTreeItemId root = tool_selector->GetRootItem();

        /* Use of GetLastChild() because ItemHasChildren can return true when there are no children */
        if (tool_selector->GetLastChild(root).IsOk()) {
          wxTreeItemId last_category = tool_selector->GetLastChild(root);
         
          if (tool_selector->GetItemText(last_category) != current_category_name) {
            /* Add category */
            last_category = tool_selector->AppendItem(root, current_category_name);
          }
         
          tool_selector->AppendItem(last_category, wxString(p.second->get_name().c_str(), wxConvLocal));
        }
        else {
          wxTreeItemId last_category = tool_selector->AppendItem(root, current_category_name);

          tool_selector->AppendItem(last_category, wxString(p.second->get_name().c_str(), wxConvLocal));
        }
      }
    }
  }
}
