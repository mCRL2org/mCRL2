// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file terminatingtransition.h
//
// Declares the terminating_transition class.

#ifndef LIBGRAPE_TERMINATINGTRANSITION_H
#define LIBGRAPE_TERMINATINGTRANSITION_H

#include "transition.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a terminating transition.
     * The termination of a Process due to an Action. A Terminating
     * Transition has a Transition Label and does not lead to another
     * State or Process Reference.
     * @see action
     * @see label
     * @see transition
     */
    class terminating_transition : public transition
    {
      protected:
      public:
        /**
         * Default constructor.
         * Initializes terminating_transition.
         * @param p_beginstate The state in which the transition begins.
         */
        terminating_transition( compound_state* p_beginstate = 0 );

        /**
         * Copy constructor.
         * Creates a new terminating transition based on an existing one.
         * @param p_tt The terminating transition to copy.
         */
        terminating_transition( const terminating_transition &p_tt );

        /**
         * Default destructor.
         * Frees allocated memory and removes all references to the object.
         */
        ~terminating_transition( void );
    };

    /**
     * Array of TerminatingTransition.
     */
    WX_DECLARE_OBJARRAY( terminating_transition, arr_terminating_transition );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_TERMINATINGTRANSITION_H
