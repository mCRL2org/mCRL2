// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_help.h
//
// Declares GraPE events for help actions.

#ifndef GRAPE_EVENT_HELP_H
#define GRAPE_EVENT_HELP_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the help event.
     */
    class grape_event_help : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_help( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_help( void );

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
     * \short Represents the about event.
     */
    class grape_event_about : public grape_event_base
    {
      private:
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_about( grape_frame *p_main_frame );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~grape_event_about( void );

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

#endif // GRAPE_EVENT_HELP_H
