// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_select.h
//
// Declares GraPE events for selection.

#ifndef GRAPE_EVENT_SELECT_H
#define GRAPE_EVENT_SELECT_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the selection event.
     */
    class grape_event_select : public grape_event_base
    {
      private:
        object          *m_obj; /**< the object that is to be selected */
        bool            m_shift_pressed; /**< A boolean indicating whether the Shift key was pressed and the object should be 'plus'-selected. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_obj The selected object.
         * @param p_shift_pressed A flag indicating whether the shift key was pressed when the event was invoked.
         */
        grape_event_select( grape_frame *p_main_frame, object* p_obj, bool p_shift_pressed );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_select( void );

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
     * \short Represents the select all event.
     */
    class grape_event_select_all : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_select_all( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_select_all( void );

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
     * \short Represents the plus selection event.
     */
    class grape_event_plus_select : public grape_event_base
    {
      private:
        object  *m_obj; /**< the object that is to be plusselected */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_obj The selected object.
         */
        grape_event_plus_select( grape_frame *p_main_frame, object* p_obj );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_plus_select( void );

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
     * \short Represents the deselect event.
     */
    class grape_event_deselect : public grape_event_base
    {
      private:
        object  *m_obj; /**< the object that is to be deselected */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         * @param p_obj Pointer to the selected object.
         */
        grape_event_deselect( grape_frame *p_main_frame, object* p_obj );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_deselect( void );

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
     * \short Represents the deselect all event.
     */
    class grape_event_deselect_all : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_deselect_all( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_deselect_all( void );

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

#endif // GRAPE_EVENT_SELECT_H
