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

namespace squadt {
  namespace GUI {
    namespace dialog {
      
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
          wxString       project_store;

          /** \brief Tree control for tool selection */
          wxTreeCtrl*    tool_selector;

          /** \brief Text field that contains a name */
          wxTextCtrl*    name;

          /** \brief The list of input objects */
          wxListCtrl*    input_objects;

          /** \brief The list of output objects */
          wxListCtrl*    output_objects;

        private:
        
          /** \brief Helper function that places the widgets */
          void build();

          /** \brief Helper function for filling the tool list with tools */
          void populate_tool_list(storage_format);

          /** \brief Helper function that actually adds a tool to the list of tools */
          void add_to_tool_list(const miscellaneous::tool_selection_helper::tools_by_category::value_type&);

        public:

          /** \brief Constructor */
          processor_details(wxWindow*, wxString);

          /** \brief Constructor sets the name field */
          inline void set_name(wxString);
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
       * @param n the new name
       **/
      inline void processor_details::set_name(wxString n) {
        name->SetValue(n);
      }
    }
  }
}

#endif

