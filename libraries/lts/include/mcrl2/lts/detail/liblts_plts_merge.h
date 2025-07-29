// Author(s): Hector Joao Rivera Verduzco
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// This file contains the merge algorithm that merges two plts's.
// Merges an LTS L with this LTS (say K) and stores the resulting LTS
// (say M) in this LTS datastructure, effectively replacing K.
// Conceptually, we just take the union of the sets of states and the
// sets of transitions of K and L:
//   States_M      = States_K + States_L
//   Transitions_M = Transitions_K + Transitions_L
// where + denotes set union.
// However, this assumes that States_K and States_L are disjoint,
// which is generally not the case. More specifically we have:
//   States_K = { 0, ..., N_K - 1 }   and
//   States_L = { 0, ..., N_L - 1 }
// for some N_K, N_L > 0.
// Therefore, state i of L will be numbered |N_K| + i in the resulting
// LTS M and state i of K will be numbered i in M. This yields:
//   States_M = { 0, ..., N_K + N_L - 1 }.


#ifndef MCRL2_LTS_LIBLTS_PLTS_MERGE_H
#define MCRL2_LTS_LIBLTS_PLTS_MERGE_H

#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_lts.h"

namespace mcrl2::lts::detail
{

template <class LTS_TYPE>
void plts_merge(LTS_TYPE& l1, const LTS_TYPE& l2)
{
  const std::size_t old_nstates=l1.num_states();
  const std::size_t old_n_prob_states = l1.num_probabilistic_states();
  l1.set_num_states(l1.num_states() + l2.num_states());

  // The resulting LTS will have state information only if BOTH LTSs
  // currently have state information.
  if (l1.has_state_info() && l2.has_state_info())
  {
    for (std::size_t i=0; i<l2.num_states(); ++i)
    {
      l1.add_state(l2.state_label(i));
    }
  }
  else
  {
    // remove state information from this LTS, if any
    l1.clear_state_labels();
  }

  // Before we can set the label data in a new transitions
  // array, we first have to collect the labels of both LTSs in a
  // map, of which the second element indicates the new index of each action label.

  typedef typename LTS_TYPE::action_label_t type1;
  typedef typename LTS_TYPE::labels_size_type type2;
  typedef typename std::pair< typename std::map < type1,type2 >::const_iterator, bool > insert_type;
  std::map < type1,type2 > labs;

  // Put the labels of the LTS l1 in a map.
  for (std::size_t i = 0; i < l1.num_action_labels(); ++i)
  {
    labs.insert(std::pair <typename LTS_TYPE::action_label_t,typename LTS_TYPE::labels_size_type>
                (l1.action_label(i),i));
  }
  // Add the labels for the LTS l2, and put them there with a new index if it was
  // not added yet.
  // Furthermore, update the hidden_action_map. 
  // If label a1 is mapped on a2 in l2, then this must be the same
  // in l1. It may be that label a1 did not exist yet in which case it needs
  // to be added too.


  for (std::size_t i=0; i<l2.num_action_labels(); ++i)
  {
    typename LTS_TYPE::labels_size_type new_index;
    const insert_type it= labs.insert(std::pair < type1,type2 >
                               (l2.action_label(i),l1.num_action_labels()));
    if (it.second) 
    {
      // New element has been inserted.
      new_index=l1.add_action(l2.action_label(i));
      if (l2.is_tau(l2.apply_hidden_label_map(i)))
      {
        l1.hidden_label_set().insert(new_index);
      }
    }
    else 
    {
      new_index=it.first->second; // Old index to which i is mapped.
      // If label i occurred in l1 and were not both mapped to the hidden label, raise an exception.
      if (l1.is_tau(l1.apply_hidden_label_map(new_index)) != l2.is_tau(l2.apply_hidden_label_map(i)))
      {
        throw mcrl2::runtime_error("The action " + pp(l2.action_label(i)) + " has incompatible hidden actions " +
                                       pp(l1.action_label(l1.apply_hidden_label_map(new_index))) + " and " +
                                       pp(l2.action_label(l2.apply_hidden_label_map(i))) + ".");
      }

    }
    assert(new_index==it.first->second);
  }

  // Update the label numbers of all transitions of the LTS l1 to reflect
  // the new indices as given by labs.
  std::vector<transition> &trans1=l1.get_transitions();
  for (transition& t : trans1)
  {
    t.set_label(labs[l1.action_label(t.label())]);
  }

  // Now add the transition labels of LTS l2
  // Now add the source and target states of the transitions of LTS l2.
  // The labels will be added below, depending on whether there is label
  // information in both LTSs.
  const std::vector<transition> &trans2=l2.get_transitions();
  for (const transition transition_to_add : trans2)
  {
    l1.add_transition(transition(transition_to_add.from()+old_nstates,
                                 labs[l2.action_label(transition_to_add.label())],
                                 transition_to_add.to()+old_n_prob_states));
  }
  
  // Now update the state number for each probability pairs of all
  // probabilistic states
  const std::size_t n_prob_states_l2 = l2.num_probabilistic_states();
  for (std::size_t i = 0; i < n_prob_states_l2; ++i)
  {
    typename LTS_TYPE::probabilistic_state_t new_prob_state;
    const typename LTS_TYPE::probabilistic_state_t& old_prob_state = l2.probabilistic_state(i);

    if (old_prob_state.size()>1)
    {
      for (const typename LTS_TYPE::probabilistic_state_t::state_probability_pair& sp_pair : old_prob_state)
      {
        new_prob_state.add(sp_pair.state()+ old_nstates, sp_pair.probability());
      }

    }
    else
    {
      new_prob_state.set(old_prob_state.get()+old_nstates);
    }
    l1.add_probabilistic_state(new_prob_state);
  }

  // Add the initial probabilistic state of both plts at the end of the merged plts.
  // First add the initia probabilistic state of l1
  l1.add_probabilistic_state(l1.initial_probabilistic_state());

  // Then add the initia probabilistic state of l2
  typename LTS_TYPE::probabilistic_state_t new_initial_prob_state_l2;
  if (l2.initial_probabilistic_state().size()<=1)
  {
    new_initial_prob_state_l2.set(l2.initial_probabilistic_state().get() + old_nstates);
  }
  else // If the initial state is a distribution with more than one state.
  {
    for (const typename LTS_TYPE::probabilistic_state_t::state_probability_pair& sp_pair : l2.initial_probabilistic_state())
    {
      new_initial_prob_state_l2.add(sp_pair.state() + old_nstates, sp_pair.probability());
    }
  }
  l1.add_probabilistic_state(new_initial_prob_state_l2);
}
} // namespace mcrl2::lts::detail

#endif  // MCRL2_LTS_LIBLTS_PLTS_MERGE_H
