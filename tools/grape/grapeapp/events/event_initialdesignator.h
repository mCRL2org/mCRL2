// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_initialdesignator.h
//
// Declares GraPE events for the initial designator.

#ifndef GRAPE_EVENT_INITDES_H
#define GRAPE_EVENT_INITDES_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the add initial designator event.
     */
    class grape_event_add_initial_designator : public grape_event_base
    {
      private:
        unsigned int              m_init; /**< The id of the to be created initial designator */
        coordinate        m_coord; /**< The coordinate of the initial designator. */
        unsigned int              m_state; /**< The id of the compound state that is to be the designated state of the new initial designator. */
        float             m_def_init_width;  /**< The default width of an initial designator. */
        float             m_def_init_height; /**< The default height of an initial designator. */
        unsigned int              m_in_diagram;     /**< The id of the diagram the initial designator will have been added to. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_state The state the initial designator is to be attached to.
         * @param p_coord The coordinate at which the initial designator is to be added.
         */
        grape_event_add_initial_designator( grape_frame *p_main_frame, compound_state* p_state, coordinate &p_coord );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_add_initial_designator( void );

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
     * \short Represents the remove initial designator event.
     */
    class grape_event_remove_initial_designator : public grape_event_base
    {
      private:
        unsigned int                m_init;         /**< The id of the to be deleted initial designator. */
        coordinate          m_coord;        /**< The coordinate of the identifier. */
        int                 m_designates;   /**< The identifier of the compound state the initial designator designated. */
        coordinate          m_coordinate;   /**< Backup of the coordinate of the initial designator. */
        float               m_width;        /**< Backup of the width of the initial designator. */
        float               m_height;       /**< Backup of the height of the initial designator. */
        wxArrayLong         m_comments;     /**< Identifiers of the comments attached to the initial designator. */
        unsigned int                m_in_diagram;  /**< Identifier of the diagram the initial designator is in. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_init A pointer to the to be deleted initial designator.
         * @param p_proc_dia_ptr A pointer to the diagram the designator is to be removed from.
         */
        grape_event_remove_initial_designator( grape_frame *p_main_frame, initial_designator* p_init, process_diagram* p_proc_dia_ptr );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_remove_initial_designator( void );

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
     * Array of remove initial designator events.
     */
    WX_DECLARE_OBJARRAY( grape_event_remove_initial_designator, arr_event_remove_init );

    /**
     * \short Represents the attach initial designator event.
     */
    class grape_event_attach_initial_designator : public grape_event_base
    {
      private:
        unsigned int                m_init;         /**< The id of the to be attached initial designator. */
        unsigned int                m_state;         /**< The id of the to be attached state. */
        unsigned int                m_diagram;      /**< The id of the diagram. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_init A pointer to the to be attached initial designator.
         * @param p_state A pointer to the state the initial designator is to be attached to.
         */
        grape_event_attach_initial_designator( grape_frame *p_main_frame, initial_designator* p_init, compound_state* p_state );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_attach_initial_designator( void );

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
     * \short Represents the attach initial designator event.
     */
    class grape_event_detach_initial_designator : public grape_event_base
    {
      private:
        unsigned int                m_init;         /**< The id of the to be detached initial designator. */
        int                 m_state;         /**< The id of the to be detached state. */
        unsigned int                m_diagram;      /**< The id of the diagram. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_init A pointer to the to be detached initial designator.
         */
        grape_event_detach_initial_designator( grape_frame *p_main_frame, initial_designator* p_init );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_detach_initial_designator( void );

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
#endif // GRAPE_EVENT_INITDES_H
