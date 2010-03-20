// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_referencestate.h
//
// Declares GraPE events for reference states.

#ifndef GRAPE_EVENT_REFSTATE_H
#define GRAPE_EVENT_REFSTATE_H

#include "event_base.h"

#include "event_initialdesignator.h"
#include "event_nonterminatingtransition.h"
#include "event_terminatingtransition.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the add reference state event.
     */
    class grape_event_add_reference_state : public grape_event_base
    {
      private:
        unsigned int                  m_ref_state; /**< Identifier of the to be created reference state. */
        coordinate            m_coord; /**< The coordinate that was clicked. */
        float                 m_def_ref_state_width;  /**< The default width of a reference state. */
        float                 m_def_ref_state_height; /**< The default height of a reference state. */
        unsigned int                  m_in_diagram; /**< Identifier of the diagram the state will have been added to. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_coord The coordinate where the reference state will be added.
         */
        grape_event_add_reference_state( grape_frame *p_main_frame, coordinate &p_coord );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_reference_state( void );

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
     * \short Represents the remove reference state event.
     */
    class grape_event_remove_reference_state : public grape_event_base
    {
      private:
        unsigned int                m_ref_state; /**< Identifier of the to be deleted reference state. */
        wxString            m_name;         /**< Backup of the name of the reference state. */
        unsigned int        m_property_of;  /**< Backup of the id of the process diagram the reference state refers to. */
        bool                m_normal;       /**< Boolean value to indicate whether the state should destroy all transitions and designators attached to it; a normal remove */
        list_of_varupdate   m_parameter_assignments;  /**< Backup of the initial parameter values of the reference state. */
        coordinate          m_coordinate;   /**< Backup of the coordinate of the reference state. */
        float               m_width;        /**< Backup of the width of the reference state. */
        float               m_height;       /**< Backup of the height of the reference state. */
        wxArrayLong         m_comments;     /**< Identifiers of the comments attached to the reference state. */
        unsigned int                m_in_diagram;  /**< Backup of the pointer to the diagram the reference state is in. */
        arr_event_remove_tt        m_terminating_transitions; /**< Backup of the terminating transitions that had the state as beginstate. */
        arr_event_remove_ntt     m_nonterminating_transitions_beginstate; /**< Backup of the nonterminating transitions that had the state as beginstate. */
        arr_event_remove_ntt     m_nonterminating_transitions_endstate; /**< Backup of the nonterminating transitions that had the state as endstate. */
        arr_event_remove_init            m_initial_designators; /**< Backup initial designators that designated this state. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_ref_state A pointer to the to be deleted reference state.
         * @param p_proc_dia_ptr A pointer to the diagram the reference is to be deleted from.
         * @param p_normal A flag indicating whether the remove is normal (true) and all transitions and initial designators should be deleted as well.
         */
        grape_event_remove_reference_state( grape_frame *p_main_frame, reference_state* p_ref_state, process_diagram* p_proc_dia_ptr, bool p_normal = true );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_reference_state( void );

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
     * \short Represents the event to change a process reference.
     */
    class grape_event_change_refstate : public grape_event_base
    {
      private:
        int m_ref_state_id; /** The ID of the reference state which is being editted. */
        int m_old_proc_id; /**< The ID of the process diagram the reference is pointed to before the event. */
        wxString m_old_proc_name; /**< The name of the process diagram the reference is pointed to before the event. */
        int m_new_proc_id; /** The ID of the process diagram the reference will point to after the event. */
        wxString m_new_proc_name; /** The name of the process diagram the reference will point to after the event. */
        wxString m_new_text; /** The new parameter initializations. */
        wxString m_old_text; /** The old parameter initializations. */
        bool m_ok_pressed; /** @c true if the user pressed OK in the dialog shown by the constructor. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_ref_state The comment whose text is to be changed.
         */
        grape_event_change_refstate( grape_frame *p_main_frame, reference_state* p_ref_state );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_refstate( void );

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
     * Array of remove reference state events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_reference_state, arr_event_remove_ref_state );
  }
}
#endif // GRAPE_EVENT_REFSTATE_H
