// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <set>
#include "mcrl2/lts/transition.h"
#include "mcrl2/lts/lts_type.h"


namespace mcrl2
{

/** \brief The main LTS namespace.
 * \details This namespace contains all data structures and members of the LTS
 * library.
 */
namespace lts
{

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

template < class STATE_LABEL_T, class ACTION_LABEL_T, class LTS_BASE>
class lts: public LTS_BASE
{
  public:

    /** \brief The type of state labels.
    */
    typedef STATE_LABEL_T state_label_t;

    /** \brief The type of action labels.
    */
    typedef ACTION_LABEL_T action_label_t;

    /** \brief The type of the used lts base.
    */
    typedef LTS_BASE base_t;

    /** \brief The sort that contains the indices of states.
    */
    typedef typename std::vector < STATE_LABEL_T >::size_type states_size_type;

    /** \brief The sort that represents the indices of labels.
    */
    typedef typename std::vector < ACTION_LABEL_T >::size_type labels_size_type;

    /** \brief The sort that contains indices of transitions.
    */
    typedef typename std::vector<transition>::size_type transitions_size_type;

  protected:

    states_size_type m_nstates;
    states_size_type m_init_state;
    std::vector<transition> m_transitions;
    std::vector<STATE_LABEL_T> m_state_labels;
    std::vector<ACTION_LABEL_T> m_action_labels; // At position 0 we always find the label that corresponds to tau.
    // The following set contains the labels that are recorded as being hidden. 
    // This allows tools to apply reductions assuming that these actions are hidden, but still provide
    // feedback, for instance using counter examples, using the original action name. 
    std::set<labels_size_type> m_hidden_label_set; 

  public:

    /** \brief Creates an empty LTS.
     */
    lts()
     : m_nstates(0)
    {
      m_action_labels.push_back(ACTION_LABEL_T::tau_action());
    }

    /** \brief Creates a copy of the supplied LTS.
     * \param[in] l The LTS to copy. */
    lts(const lts& l):
      LTS_BASE(l), 
      m_nstates(l.m_nstates),
      m_init_state(l.m_init_state),
      m_transitions(l.m_transitions),
      m_state_labels(l.m_state_labels),
      m_action_labels(l.m_action_labels),
      m_hidden_label_set(l.m_hidden_label_set)
    {
      assert(m_action_labels.size()>0 && m_action_labels[const_tau_label_index]==ACTION_LABEL_T::tau_action());
    }

    lts& operator=(const lts& l)
    {
      static_cast<LTS_BASE&>(*this)=l;
      m_nstates = l.m_nstates;
      m_init_state = l.m_init_state;
      m_transitions = l.m_transitions;
      m_state_labels = l.m_state_labels;
      m_action_labels = l.m_action_labels;
      m_hidden_label_set = l.m_hidden_label_set;
      assert(m_action_labels.size()>0 && m_action_labels[const_tau_label_index]==ACTION_LABEL_T::tau_action());
      return *this;
    }

    /** \brief Swap this lts with the supplied supplied LTS.
     * \param[in] l The LTS to swap. */
    void swap(lts& l)
    {
      static_cast<LTS_BASE&>(*this).swap(static_cast<LTS_BASE&>(l));
      {
        const states_size_type aux=m_init_state;
        m_init_state=l.m_init_state;
        l.m_init_state=aux;
      }
      {
        const states_size_type aux=m_nstates;
        m_nstates=l.m_nstates;
        l.m_nstates=aux;
      }
      m_transitions.swap(l.m_transitions);
      m_state_labels.swap(l.m_state_labels);
      m_action_labels.swap(l.m_action_labels);
      assert(m_action_labels.size()>0 && m_action_labels[const_tau_label_index]==ACTION_LABEL_T::tau_action());
      assert(l.m_action_labels.size()>0 && l.m_action_labels[const_tau_label_index]==ACTION_LABEL_T::tau_action());
      m_hidden_label_set.swap(l.m_hidden_label_set);
    }

