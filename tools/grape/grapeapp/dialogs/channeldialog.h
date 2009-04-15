// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file channeldialog.h
//
// Declares the channel dialog.

#ifndef GRAPE_CHANNEL_DIALOG_H
#define GRAPE_CHANNEL_DIALOG_H

#include <wx/dialog.h>
#include <wx/event.h>

#include "channel.h"

class wxTextCtrl;

using namespace grape::libgrape;

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short A class showing a dialog with two explaining text and text fields
     */
    class grape_channel_dlg : protected wxDialog
    {
      private:
        wxTextCtrl       *m_name_input;         /**< The name field shown in the dialog. */
        wxTextCtrl       *m_rename_input;       /**< The rename field shown in the dialog. */
        wxComboBox       *m_combobox;           /**< The property combobox shown in the dialog. */

        /**
         * Private default constructor.
         */
        grape_channel_dlg();
        
        /**
         * Update validation event handler.
         * @param p_event The generated event.
         */
        void event_update_validation( wxCommandEvent &p_event );

        /**
         * Update validation function
         * @return Returns whether all input fields are valid
         */
        bool update_validation();

        DECLARE_EVENT_TABLE();		/**< The event table of this grid. */
      public:
        /**
         * Default destructor.
         */
        ~grape_channel_dlg();

        /**
         * Constructor.
         * @param p_channel A pointer to the label which is being editted.
         */
        grape_channel_dlg( channel &p_channel );

        /**
         * Shows the dialog.
         * @param p_channel Shall contain the value of the input box.
         * @return @c true if the user pressed OK. @c false if the user cancelled the dialog.
         */
        bool show_modal( channel &p_channel );
    };
  }
} 

#endif // GRAPE_CHANNEL_DIALOG_H
