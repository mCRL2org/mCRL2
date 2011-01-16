// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file lts.h
 *
 * \brief The file containing the core class for transition systems.
 * \details This is the LTS library's main header file. It declares all publicly
 * accessible data structures that form the interface of the library.
 * \author Muck van Weerdenburg, Jan Friso Groote
 */

#ifndef MCRL2_LTS_LTS_H
#define MCRL2_LTS_LTS_H

#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/lts/transition.h"
#include "mcrl2/exception.h"


namespace mcrl2
{

  /** \brief The main LTS namespace.
   * \details This namespace contains all data structures and members of the LTS
   * library. 
   */
  namespace lts
  {
    
    /** \brief The enumerated type lts_type contains an index for every type
     *   type of labelled transition system that is supported by the system.
     *   \details Every type has an associated labelled transition format. E.g.
     *   for lts_lts the type of the lts is lts_lts_t. Similarly, lts_aut_t, 
     *   lts_svc_t, etc. are available. Files in which the lts's are stored have
     *   the name file.lts, file.aut, etc.
     */
    enum lts_type
    {
      lts_none, /**< unknown or no format */
      lts_lts,  /**< mCRL2 SVC format */
      lts_aut,  /**< Ald&eacute;baran format (CADP) */
      lts_fsm,  /**< FSM format */
      lts_dot, /**< GraphViz format */
      lts_svc,  /**< SVC format 
                  * \note The svc format is outdated. */  
#ifdef USE_BCG
      lts_bcg,  /**< BCG format
                  * \note Only available if the LTS library is built with BCG
                  * support.*/
      lts_type_max = lts_bcg,
#else
      lts_type_max = lts_svc,
#endif
      lts_type_min = lts_none
    };


    /** \brief A class that contains a labelled transition system.
        \details The state labels and action labels can be any type.
            Essentially, a labelled transition system consists of a 
            vector of transitions. Each transition is a triple of
            three numbers <from, label, to>. For these numbers, there
            can be associated state labels (for 'from' and 'to'), and 
            action labels (for 'label'). But it is also possible, that
            no state or action labels are provided. For all action labels
            it is maintained whether this action label is an internal
            'tau' action. This can be indicated for each action label
            separately. Finally, the number of states is recalled as
            a separate variable.
    */

    template < class STATE_LABEL_T, class ACTION_LABEL_T >
    class lts
    {
      public:

        /** \brief The type of state labels.
        */
        typedef STATE_LABEL_T state_label_t;

        /** \brief The type of action labels.
        */
        typedef ACTION_LABEL_T action_label_t;

        /** \brief The sort that contains the indices of states.
        */
        typedef typename atermpp::vector < STATE_LABEL_T >::size_type states_size_type;
        
        /** \brief The sort that represents the indices of labels.
        */
        typedef typename atermpp::vector < ACTION_LABEL_T >::size_type labels_size_type;

        /** \brief The sort that contains indices of transitions.
        */
        typedef typename std::vector<transition>::size_type transitions_size_type;

      protected: 
        
        states_size_type nstates;
        states_size_type init_state;
        std::vector<transition> transitions;
        atermpp::vector<STATE_LABEL_T> state_labels;
        atermpp::vector<ACTION_LABEL_T> action_labels;
        std::vector<bool> taus; // A vector indicating which labels are to be viewed as tau's.

      public:

        /** \brief Creates an empty LTS.
         */
        lts():nstates(0)
        {};

        /** \brief Creates a copy of the supplied LTS.
         * \param[in] l The LTS to copy. */
        lts(const lts &l):
                nstates(l.nstates),
                init_state(l.init_state),
                transitions(l.transitions),
                state_labels(l.state_labels),
                action_labels(l.action_labels),
                taus(l.taus)
        {};

        /** \brief Standard destructor for the class lts.
        */
        ~lts()
        { 
        }

        /** \brief Swap this lts with the supplied supplied LTS.
         * \param[in] l The LTS to swap. */
        void swap(lts &l)
        {
          { const states_size_type aux=init_state; init_state=l.init_state;   l.init_state=aux; }
          { const states_size_type aux=nstates;    nstates=l.nstates;         l.nstates=aux; }
          transitions.swap(l.transitions);
          state_labels.swap(l.state_labels);
          taus.swap(l.taus);
          action_labels.swap(l.action_labels);
        };

        /** \brief Gets the lts_type of the lts.
         */
        lts_type type() const
        {
          return lts_none;
        }

        /** \brief Gets the number of states of this LTS.
         * \return The number of states of this LTS. */
        states_size_type num_states() const
        {
          return nstates;
        }

        /** \brief Gets the number of state labels of this LTS.
         *  \details As states do not need to have state labels,
         *  the number of state labels can differ from the number of
         *  states.
         *  \return The number of state labels of this LTS. */
        states_size_type num_state_labels() const
        {
          return state_labels.size();
        }

