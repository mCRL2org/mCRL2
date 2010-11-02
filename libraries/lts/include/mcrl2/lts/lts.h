// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief The main header file of the LTS library.
 * \details This is the LTS library's main header file. It declares all publicly
 * accessible data structures that form the interface of the library.
 * \author Muck van Weerdenburg
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
#ifdef USE_BCG
      lts_bcg,  /**< BCG format
                  * \note Only available if the LTS library is built with BCG
                  * support.*/
#endif
      lts_dot, /**< GraphViz format */
      lts_svc,  /**< SVC format 
                  * \note The svc format is outdated. */  
      lts_type_min = lts_none,
      lts_type_max = lts_svc
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
        
        // The variable below indicates that the state and transition labels are mCRL2 terms. 
        // A transition label is a multi-action, and a state label has the shape STATE(t1,...,tn)
        // where t_i are data_expressions.
        // If false, state values have the shape STATE(s1,...,sn) with si strings and label values are strings.
        // This ought to be replaced by Template code for state and transition labels.
        // bool state_and_label_values_are_mcrl2_terms; 

        states_size_type nstates;
        states_size_type init_state;
        std::vector<transition> transitions;
        atermpp::vector<STATE_LABEL_T> state_values;
        atermpp::vector<ACTION_LABEL_T> label_values;
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
                state_values(l.state_values),
                label_values(l.label_values),
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
          state_values.swap(l.state_values);
          taus.swap(l.taus);
          label_values.swap(l.label_values);
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
         * \return The number of state labels of this LTS. */
        states_size_type num_state_labels() const
        {
          return state_values.size();
        }

        /** \brief Sets the number of states of this LTS.
         * \param[in] n The number of states of this LTS. */
        void set_num_states(const states_size_type n, const bool has_state_values = true)
        {
          nstates = n;
          if (has_state_values)
          {
            if (state_values.size() > 0)
            {
              state_values.resize(n);
            }
            else
            {
              state_values = atermpp::vector<STATE_LABEL_T>();
            }
          }
          else
          {
            state_values = atermpp::vector<STATE_LABEL_T>();
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
          label_values.resize(n);
        }

        /** \brief Gets the number of action labels of this LTS.
         * \return The number of action labels of this LTS. */
        labels_size_type num_action_labels() const
        {
          return label_values.size();
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
         * \param[in] value The value of the state. If one state has a state
         *             label, all states must have state labels.
         * \return The number of the added state label. */
        states_size_type add_state(const STATE_LABEL_T value)
        {
          assert(nstates==state_values.size());
          state_values.push_back(value);
          return nstates++;
        }

        /** \brief Adds a state to this LTS. No state label is set. 
         *  \details It is not checked whether the added state already exists.
         *           Furthermore, no other states can have state labels.
         * \return The number of the added state label. */
        states_size_type add_state()
        {
          assert(state_values.size()==0);
          return nstates++;
        }

        /** \brief Adds an action label to this LTS.
         * \details It is not checked whether this action label already exists.
         * \param[in] value The value of the label.
         * \param[in] is_tau Indicates whether the label is a tau action.
         * \return The number of the added label. */
        labels_size_type add_label(const ACTION_LABEL_T value, bool is_tau = false)
        {
          // std::cerr << "  label_values size " << label_values.size() << "\n";
          assert(label_values.size()==taus.size());
          const labels_size_type label_index=label_values.size();
          taus.push_back(is_tau);
          label_values.push_back(value);
          return label_index;
        }

        /** \brief Sets the value of a state.
         * \param[in] state The number of the state.
         * \param[in] value The value that will be assigned to the state. */
        void set_state_value(states_size_type state, STATE_LABEL_T value)
        {
          assert(state<nstates);
          assert(nstates==state_values.size());
          state_values[state] = value;
        }

        /** \brief Sets the value of a label.
         * \param[in] label The number of the label.
         * \param[in] value The value that will be assigned to the label.
         * \param[in] is_tau Indicates whether the label is a tau action. */
        void set_label_value(labels_size_type label, ACTION_LABEL_T value, bool is_tau = false)
        { 
          assert(label<label_values.size());
          label_values[label] = value;
          assert(label<taus.size());
          taus[label] = is_tau;
        }
        ;

        /** \brief Gets the value of a state.
         * \param[in] state The number of the state.
         * \return The value of the state. */
        STATE_LABEL_T state_value(const states_size_type state) const
        {
          assert(state<nstates);
          assert(nstates==state_values.size());
          return state_values[state];
        }

        /** \brief Gets the value of a label.
         * \param[in] label The number of the label.
         * \return The value of the label. */
        
        ACTION_LABEL_T label_value(const labels_size_type label) const
        { 
         // std::cerr << "ACT LABEL " << label << "   " << label_values.size() << "\n";
         assert(label < label_values.size());
         return label_values[label];
        }

        /** \brief Clear the transitions of an lts.
         * \details This resets the transition vector in an lts, but
         *          leaves other related items, such as state or
         *          action labels untouched. */
        void clear_transitions()
        {
          transitions = std::vector<transition>();
        }

        /** \brief Clear the labels of an lts.
         * \details This removes the action labels of an lts.
         *          It also resets the information
         *          regarding to what actions labels are tau. 
         *          It will not change the number of action labels. */
        void clear_action_labels()
        { 
          label_values = atermpp::vector<ACTION_LABEL_T>();
          taus = std::vector<bool>();
        }

        /** \brief Clear the labels of an lts.
         * \details This removes the action labels of an lts.
         *          It does not change the number of 
         *          state labels */
        void clear_state_labels()
        { 
          state_values = atermpp::vector<STATE_LABEL_T>();
        }

        void
        clear()
        { 
          clear_state_labels();
          clear_action_labels();
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
         */
        void add_transition(const transition &t)
        { // std::cerr << "Add transition " << t.from() << "-" << t.label() << "->" << t.to() << "  #S " <<
          //         nstates << "#L " << label_values.size() << "\n";
          // We allow transitions to be added, even if they have not (yet) state and action labels.
          // assert((nstates==0 || t.from()<nstates) && 
          //        (t.label()<label_values.size()) && 
          //        (nstates==0 || t.to()<nstates));
          transitions.push_back(t);
        }

        /** \brief Checks whether a label is a tau action.
         * \param[in] label The number of the label.
         * \retval true if the label is a tau action;
         * \retval false otherwise.  */
        bool is_tau(labels_size_type label) const
        {
          assert(label<taus.size());
          return taus[label];
        }

        /** \brief Sets the tau attribute of a label.
         * \param[in] label The number of the label.
         * \param[in] is_tau Indicates whether the label should become a tau action. */
        void set_tau(labels_size_type label, bool is_tau = true)
        {
          assert(label<taus.size());
          taus[label] = is_tau;
        }

        /** \brief Sets all labels with string that occurs in tau_actions to tau.
         *  \details After hiding actions, it checks whether action labels became
         *           equal and merges these action labels.
         *  \param[tau_actions] Vector with strings indicating which labels must be
         *       considered to be equal to tau's */
        void hide_actions(const std::vector<std::string> &tau_actions)
        { 
          using namespace std;

          if (tau_actions.size()==0)
          { 
            return;
          }
        
          for(labels_size_type i=0; i< num_action_labels(); ++i)
          { 
            label_value(i).hide_actions(tau_actions);
          }
        
          // Now the labels have been adapted to the hiding operator. Check now whether labels
          // did become equal.
        
          map < labels_size_type, labels_size_type> map_multiaction_indices;
          for(labels_size_type i=0; i<num_action_labels(); ++i)
          { for (labels_size_type j=0; j!=i; ++j)
            { if (label_value(i)==label_value(j))  
              { assert(map_multiaction_indices.count(i)==0);
                map_multiaction_indices.insert(pair<labels_size_type, labels_size_type>(i,j));
                break;
              }
            }
          }
        
          // If labels became equal, take care they get equal numbers in the transition
          // system, because all behavioural reduction algorithms only compare the labels.
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
          return state_values.size() > 0;
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
        unsigned int*
        get_transition_indices()
        {
          unsigned int *A = (unsigned int*)malloc((nstates+1)*sizeof(unsigned int));
          if (A == NULL) 
          {
            throw mcrl2::runtime_error("Out of memory.");
          }
          unsigned int t = 0;
          A[0] = 0;
          for (unsigned int s = 1; s <= nstates; ++s) {
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
