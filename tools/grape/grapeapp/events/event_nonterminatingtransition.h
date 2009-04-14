// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_nonterminatingtransition.h
//
// Declares GraPE events for nonterminating transitions

#ifndef GRAPE_EVENT_NONTERMINATINGTRANSITION_H
#define GRAPE_EVENT_NONTERMINATINGTRANSITION_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
/**
     * \short Represents the add nonterminating transition event.
     */
    class grape_event_add_nonterminating_transition : public grape_event_base
    {
      private:
        unsigned int       m_ntt;         /**< The identifier of the terminating transition that will be created. */
        unsigned int       m_beginstate;  /**< The identifier of the beginstate of the terminating transition. */
        unsigned int       m_endstate;    /**< The identifier of the endstate of the terminating transition. */
        unsigned int       m_in_diagram;  /**< The identifier of the transition will have been added to. */
      public:
        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_beginstate The beginstate of the transition.
         * @param p_endstate The endstate of the transition.
         */
        grape_event_add_nonterminating_transition( grape_frame *p_main_frame, compound_state* p_beginstate, compound_state* p_endstate );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_nonterminating_transition( void );

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
     * \short Represents the remove nonterminating transition event.
     */
    class grape_event_remove_nonterminating_transition : public grape_event_base
    {
      private:
        unsigned int        m_ntt;          /**< The identifier of the transition. */
        label               m_label;        /**< A backup of the transition label. */
        int                 m_beginstate;   /**< The identifier of the beginstate of the transition. */
        int                 m_endstate;     /**< The identifier of the endstate of the transition. */
        coordinate          m_coordinate;   /**< Backup of the coordinate of the transition. */
        float               m_width;        /**< Backup of the width of the transition. */
        float               m_height;       /**< Backup of the height of the transition. */
        wxArrayLong         m_comments;     /**< The identifiers of comments attached to the transition. */
        unsigned int        m_in_diagram;   /**< The identifier of the diagram the transition is in. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_ntt A pointer to the to be deleted transition.
         * @param p_proc_dia_ptr A pointer to the diagram the transition is to be removed from.
         */
        grape_event_remove_nonterminating_transition( grape_frame *p_main_frame, nonterminating_transition* p_ntt, process_diagram* p_proc_dia_ptr );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_nonterminating_transition( void );

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
     * \short Represents the attach nonterminating transition to endstate event.
     */
    class grape_event_attach_nonterminating_transition_endstate : public grape_event_base
    {
      private:
        unsigned int    m_ntt;      /**< Identifier of the to be attached nonterminating transition. */
        unsigned int    m_state;    /**< Identifier of the state the nonterminating transition is to be attached to. */
        unsigned int    m_diagram;  /**< Identifier of the diagram the event occurs in. */
      public:
        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_ntt A pointer to the to be attached transition.
         * @param p_state A pointer to the to be attached state.
         */
        grape_event_attach_nonterminating_transition_endstate( grape_frame *p_main_frame, nonterminating_transition* p_ntt, compound_state* p_state );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_attach_nonterminating_transition_endstate( void );

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
     * \short Represents the detach nonterminating transition from endstate event.
     */
    class grape_event_detach_nonterminating_transition_endstate : public grape_event_base
    {
      private:
        unsigned int    m_ntt;      /**< Identifier of the to be detached nonterminating transition. */
        int             m_state;    /**< Identifier of the state the nonterminating transition is to be detached from. */
        unsigned int    m_diagram;  /**< Identifier of the diagram the event occurs in. */
      public:
        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_ntt A pointer to the to be detached transition.
         */
        grape_event_detach_nonterminating_transition_endstate( grape_frame *p_main_frame, nonterminating_transition* p_ntt );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_detach_nonterminating_transition_endstate( void );

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
     * Array of remove nonterminating transition events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_nonterminating_transition, arr_event_remove_ntt );
  }
}

#endif // GRAPE_EVENT_NONTERMINATINGTRANSITION_H
