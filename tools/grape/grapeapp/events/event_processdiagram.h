// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_processdiagram.h
//
// Declares GraPE events for process diagrams.

#ifndef GRAPE_EVENT_PROCESSDIAGRAM_H
#define GRAPE_EVENT_PROCESSDIAGRAM_H

#include "event_base.h"

namespace grape
{
  using libgrape::preamble;

  namespace grapeapp
  {
    /**
     * \short Represents the add process diagram event.
     */
    class grape_event_add_process_diagram : public grape_event_base
    {
      private:
        unsigned int m_proc;  /**< Identifier of the created process diagram. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_add_process_diagram( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_process_diagram( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };

    /**
     * \short Represents the change preamble event.
     */
    class grape_event_change_preamble : public grape_event_base
    {
      private:
        preamble *m_preamble;
        wxString m_old_parameter_decls; /**< The old parameter declarations. */
        wxString m_new_parameter_decls; /**< The new parameter declarations. */
        wxString m_old_local_var_decls; /**< The old local variable updates. */
        wxString m_new_local_var_decls; /**< The new local variable updates. */
        bool m_ok_pressed; /** Contains @c true if the dialog was clicked with OK. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_preamble The to be renamed preamble.
         */
        grape_event_change_preamble( grape_frame *p_main_frame, preamble *p_preamble );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_preamble( void );

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do( void );

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo( void );
    };
  }
}
#endif // GRAPE_EVENT_PROCESSDIAGRAM_H
