// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transition.h
//
// Declares the transition class.

#ifndef LIBGRAPE_TRANSITION_H
#define LIBGRAPE_TRANSITION_H

#include "object.h"
#include "label.h"
#include "linetype.h"

namespace grape
{
  namespace libgrape
  {

    // forward declaration
    class compound_state;

    /**
     * \short Represents a transition.
     * The change of a process's State to another (possibly the same)
     * State or to a Process Reference due to an Action. A non-
     * terminating Transition has a Transition Label.
     * @see transition
     * @see label
     */
    class transition : public object
    {
      private:
        /**
         * Default constructor. Shouldn't be called from GraPE code.
         */
        transition( void );

      protected:
        label               m_label;        /**< transition label. */
        linetype            m_linetype;     /**< type of line associated with this transition. */
        list_of_coordinate  m_breakpoints;  /**< breakpoints of line associated with this transition. */

        compound_state      *m_beginstate;  /**< beginstate of this transition. */
      public:
        /**
         * Constructor.
         * Initializes transition.
         * @param p_type The object type. Should be either @c NONTERMINATING_TRANSITION or @c TERMINATING_TRANSITION .
         * @param p_beginstate The state in which the transition begins.
         */
        transition( object_type p_type, compound_state* p_beginstate = 0 );

        /**
         * Copy constructor.
         * Creates a new transition based on an existing one.
         * @param p_transition The transition to copy.
         */
        transition( const transition &p_transition );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        virtual ~transition( void );

        void set_label( const label& p_label );

        /**
         * Label retrieval function.
         * @return Returns a pointer to the label.
         */
        label * get_label( void );

        /**
         * Beginstate association function.
         * @param p_beginstate The new state that is the beginstate of this transition.
         */
        void attach_beginstate( compound_state* p_beginstate );

        /**
         * Beginstate retrieval function.
         * @return Returns the beginstate of the transition.
         */
        compound_state * get_beginstate( void );

        /**
         * Beginstate dissociation function.
         */
        void detach_beginstate( void );

        /**
         * Transition linetype retrieval function
         * @return The line type
         */
        linetype get_linetype( void ) const;

        /**
         * Transition linetype assignment function
         * Takes a new linetype and assigns it to the linetype of the transition.
         * @param p_linetype The new linetype
         */
        void set_linetype( const linetype &p_linetype );

        /**
         * Transition breakpoint adding function
         * Takes a new coordinate and adds it to the breakpoint(s) of the transition.
         * @param p_coordinate The new coordinate
         */
        void add_breakpoint( const coordinate &p_coordinate );

        /**
         * Transition breakpoint moving function
         * Takes a breakpoint and assigns a new coordinate to it.
         * @param p_breakpoint The breakpoint
         * @param p_coordinate The new coordinate
         */
        void move_breakpoint( coordinate &p_breakpoint, const coordinate &p_coordinate );

        /**
         * Transition breakpoint removing function
         * Takes a breakpoint and removes it from the breakpoint(s) of the transition.
         * @param p_breakpoint The breakpoint
         */
        void remove_breakpoint( coordinate &p_breakpoint );

        /**
         * Transition breakpoint retrieval function
         * Returns a pointer to the list of breakpoints.
         * @return The pointer to list of breakpoints
         */
        list_of_coordinate *get_breakpoints( void );

        /**
         * Transition movable retrieval function
         * @return @c true if the coordinate of the transition can be changed, otherwise @c false
         */
        bool movable( void ) const;
        
        /**
         * Transition get begin coordinate
         * Get the begin coordinate of a transition
         */
        coordinate get_begin_coordinate( void );
    };

    /**
     * Array of Transition pointers.
     */
    WX_DEFINE_ARRAY_PTR( transition *, arr_transition_ptr );

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_TRANSITION_H
