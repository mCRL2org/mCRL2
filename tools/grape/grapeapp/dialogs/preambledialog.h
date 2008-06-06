// Author(s): VitaminB100
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
        wxTextCtrl *m_parameters; /**< A text control containing the parameter initializations.*/
        wxTextCtrl *m_localvars; /** A text control containing the local variable declarations. */

        /** Default constructor. */
        grape_preamble_dialog();
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
    };
  }
}

#endif // GRAPE_PREAMBLE_DIALOG_H
