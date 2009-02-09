// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file nonterminatingtransition.h
//
// Declares the nonterminating_transition class.

#ifndef LIBGRAPE_NONTERMINATINGTRANSITION_H
#define LIBGRAPE_NONTERMINATINGTRANSITION_H

#include "transition.h"

namespace grape
{
  namespace libgrape
  {

    // forward declarations
    class compound_state;

    /**
     * \short Represents a non terminating transition.
     * The change of a process's State to another (possibly the same)
     * State or to a Process Reference due to an Action. A non-
     * terminating Transition has a Transition Label.
     * @see transition
     * @see label
     */
    class nonterminating_transition : public transition
    {
      protected:
        compound_state   *m_endstate;  /**< endstate belonging to this non terminating transition. */

      public:
        /**
         * Default constructor.
         * Initializes nonterminating_transition with beginstate and endstate.
         * @param p_beginstate The state in which the transition begins.
         * @param p_endstate The state in which the transition ends
         */
        nonterminating_transition( compound_state* p_beginstate = 0, compound_state* p_endstate = 0 );

        /**
         * Copy constructor.
         * Creates a new non-terminating transition based on an existing one.
         * @param p_ntt The non-terminating transition to copy.
         */
        nonterminating_transition( const nonterminating_transition &p_ntt );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        ~nonterminating_transition( void );

        /**
         * Endstate association function.
         * @param p_endstate The new state that is the endstate of this transition.
         */
        void attach_endstate( compound_state* p_endstate );


        /**
         * Endstate retrieval function.
         * @return Returns the endstate of the transition.
         */
        compound_state * get_endstate( void );

        /**
         * Endstate dissociation function.
         */
        void detach_endstate( void );

        /**
         * Transition set center-coordinate.
         * Calculates the new center coordinate of the transition, based on the coordinates of the compound states it is attached to.
         */
        void set_center_coordinate( void );

        /**
         * Transition get end coordinate
         * Get the end coordinate of a transition
         */
        coordinate get_end_coordinate( void );

        /**
         * Transition get begin coordinate
         * Get the begin coordinate of a transition
         */
        virtual coordinate get_begin_coordinate( void );
    };

    /**
     * Array of NonTerminatingTransition pointers.
     */
    WX_DEFINE_ARRAY_PTR( nonterminating_transition *, arr_nonterminating_transition_ptr);

    /**
     * Array of NonTerminatingTransition.
     */
    WX_DECLARE_OBJARRAY( nonterminating_transition, arr_nonterminating_transition);

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_NONTERMINATINGTRANSITION_H
