// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/dialog/processor_settings.cpp
/// \brief Add your file description here.

#include "wx.hpp" // precompiled headers

#include <cmath>
#include <ctime>
#include <stack>

#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "gui/main.hpp"
#include "gui/dialog/processor_settings.hpp"

#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/notebook.h>
#include <wx/sizer.h>

inline std::string filename(boost::filesystem::path const& p) {
#if (103500 < BOOST_VERSION)
  return p.filename();
#else
  return p.leaf();
#endif
}

namespace squadt {
  namespace GUI {
    namespace dialog {

      /** \brief Helper function to avoid casts (necessary for MSVC compilation) */
      inline double log(unsigned long const& c) {
        return (std::log(static_cast < long double > (c)));
      }

      /** \brief Helper function to avoid casts (necessary for MSVC compilation) */
      inline double pow(unsigned long const& c, int const& p) {
        return (std::pow(static_cast < long double > (c), p));
      }

      /** \brief SI prefixes for binary multiples */
      const char* prefixes_for_binary_multiples[7] = { "", "Ki", "Mi", "Gi", "Ti", "Pi", "Ei" };

      boost::format size_format("%s %sB");

      /**
       * @param o a pointer to the parent window
       * @param s path to the project store
       * @param p the processor of which to display data
       **/
      processor_details::processor_details(wxWindow* o, boost::shared_ptr< squadt::processor > p,
                                                        boost::shared_ptr< squadt::processor::object_descriptor > const& object) :
                                                dialog::processor(o, wxT("Generation details")),
                                                m_input_objects(0), m_output_objects(0),
                                                m_target_processor(p), m_tools_selectable(true) {
        build(object);

        GetSizer()->Show(button_cancel, false, true);
        GetSizer()->Layout();

        Connect(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(processor_details::on_tool_selector_item_selected), 0, this);
        Connect(wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler(processor_details::on_tool_selector_item_select), 0, this);
      }

      void processor_details::show_inputs() {
        using namespace boost::filesystem;

        m_input_objects->ClearAll();
        m_input_objects->InsertColumn(0, wxT("Name"));
        m_input_objects->InsertColumn(1, wxT("Size"), wxLIST_FORMAT_RIGHT);
        m_input_objects->InsertColumn(2, wxT("Date"));

        long row = 0;

        boost::iterator_range < squadt::processor::input_object_iterator > input_range(m_target_processor->get_input_iterators());

        BOOST_FOREACH(boost::shared_ptr < squadt::processor::object_descriptor > i, input_range) {
          if (i.get() != 0) {
            path path_to_file(i->get_location());

            m_input_objects->InsertItem(row, wxString(path_to_file.leaf().c_str(), wxConvLocal));

            if (exists(path_to_file)) {
              unsigned long size       = file_size(path_to_file);
              wxDateTime    write_time(last_write_time(path_to_file));
              unsigned char magnitude  = static_cast < unsigned char > (floor(log(size) / log(1024)));

              size = (unsigned long) (size / pow(1024, magnitude));

              m_input_objects->SetItem(row, 1, wxString(str((size_format % size %
                         prefixes_for_binary_multiples[magnitude])).c_str(), wxConvLocal));

              m_input_objects->SetItem(row, 2, write_time.Format(wxT("%x %X")));
            }

            ++row;
          }
        }

        m_input_objects->SetColumnWidth(0, wxLIST_AUTOSIZE);
        m_input_objects->SetColumnWidth(1, wxLIST_AUTOSIZE);
        m_input_objects->SetColumnWidth(2, wxLIST_AUTOSIZE);
      }

