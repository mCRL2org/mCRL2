// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_nonterminatingtransition.h
//
// Declares GraPE events for nonterminating transitions

#ifndef GRAPE_EVENT_TRANSITION_H
#define GRAPE_EVENT_TRANSITION_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the attach transition to beginstate event.
     */
    class grape_event_attach_transition_beginstate : public grape_event_base
    {
      private:
        unsigned int    m_trans; /**< Identifier of the to be attached transition. */
        unsigned int    m_state; /**< Identifier of the state the transition is to be attached to. */
        unsigned int    m_diagram; /**< Identifier of the diagram the event occurs in. */
      public:
        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_trans A pointer to the to be attached transition.
         * @param p_state A pointer to the to be attached state.
         */
        grape_event_attach_transition_beginstate( grape_frame *p_main_frame, transition* p_trans, compound_state* p_state );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_attach_transition_beginstate( void );

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
     * \short Represents the detach transition from beginstate event.
     */
    class grape_event_detach_transition_beginstate : public grape_event_base
    {
      private:
        unsigned int    m_trans; /**< Identifier of the to be detached transition. */
        int     m_state; /**< Identifier of the state the transition is to be detached from. */
        unsigned int    m_diagram; /**< Identifier of the diagram the event occurs in. */
      public:
        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_trans A pointer to the to be detached transition.
         */
        grape_event_detach_transition_beginstate( grape_frame *p_main_frame, transition* p_trans );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_detach_transition_beginstate( void );

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
     * \short Represents the event to change a transition label.
     */
    class grape_event_change_transition : public grape_event_base
    {
      private:
        unsigned int  m_trans;      /**< The identifier of the transition to be changed. */
        wxString      m_old_text;   /**< A string containing the previous text of the label. */
        wxString      m_new_text;   /**< A string containing the new text of the label. */
        bool          m_pressed_ok; /**< A flag indicating whether the user has confirmed changing the label. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame A pointer to the main frame.
         * @param p_transition A pointer to the transition whose label is going to be changed.
         */
        grape_event_change_transition(grape_frame *p_main_frame, transition *p_transition);

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_change_transition(void);

        /**
         * Overloaded Do function.
         * Performs the event.
         */
        bool Do(void);

        /**
         * Overloaded Undo function.
         * Reverts the event.
         */
        bool Undo(void);
    };

  }
}

#endif // GRAPE_EVENT_TRANSITION_H
