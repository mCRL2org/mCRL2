// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file preambledialog.h
//
// Declares the preamble dialog.

#ifndef GRAPE_PREAMBLE_DIALOG_H
#define GRAPE_PREAMBLE_DIALOG_H

#include <wx/dialog.h>
#include <wx/grid.h>
#include <wx/event.h>

#include "preamble.h"

class wxTextCtrl;

using namespace grape::libgrape;

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short A class for editting a preamble with a dialog.
     */
    class grape_preamble_dialog : public wxDialog
    {
      private:
        //wxTextCtrl *m_parameters; /**< A text control containing the parameter initializations.*/
        //wxTextCtrl *m_localvars; /** A text control containing the local variable declarations. */
        wxGrid        *m_parameter_grid; /**< Grid shown in the dialog, used for parameter declarations.*/
        wxGrid        *m_localvar_grid; /**< Grid shown in the dialog, used for localvar declarations.*/

        /** Default constructor. */
        grape_preamble_dialog();

        DECLARE_EVENT_TABLE()		/**< The event table of this grid. */
      public:
        /**
        * Constructor.
        * @param p_preamble A pointer to the preamble which is being editted.
        */
        grape_preamble_dialog( preamble *p_preamble );

        /** Destructor. */
        ~grape_preamble_dialog();

        /** @return The value of the parameter declaration input box. */
        wxString get_parameter_declarations() const;

        /** @return The value of the local variable declaration input box. */
        wxString get_local_variable_declarations() const;


        /**
         * Check wether the text is valid.
         * If not, the OK button is disabled
         */
        void check_text();

        /**
         * Change parameter grid event handler.
         * Appending grid rows.
         * @param p_event The generated event.
         */
        void event_change_parameter_text( wxGridEvent &p_event );

        /**
         * Change localvar grid event handler.
         * Appending grid rows.
         * @param p_event The generated event.
         */
        void event_change_localvar_text( wxGridEvent &p_event );
    };
  }
}

#endif // GRAPE_PREAMBLE_DIALOG_H
