// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file referencedialog.h
//
// Declares the reference selection dialog.

#ifndef grape_reference_dialog_H
#define grape_reference_dialog_H

#include <wx/dialog.h>
#include <wx/grid.h>
#include <wx/event.h>

#include "architecturereference.h"
#include "processreference.h"
#include "specification.h"

class wxComboBox;
class wxPanel;
class wxTextCtrl;

using namespace grape::libgrape;

WX_DECLARE_HASH_MAP( int, int, wxIntegerHash, wxIntegerEqual, diagramhash );

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short A class showing a dialog with a combobox and text edit.
     */
    class grape_reference_dialog : protected wxDialog
    {
      private:
        wxComboBox    *m_combo; /**< Combobox with all diagrams. */
        diagramhash   m_pos2diagramid; /**< Maps the combobox positions to references. */
        wxGrid        *m_grid; /**< Grid shown in the dialog, used for parameter declarations.*/
        /**
         * Private default constructor.
         */
        grape_reference_dialog();

        /**
         * Initializes the dialog to choose process diagrams.
         * @param p_diagram The diagram the reference to be editted points to. This will be preselected
         * in the combobox.
         * @param p_list_of_varupdate varupdate pointer list
         * @param p_spec The specification containing the diagrams the reference @p p_ref could point to.
         */
        void init_for_processes( diagram *p_diagram, list_of_varupdate p_list_of_varupdate, grape_specification *p_spec );

        /**
         * Initializes the dialog, using the panel @p p_panel
         * @param p_panel The panel used to show above the OK and Cancel buttons.
         */
        void init( wxPanel *p_panel );

        DECLARE_EVENT_TABLE();		/**< The event table of this grid. */

      public:

        /**
         * Constructor.
         * @param p_ref The process reference to edit.
         * @param p_spec The specification containing the diagrams the reference @p p_ref could point to.
         */
        grape_reference_dialog( process_reference *p_ref, grape_specification *p_spec );

        /**
         * Constructor.
         * @param p_ref The reference state to edit.
         * @param p_spec The specification containing the diagrams the reference @p p_ref could point to.
         */
        grape_reference_dialog( reference_state *p_ref , grape_specification *p_spec );

        /**
         * Constructor.
         * @param p_ref The architecture reference to edit.
         * @param p_spec The specification containing the diagrams the reference @p p_ref could point to.
         */
        grape_reference_dialog( architecture_reference *p_ref , grape_specification *p_spec );

        /**
         * Default destructor.
         */
        ~grape_reference_dialog();

        /**
         * Shows the dialog.
         * @return @c true if the user pressed OK. @c false if the user cancelled the dialog.
         */
        bool show_modal();

        /** @return The ID of the diagram which is selected. */
        int get_diagram_id();

        /** @return The name of the reference. */
        wxString get_diagram_name() const;

        /** @return The variable initializations entered in the input field. */
        wxString get_initializations() const;
        
        /**
         * Check wether the text is valid.
         * If not, the OK button is disabled
         */
        void check_text();         

	    /**
	     * Change grid event handler.
	     * Appending grid rows.
	     * @param p_event The generated event.
    	 */
        void event_change_text( wxGridEvent &p_event );
    };
  }
}

#endif // grape_reference_dialog_H
