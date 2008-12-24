// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file compoundstate.h
//
// Declares the compound_state class.

#ifndef LIBGRAPE_COMPOUNDSTATE_H
#define LIBGRAPE_COMPOUNDSTATE_H

#include "initialdesignator.h"
#include "nonterminatingtransition.h"

namespace grape
{
  namespace libgrape
  {
    /**
     * \short Represents a compound (reference- or normal) state.
     * @see reference_state
     * @see state
     */
    class compound_state : public object
    {
      private:
        /**
         * Default constructor. Shouldn't be called from GraPE code.
         * Initializes compound_state.
         */
        compound_state( void );

      protected:
        wxString                    m_name;         /**< name of this state. */
        bool                        m_current_state; /**< flag used during simulation. */

        arr_initial_designator_ptr  m_designates;   /**< initial designators associated with this state. */
        arr_transition_ptr          m_beginstate;   /**< transitions this state is the beginstate of. */
        arr_nonterminating_transition_ptr
                                    m_endstate;     /**< non terminating transitions this state is the end state of. */

      public:
        /**
         * Default constructor.
         * Initializes compound_state.
         * @param p_type The type of this reference. Should be either @c STATE or @c REFERENCE_STATE .
         */
        compound_state( object_type p_type );

        /**
         * Copy constructor.
         * Creates a new compound state from an existing one.
         * @param p_compound The connection to copy.
         */
        compound_state( const compound_state &p_compound );

        /**
         * Default destructor.
         * Frees allocated memory  and removes all references to the object.
         */
        virtual ~compound_state( void );

        /**
         * Name retrieval function.
         * @return The name of the compound_state
         */
        wxString get_name( void );

        /**
         * Name assignment function.
         * @pre The compound state is selected.
         * @param p_name The new name of the compound state
         */
        void set_name( const wxString &p_name );

        /**
         * Beginstate association function.
         * @param p_transition The new transition that this compound state is to become the beginstate of.
         */
        void attach_transition_beginstate( transition* p_transition );

        /**
         * Transition retrieval function
         * @param p_index An index.
         * @return A pointer to the transition at the specified index in the list of transitions that begin in this state.
         */
        transition* get_transition_beginstate( int p_index );

        /**
         * Transition count function
         * @return The length of the list of transitions that begin in this state.
         */
        unsigned int count_transition_beginstate( void );

        /**
         * Beginstate dissociation function.
         * @param p_transition The transition that this compound_state is no longer to be the beginstate of.
         */
        void detach_transition_beginstate( transition* p_transition );

        /**
         * Endstate association function.
         * @param p_nonterminating_transition The new nonterminating transition that this compound_state is to become the endstate of.
         */
        void attach_transition_endstate( nonterminating_transition* p_nonterminating_transition );

        /**
         * Transition retrieval function
         * @param p_index An index.
         * @return A pointer to the transition at the specified index in the list of transitions that end in this state.
         */
        nonterminating_transition* get_transition_endstate( int p_index );

        /**
         * Transition count function
         * @return The length of the list of transitions that end in this state.
         */
        unsigned int count_transition_endstate( void );

        /**
         * Endstate dissociation function.
         * @param p_nonterminating_transition The transition that this compound_state is no longer to be the endstate of.
         */
        void detach_transition_endstate( nonterminating_transition* p_nonterminating_transition );

        /**
         * Initial designator association function.
         * @param p_init The new initial_designator that this compound state is to become the endstate of.
         */
        void attach_initial_designator( initial_designator* p_init );

        /**
         * Initial designator retrieval function
         * @param p_index An index.
         * @return A pointer to the initial designator in the list of initial designators that designate this state
         */
        initial_designator* get_initial_designator( int p_index );

        /**
         * Initial designator count function
         * @return The length of the list of initial designators that designate this state.
         */
        unsigned int count_initial_designator( void );

        /**
         * Endstate dissociation function.
         * @param p_init The transition that this compound state is no longer to be the endstate of.
         */
        void detach_initial_designator( initial_designator* p_init );

        /**
         * Current state status retrieval function
         * @return Returns whether the state is currently enabled in the simulation
         */
        bool get_current_state( void ) const;

        /**
         * Current state status update function
         * @param p_current_state contains true if the state should be the current state and false otherwise
         */
        void set_current_state( bool p_current_state );

    };

  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_COMPOUNDSTATE_H
