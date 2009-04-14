// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// ( See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt )
//
/// \file event_architecturediagram.h
//
// Declares GraPE events for architecture diagrams.

#ifndef GRAPE_EVENT_ARCHITECTUREDIAGRAM_H
#define GRAPE_EVENT_ARCHITECTUREDIAGRAM_H

#include "event_base.h"

namespace grape
{
  namespace grapeapp
  {
    /**
     * \short Represents the add architecture diagram event.
     */
    class grape_event_add_architecture_diagram : public grape_event_base
    {
      private:
        unsigned int    m_arch; /**< Identifier of the created architecture diagram. */
      public:

        /**
         * Initialization constructor.
         * Initializes the event.
         * @param p_main_frame Pointer to the main frame.
         */
        grape_event_add_architecture_diagram( grape_frame *p_main_frame );

        /**
         * Default desdtructor.
         * Frees allocated memory.
         */
        ~grape_event_add_architecture_diagram( void );

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
#endif // GRAPE_EVENT_ARCHITECTUREDIAGRAM_H