        /** \brief Sets the number of states of this LTS.
         * \param[in] n The number of states of this LTS. */
        void set_num_states(const states_size_type n, const bool has_state_labels = true)
        {
          nstates = n;
          if (has_state_labels)
          {
            if (state_labels.size() > 0)
            {
              state_labels.resize(n);
            }
            else
            {
              state_labels = atermpp::vector<STATE_LABEL_T>();
            }
          }
          else
          {
            state_labels = atermpp::vector<STATE_LABEL_T>();
          }
        }

        /** \brief Gets the number of transitions of this LTS.
         * \return The number of transitions of this LTS. */
        transitions_size_type num_transitions() const
        {
          return transitions.size();
        }

        /** \brief Sets the number of action labels of this LTS.
         * \details If space is reserved for new action labels,
         *          these are set to the default action label. */
        void set_num_action_labels(const labels_size_type n)
        {
          action_labels.resize(n);
        }

        /** \brief Gets the number of action labels of this LTS.
         * \return The number of action labels of this LTS. */
        labels_size_type num_action_labels() const
        {
          return action_labels.size();
        }

        /** \brief Gets the initial state number of this LTS.
         * \return The number of the initial state of this LTS. */
        states_size_type initial_state() const
        {
          return init_state;
        }

        /** \brief Sets the initial state number of this LTS.
         * \param[in] state The number of the state that will become the initial
         * state. */
        void set_initial_state(states_size_type state)
        {
          assert(state<nstates);
          init_state = state;
        }

        /** \brief Adds a state to this LTS.
         *  \details It is not checked whether the added state already exists.
         * \param[in] label The label of the state. If one state has a state
         *             label, all states must have state labels. If
         *             no state label is given, it must be the case that no
         *             state has a label.
         * \return The number of the added state label. */
        states_size_type add_state(const STATE_LABEL_T label=STATE_LABEL_T())
        {
          if (label==STATE_LABEL_T())
          { 
            assert(state_labels.size()==0);
          }
          else
          {
            assert(nstates==state_labels.size());
            state_labels.push_back(label);
          }
          return nstates++;
        }

        /** \brief Adds an action with a label to this LTS.
         * \details It is not checked whether this action label already exists.
         * \param[in] label The label of the label.
         * \param[in] is_tau Indicates whether the label is a tau action.
         * \return The number of the added label. */
        labels_size_type add_action(const ACTION_LABEL_T label, bool is_tau = false)
        {
          assert(action_labels.size()==taus.size());
          const labels_size_type label_index=action_labels.size();
          taus.push_back(is_tau);
          action_labels.push_back(label);
          return label_index;
        }

        /** \brief Sets the label of a state.
         * \param[in] state The number of the state.
         * \param[in] label The label that will be assigned to the state. */
        void set_state_label(states_size_type state, STATE_LABEL_T label)
        {
          assert(state<nstates);
          assert(nstates==state_labels.size());
          state_labels[state] = label;
        }

        /** \brief Sets the label of an action.
         * \param[in] label The number of the label.
         * \param[in] label The label that will be assigned to the label.
         * \param[in] is_tau Indicates whether the label is a tau action. */
        void set_action_label(labels_size_type action, ACTION_LABEL_T label, bool is_tau = false)
        { 
          assert(action<action_labels.size());
          action_labels[action] = label;
          assert(action<taus.size());
          taus[action] = is_tau;
        }
        ;

        /** \brief Gets the label of a state.
         * \param[in] state The number of the state.
         * \return The label of the state. */
        STATE_LABEL_T state_label(const states_size_type state) const
        {
          assert(state<nstates);
          assert(nstates==state_labels.size());
          return state_labels[state];
        }

        /** \brief Gets the label of an action.
         *  \param[in] action The number of the action.
         *  \return The label of the action. */
        
        ACTION_LABEL_T action_label(const labels_size_type action) const
        { 
         assert(action < action_labels.size());
         return action_labels[action];
        }

        /** \brief Clear the transitions of an lts.
         *  \details This resets the transition vector in an lts, but
         *          leaves other related items, such as state or
         *          action labels untouched. */
        void clear_transitions()
        {
          transitions = std::vector<transition>();
        }

        /** \brief Clear the action labels of an lts.
         *  \details This removes the action labels of an lts.
         *           It also resets the information
         *           regarding to what actions labels are tau. 
         *           It will not change the number of action labels. */
        void clear_actions()
        { 
          action_labels = atermpp::vector<ACTION_LABEL_T>();
          taus = std::vector<bool>();
        }

        /** \brief Clear the labels of an lts.
         *  \details This removes the action labels of an lts.
         *           It does not change the number of 
         *           state labels */
        void clear_state_labels()
        { 
          state_labels = atermpp::vector<STATE_LABEL_T>();
        }

