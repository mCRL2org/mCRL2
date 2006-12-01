#include <cmath>
#include <ctime>
#include <stack>

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "gui_main.h"
#include "gui_dialog_processor.h"

namespace squadt {
  namespace GUI {
    namespace dialog {

      /** \brief SI prefixes for binary multiples */
      const char* prefixes_for_binary_multiples[7] = { "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei" };

      boost::format time_format("%s %sB");

      /**
       * @param o a pointer to the parent window
       * @param s path to the project store
       * @param p the processor of which to display data
       **/
      processor_details::processor_details(wxWindow* o, wxString s, squadt::processor::sptr p) :
                                                dialog::processor(o, wxT("View and change details")),
                                                project_store(s), target_processor(p), tools_selectable(true) {
        build();

        Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(processor_details::on_tool_selector_item_selected), 0, this);
        Connect(wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler(processor_details::on_tool_selector_item_select), 0, this);
      }

      void processor_details::build() {
        using namespace boost::filesystem;

        wxBoxSizer*       s = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer*       t = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer*       u = new wxBoxSizer(wxHORIZONTAL);

        name = new wxStaticText(main_panel, wxID_ANY, wxT(""));

        u->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Main input : ")));
        u->AddSpacer(5);
        u->Add(name, 1, wxEXPAND);

        wxNotebook* notebook = new wxNotebook(main_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

        input_objects = new wxListCtrl(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                        wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_SINGLE_SEL|wxLC_VRULES|wxLC_HRULES);
        input_objects->InsertColumn(0, wxT("Name"));
        input_objects->InsertColumn(1, wxT("Size"), wxLIST_FORMAT_RIGHT);
        input_objects->InsertColumn(2, wxT("Date"));

        long row = 0;

        for (squadt::processor::input_object_iterator i = target_processor->get_input_iterator(); i.valid(); ++i) {
          if (*i != 0) {
            path p(project_store.fn_str());
            
            p = p / path((*i)->location);

            input_objects->InsertItem(row, wxString(p.leaf().c_str(), wxConvLocal));

            if (exists(p)) {
              unsigned long size       = file_size(p);
              time_t        write_time = last_write_time(p);
              unsigned char magnitude  = static_cast < unsigned char > (floor(log(size) / log(1024)));

              size = (unsigned long) (size / pow(1024, magnitude));

              input_objects->SetItem(row, 1, wxString(str((time_format % size %
                         prefixes_for_binary_multiples[magnitude])).c_str(), wxConvLocal));

              input_objects->SetItem(row, 2, wxString(ctime(&write_time), wxConvLocal));
            }

            ++row;
          }
        }

        input_objects->SetColumnWidth(0, wxLIST_AUTOSIZE);
        input_objects->SetColumnWidth(1, wxLIST_AUTOSIZE);
        input_objects->SetColumnWidth(2, wxLIST_AUTOSIZE);

        output_objects = new wxListCtrl(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                        wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_SINGLE_SEL|wxLC_VRULES|wxLC_HRULES);
        output_objects->InsertColumn(0, wxT("Name"));
        output_objects->InsertColumn(1, wxT("Size"), wxLIST_FORMAT_RIGHT);
        output_objects->InsertColumn(2, wxT("Date"));

        row = 0;

        for (squadt::processor::output_object_iterator i = target_processor->get_output_iterator(); i.valid(); ++i) {
          if (*i != 0) {
            path p(project_store.fn_str());
            
            p = p / path((*i)->location);

            output_objects->InsertItem(row, wxString(p.leaf().c_str(), wxConvLocal));

            if (exists(p)) {
              unsigned long size       = file_size(p);
              time_t        write_time = last_write_time(p);
              unsigned char magnitude  = static_cast < unsigned char > (floor(log(size) / log(1024)));

              size = (unsigned long) (size / pow(1024, magnitude));

              output_objects->SetItem(row, 1, wxString(str((time_format % size %
                        prefixes_for_binary_multiples[magnitude])).c_str(), wxConvLocal));

              output_objects->SetItem(row, 2, wxString(ctime(&write_time), wxConvLocal));
            }

            ++row;
          }
        }

        output_objects->SetColumnWidth(0, wxLIST_AUTOSIZE);
        output_objects->SetColumnWidth(1, wxLIST_AUTOSIZE);
        output_objects->SetColumnWidth(2, wxLIST_AUTOSIZE);

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
        if (tool_selector->GetItemParent(e.GetItem()) == tool_selector->GetRootItem() || (!tools_selectable && (e.GetItem() != selected_tool))) {
          e.Veto();
        }
      }

      void processor_details::on_tool_selector_item_collapsed(wxTreeEvent& e) {
        tool_selector->EnsureVisible(selected_tool);
        tool_selector->SelectItem(selected_tool);
        tool_selector->ScrollTo(selected_tool);
      }

      void processor_details::select_tool(sip::tool::capabilities::input_combination const* combination, std::string const& name) {
        wxString category(combination->m_category.get_name().c_str(), wxConvLocal);

        std::stack < wxTreeItemId > id_stack;

        id_stack.push(tool_selector->GetRootItem());

        /* Recursively search the tree to find the node to select (if only there where a search method) */
        while (!id_stack.empty()) {
          wxTreeItemIdValue cookie;             // For wxTreeCtrl traversal
          wxTreeItemId      c = id_stack.top(); // The current node

          id_stack.pop();

          for (wxTreeItemId j = tool_selector->GetFirstChild(c, cookie);
                                    j.IsOk(); j = tool_selector->GetNextChild(c, cookie)) {

            if (tool_selector->GetItemText(j) == category) {
              wxTreeItemIdValue cookie1; // For wxTreeCtrl traversal

              /* Found category */
              for (wxTreeItemId k = tool_selector->GetFirstChild(j, cookie1);
                                    k.IsOk(); k = tool_selector->GetNextChild(j, cookie1)) {

                if (tool_selector->GetItemText(k) == wxString(name.c_str(), wxConvLocal)) {
                  /* Found tool */
                  tool_selector->SelectItem(k);
                  tool_selector->EnsureVisible(k);
                  tool_selector->ScrollTo(k);

                  selected_tool = k;

                  Connect(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler(processor_details::on_tool_selector_item_collapsed), 0, this);

                  break;
                }
              }

              break;
            }

            if (tool_selector->ItemHasChildren(j)) {
              id_stack.push(j);
            }
          }
        }
      }

      /**
       * \param[in] range the pairs of categories and tool types to display
       **/
      void processor_details::populate_tool_list(type_registry::tool_sequence const& range) {

        tool_selector->DeleteChildren(tool_selector->GetRootItem());

        BOOST_FOREACH(type_registry::tool_sequence::value_type i, range) {
          wxString     current_category_name = wxString(i.first.get_name().c_str(), wxConvLocal);
          wxTreeItemId root                  = tool_selector->GetRootItem();
         
          /* Use of GetLastChild() because ItemHasChildren can return true when there are no children */
          if (tool_selector->GetLastChild(root).IsOk()) {
            wxTreeItemId last_category = tool_selector->GetLastChild(root);
           
            if (tool_selector->GetItemText(last_category) != current_category_name) {
              /* Add category */
              last_category = tool_selector->AppendItem(root, current_category_name);
            }
           
            tool_selector->AppendItem(last_category, wxString(i.second->get_name().c_str(), wxConvLocal));
          }
          else {
            wxTreeItemId last_category = tool_selector->AppendItem(root, current_category_name);
         
            tool_selector->AppendItem(last_category, wxString(i.second->get_name().c_str(), wxConvLocal));
          }
        }

        Refresh();
      }
    }
  }
}
