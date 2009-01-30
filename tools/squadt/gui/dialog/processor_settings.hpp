// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gui/dialog/processor_settings.h
/// \brief Add your file description here.

#ifndef GUI_DIALOG_PROCESSOR_H
#define GUI_DIALOG_PROCESSOR_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/treectrl.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>

#include "base.hpp"
#include "../../type_registry.hpp"

namespace squadt {
  namespace GUI {
    namespace dialog {

      /** \brief Base class for processor level dialogs */
      class processor : public dialog::basic {
        public:

          /** \brief Constructor */
          processor(wxWindow* p, wxString t) : dialog::basic(p, t, wxSize(800, 375)) {
          }

          /** \brief Virtual destructor */
          virtual ~processor() {};
      };

      class processor_details : public dialog::processor {
        friend class squadt::GUI::project;

        private:

          /** \brief Tree control for tool selection */
          wxTreeCtrl*                             m_tool_selector;

          /** \brief Text field that contains a name */
          wxStaticText*                           m_name;

          /** \brief The list of input objects */
          wxListCtrl*                             m_input_objects;

          /** \brief The list of output objects */
          wxListCtrl*                             m_output_objects;

          /** \brief The processor for which to display information */
          boost::shared_ptr < squadt::processor > m_target_processor;

          /** \brief Whether tools are selectable or not */
          bool                                    m_tools_selectable;

          /** \brief Selected tool id, because selections are lost when a part containing it is collapsed */
          wxTreeItemId                            m_selected_tool;

        private:

          /** \brief Helper function that places the widgets */
          void build(boost::shared_ptr< squadt::processor::object_descriptor > const&);

          void show_inputs();

          void show_outputs();

          /** \brief Helper function for filling the tool list with tools */
          void populate_tool_list(type_registry::tool_sequence const&);

          /** \brief Event handler for when something is selected in the tool_selector control */
          void on_tool_selector_item_selected(wxTreeEvent&);

          /** \brief Event handler for when something is about to be selected in the tool_selector control */
          void on_tool_selector_item_select(wxTreeEvent&);

          /** \brief Event handler for when something is about to be collapsed in the tool_selector control */
          void on_tool_selector_item_collapsed(wxTreeEvent&);

          /** \brief Select a tool by its name */
          void select_tool(tipi::tool::capabilities::input_configuration const*, std::string const&);

        public:

          /** \brief Constructor */
          processor_details(wxWindow*, boost::shared_ptr < squadt::processor >, boost::shared_ptr< squadt::processor::object_descriptor > const& object);

          /** \brief Whether tools in the tool list are selectable */
          inline void allow_tool_selection(bool b) {
            m_tools_selectable = b;
          }

          /** \brief Show or hide the tool selector */
          inline void show_tool_selector(bool b) {
            m_tool_selector->Show(b);
            Update();
            Refresh();
          }

          /** \brief Show or hide the list of input objects */
          inline void show_input_objects(bool b) {
            if (m_input_objects != 0) {
              m_input_objects->Show(b);
            }
          }

          /** \brief Show or hide the list of output objects */
          inline void show_output_objects(bool b) {
            if (m_output_objects != 0) {
              m_output_objects->Show(b);
            }
          }

          /** \brief Sets the value of the name field */
          inline void set_name(wxString const& n) {
            m_name->SetLabel(n);
          }

          inline wxString get_name() {
            return (m_name->GetLabel());
          }
      };
    }
  }
}

#endif