        /** \brief Clear the transitions system.
         *  \details The state values, action values and transitions are 
         *  reset. The number of states, actions and transitions are set to 0. */
        void
        clear()
        { 
          clear_state_labels();
          clear_actions();
          clear_transitions();;
          nstates = 0;
        }

        /** \brief Gets an iterator range to the transitions of this LTS.
         * \return A const iterator range to the transitions of this LTS. */
        transition_const_range get_transitions() const
        {
          return transition_const_range(transitions);
        }

        /** \brief Gets an iterator range to the transitions of this LTS.
         * \return An iterator range to the transitions of this LTS.
         *         The transitions can be altered. */
        transition_range get_transitions()
        {
          return transition_range(transitions);
        }

        /** \brief Add a transition to the lts.
            \details The transition can be added, even if there are not (yet) valid state and
                     action labels for it. 
         */
        void add_transition(const transition &t)
        { 
          transitions.push_back(t);
        }

        /** \brief Checks whether an action is a tau action.
         * \param[in] action The number of the action.
         * \retval true if the action is a tau action;
         * \retval false otherwise.  */
        bool is_tau(labels_size_type action) const
        {
          assert(action<taus.size());
          return taus[action];
        }

        /** \brief Sets whether an action is internal, i.e., a tau action.
         * \param[in] action The number of the action.
         * \param[in] is_tau Indicates whether the action should become a tau action. */
        void set_tau(labels_size_type action, bool is_tau = true)
        {
          assert(action<taus.size());
          taus[action] = is_tau;
        }

        /** \brief Sets all actions with a string that occurs in tau_actions to tau.
         *  \details After hiding actions, it checks whether action labels are
         *           equal and merges actions with the same labels in the lts.
         *  \param[tau_actions] Vector with strings indicating which actions must be
         *       transformed to tau's */
        void hide_actions(const std::vector<std::string> &tau_actions)
        { 
          using namespace std;

          if (tau_actions.size()==0)
          { 
            return;
          }
        
          for(labels_size_type i=0; i< num_action_labels(); ++i)
          { 
            ACTION_LABEL_T a=action_label(i);
            bool is_tau=a.hide_actions(tau_actions);
            set_action_label(i,a,is_tau);
          }
        
          // Now the actions have been adapted to the hiding operator. Check now whether actions
          // did become equal.
        
          map < labels_size_type, labels_size_type> map_multiaction_indices;
          for(labels_size_type i=0; i<num_action_labels(); ++i)
          { for (labels_size_type j=0; j!=i; ++j)
            { if (action_label(i)==action_label(j))  
              { assert(map_multiaction_indices.count(i)==0);
                map_multiaction_indices.insert(pair<labels_size_type, labels_size_type>(i,j));
                break;
              }
            }
          }
        
          // If actions became equal, take care they get equal numbers in the transition
          // system, because all behavioural reduction algorithms only compare the actions.
          if (!map_multiaction_indices.empty())
          { 
            for (transition_range r=get_transitions(); !r.empty(); r.advance_begin(1))
            { 
              transition &t=r.front();
              if (map_multiaction_indices.count(t.label())>0)
              { t.set_label(map_multiaction_indices[t.label()]);
              }
            }
          }
        }

        /** \brief Checks whether this LTS has state values associated with its states.
         * \retval true if the LTS has state information;
         * \retval false otherwise.  
        */
        bool has_state_info() const
        {
          return state_labels.size() > 0;
        }

        /** \brief Sorts the transitions using a sort style.
         * \param[in] ts The sort style to use. 
         * \note Deprecated */
        inline void sort_transitions(transition_sort_style ts = src_lbl_tgt)
        {
          switch (ts)
          {
            case lbl_tgt_src:
              sort(transitions.begin(),transitions.end(),detail::compare_transitions_lts);
              break;
            case src_lbl_tgt:
            default:
              sort(transitions.begin(),transitions.end(),detail::compare_transitions_slt);
              break;
          }
        }

        /** \brief Gets the outgoing transitions for each state.
         * \details Gets an array specifying for each state, the range of
         * transitions of which that state is the source state.
         * \pre The transitions are sorted on source state number.
         * \return An array \e A of size \ref num_states()+1 such that
         * for every state <em>s</em>:
         * [ \e A[\e s] .. \e A[<em>s</em>+1] )
         * are all transitions of which \e s is the source state. 
         * \note Deprecated */
        size_t*
        get_transition_indices()
        {
          size_t *A = (size_t*)malloc((nstates+1)*sizeof(size_t));
          if (A == NULL) 
          {
            throw mcrl2::runtime_error("Out of memory.");
          }
          size_t t = 0;
          A[0] = 0;
          for (size_t s = 1; s <= nstates; ++s) {
            while (t < num_transitions() && transitions[t].from() == s-1) {
              ++t;
            }
            A[s] = t;
          }
          return A;
        }
    };

  }
}

#endif // MCRL2_LTS_LTS_H
