// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parameterdialog.h
//
// Declares the parameter initialisation dialog.

#ifndef GRAPE_PARAMETER_DIALOG_H
#define GRAPE_PARAMETER_DIALOG_H

#include <wx/dialog.h>

#include "preamble.h"

#define   GRAPE_PARAMETER_DIALOG_COMBO      2002

class wxComboBox;
class wxPanel;
class wxTextCtrl;

using namespace grape::libgrape;

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short A class showing a dialog with a combobox and text edit.
     */
    class grape_parameter_dialog : protected wxDialog
    {
      private:
        wxComboBox    *m_combo;         /**< Combobox with all parameters. */
        wxTextCtrl    *m_input;         /**< The input field shown in the dialog, used for parameter initialisation.*/
        list_of_decl_init m_init;       /**< The parameter initialisation. */
        int           m_combo_current;  /**< The current selected value in the combobox.*/

        /**
         * Private default constructor.
         */
        grape_parameter_dialog();

        DECLARE_EVENT_TABLE();

      public:

        /**
         * Constructor.
         * @param p_parameter_declarations The list of parameter declarations to show in the dialog.
         */
        grape_parameter_dialog(list_of_decl &p_parameter_declarations);

        /**
         * Default destructor.
         */
        ~grape_parameter_dialog();

        /**
         * Shows the dialog.
         * @return @c true if the user pressed OK. @c false if the user cancelled the dialog.
         */
        bool show_modal();

        /**
         * Returns the initialisations.
         * @return The list of parameter declarations, along with their initialisation.
         */
        list_of_decl_init get_initialisations() const;

        /**
         * Combobox selection change event handler.
         * @param p_event The generated event.
         */
        void event_combo(wxCommandEvent &p_event);

        /**
         * Ok button event handler.
         * @param p_event The generated event.
         */
        void event_ok(wxCommandEvent &p_event);
    };
  }
}

#endif // GRAPE_SELECT_DIALOG_H
