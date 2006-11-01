#ifndef _LIBNEXTSTATE_H
#define _LIBNEXTSTATE_H

#include <aterm2.h>
#include "libenum.h"
#include "librewrite.h"

/** \brief Internal NextState state storage method **/
typedef enum { GS_STATE_VECTOR  /** \brief Store state as vector (ATermAppl) **/
             , GS_STATE_TREE    /** \brief Store states in a binary tree **/
             } NextStateFormat;

/** \brief Strategies for exploring the next states. **/
typedef enum { nsStandard } NextStateStrategy;

/**
 * \brief Iterator-like class interface to get the transitions requested via a
 *        NextState object.
 *
 * NextStateGenerator objects are returned by NextState objects to enumerate
 * all transitions from a given state. Typical use is as follows (with ns a
 * NextState object):
 *
 * \code
 *   NextStateGenerator *gen = ns->getNextStates(state);
 *   while ( gen->next(&t,&s) )
 *   {
 *     // transition t to state s
 *     ...
 *   }
 * \endcode
 **/
class NextStateGenerator
{
	public:
		/** \brief Destructor. **/
		virtual ~NextStateGenerator();

		/**
		 * \brief Get next transition (if available).
		 * \param Transition  A place to store the mCRL2 action of a
		 *                    transition.
		 * \param State       A place to store the state to which a
		 *                    transition leads.
		 * \param prioritised A place to store a boolean indicating
		 *                    whether or not this transition was
		 *                    prioritised. (Unless this is NULL).
		 * \return Whether or not a transition was found and stored in 
		 *         the variables pointed to by the arguments.
		 *
		 * Once this function returns false, it will continue to return
		 * false. That is, when false is returned all transitions have
		 * been explored.
		 **/
		virtual bool next(ATermAppl *Transition, ATerm *State, bool *prioritised = NULL) = 0;
		/**
		 * \brief Check whether or not an error occurred during
		 *        exploration.
		 * \return Whether or not an error occurred during exploration.
		 **/
		virtual bool errorOccurred() = 0;

		/**
		 * \brief Get the state from which the transitions (if any)
		 *        originate.
		 * \return The state being explored.
		 **/
		virtual ATerm get_state() = 0;
};

/**
 * \brief Interface class for exploring the possible transitions from states.
 *
 * This is the interface class for exploration of the transitions from states.
 * Actual NextState objects need to be created with createNextState().
 *
 * Note that states and NextStateGenerators returned by a NextState object are
 * only to be used with this same NextState object.
 **/
class NextState
{
	public:
		/** \brief Destructor. **/
		virtual ~NextState();

		/**
		 * \brief Prioritise an action.
		 * \param action The name of the action to be prioritised.
		 *
		 * Prioritising an action means that the NextStateGenerator
		 * returned by getNextStates() will first return all actions
		 * matching the given action name. If prioritise is called
		 * repeatedly, previous prioritisations are delayed until after
		 * the last one is finished. That is,
		 * \code prioritise("a"); prioritise("b"); \endcode
		 * will first give all "b" actions, then all "a" actions and
		 * then the remaining actions.
		 **/
		virtual void prioritise(char *action) = 0;

		/**
		 * \brief Get the initial state.
		 * \return The initial state. Note that the format of this
		 *         state is unspecified. Use the functions
		 *         getStateArgument() and makeStateVector() to get
		 *         a specific element of the state, respectively
		 *         a mCRL2 LTS state vector.
		 **/
		virtual ATerm getInitialState() = 0;
		/**
		 * \brief Get the transitions from a given state.
		 * \param state The state to explore.
		 * \param old   A NextStateGenerator to be used for retreiving
		 *              the transitions. If NULL, a new object is
		 *              created.
		 * \return A NextStateGenerator which can be used to retreive
		 *         the transitions from state. If old is not NULL, old
		 *         itself is returned (reinitialised for the new state).
		 *
		 * The optional argument old allows one to reuse
		 * NextStateGenerator objects (avoiding superfluous allocation
		 * and deallocation). Typical use is as follows (where ns is a
		 * NextState object):
		 *
		 * \code
		 *   NextStateGenerator *gen = NULL;
		 *   while ( c )
		 *   {
		 *     gen = ns->getNextStates(state,gen);
		 *     ...
		 *   }
		 * \endcode
		 **/
		virtual NextStateGenerator *getNextStates(
					ATerm state,
					NextStateGenerator *old = NULL
					) = 0;

		/**
		 * \brief Get number of state parameters.
		 * \return Number of state parameters.
		 **/
		virtual int getStateLength() = 0;
		/**
		 * \brief Get an argument from a state.
		 * \param state The state to get the argument from.
		 * \param index The number of the state argument (starting from
		 *              0).
		 * \return Argument index of state.
		 **/
		virtual ATermAppl getStateArgument(ATerm state, int index) = 0;
		/**
		 * \brief Make a mCRL2 LTS state vector from a state.
		 * \param state A state.
		 * \return The mCRL2 LTS state vector representation of state.
		 **/
		virtual ATermAppl makeStateVector(ATerm state) = 0;

		/**
		 * \brief Get rewriter used by this object.
		 * \return Rewriter object used by this NextState object.
		 **/
		virtual Rewriter *getRewriter() = 0;
};

/**
 * \brief Create a NextState object.
 * \param spec                A mCRL2 LPE containing the process to be explored.
 * \param allow_free_vars     Whether to allow free variables or to substitute
 *                            them  with dummy values.
 * \param state_format        Format to store the state in (internally).
 * \param e                   Enumerator to use for finding solutions of
 *                            conditions.
 * \param clean_up_enumerator Whether or not to delete the enumerator object on
 *                            destruction of the returned NextState object.
 * \param strategy            The strategy to use for state exploration.
 * \return A NextState object with the given parameters.
 **/
NextState *createNextState(
		ATermAppl spec,
		bool allow_free_vars,
		int state_format,
		Enumerator *e,
		bool clean_up_enumerator = false,
		NextStateStrategy strategy = nsStandard
		);

/**
 * \brief Create a NextState object.
 * \param spec                A mCRL2 LPE containing the process to be explored.
 * \param allow_free_vars     Whether to allow free variables or to substitute
 *                            them  with dummy values.
 * \param state_format        Format to store the state in (internally).
 * \param rewrite_strategy    The strategy to use for rewriting data
 *                            expressions.
 * \param enumerator_strategy The strategy to use for enumerating solutions of
 *                            conditions.
 * \param strategy            The strategy to use for state exploration.
 * \return A NextState object with the given parameters.
 **/
NextState *createNextState(
		ATermAppl spec,
		bool allow_free_vars = true,
		int state_format = GS_STATE_VECTOR,
		RewriteStrategy rewrite_strategy = GS_REWR_INNER,
		EnumerateStrategy enumerator_strategy = ENUM_STANDARD,
		NextStateStrategy strategy = nsStandard
		);

#endif