    /** \brief Gets the number of states of this LTS.
     * \return The number of states of this LTS. */
    states_size_type num_states() const
    {
      return m_nstates;
    }

    /** \brief Provides the state labels of an LTS.
     ** \return A reference to the state label vector of the LTS.
    */
    std::vector<STATE_LABEL_T>& state_labels()
    {
      return m_state_labels;
    }

    /** \brief Provides the state labels of an LTS.
     ** \return A reference to the state label vector of the LTS.
    */
    const std::vector<STATE_LABEL_T>& state_labels() const
    {
      return m_state_labels;
    }

    /** \brief Gets the number of state labels of this LTS.
     *  \details As states do not need to have state labels,
     *  the number of state labels can differ from the number of
     *  states.
     *  \return The number of state labels of this LTS. */
    states_size_type num_state_labels() const
    {
      return m_state_labels.size();
    }

    /** \brief Sets the number of states of this LTS.
     * \param[in] n The number of states of this LTS.
     * \param[in] has_state_labels If true state labels are initialised
     */
    void set_num_states(const states_size_type n, const bool has_state_labels = true)
    {
      m_nstates = n;
      if (has_state_labels)
      {
        if (m_state_labels.size() > 0)
        {
          m_state_labels.resize(n);
        }
        else
        {
          m_state_labels = std::vector<STATE_LABEL_T>();
        }
      }
      else
      {
        m_state_labels = std::vector<STATE_LABEL_T>();
      }
    }

    /** \brief Gets the number of transitions of this LTS.
     * \return The number of transitions of this LTS. */
    transitions_size_type num_transitions() const
    {
      return m_transitions.size();
    }

    /** \brief Sets the number of action labels of this LTS.
     * \details If space is reserved for new action labels,
     *          these are set to the default action label. */
    void set_num_action_labels(const labels_size_type n)
    {
      m_action_labels.resize(n);
      assert(m_action_labels.size()>0 && m_action_labels[const_tau_label_index]==ACTION_LABEL_T::tau_action());
    } 

    /** \brief Gets the number of action labels of this LTS.
     * \return The number of action labels of this LTS. */
    labels_size_type num_action_labels() const
    {
      assert(m_action_labels.size()>0 && m_action_labels[const_tau_label_index]==ACTION_LABEL_T::tau_action());
      return m_action_labels.size();
    }

    /** \brief Gets the initial state number of this LTS.
     * \return The number of the initial state of this LTS. */
    states_size_type initial_state() const
    {
      return m_init_state;
    }

    /** \brief Sets the initial state number of this LTS.
     * \param[in] state The number of the state that will become the initial
     * state. */
    void set_initial_state(const states_size_type state)
    {
      assert(state<m_nstates);
      m_init_state = state;
    }

    /** \brief Adds a state to this LTS.
     *  \details It is not checked whether the added state already exists.
     * \param[in] label The label of the state. If one state has a state
     *             label, all states must have state labels. If
     *             no state label is given, it must be the case that no
     *             state has a label.
     * \return The number of the added state label. */
    states_size_type add_state(const STATE_LABEL_T& label=STATE_LABEL_T())
    {
      if (label!=STATE_LABEL_T())
      {
        m_state_labels.resize(m_nstates);
        m_state_labels.push_back(label);
      }
      return m_nstates++;
    }

    /** \brief Adds an action with a label to this LTS.
     * \details It is not checked whether this action label already exists.
     * \param[in] label The label of the label.
     * \return The number of the added label. */
    labels_size_type add_action(const ACTION_LABEL_T& label)
    {
      if (label==ACTION_LABEL_T::tau_action())
      {
        return const_tau_label_index;
      }
      assert(std::find(m_action_labels.begin(),m_action_labels.end(),label)==m_action_labels.end()); // Action labels must be unique. 
      const labels_size_type label_index=m_action_labels.size();
      m_action_labels.push_back(label);
      return label_index;
    }

