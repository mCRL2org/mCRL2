// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_edit.h
//
// Declares GraPE events for editting operations.

#ifndef GRAPE_EVENT_EDIT_H
#define GRAPE_EVENT_EDIT_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the cut event.
     */
    class grape_event_cut : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_cut( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_cut( void );

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
     * \short Represents the copy event.
     */
    class grape_event_copy : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_copy( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_copy( void );

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
     * \short Represents the paste event.
     */
    class grape_event_paste : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_paste( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_paste( void );

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

#endif // GRAPE_EVENT_EDIT_H
