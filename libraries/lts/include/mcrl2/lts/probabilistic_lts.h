// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file probabilistic_lts.h
 *
 * \brief The file containing the core class for transition systems.
 * \details This is the LTS library's main header file. It declares all publicly
 * accessible data structures that form the interface of the library.
 * \author Muck van Weerdenburg, Jan Friso Groote
 */

#ifndef MCRL2_LTS_PROBABILISTIC_LTS_H
#define MCRL2_LTS_PROBABILISTIC_LTS_H

#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/probabilistic_label.h"


namespace mcrl2
{

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

template < class STATE_LABEL_T, class ACTION_LABEL_T, class PROBABILISTIC_LABEL_T = probabilistic_label>
class probabilistic_lts: public lts<STATE_LABEL_T, ACTION_LABEL_T>
{
  public:

    typedef lts<STATE_LABEL_T, ACTION_LABEL_T> super;

    /** \brief The type of probabilistic labels.
    */
    typedef PROBABILISTIC_LABEL_T probabilistic_label_t;

    typedef typename super::state_label_t         state_label_t        ;
    typedef typename super::action_label_t        action_label_t       ;
    typedef typename super::states_size_type      states_size_type     ;
    typedef typename super::labels_size_type      labels_size_type     ;
    typedef typename super::transitions_size_type transitions_size_type;

  protected:

    std::vector<PROBABILISTIC_LABEL_T> m_probabilistic_labels;
    std::vector<bool> m_is_probabilistic_state; // A vector indicating for each state whether
                                                // it is an ordinary state (false), or whether
                                                // it is a probabilistic state (true). This
                                                // vector has at most the length of m_nstates.
                                                // States not in this vector are not probabilistic.

  public:

    /** \brief Creates an empty LTS.
     */
    probabilistic_lts()
    {}

    /** \brief Creates a copy of the supplied LTS.
     * \param[in] l The LTS to copy. */
    probabilistic_lts(const probabilistic_lts& l)
      : super(l),
        m_probabilistic_labels(l.m_probabilistic_labels),
        m_is_probabilistic_state(l.m_is_probabilistic_state)
    {};

    /** \brief Swap this lts with the supplied supplied LTS.
     * \param[in] l The LTS to swap. */
    void swap(probabilistic_lts& l)
    {
      super::swap(l);
      m_probabilistic_labels.swap(l.m_probabilistic_labels);
      m_is_probabilistic_state.swap(l.m_is_probabilistic_state);
    };

    /** \brief Gets the number of probabilistic labels of this LTS.
     * \return The number of action labels of this LTS. */
    labels_size_type num_probabilistic_labels() const
    {
      return m_probabilistic_labels.size();
    }

    /** \brief Adds a state to this LTS.
     *  \details It is not checked whether the added state already exists.
     * \param[in] label The label of the state. If one state has a state
     *             label, all states must have state labels. If
     *             no state label is given, it must be the case that no
     *             state has a label.
     * \param[in] is_probabilistic This boolean indicates whether this state
     *            is a probabilistic state, of which the outgoing transitions are
     *            labelled with probabilities.
     * \return The number of the added state label. */
    states_size_type add_state(const STATE_LABEL_T label=STATE_LABEL_T(), bool is_probabilistic=false)
    {
      if (label!=STATE_LABEL_T())
      {
        super::m_state_labels.resize(super::m_nstates);
        super::m_state_labels.push_back(label);
      }
      if (is_probabilistic)
      {
        m_is_probabilistic_state.resize(super::m_nstates, false);
        m_is_probabilistic_state.push_back(true);
      }
      return super::m_nstates++;
    }

    /** \brief Adds a probabilistic label to this LTS.
     * \details It is not checked whether this action label already exists.
     * \param[in] label The label of the label.
     * \return The number of the added label. */
    labels_size_type add_probabilistic_label(const PROBABILISTIC_LABEL_T label)
    {
      const labels_size_type label_index=m_probabilistic_labels.size();
      m_probabilistic_labels.push_back(label);
      return label_index;
    }

    /** \brief Sets whether this state is a probabilistic node.
     * \param[in] state The number of the state.
     * \param[in] is_probabilistic A boolean indicating whether state is probabilistic. */
    void set_is_probabilistic(states_size_type state, bool is_probabilistic)
    {
      assert(state < super::m_nstates);
      if (m_is_probabilistic_state.size()<=state)
      {
        m_is_probabilistic_state.resize(state+1);
      }
      m_is_probabilistic_state[state] = is_probabilistic;
    }

    /** \brief Sets the probabilistic label corresponding to some index.
     * \param[in] action The number of the label.
     * \param[in] label The label that will be assigned to the action.  */
    void set_probabilistic_label(labels_size_type action, PROBABILISTIC_LABEL_T label)
    {
      assert(action<m_probabilistic_labels.size());
      m_probabilistic_labels[action] = label;
    }

    /** \brief Gets the label of a state.
     * \param[in] state The number of the state.
     * \return The label of the state. */
    bool is_probabilistic(const states_size_type state) const
    {
      assert(state < super::m_nstates);
      if (state<m_is_probabilistic_state.size())
      {
        return m_is_probabilistic_state[state];
      }
      return false;
    }

    /** \brief Gets the probabilistic label of an index.
     *  \param[in] action The number of the index.
     *  \return The probabilistic label of the index. */
    const PROBABILISTIC_LABEL_T& probabilistic_label(const labels_size_type action) const
    {
      assert(action < m_probabilistic_labels.size());
      return m_probabilistic_labels[action];
    }

    /** \brief Clear the action labels of an lts.
     *  \details This removes the action labels of an lts.
     *           It also resets the information
     *           regarding to what actions labels are tau.
     *           It will not change the number of action labels. */
    void clear_actions()
    {
      super::clear_actions();
      m_probabilistic_labels = std::vector<PROBABILISTIC_LABEL_T>();
    }

    /** \brief Set all states as being non-probabilistic.
     *  \details This removes the vector with information whether states
     *           are probabilistic, effectively indicating that states are
     *           all non probabilistic. */
    void clear_is_probabilistic()
    {
      m_is_probabilistic_state.clear();
    }

    /** \brief Clear the transitions system.
     *  \details The state values, action values and transitions are
     *  reset. The number of states, actions and transitions are set to 0. */
    void
    clear()
    {
      super::clear();
      m_probabilistic_labels.clear();
      m_is_probabilistic_state.clear();
    }
};

}
}

#endif // MCRL2_LTS_LTS_H
