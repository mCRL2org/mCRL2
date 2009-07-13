// Author(s): Diana Koenraadt, Remco Blewanus, Bram Schoenmakers, Thorstin Crijns, Hans Poppelaars, Bas Luksenburg, Jonathan Nelisse
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file processdiagram.h
//
// Declares the process_diagram class.

#ifndef LIBGRAPE_PROCESSDIAGRAM_H
#define LIBGRAPE_PROCESSDIAGRAM_H

#include "diagram.h"
#include "preamble.h"
#include "referencestate.h"
#include "state.h"
#include "terminatingtransition.h"

namespace grape
{
  namespace libgrape
  {

    /**
     * \short Represents a process diagram.
     * A description of the behavior of a process by means of a state
     * diagram extended with mCRL2 data types. A Process Diagram
     * contains a preamble and a number of States, Process References
     * and Transitions.
     * @see state
     * @see process_reference
     * @see transition
     */
    class process_diagram : public diagram
    {
      protected:
        preamble                      m_preamble;                 /**< preamble of this process diagram. */

        arr_initial_designator        m_initial_designators;      /**< initial designators in this process diagram. */
        arr_reference_state           m_reference_states;         /**< reference states in this process diagram. */
        arr_state                     m_states;                   /**< states in this process diagram. */
        arr_terminating_transition    m_terminating_transitions;  /**< non terminating transitions in this process diagram. */
        arr_nonterminating_transition m_nonterminating_transitions;   /**< terminating transitions in this process diagram. */

      public:
        /**
         * Default constructor.
         * Initializes process_diagram.
         */
        process_diagram( void );

        /**
         * Copy constructor.
         * Creates a new process diagram based on an existing one.
         * @param p_process_diagram The process diagram to copy.
         */
        process_diagram( const process_diagram &p_process_diagram );

        /**
         * Default destructor.
         * Frees allocated memory.
         */
        ~process_diagram( void );

        /**
         * preamble retrieval function
         * @return Returns a pointer to the preamble.
         */
        preamble* get_preamble( void );

        /**
         * Set the preamble of the process diagram.
         * @param p_preamble The preamble to assign to this diagram.
         */
        void set_preamble( const preamble &p_preamble );

        /**
         * state addition function.
         * Deselects all objects in the diagram and creates a new state.
         * @param p_id The id of the state.
         * @param p_coord The coordinate at which the state should be created.
         * @param p_def_width The default width of a state.
         * @param p_def_height The default height of a state.
         * @return Returns a pointer to the newly created state.
         */
        state* add_state( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height );

        /**
         * state removal function.
         * Removes the state and all associated transitions and initial designators.
         * @param p_state A pointer to the state.
         */
        void remove_state( state* p_state );

        /**
         * State count function.
         * @return Returns the number of states in the process diagram.
         */
        unsigned int count_state( void );

        /**
         * State retrieval function.
         * @param p_i The index of the to be retrieved state.
         * @return Returns a pointer to the state at index p_i in the list of states.
         */
        state* get_state( int p_i );

        /**
         * To be used exclusively during XML-conversion.
         * @return A pointer to list of states
         */
        arr_state* get_state_list( void );

        /**
         * Reference state addition function.
         * Deselects all objects in the diagram and creates a new reference state.
         * @param p_id The id of the reference state.
         * @param p_coord The coordinate at which the reference state should be created.
         * @param p_def_width The default width of a reference state.
         * @param p_def_height The default height of a reference state.
         * @return Returns a pointer to the newly created reference state.
         */
        reference_state* add_reference_state( unsigned int p_id, coordinate &p_coord, float p_def_width, float p_def_height );

        /**
         * Reference state removal function.
         * Removes the reference state and all associated transitions and initial designators.
         * @param p_state A pointer to the reference state.
         */
        void remove_reference_state( reference_state* p_state );

        /**
         * Reference state count function.
         * @return Returns the number of reference states in the process diagram.
         */
        unsigned int count_reference_state( void );

