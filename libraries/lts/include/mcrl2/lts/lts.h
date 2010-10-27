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
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

#include "boost/iterator/transform_iterator.hpp"

#include "mcrl2/core/detail/struct_core.h"

#include "aterm2.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/container_utility.h"

#include "mcrl2/core/parse.h"
#include "mcrl2/lts/transition.h"
#include "mcrl2/exception.h"

#ifdef USE_BCG
#include <bcg_user.h>
#endif

namespace mcrl2
{

  /** \brief The main LTS namespace.
   * \details This namespace contains all data structures and members of the LTS
   * library.
   */
  namespace lts
  {
    
    enum lts_type
    {
      lts_none, /**< unknown or no format */
      lts_lts, /**< mCRL2 SVC format */
      lts_aut, /**< Ald&eacute;baran format (CADP) */
      /* lts_svc, / **< SVC format The svc format is outdated. */  
      lts_fsm, /**< FSM format */
#ifdef USE_BCG
      lts_bcg, /**< BCG format
       * \note Only available if the LTS library is built with BCG
       * support.*/
#endif
      lts_dot, /**< GraphViz format */
      lts_type_min = lts_none,
      lts_type_max = lts_dot
    };


    /** \brief A class that contains a labelled transition system.
        \details The labels of 
    */

    template < class STATE_LABEL_T, class ACTION_LABEL_T >
    class lts_
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
        labels_size_type nlabels;
        states_size_type init_state;
        std::vector<transition> transitions;
        atermpp::vector<STATE_LABEL_T> state_values;
        atermpp::vector<ACTION_LABEL_T> label_values;
        std::vector<bool> taus; // A vector indicating which labels are to be viewed as tau's.

      public:
        // The items below are public, to allow restructuring of the code of the lts library.

        void
        clear_states()
        { 
          remove_state_values();
          nstates = 0;
        }

        void
        clear()
        { 
          clear_states();
          clear_labels();
          clear_transitions();;
        }

      public:

        /** \brief Creates an empty LTS.
         */
        lts_():nstates(0),nlabels(0)
        {};

        /** \brief Creates a copy of the supplied LTS.
         * \param[in] l The LTS to copy. */
        lts_(const lts_ &l):
                nstates(l.nstates),
                nlabels(l.nlabels),
                init_state(l.init_state),
                transitions(l.transitions),
                state_values(l.state_values),
                label_values(l.label_values),
                taus(l.taus)
        {};

        /** \brief Standard destructor for the class lts.
        */
        ~lts_()
        {}

        /** \brief Swap this lts with the supplied supplied LTS.
         * \param[in] l The LTS to swap. */
        void swap(lts_ &l)
        {
          { const states_size_type aux=init_state; init_state=l.init_state;   l.init_state=aux; }
          { const states_size_type aux=nstates;    nstates=l.nstates;         l.nstates=aux; }
          { const labels_size_type aux=nlabels;    nlabels=l.nlabels;         l.nlabels=aux; }
          transitions.swap(l.transitions);
          state_values.swap(l.state_values);
          taus.swap(l.taus);
          label_values.swap(l.label_values);
        };

        /** \brief Gets the number of states of this LTS.
         * \return The number of states of this LTS. */
        states_size_type num_states() const
        {
          return nstates;
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

        /** \brief Gets the number of labels of this LTS.
         * \return The number of labels of this LTS. */
        labels_size_type num_labels() const
        {
          return nlabels;
        }

        /** \brief Sets the number of labels of this LTS.
         *  \param [in] n The new number of labels.
         *  \param [in] has_label_info An optional boolean indicating whether the lts
         *              has label values.
         */
        void set_num_labels(labels_size_type n, bool has_label_info = true)
        {
          nlabels = n;
          taus.resize(n, false);
          if (has_label_info)
          {
            label_values.resize(n);
          }
          else
          {
            label_values = atermpp::vector<ACTION_LABEL_T>();
          }
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
         * \param[in] value The value of the state. If value is ommitted, only the state, and
         *                  no state value is added. This is only allowed, if there are no
         *                  state_values.
         * \return The number of the added state. */
        states_size_type add_state(const STATE_LABEL_T value = STATE_LABEL_T())
        {
          if (value != STATE_LABEL_T())
          {
            assert(nstates==state_values.size());
            state_values.push_back(value);
          }
          return nstates++;
        }

        

        /** \brief Adds a label to this LTS.
         * \param[in] value The value of the label.
         * \param[in] is_tau Indicates whether the label is a tau action.
         * \return The number of the added label. */
        labels_size_type add_label(const ACTION_LABEL_T value = ACTION_LABEL_T(), bool is_tau = false)
        {
          // std::cerr << "nlabels: " << nlabels << "  label_values size " << label_values.size() << "\n";
          assert(nlabels==taus.size());
          taus.push_back(is_tau);

          if (value != ACTION_LABEL_T())
          { 
            assert(nlabels==label_values.size());
            label_values.push_back(value);
          }
          else
          {
            assert(label_values.size()==0);
          }
          return nlabels++;
        }

        /** \brief Adds a label to this LTS.
         * \param[in] is_tau Indicates whether the label is a tau action.
         * \return The number of the added label. */
        labels_size_type add_label(const bool is_tau)
        {
          return add_label(ACTION_LABEL_T(), is_tau);
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
          assert(label<nlabels);
          assert(nlabels==label_values.size());
          label_values[label] = value;
          assert(nlabels==taus.size());
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
         * \details This resets the labels of an lts, but
         *          leaves the number of labels untouched.
         *          it also does not change the information
         *          regarding to what actions are tau */
        void clear_labels()
        { // std::cerr << "Clear labels\n";
          label_values = atermpp::vector<ACTION_LABEL_T>();
          taus = std::vector<bool>();
          nlabels = 0;
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
          //       nstates << "#L " << nlabels << "\n";
          assert(t.from()<nstates && t.label()<nlabels && t.to()<nstates);
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
         *  \param[tau_actions] Vector with strings indicating which labels must be
         *       considered to be equal to tau's */
        void hide_actions(const std::vector<std::string> &tau_actions)
        { 
          using namespace std;

          if (tau_actions.size()==0)
          { 
            return;
          }
        
          for(labels_size_type i=0; i< num_labels(); ++i)
          { 
            label_value(i).hide_actions(tau_actions);
          }
        
          // Now the labels have been adapted to the hiding operator. Check now whether labels
          // did become equal.
        
          map < labels_size_type, labels_size_type> map_multiaction_indices;
          for(labels_size_type i=0; i<num_labels(); ++i)
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

        /** \brief Checks whether this LTS has label values associated with its labels.
         * \retval true if the LTS has label information;
         * \retval false otherwise.  
        */
        bool has_label_info() const
        {
          return label_values.size() > 0;
        }

        /** \brief Removes the state values from all states. 
        */
        void remove_state_values()
        {
         state_values=atermpp::vector<STATE_LABEL_T>();
        }


        /** \brief Sorts the transitions using a sort style.
         * \param[in] ts The sort style to use. */
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
         * are all transitions of which \e s is the source state. */
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
