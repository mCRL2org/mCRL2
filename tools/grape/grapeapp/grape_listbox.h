// Author(s): Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file grape_listbox.cpp
//
// Implements the listbox class to list diagrams.

#ifndef GRAPEAPP_GRAPE_LISTBOX_H
#define GRAPEAPP_GRAPE_LISTBOX_H

#include <wx/wx.h>

namespace grape
{
  namespace grapeapp
  {
    class grape_frame;

     /**
      * \short An wxListBox, used when listing diagrams.
      */
    class grape_listbox : public wxListBox
    {
      private:
	grape_frame*	m_main_frame;	/**< A pointer to the main grape frame. */
        int		m_diagram_type;	/**< Identifier of the diagram_type. */

        DECLARE_EVENT_TABLE();		/**< The event table of this listbox. */

      public:
        /**
         * Initialization constructor.
         * Sets parent and initializes grape_listbox with specified arguments.
         * @param p_parent Pointer to parent window.
         * @param p_id Window id of the main frame.
         * @param p_main_frame The main frame containing this listbox.
         */
        grape_listbox( wxWindow *p_parent, wxWindowID p_id, grape_frame *p_main_frame );

        /**
	 * Default destructor.
	 * Frees allocated memory.
	 */
        ~grape_listbox( void );

	/**
	 * Delete diagram event handler.
	 * Processes the delete diagram event.
	 * @param p_event The generated event.
	 */
	void event_delete( wxKeyEvent &p_event );

#ifndef __WINDOWS__
	/**
	 * select diagram event handler.
	 * Processes the select listbox event.
	 * @param p_event The generated event.
	 */
	void event_click( wxMouseEvent &p_event );
#endif

	/**
	 * Enter listbox event handler.
	 * Processes the select diagram event.
	 * @param p_event The generated event.
	 */
	void event_select( wxCommandEvent &p_event );
    };
  } // namespace grapeapp
} // namespace grape

#endif // GRAPEAPP_GRAPE_LISTBOX_H