        /**
         * Reference state retrieval function.
         * @param p_i The index of the to be retrieved reference state.
         * @return Returns a pointer to the reference state at index p_i in the list of reference states.
         */
        reference_state* get_reference_state( int p_i );

        /**
         * To be used exclusively during XML-conversion.
         * @return A pointer to list of reference states
         */
        arr_reference_state* get_reference_state_list( void );

        /**
         * Nonterminating transition addition function.
         * Deselects all objects in the diagram and creates a new nonterminating transition.
         * @pre The specified beginstate and endstate are both in this diagram.
         * @param p_id The id of the nonterminating transition.
         * @param p_beginstate The state in which the nonterminating transition begins.
         * @param p_endstate The state in which the nonterminating transition ends.
         * @return Returns a pointer to the newly created nonterminating trannsition
         */
        nonterminating_transition* add_nonterminating_transition( unsigned int p_id, compound_state* p_beginstate, compound_state* p_endstate );

        /**
         * Nonterminating Transition removal function.
         * Removes the nonterminating transition.
         * @param p_ntt A pointer to the nonterminating transition.
         */
        void remove_nonterminating_transition( nonterminating_transition* p_ntt );

        /**
         * Nonterminating transition count function.
         * @return Returns the number of nonterminating transitions in the process diagram.
         */
        unsigned int count_nonterminating_transition( void );

        /**
         * Nonerminating transition retrieval function.
         * @param p_i The index of the to be retrieved nonterminating transition.
         * @return Returns a pointer to the nonterminating transition at index p_i in the list of nonterminating transitions.
         */
        nonterminating_transition* get_nonterminating_transition( int p_i );

        /**
         * To be used exclusively during XML-conversion.
         * @return A pointer to list of nonterminating transitions
         */
        arr_nonterminating_transition* get_nonterminating_transition_list( void );

        /**
         * Terminating transition addition function.
         * Deselects all objects in the diagram and creates a new terminating transition.
         * @pre The specified beginstate is in this diagram.
         * @param p_id The id of the terminating transition.
         * @param p_beginstate The state in which the terminating transition begins.
         * @param p_endcoordinate The state in which the terminating transition ends.
         * @return Returns a pointer to the newly created terminating transition.
         */
        terminating_transition* add_terminating_transition( unsigned int p_id, compound_state* p_beginstate, coordinate &p_endcoordinate );

        /**
         * Terminating Transition removal function.
         * Removes the terminating transition.
         * @param p_trans A pointer to the terminating transition.
         */
        void remove_terminating_transition( terminating_transition* p_trans );

        /**
         * Terminating transition count function.
         * @return Returns the number of terminating transitions in the process diagram.
         */
        unsigned int count_terminating_transition( void );

        /**
         * Terminating transition retrieval function.
         * @param p_i The index of the to be retrieved terminating transition.
         * @return Returns a pointer to the terminating transition at index p_i in the list of terminating transitions.
         */
        terminating_transition* get_terminating_transition( int p_i );

        /**
         * To be used exclusively during XML-conversion.
         * @return A pointer to list of terminating transitions
         */
        arr_terminating_transition* get_terminating_transition_list( void );

        /**
         * Transition beginstate attachment function.
         * Attaches the transition to the specified compound state as beginstate.
         * Detaches the transition from an existing attached compound state first, if necessary.
         * @pre The transition and compound state are both in this diagram.
         * @param p_trans A pointer to the transition.
         * @param p_state A pointer to the desired beginstate.
         */
        void attach_transition_beginstate( transition* p_trans, compound_state* p_state );

        /**
         * Transition beginstate detachment function.
         * Detaches the transition from its beginstate
         * @param p_trans A pointer to the transition.
         */
        void detach_transition_beginstate( transition* p_trans );

