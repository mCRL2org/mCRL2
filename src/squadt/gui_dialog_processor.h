#ifndef GUI_DIALOG_PROCESSOR_H
#define GUI_DIALOG_PROCESSOR_H

#include <wx/wx.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>
#include <wx/notebook.h>

#include "gui_dialog_base.h"
#include "gui_miscellaneous.h"

namespace squadt {
  namespace GUI {
    namespace dialog {

      using squadt::miscellaneous::type_registry;
      
      /** \brief Base class for processor level dialogs */
      class processor : public dialog::basic {
        public:

          /** \brief Constructor */
          processor(wxWindow*, wxString);

          /** \brief Virtual destructor */
          virtual ~processor() = 0;
      };

      class processor_details : public dialog::processor {
        friend class squadt::GUI::project;

        private:

          /** \brief Path to the project store where input/output objects are stored */
          wxString                project_store;

          /** \brief Tree control for tool selection */
          wxTreeCtrl*             tool_selector;

          /** \brief Text field that contains a name */
          wxStaticText*           name;

          /** \brief The list of input objects */
          wxListCtrl*             input_objects;

          /** \brief The list of output objects */
          wxListCtrl*             output_objects;

          /** \brief The processor for which to display information */
          squadt::processor::sptr target_processor;

          /** \brief Whether tools are selectable or not */
          bool                    tools_selectable;

          /** \brief Selected tool id, because selections are lost when a part containing it is collapsed */
          wxTreeItemId            selected_tool;

        private:
        
          /** \brief Helper function that places the widgets */
          void build();

          /** \brief Helper function for filling the tool list with tools */
          void populate_tool_list(type_registry::tool_sequence const&);

          /** \brief Event handler for when something is selected in the tool_selector control */
          void on_tool_selector_item_selected(wxTreeEvent& e);

          /** \brief Event handler for when something is about to be selected in the tool_selector control */
          void on_tool_selector_item_select(wxTreeEvent& e);

          /** \brief Event handler for when something is about to be collapsed in the tool_selector control */
          void on_tool_selector_item_collapsed(wxTreeEvent& e);

          /** \brief Select a tool by its name */
          void select_tool(sip::tool::capabilities::input_combination const*, std::string const&);

        public:

          /** \brief Constructor */
          processor_details(wxWindow*, wxString, squadt::processor::sptr);

          /** \brief Whether tools in the tool list are selectable */
          inline void allow_tool_selection(bool b);

          /** \brief Show or hide the tool selector */
          inline void show_tool_selector(bool b);

          /** \brief Show or hide the list of input objects */
          inline void show_input_objects(bool b);

          /** \brief Show or hide the list of output objects */
          inline void show_output_objects(bool b);

          /** \brief Sets the value of the name field */
          inline void set_name(wxString);

          /** \brief Gets the value of the name field */
          inline wxString get_name();
      };

      /**
       * @param p the parent window
       * @param t the title for the window
       **/
      inline processor::processor(wxWindow* p, wxString t) : dialog::basic(p, t, wxSize(650, 375)) {
      }

      inline processor::~processor() {
      }

      /**
       * @param b whether the tool list should allow selection
       **/
      inline void processor_details::allow_tool_selection(bool b) {
        tools_selectable = b;
      }

      /**
       * @param b whether to show tool_selector
       **/
      inline void processor_details::show_tool_selector(bool b) {
        tool_selector->Show(b);
      }

      /**
       * @param b whether to show input_objects
       **/
      inline void processor_details::show_input_objects(bool b) {
        input_objects->Show(b);
      }

      /**
       * @param[in] b whether to show output_objects
       **/
      inline void processor_details::show_output_objects(bool b) {
        output_objects->Show(b);
      }

      /**
       * @param[in] n the new name
       **/
      inline void processor_details::set_name(wxString n) {
        name->SetLabel(n);
      }

      inline wxString processor_details::get_name() {
        return (name->GetLabel());
      }
    }
  }
}

#endif