      void processor_details::show_outputs() {
        using namespace boost::filesystem;

        m_output_objects->ClearAll();
        m_output_objects->InsertColumn(0, wxT("Name"));
        m_output_objects->InsertColumn(1, wxT("Size"), wxLIST_FORMAT_RIGHT);
        m_output_objects->InsertColumn(2, wxT("Date"));
        m_output_objects->InsertColumn(3, wxT("Type"));

        long row = 0;

        boost::iterator_range < squadt::processor::output_object_iterator > output_range(m_target_processor->get_output_iterators());

        BOOST_FOREACH(boost::shared_ptr < squadt::processor::object_descriptor > o, output_range) {
          if (o.get() != 0) {
            path path_to_file(o->get_location());

            m_output_objects->InsertItem(row, wxString(path_to_file.leaf().c_str(), wxConvLocal));

            if (exists(path_to_file)) {
              unsigned long size       = file_size(path_to_file);
              wxDateTime    write_time(last_write_time(path_to_file));
              unsigned char magnitude  = static_cast < unsigned char > (floor(log(size) / log(1024)));

              size = (unsigned long) (size / pow(1024, magnitude));

              m_output_objects->SetItem(row, 1, wxString(str((size_format % size %
                        prefixes_for_binary_multiples[magnitude])).c_str(), wxConvLocal));

              m_output_objects->SetItem(row, 2, write_time.Format(wxT("%x %X")));
              m_output_objects->SetItem(row, 3, wxString(o->get_format().string().c_str(), wxConvLocal));
            }

            ++row;
          }
        }

        m_output_objects->SetColumnWidth(0, wxLIST_AUTOSIZE);
        m_output_objects->SetColumnWidth(1, wxLIST_AUTOSIZE);
        m_output_objects->SetColumnWidth(2, wxLIST_AUTOSIZE);
      }

      void processor_details::build(boost::shared_ptr< squadt::processor::object_descriptor > const& object) {
        using namespace boost::filesystem;

        wxBoxSizer*       s = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer*       t = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer*       u = new wxBoxSizer(wxHORIZONTAL);

        m_name = new wxStaticText(main_panel, wxID_ANY, wxString(filename(object->get_location()).c_str(), wxConvLocal));
        u->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Main input: ")), 0, wxALIGN_CENTRE);
        u->AddSpacer(5);
        u->Add(m_name, 0, wxALIGN_CENTRE);

        if (m_target_processor->number_of_inputs() == 0) {
          wxComboBox* m_type = new wxComboBox(main_panel, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, 0, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT);

          std::set < build_system::storage_format > types(global_build_system.get_type_registry().get_mime_types());

          for (std::set < build_system::storage_format >::const_iterator i = types.begin(); i != types.end(); ++i) {
            m_type->Append(wxString(i->string().c_str(), wxConvLocal));
          }

          m_type->SetStringSelection(wxString(object->get_format().string().c_str(), wxConvLocal));

          u->AddStretchSpacer(3);
          u->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Type: ")), 0, wxALIGN_CENTRE);
          u->AddSpacer(5);
          u->Add(m_type, 2, wxEXPAND|wxALIGN_CENTRE);

          // Helper class for wxEventHandling
          class wx_handler : public wxEvtHandler {
            private:

              processor_details&                                        m_window;
              boost::shared_ptr< squadt::processor::object_descriptor > m_object;

            public:

              void on_main_type_change(wxCommandEvent& e) {
                m_object->get_generator()->change_format(*m_object, build_system::storage_format(std::string(e.GetString().fn_str())));

                m_window.show_outputs();
              }

              wx_handler(processor_details& w, boost::shared_ptr< squadt::processor::object_descriptor > const& object) : m_window(w), m_object(object) {
              }
          };

          wx_handler* handler = new wx_handler(*this, object);

          // It is undocumented whether wxWidgets takes care of destruction
          PushEventHandler(handler);

          handler->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(wx_handler::on_main_type_change), 0, handler);
        }

        wxNotebook* notebook = new wxNotebook(main_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

        t->Add(u, 0, wxEXPAND);
        t->AddSpacer(5);
        t->Add(notebook, 1, wxEXPAND);

        if (0 < m_target_processor->number_of_inputs()) {
          m_input_objects = new wxListCtrl(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                          wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_SINGLE_SEL|wxLC_VRULES|wxLC_HRULES);
          show_inputs();

          notebook->AddPage(m_input_objects, wxT("Input"));
        }

        if (0 < m_target_processor->number_of_outputs()) {
          m_output_objects = new wxListCtrl(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                          wxLC_REPORT|wxLC_ALIGN_LEFT|wxLC_SINGLE_SEL|wxLC_VRULES|wxLC_HRULES);

          show_outputs();

          notebook->AddPage(m_output_objects, wxT("Output"));
        }

        m_tool_selector = new wxTreeCtrl(main_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                        wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_SINGLE|wxSUNKEN_BORDER);

        m_tool_selector->AddRoot(wxEmptyString);

        s->AddSpacer(20);
        if (0 < m_target_processor->number_of_inputs()) {
          s->Add(m_tool_selector, 3, wxEXPAND|wxTOP|wxBOTTOM, 10);
          s->AddSpacer(10);
        }
        s->Add(t, 5, wxEXPAND|wxTOP|wxBOTTOM, 10);
        s->AddSpacer(20);

        main_panel->SetSizer(s);

        Layout();
      }

