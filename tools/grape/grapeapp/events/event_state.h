// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_state.h
//
// Declares GraPE events for ....

#ifndef GRAPE_EVENT_STATE_H
#define GRAPE_EVENT_STATE_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the add state event.
     */
    class grape_event_add_state : public grape_event_base
    {
      private:
        unsigned int            m_state; /**< The id of the to be created state. */
        coordinate      m_coord; /**< the coordinate that was clicked */
        float           m_def_state_width;  /**< The default width of a state. */
        float           m_def_state_height; /**< The default height of a state. */
        unsigned int            m_in_diagram; /**< The id of the diagram the state will have been added to. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate where the state will be added.
         */
        grape_event_add_state( grape_frame *p_main_frame, coordinate &p_coord );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_state( void );

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
     * \short Represents the remove state event.
     */
    class grape_event_remove_state : public grape_event_base
    {
      private:
        unsigned int                m_state;        /**< Identifier of the to be deleted state. */
        bool                m_normal;       /**< Boolean value to indicate whether the state should destroy all transitions and designators attached to it; a normal remove */
        wxString            m_name;         /**< Backup of the name of the state. */
        coordinate          m_coordinate;   /**< Backup of the coordinate of the state. */
        float               m_width;        /**< Backup of the width of the state. */
        float               m_height;       /**< Backup of the height of the state. */
        wxArrayLong         m_comments;     /**< Identifiers of the comments attached to the state. */
        unsigned int                m_in_diagram;  /**< Identifier of the diagram the state is in. */
        arr_event_remove_tt        m_terminating_transitions; /**< Backup of the terminating transitions that had the state as beginstate. */
        arr_event_remove_ntt     m_nonterminating_transitions_beginstate; /**< Backup of the nonterminating transitions that had the state as beginstate. */
        arr_event_remove_ntt     m_nonterminating_transitions_endstate; /**< Backup of the nonterminating transitions that had the state as endstate. */
        arr_event_remove_init            m_initial_designators; /**< Backup initial designators that designated this state. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_state A pointer to the to be deleted state.
         * @param p_proc_dia_ptr A pointer to the diagram the state is to be deleted from.
         * @param p_normal A flag indicating whether it's a normal remove and all transitions and initial designators should be removed as well.
         */
        grape_event_remove_state( grape_frame *p_main_frame, state* p_state, process_diagram* p_proc_dia_ptr, bool p_normal = true );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_state( void );

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
     * \short Represents the event to change a state name.
     */
    class grape_event_change_state : public grape_event_base
    {
      private:
        unsigned int      m_state; /**< A pointer to the state of which the name is to be changed. */
        wxString  m_old_text; /**< A string containing the previous name of the state. */
        wxString  m_new_text; /**< A string containing the new name of the state. */
        bool      m_pressed_ok; /**< A boolean indicating whether the user confirmed the rename action. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_state The state whose name is to be changed.
         */
        grape_event_change_state( grape_frame *p_main_frame, state* p_state );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_state( void );

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
     * Array of remove state events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_state, arr_event_remove_state );
  }
}
#endif // GRAPE_EVENT_STATE_H
