// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// This file contains the merge algorithm that merges two lts's.
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


#ifndef MCRL2_LTS_LIBLTS_MERGE_H
#define MCRL2_LTS_LIBLTS_MERGE_H

#include "mcrl2/atermpp/map.h"
#include "mcrl2/lts/lts.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

template <class LTS_TYPE>
    void merge(LTS_TYPE &l1, const LTS_TYPE &l2)
{ 
  const unsigned int old_nstates=l1.num_states();
  l1.set_num_states(l1.num_states() + l2.num_states());
  

  // The resulting LTS will have state information only if BOTH LTSs
  // currently have state information.
  if ( l1.has_state_info() && l2.has_state_info() )
  {
    for (unsigned int i=0; i<l2.num_states(); ++i)
    {
      l1.add_state(l2.state_value(i));
    }
  }
  else
  {
    // remove state information from this LTS, if any
    l1.clear_states();
  }

  typename LTS_TYPE::labels_size_type new_nlabels = 0;
  if (l1.has_label_info() && l2.has_label_info())
  {
    // Before we can set the label data in the realloc'ed transitions
    // array, we first have to collect the labels of both LTSs in a
    // map, of which the second element indicates the index of each action label.
    // ATermIndexedSet labs = ATindexedSetCreate(l1.num_labels() + l2.num_labels(),75);
    typedef typename LTS_TYPE::action_label_t type1;
    typedef typename LTS_TYPE::labels_size_type type2;
    atermpp::map < type1,type2 > labs;
    // ATbool b;

    // Add the labels of this LTS and count the number of labels that
    // the resulting LTS will contain
    for (unsigned int i = 0; i < l1.num_labels(); ++i)
    {
      /* ATindexedSetPut(labs,l1.label_value(i).label(),&b);
      if ( b )
      */
      if (labs.insert(std::pair <typename LTS_TYPE::action_label_t,typename LTS_TYPE::labels_size_type> 
               (l1.label_value(i),new_nlabels)).second)
      {
        ++new_nlabels;
      } 
    }
    // Same for LTS l2
    for (unsigned int i=0; i<l2.num_labels(); ++i)
    {
      /* ATindexedSetPut(labs,l2.label_value(i).label(),&b);
      if ( b ) */
      if (labs.insert(std::pair <type1,type2> 
               (l2.label_value(i),new_nlabels)).second)
      {
        ++new_nlabels;
      }
    }

    // Update the tau-information
    std::vector<bool> new_taus(new_nlabels,false);
    for (unsigned int i = 0; i < l1.num_labels(); ++i)
    { 
      // assert(ATindexedSetGetIndex(labs,l1.label_value(i).label())<(int)new_taus.size());
      // new_taus[ATindexedSetGetIndex(labs,l1.label_value(i).label())] = l1.is_tau(i);
      assert(labs[l1.label_value(i)] < (unsigned int)new_taus.size());
      new_taus[labs[l1.label_value(i)]] = l1.is_tau(i);
    }
    for (unsigned int i = 0; i < l2.num_labels(); ++i)
    { 
      // assert(ATindexedSetGetIndex(labs,l2.label_value(i).label())<(int)new_taus.size());
      // new_taus[ATindexedSetGetIndex(labs,l2.label_value(i).label())] = l2.is_tau(i);
      assert(labs[l2.label_value(i)] < new_taus.size());
      new_taus[labs[l2.label_value(i)]] = l2.is_tau(i);
    }
    
    // Store the label values contained in the indexed set
    l1.clear_labels(); 

    for (typename atermpp::map < type1,type2 >::const_iterator i = labs.begin(); i != labs.end(); ++i)
    {
      l1.add_label(i->first,new_taus[i->second]);
    }

    // Update the label numbers of all transitions of this LTS to
    // the new indices as given by the indexed set.
    
    for (transition_range r = l1.get_transitions(); !r.empty(); r.advance_begin(1))
    { 
      r.front().set_label(labs[l1.label_value(r.front().label())]);
    }
    // Now add the transition labels of LTS l2

    // Now add the source and target states of the transitions of LTS l2.
    // The labels will be added below, depending on whether there is label
    // information in both LTSs.
    for (transition_const_range r = l2.get_transitions(); !r.empty(); r.advance_begin(1))
    { const transition transition_to_add=r.front();
      l1.add_transition(transition(transition_to_add.from()+old_nstates,
                                labs[l2.label_value(transition_to_add.label())],
                                transition_to_add.to()+old_nstates));
    }
  }
  else
  {
    // One of the LTSs does not have label info, so the resulting LTS
    // will not have label info either. Moreover, we consider the sets
    // of labels of the LTSs to be disjoint
    const unsigned int old_nlabels=l1.num_labels();

    // Remove label info from this LTS, if any
    if ( l1.has_label_info() )
    {
      l1.set_num_labels(old_nlabels,false);
    }
    // Now add the source and target states of the transitions of LTS l2.
    // The labels will be added below, depending on whether there is label
    // information in both LTSs.
    
    // Add taus from LTS l2
    for (unsigned int i = 0; i < l2.num_labels(); ++i)
    {
      l1.add_label(l2.is_tau(i));
    }

    for (transition_const_range r = l2.get_transitions(); !r.empty(); r.advance_begin(1))
    { const transition transition_to_add=r.front();
      l1.add_transition(transition(transition_to_add.from()+old_nstates,
                                   transition_to_add.label()+old_nlabels,
                                   transition_to_add.to()+old_nstates));
    }

  }

  // Update the fields that have not been updated yet
}
} // namespace detail
} // namespace lts
} // namespace mcrl2


#endif