      void processor_details::on_tool_selector_item_selected(wxTreeEvent& e) {
        if (m_tool_selector->GetItemParent(e.GetItem()) == m_tool_selector->GetRootItem()) {
          m_tool_selector->Toggle(e.GetItem());

          m_tool_selector->UnselectItem(e.GetItem());
        }
      }

      void processor_details::on_tool_selector_item_select(wxTreeEvent& e) {
        if (m_tool_selector->GetItemParent(e.GetItem()) == m_tool_selector->GetRootItem() || (!m_tools_selectable && (e.GetItem() != m_selected_tool))) {
          e.Veto();
        }
      }

      void processor_details::on_tool_selector_item_collapsed(wxTreeEvent& e) {
        m_tool_selector->EnsureVisible(m_selected_tool);
        m_tool_selector->SelectItem(m_selected_tool);
        m_tool_selector->ScrollTo(m_selected_tool);
      }

      void processor_details::select_tool(tipi::tool::capabilities::input_configuration const* configuration, std::string const& name) {
        wxString category(configuration->get_category().get_name().c_str(), wxConvLocal);

        std::stack < wxTreeItemId > id_stack;

        id_stack.push(m_tool_selector->GetRootItem());

        /* Recursively search the tree to find the node to select (if only there where a search method) */
        while (!id_stack.empty()) {
          wxTreeItemIdValue cookie;             // For wxTreeCtrl traversal
          wxTreeItemId      c = id_stack.top(); // The current node

          id_stack.pop();

          for (wxTreeItemId j = m_tool_selector->GetFirstChild(c, cookie);
                                    j.IsOk(); j = m_tool_selector->GetNextChild(c, cookie)) {

            if (m_tool_selector->GetItemText(j) == category) {
              wxTreeItemIdValue cookie1; // For wxTreeCtrl traversal

              /* Found category */
              for (wxTreeItemId k = m_tool_selector->GetFirstChild(j, cookie1);
                                    k.IsOk(); k = m_tool_selector->GetNextChild(j, cookie1)) {

                if (m_tool_selector->GetItemText(k) == wxString(name.c_str(), wxConvLocal)) {
                  /* Found tool */
                  m_tool_selector->SelectItem(k);
                  m_tool_selector->EnsureVisible(k);
                  m_tool_selector->ScrollTo(k);

                  m_selected_tool = k;

                  Connect(wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler(processor_details::on_tool_selector_item_collapsed), 0, this);

                  break;
                }
              }

              break;
            }

            if (m_tool_selector->ItemHasChildren(j)) {
              id_stack.push(j);
            }
          }
        }
      }

      /**
       * \param[in] range the pairs of categories and tool types to display
       **/
      void processor_details::populate_tool_list(type_registry::tool_sequence const& range) {

        m_tool_selector->DeleteChildren(m_tool_selector->GetRootItem());

        BOOST_FOREACH(type_registry::tool_sequence::value_type i, range) {
          wxString     current_category_name = wxString(i.first.get_name().c_str(), wxConvLocal);
          wxTreeItemId root                  = m_tool_selector->GetRootItem();

          /* Use of GetLastChild() because ItemHasChildren() can return true when there are no children */
          if (m_tool_selector->GetLastChild(root).IsOk()) {
            wxTreeItemId last_category = m_tool_selector->GetLastChild(root);

            if (m_tool_selector->GetItemText(last_category) != current_category_name) {
              /* Add category */
              last_category = m_tool_selector->AppendItem(root, current_category_name);
            }

            m_tool_selector->AppendItem(last_category, wxString(i.second->get_name().c_str(), wxConvLocal));
          }
          else {
            wxTreeItemId last_category = m_tool_selector->AppendItem(root, current_category_name);

            m_tool_selector->AppendItem(last_category, wxString(i.second->get_name().c_str(), wxConvLocal));
          }
        }

        Refresh();
        Update();
      }
    }
  }
}