    /** \brief Provide the index of the label that represents tau.
     *  \return const_tau_label_index, which is 0, i.e. the index of the label tau.
     */
    const labels_size_type tau_label_index() const
    {
      return const_tau_label_index;
    }

    /** \brief Sets the label of a state.
     * \param[in] state The number of the state.
     * \param[in] label The label that will be assigned to the state. */
    void set_state_label(const states_size_type state, const STATE_LABEL_T& label)
    {
      assert(state<m_nstates);
      assert(m_nstates==m_state_labels.size());
      m_state_labels[state] = label;
    }

    /** \brief The action labels in this lts. 
        \return The action labels of this lts.  **/
    const std::vector<ACTION_LABEL_T>& action_labels() const
    {
      return m_action_labels;
    } 

    /** \brief Sets the label of an action.
     * \param[in] action The number of the action.
     * \param[in] label The label that will be assigned to the action. */
     
    void set_action_label(const labels_size_type action, const ACTION_LABEL_T& label)
    {
      assert(action<m_action_labels.size());
      assert((action==const_tau_label_index) == (label==ACTION_LABEL_T::tau_action())); // a tau action is always stored at index 0.
      assert(m_action_labels[action] == label ||
             std::find(m_action_labels.begin(),m_action_labels.end(),label)==m_action_labels.end()); // Action labels must be unique. 
      m_action_labels[action] = label;
    }

    /** \brief Returns the hidden label set that tells for each label what its corresponding
               hidden label is.
        \return The hidden action set */
    const std::set<labels_size_type>& hidden_label_set() const
    {
      return m_hidden_label_set;
    }

    /** \brief Returns the hidden label set that tells for each label what its corresponding
               hidden label is.
        \return The hidden action set */
    std::set<labels_size_type>& hidden_label_set()
    {
      return m_hidden_label_set;
    }

    /** \brief Sets the hidden label map that tells for each label what its corresponding
               hidden label is. 
      * \param[in] m The new hidden label map. */
    void set_hidden_label_set(const std::set<labels_size_type>& m)
    {
      m_hidden_label_set=m;
    }

    /** \brief If the action label is registered hidden, it returns tau, otherwise the original label. 
        \param[in] action The index of an action label.
        \return The index of the corresponding action label in which actions are hidden. */
    labels_size_type apply_hidden_label_map(const labels_size_type action) const
    {
      if (m_hidden_label_set.count(action)>0)
      {
        return tau_label_index();
      }
      return action;
    }

    /** \brief Gets the label of a state.
     * \param[in] state The number of the state.
     * \return The label of the state. */
    STATE_LABEL_T state_label(const states_size_type state) const
    {
      assert(state<m_nstates);
      assert(m_nstates==m_state_labels.size());
      return m_state_labels[state];
    }

    /** \brief Gets the label of a state.
     * \param[in] state The number of the state.
     * \return The label of the state. */
    bool is_probabilistic(const states_size_type state) const
    {
      (void)state;  // Suppress an unused variable warning. 
      assert(state<m_nstates);
      return false;
    }

    /** \brief Gets the label of an action.
     *  \param[in] action The number of the action.
     *  \return The label of the action. */
    ACTION_LABEL_T action_label(const labels_size_type action) const
    {
      assert(action < m_action_labels.size());
      return m_action_labels[action];
    }

    /** \brief Clear the transitions of an lts.
     *  \param[in] n An optional parameter that indicates how
     *             many transitions are to be expected. This is
     *             used to set the reserved size of a vector, to
     *             prevent unnecessary resizing.
     *  \details This resets the transition vector in an lts, but
     *          leaves other related items, such as state or
     *          action labels untouched. */
    void clear_transitions(const std::size_t n=0)
    {
      m_transitions = std::vector<transition>();
      m_transitions.reserve(n);
    }

