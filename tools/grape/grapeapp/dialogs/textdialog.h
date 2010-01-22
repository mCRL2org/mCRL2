// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file textdialog.h
//
// Declares the rename dialog.

#ifndef GRAPE_TEXT_DIALOG_H
#define GRAPE_TEXT_DIALOG_H

#include <wx/dialog.h>

class wxTextCtrl;

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short A class showing a dialog with an explaining text and text field (eventually multiline).
     */
    class grape_text_dlg : public wxDialog
    {
      private:
        wxTextCtrl *m_input; /**< The input field shown in the dialog. */

        /**
         * Private default constructor.
         */
        grape_text_dlg();

        /**
         * Update validation event handler.
         * @param p_event The generated event.
         */
        void event_update_validation( wxCommandEvent &p_event );

        /**
         * Update validation function
         * @return Returns whether the input field is valid
         */
        bool update_validation();

        DECLARE_EVENT_TABLE()		/**< The event table of this dialog. */
      public:
        /**
         * Constructor.
         * @param p_title The window title.
         * @param p_message The text shown above the rename dialog.
         * @param p_initial_text The initial text shown in the input box.
         * @param p_multiline @c true if the dialog should show a multiline text edit.
         */
        grape_text_dlg( const wxString &p_title, const wxString &p_message, const wxString &p_initial_text, bool p_multiline = false );

        /**
         * Default destructor.
         */
        ~grape_text_dlg();

        /**
         * Shows the dialog.
         * @param p_text Shall contain the value of the input box.
         * @return @c true if the user pressed OK. @c false if the user cancelled the dialog.
         */
        bool show_modal( wxString &p_text );
    };
  }
}

#endif // GRAPE_TEXT_DIALOG_H
