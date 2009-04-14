// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_terminatingtransition.h
//
// Declares GraPE events for terminating transitions

#ifndef GRAPE_EVENT_TERMINATINGTRANSITION_H
#define GRAPE_EVENT_TERMINATINGTRANSITION_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the add terminating transition event.
     */
    class grape_event_add_terminating_transition : public grape_event_base
    {
      private:
        unsigned int      m_tt;         /**< Identifier of the terminating transition that will be created. */
        unsigned int      m_beginstate; /**< Identifier of the beginstate of the terminating transition. */
        coordinate        m_coord;      /**< The coordinate that was clicked, at which the terminating transition ends. */
        unsigned int      m_in_diagram; /**< Identifier of the diagram the transition will have been added to. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_beginstate The beginstate of the terminating transition.
         * @param p_coord The coordinate where the terminating transition ends.
         */
        grape_event_add_terminating_transition( grape_frame *p_main_frame, compound_state* p_beginstate,coordinate &p_coord  );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_terminating_transition( void );

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
     * \short Represents the remove terminating transition event.
     */
    class grape_event_remove_terminating_transition : public grape_event_base
    {
      private:
        unsigned int        m_tt;           /**< Identifier of the to be deleted transition. */
        label               m_label;        /**< A backup of the transition label. */
        int                 m_beginstate;   /**< Identifier of the beginstate of this transition. */
        coordinate          m_coordinate;   /**< Backup of the coordinate of the transition. */
        float               m_width;        /**< Backup of the width of the transition. */
        float               m_height;       /**< Backup of the height of the transition. */
        wxArrayLong         m_comments;     /**< Identifiers of the comments attached to the transition. */
        unsigned int        m_in_diagram;   /**< Identifier of the diagram the transition is in. */
      public:

        /**
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_tt A pointer to the to be deleted transition.
         * @param p_dia_ptr A pointer to the diagram the transition is in.
         */
        grape_event_remove_terminating_transition( grape_frame *p_main_frame, terminating_transition* p_tt, process_diagram* p_dia_ptr  );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_terminating_transition( void );

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
     * Array of remove terminating tansition events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_terminating_transition, arr_event_remove_tt );
  }
}
#endif // GRAPE_EVENT_TERMINATINGTRANSITION_H