    /** \brief Clear the action labels of an lts.
     *  \details This removes the action labels of an lts.
     *           It also resets the information
     *           regarding to what actions labels are tau.
     *           It will not change the number of action labels. */
    void clear_actions()
    {
      m_action_labels.clear();
      m_action_labels.push_back(ACTION_LABEL_T::tau_action());
      m_hidden_label_set.clear();
    }

    /** \brief Clear the labels of an lts.
     *  \details This removes the action labels of an lts.
     *           It does not change the number of
     *           state labels */
    void clear_state_labels()
    {
      m_state_labels.clear();
    }

    /** \brief Clear the transitions system.
     *  \details The state values, action values and transitions are
     *  reset. The number of states, actions and transitions are set to 0. */
    void clear()
    {
      clear_state_labels();
      clear_actions();
      clear_transitions();
      m_nstates = 0;
    }

    /** \brief Gets a const reference to the vector of transitions of the current lts.
     *  \details As this vector can be huge, it is adviced to avoid
     *           to copy this vector.
     * \return   A const reference to the vector. */
    const std::vector<transition>& get_transitions() const
    {
      return m_transitions;
    }

    /** \brief Gets a reference to the vector of transitions of the current lts.
     *  \details As this vector can be huge, it is adviced to avoid
     *           to copy this vector.
     * \return   A reference to the vector. */
    std::vector<transition>& get_transitions()
    {
      return m_transitions;
    }

    /** \brief Add a transition to the lts.
        \details The transition can be added, even if there are not (yet) valid state and
                 action labels for it.
     */
    void add_transition(const transition& t)
    {
      m_transitions.push_back(t);
    }

    /** \brief Checks whether an action is a tau action.
     * \param[in] action The number of the action.
     * \retval true if the action is a tau action;
     * \retval false otherwise.  */
    bool is_tau(labels_size_type action) const
    {
      assert(m_action_labels.size()>0 && m_action_labels[const_tau_label_index]==ACTION_LABEL_T::tau_action());
      return (action==const_tau_label_index);
    }

    /** \brief Records all actions with a string that occurs in tau_actions internally.
     *  \details After hiding actions, it checks whether action labels are
     *           equal and merges actions with the same labels in the lts.
     *  \param[in] tau_actions Vector with strings indicating which actions must be
     *       transformed to tau's */
    void record_hidden_actions(const std::vector<std::string>& tau_actions)
    {
      if (tau_actions.size()==0)
      {
        return;
      }

      for (labels_size_type i=0; i< num_action_labels(); ++i)
      {
        ACTION_LABEL_T a=action_label(i);
        a.hide_actions(tau_actions);
        if (a==ACTION_LABEL_T())  
        {
          m_hidden_label_set.insert(i);
        }
        else if (a!=action_label(i))
        {
          set_action_label(i,a);  
        }
      }
    }

    /** \brief Apply the recorded actions that are renamed to internal actions to the lts. 
     *  \details After hiding actions, it checks whether action labels are
     *           equal and merges actions with the same labels in the lts.
     *  \param[in] tau_actions Vector with strings indicating which actions must be
     *       transformed to tau's */
    void apply_hidden_actions(void)
    {
      if (m_hidden_label_set.size()>0)    // Check whether there is something to rename.
      {
        for(transition& t: m_transitions)
        {
          if (m_hidden_label_set.count(t.label()))
          { 
            t=transition(t.from(),tau_label_index(),t.to());
          }
        }
        m_hidden_label_set.clear();       // Empty the hidden label set. 
      }
    }
    /** \brief Checks whether this LTS has state values associated with its states.
     * \retval true if the LTS has state information;
     * \retval false otherwise.
    */
    bool has_state_info() const
    {
      return m_state_labels.size() > 0;
    }

    /** \brief Checks whether this LTS has labels associated with its actions, which are numbers.
     * \retval true if the LTS has values for labels;
     * \retval false otherwise.
    */
    bool has_action_info() const
    {
      return m_action_labels.size() > 1;
    }
};

}
}

#endif // MCRL2_LTS_LTS_H
