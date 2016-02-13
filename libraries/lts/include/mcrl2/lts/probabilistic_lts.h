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
#include "mcrl2/lts/probabilistic_state.h"


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

template < class STATE_LABEL_T, class ACTION_LABEL_T, class PROBABILISTIC_STATE_T, class LTS_BASE >
class probabilistic_lts: public lts<STATE_LABEL_T, ACTION_LABEL_T, LTS_BASE>
{
  public:

    typedef lts<STATE_LABEL_T, ACTION_LABEL_T, LTS_BASE> super;

    /** \brief The type of probabilistic labels.
    */
    typedef PROBABILISTIC_STATE_T probabilistic_state_t;

    typedef typename super::state_label_t         state_label_t        ;
    typedef typename super::action_label_t        action_label_t       ;
    typedef typename super::states_size_type      states_size_type     ;
    typedef typename super::labels_size_type      labels_size_type     ;
    typedef typename super::transitions_size_type transitions_size_type;

  protected:

    std::vector<PROBABILISTIC_STATE_T> m_probabilistic_states;
    PROBABILISTIC_STATE_T m_init_probabilistic_state;

    /* This function is made protected to prevent its use in a probabilistic transition system */  
    states_size_type initial_state() const
    {
      return super::initial_state();
    }

    /* This function is made protected to prevent its use in a probabilistic transition system */  
    void set_initial_state(const states_size_type s)
    {
      super::set_initial_state(s);
    }

  public:

    /** \brief Creates an empty LTS.
     */
    probabilistic_lts()
    {}

    /** \brief Creates a copy of the supplied LTS.
     * \param[in] l The LTS to copy. */
    probabilistic_lts(const probabilistic_lts& l)
      : super(l),
        m_probabilistic_states(l.m_probabilistic_states)
    {};

    /** \brief Swap this lts with the supplied supplied LTS.
     * \param[in] l The LTS to swap. */
    void swap(probabilistic_lts& l)
    {
      super::swap(l);
      m_probabilistic_states.swap(l.m_probabilistic_states);
    };

    /** \brief Gets the initial state number of this LTS.
      * \return The number of the initial state of this LTS. */
    const PROBABILISTIC_STATE_T& initial_probabilistic_state() const
    {
      return m_init_probabilistic_state;
    }

    /** \brief Sets the probabilistic initial state number of this LTS.
     * \param[in] state The number of the state that will become the initial
     * state. */
    void set_initial_probabilistic_state(const PROBABILISTIC_STATE_T& state)
    {
      // Prevent that the initial state of the lts, which is not used in a probabilistic state
      // has a random value.
      set_initial_state(0);
      m_init_probabilistic_state = state;
    }


    /** \brief Gets the number of probabilistic labels of this LTS.
     * \return The number of action labels of this LTS. */
    labels_size_type num_probabilistic_labels() const
    {
      return m_probabilistic_states.size();
    }

    /** \brief Adds a probabilistic state to this LTS.
     * \details It is not checked whether this action label already exists.
     * \param[in] label The label of the label.
     * \return The number of the added label. */
    labels_size_type add_probabilistic_state(const PROBABILISTIC_STATE_T& s)
    {
      const labels_size_type label_index=m_probabilistic_states.size();
      m_probabilistic_states.push_back(s);
      return label_index;
    }

    /** \brief Sets the probabilistic label corresponding to some index.
     * \param[in] action The number of the label.
     * \param[in] label The label that will be assigned to the action.  */
    void set_probabilistic_state(const labels_size_type index, const PROBABILISTIC_STATE_T& s)
    {
      assert(index<m_probabilistic_states.size());
      m_probabilistic_states[index] = s;
    }

    /** \brief Gets the probabilistic label of an index.
     *  \param[in] action The number of the index.
     *  \return The probabilistic label of the index. */
    const PROBABILISTIC_STATE_T& probabilistic_state(const labels_size_type index) const
    {
      assert(index < m_probabilistic_states.size());
      return m_probabilistic_states[index];
    }

    /** \brief Clear the probabilistic states in this probabilistic transitions system.
    */
    void clear_probabilistic_states()
    {
      m_probabilistic_states.clear();
    }

    /** \brief Clear the transitions system.
     *  \details The state values, action values and transitions are
     *  reset. The number of states, actions and transitions are set to 0. */
    void clear()
    {
      super::clear();
      m_probabilistic_states.clear();
      m_probabilistic_states.shrink_to_fit();
    }
};

}
}

#endif // MCRL2_LTS_LTS_H