        /**
         * Nonterminating Transition endstate attachment function.
         * Attaches the nonterminating transition to the specified compound state as endstate.
         * Detaches the nonterminating transition from an existing attached compound state first, if necessary.
         * @pre The nonterminating transition and compound state are both in this diagram.
         * @param p_ntt A pointer to the nonterminating transition.
         * @param p_state A pointer to the desired endstate.
         */
        void attach_nonterminating_transition_endstate( nonterminating_transition* p_ntt, compound_state* p_state );

        /**
         * Nonterminating Transition endstate detachment function.
         * Detaches the nonterminating transition from its endstate.
         * @param p_ntt A pointer to the nonterminating transition.
         */
        void detach_nonterminating_transition_endstate( nonterminating_transition* p_ntt );

        /**
         * initial_designator addition function.
         * Deselects all objects in the diagram and creates a new initial designator.
         * @pre The specified designated state is in this diagram.
         * @param p_id The id of the initial_designator.
         * @param p_state The compound_state that the initial_designator designates.
         * @param p_def_width The default width of an initial designator.
         * @param p_def_height The default height of an initial designator.
         * @param p_coord The coordinate at which the initial designator is to be added.
         * @return Returns a pointer to the newly created initial designator.
         */
        initial_designator* add_initial_designator( unsigned int p_id, compound_state* p_state, float p_def_width, float p_def_height, coordinate &p_coord );

        /**
         * Initial designator removal function.
         * Removes the initial designator.
         * @param p_init A pointer to the initial designator.
         */
        void remove_initial_designator( initial_designator* p_init );

        /**
         * Initial designator count function.
         * @return Returns the number of intial designators in the process diagram.
         */
        unsigned int count_initial_designator( void );

        /**
         * Initial designator retrieval function.
         * @param p_i The index of the to be retrieved initial designator.
         * @return Returns a pointer to the initial designator at index p_i in the list of initial designators.
         */
        initial_designator* get_initial_designator( int p_i );

        /**
         * To be used exclusively during XML-conversion.
         * @return A pointer to list of initial designators
         */
        arr_initial_designator* get_initial_designator_list( void );


        /**
         * Initial designator attachment function.
         * Attaches the initial designator to the specified compound state.
         * Detaches the initial designator from an existing attached compound state first, if necessary.
         * @pre The initial designator and compound state are both in this diagram.
         * @param p_init A pointer to the initial designator.
         * @param p_state A pointer to the state.
         */
        void attach_initial_designator( initial_designator* p_init, compound_state* p_state );

        /**
         * Initial designator detachment function.
         * Detaches the initial designator from the state it is attached to.
         * @param p_init A pointer to the initial designator.
         * @param p_coord The coordinate where the the initial designator was released.
         */
        void detach_initial_designator( initial_designator* p_init, coordinate &p_coord );

        /**
         * Object selection function.
         * Selects all objects in the diagram.
         */
        void select_all_objects( void );

        /**
         * Object deselection function.
         * Deselects all objects in the diagram.
         */
        void deselect_all_objects();

        /**
         * Object retrieval function.
         * @param p_proc_dia The process diagram to investigate.
         * @param p_id The identifier of the desired object.
         * @param p_type The type of the object you want to retrieve.
         * @return Returns the object that has the specified id, if it exists (0 if not).
         */
        static object* find_object( process_diagram* p_proc_dia, unsigned int p_id, object_type p_type = ANY );
      private:
        /**
         * Transition removal function.
         * Performs a dynamic cast on the specified pointer and removes the object from the corresponding list, which is either the list of nonterminating transitions or the list of terminating transitions.
         */
        void delete_transition( transition* p_trans );

        /**
         * State existance function.
         * @param p_name The base name of the state.
         * @param p_index The index appearing in the state's name.
         * @return Returns whether a state with the specified name exists.
         */
        bool exists_state( const wxString &p_name, int p_index );
    };

    /**
     * Array of process_diagram.
     */
    WX_DECLARE_OBJARRAY( process_diagram, arr_process_diagram );
  } // namespace libgrape
} // namespace grape

#endif // LIBGRAPE_PROCESSDIAGRAM_H
