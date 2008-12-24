// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file labeldialog.h
//
// Declares the label dialog.

#ifndef grape_label_dialog_H
#define grape_label_dialog_H

#include <wx/dialog.h>
#include <wx/grid.h>
#include <wx/notebook.h>
#include <wx/event.h>

#include "label.h"

class wxTextCtrl;

using namespace grape::libgrape;

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short A class for editting a label with a dialog.
     */
    class grape_label_dialog : public wxDialog
    {
      private:
        wxGrid         *m_var_decls_grid;   /**< Grid shown in the dialog, used for variable declarations.*/ 
        wxTextCtrl     *m_condition_input;  /**< Input shown in the dialog, used for condition.*/
        wxTextCtrl     *m_timestamp_input;  /**< Input shown in the dialog, used for timestamp.*/
        
        wxGrid         *m_multiaction_grid; /**< Grid shown in the dialog, used for a multi action.*/
        wxGrid         *m_timestamp_grid;   /**< Grid shown in the dialog, used for time.*/
        wxGrid         *m_var_updates_grid; /**< Grid shown in the dialog, used for variable updates.*/
         
        wxStaticText   *m_preview_text;     /**< Preview text.*/
         
        wxNotebook     *m_notebook;         /**< Notebook shown in the dialog, used for all the grids.*/
        
        wxNotebookPage *m_var_decls_page;   /**< Notebook variable declaration page used in the dialog.*/
        wxNotebookPage *m_condition_page;   /**< Notebook condition page used in the dialog.*/
        wxNotebookPage *m_multiaction_page; /**< Notebook multiaction page used in the dialog.*/
        wxNotebookPage *m_timestamp_page;   /**< Notebook timestamp page used in the dialog.*/
        wxNotebookPage *m_var_updates_page; /**< Notebook variable update page used in the dialog.*/
        
        label          *m_label;            /**< label for temporarily storage.*/
        /** Default constructor. */
        grape_label_dialog();
        
        DECLARE_EVENT_TABLE();		/**< The event table of this grid. */
      public:
        /**
        * Constructor.
        * @param p_label A pointer to the label which is being editted.
        */
        grape_label_dialog( const wxString &p_text );

        /** Destructor. */
        ~grape_label_dialog();

        /**
         * Change var decl grid event handler.
         * Appending grid rows.
         * @param p_event The generated event.
         */
        void event_change_var_decls_text( wxGridEvent &p_event );
        
        /**
         * Change var update grid event handler.
         * Appending grid rows.
         * @param p_event The generated event.
         */
        void event_change_var_updates_text( wxGridEvent &p_event );
        
        /**
         * Change multi action grid event handler.
         * Appending grid rows.
         * @param p_event The generated event.
         */
        void event_change_multiaction_text( wxGridEvent &p_event );

        /**
         * Change condition grid event handler.
         * Appending grid rows.
         * @param p_event The generated event.
         */
        void event_change_condition_text( wxCommandEvent &p_event );
        
        /**
         * Change timestamp grid event handler.
         * Appending grid rows.
         * @param p_event The generated event.
         */
        void event_change_timestamp_text( wxCommandEvent &p_event );
        
        /**
         * Shows the dialog.
         * @param p_label Shall contain the value of the label.
         * @return @c true if the user pressed OK. @c false if the user cancelled the dialog.
         */
        bool show_modal( wxString &p_text );
        
        void update_preview();
    };
  }
}

#endif // grape_label_dialog_H
