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
 * \brief Algorithms for LTS, such as equivalence reductions, determinisation, etc.
 * \details This contains the main algorithms useful to manipulate with
 * labelled transition systems. Typically, it contains algorithms for bisimulation
 * reduction, removal of tau loops, making an lts deterministic etc.
 * \author Jan Friso Groote, Bas Ploeger, Muck van Weerdenburg
 */

#ifndef MCRL2_LTS_LTS_ALGORITHM_H
#define MCRL2_LTS_LTS_ALGORITHM_H

#include <string>
#include <vector>
#include <set>
#include <stack>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <boost/bind.hpp>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/detail/liblts_bisim.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_sim.h"
#include "mcrl2/lts/detail/liblts_tau_star_reduce.h"
#include "mcrl2/exception.h"
#include "mcrl2/lts/detail/tree_set.h"
#include "mcrl2/lts/lts_equivalence.h"
#include "mcrl2/lts/lts_preorder.h"

namespace mcrl2
{
namespace lts
{

/* Auxiliary function */
template<typename T>
static bool lts_named_cmp(std::string N[], T a, T b)
{
  return N[a] < N[b];
}


/** \brief Applies a reduction algorithm to this LTS.
 * \param[in] eq The equivalence with respect to which the LTS will be
 * reduced.
 **/

template <class LTS_TYPE>
void reduce(LTS_TYPE& l, lts_equivalence eq);

/** \brief Checks whether this LTS is equivalent to another LTS.
 * \param[in] l1 The first LTS that will be compared.
 * \param[in] l2 The second LTS that will be compared.
 * \param[in] eq The equivalence with respect to which the LTSs will be
 * compared.
 * \param[in] generate_counter_examples
 * \retval true if the LTSs are found to be equivalent.
 * \retval false otherwise.
 * \warning This function alters the internal data structure of
 * both LTSs for efficiency reasons. After comparison, this LTS is
 * equivalent to the original LTS by equivalence \a eq, and
 * similarly for the LTS \a l.
 */
template <class LTS_TYPE>
bool destructive_compare(LTS_TYPE& l1,
                         LTS_TYPE& l2,
                         const lts_equivalence eq,
                         const bool generate_counter_examples = false)
{
  // Merge this LTS and l and store the result in this LTS.
  // In the resulting LTS, the initial state i of l will have the
  // state number i + N where N is the number of states in this
  // LTS (before the merge).

  switch (eq)
  {
    case lts_eq_none:
      return false;
    case lts_eq_bisim:
    {
      return detail::destructive_bisimulation_compare(l1,l2, false,false,generate_counter_examples);
    }
    case lts_eq_branching_bisim:
    {
      return detail::destructive_bisimulation_compare(l1,l2, true,false,generate_counter_examples);
    }
    case lts_eq_divergence_preserving_branching_bisim:
    {
      return detail::destructive_bisimulation_compare(l1,l2, true,true,generate_counter_examples);
    }
    case lts_eq_sim:
    {
      if (generate_counter_examples)
      {
        mCRL2log(log::warning) << "cannot generate counter example traces for simulation equivalence\n";
      }
      // Run the partitioning algorithm on this merged LTS
      size_t init_l2 = l2.initial_state() + l1.num_states();
      detail::merge(l1,l2);
      l2.clear(); // l is not needed anymore.
      detail::sim_partitioner<LTS_TYPE> sp(l1);
      sp.partitioning_algorithm();

      return sp.in_same_class(l1.initial_state(),init_l2);
    }
    case lts_eq_trace:
    {
      // Determinise first LTS
      detail::bisimulation_reduce(l1,false);
      determinise(l1);

      // Determinise second LTS
      detail::bisimulation_reduce(l2,false);
      determinise(l2);

      // Trace equivalence now corresponds to bisimilarity
      return detail::destructive_bisimulation_compare(l1,l2,false,false,generate_counter_examples);
    }
    case lts_eq_weak_trace:
    {
      // Eliminate silent steps and determinise first LTS
      detail::bisimulation_reduce(l1,true,false);
      detail::tau_star_reduce(l1);
      detail::bisimulation_reduce(l2,false);
      determinise(l1);

      // Eliminate silent steps and determinise second LTS
      detail::bisimulation_reduce(l2,true,false);
      detail::tau_star_reduce(l2);
      detail::bisimulation_reduce(l2,false);
      determinise(l2);

      // Weak trace equivalence now corresponds to bisimilarity
      return detail::destructive_bisimulation_compare(l1,l2,false,false,generate_counter_examples);
    }
    default:
      throw mcrl2::runtime_error("Comparison for this equivalence is not available");
      return false;
  }
}

/** \brief Checks whether this LTS is equivalent to another LTS.
 * \details The input labelled transition systems are duplicated in memory to carry
 *          out the comparison. When space efficiency is a concern, one can consider
 *          to use destructive_compare.
 * \param[in] l The LTS to which this LTS will be compared.
 * \param[in] eq The equivalence with respect to which the LTSs will be
 * compared.
 * \param[in] generate_counter_example If true counter examples are written to file.
 * \retval true if the LTSs are found to be equivalent.
 * \retval false otherwise.
 */
template <class LTS_TYPE>
bool compare(const LTS_TYPE& l1,
             const LTS_TYPE& l2,
             const lts_equivalence eq,
             const bool generate_counter_examples = false);

/** \brief Checks whether this LTS is smaller than another LTS according
 * to a preorder.
 * \param[in] l The LTS to which this LTS will be compared.
 * \param[in] pre The preorder with respect to which the LTSs will be
 * compared.
 * \param[in] opts The options that will be used for the comparison.
 * \retval true if this LTS is smaller than LTS \a l according to
 * preorder \a pre.
 * \retval false otherwise.
 * \warning This function alters the internal data structure of
 * both LTSs for efficiency reasons. After comparison, this LTS is
 * equivalent to the original LTS by equivalence \a eq, and
 * similarly for the LTS \a l, where \a eq is the equivalence
 * induced by the preorder \a pre (i.e. \f$eq = pre \cap
 * pre^{-1}\f$).
 */
template <class LTS_TYPE >
bool destructive_compare(LTS_TYPE& l1,
                         LTS_TYPE& l2,
                         const lts_preorder pre);

/** \brief Checks whether this LTS is smaller than another LTS according
 * to a preorder.
 * \param[in] l The LTS to which this LTS will be compared.
 * \param[in] pre The preorder with respect to which the LTSs will be
 * compared.
 * \retval true if this LTS is smaller than LTS \a l according to
 * preorder \a pre.
 * \retval false otherwise.
 */
template <class  LTS_TYPE >
bool compare(const LTS_TYPE&  l1,
             const  LTS_TYPE& l2,
             const lts_preorder pre);

/** \brief Determinises this LTS. */
template <class LTS_TYPE>
void determinise(LTS_TYPE& l);

/** \brief Checks whether all states in this LTS are reachable
 * from the initial state and remove unreachable states if required.
 * \details Runs in O(\ref num_states * \ref num_transitions) time.
 * \param[in] remove_unreachable Indicates whether all unreachable states
 *            should be removed from the LTS. This option does not
 *            influence the return value; the return value is with
 *            respect to the original LTS.
 * \retval true if all states are reachable from the initial state;
 * \retval false otherwise. */
template < class LTS_TYPE >
bool reachability_check(LTS_TYPE&  l, bool remove_unreachable = false)
{
  // First calculate which states can be reached, and store this in the array visited.
  const outgoing_transitions_per_state_t out_trans=transitions_per_outgoing_state(l.get_transitions());

  std::vector < bool > visited(l.num_states(),false);
  visited[l.initial_state()]=true;
  std::stack<size_t> todo;
  todo.push(l.initial_state());

  while (!todo.empty())
  {
    size_t state_to_consider=todo.top();
    todo.pop();
    for (outgoing_transitions_per_state_t::const_iterator i=out_trans.lower_bound(state_to_consider);
         i!=out_trans.upper_bound(state_to_consider); ++i)
    {
      assert(from(i)<l.num_states() && to(i)<l.num_states());
      if (visited[from(i)] && !visited[to(i)])
      {
        visited[to(i)]=true;
        todo.push(to(i));
      }
    }
  }

  // Property: in_visited(s) == true: state s is reachable from the initial state

  // check to see if all states are reachable from the initial state, i.e.
  // whether all bits are set.
  bool all_reachable = find(visited.begin(),visited.end(),false)==visited.end();

  if (!all_reachable && remove_unreachable)
  {
    // Remove all unreachable states, transitions from such states and labels
    // that are only used in these transitions.

    std::map < size_t , size_t > state_map;
    std::map < size_t , size_t > label_map;

    LTS_TYPE new_lts;

    size_t new_nstates = 0;
    for (size_t i=0; i<l.num_states(); i++)
    {
      if (visited[i])
      {
        state_map[i] = new_nstates;
        if (l.has_state_info())
        {
          new_lts.add_state(l.state_label(i));
        }
        else
        {
          new_lts.add_state();
        }
        new_nstates++;
      }
    }

    const std::vector<transition> &trans=l.get_transitions();
    for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
    {
      if (visited[r->from()])
      {
        label_map[r->label()] = 1;
      }
    }

    size_t new_nlabels = 0;
    for (size_t i=0; i<l.num_action_labels(); i++)
    {
      if (label_map.count(i)>0)   // Label i is used.
      {
        label_map[i] = new_nlabels;
        new_lts.add_action(l.action_label(i),l.is_tau(i));
        new_nlabels++;
      }
    }

    const std::vector<transition> &trans1=l.get_transitions();
    for (std::vector<transition>::const_iterator r=trans1.begin(); r!=trans1.end(); ++r)
    {
      if (visited[r->from()])
      {
        new_lts.add_transition(transition(state_map[r->from()],label_map[r->label()],state_map[r->to()]));
      }
    }

    new_lts.set_initial_state(state_map[l.initial_state()]);
    l.swap(new_lts);
  }

  return all_reachable;
}

/** \brief Checks whether this LTS is deterministic.
 * \retval true if this LTS is deterministic;
 * \retval false otherwise. */
template <class LTS_TYPE>
bool is_deterministic(const LTS_TYPE& l);

/** \brief Removes tau cycles by mapping all the states on a cycle to one state.
 *  \details This routine is linear in the number of states and transitions.
 *  \param[in] l Transition system to be reduced.
 *  \param[in] preserve_divergence_loops If true leave a self loop on states that resided on a tau
 *            cycle in the original transition system.
 */
/*
 * Is already defined in liblts_scc.h, which is included here.
template <class LTS_TYPE>
void scc_reduce(LTS_TYPE &l,const bool preserve_divergence_loops=false);
 */

/** \brief Merge the second lts into the first lts.
    \param[in/out] l1 The transition system in which l2 is merged.
    \param[in] l2 The second transition system, which remains unchanged
 */
template <class LTS_TYPE>
void merge(LTS_TYPE& l1, const LTS_TYPE& l)
{
  detail::merge(l1,l);
}


/* Here the implementations of the declared functions above are given.
   Originally these were in a .cpp file, before lts's were templated  */



template <class LTS_TYPE>
void reduce(LTS_TYPE& l,lts_equivalence eq)
{

  switch (eq)
  {
    case lts_eq_none:
      return;
    case lts_eq_bisim:
    {
      detail::bisimulation_reduce(l,false,false);
      return;
    }
    case lts_eq_branching_bisim:
    {
      detail::bisimulation_reduce(l,true,false);
      return;
    }
    case lts_eq_divergence_preserving_branching_bisim:
    {
      detail::bisimulation_reduce(l,true,true);
      return;
    }
    case lts_eq_sim:
    {
      // Run the partitioning algorithm on this LTS
      detail::sim_partitioner<LTS_TYPE> sp(l);
      sp.partitioning_algorithm();

      // Clear this LTS, but keep the labels
      // l.clear_type();
      l.clear_state_labels();
      l.clear_transitions();

      // Assign the reduced LTS
      l.set_num_states(sp.num_eq_classes());
      l.set_initial_state(sp.get_eq_class(l.initial_state()));

      const std::vector <transition> trans=sp.get_transitions();
      l.clear_transitions();
      for (std::vector <transition>::const_iterator i=trans.begin(); i!=trans.end(); ++i)
      {
        l.add_transition(*i);
      }
      // Remove unreachable parts

      reachability_check(l,true);

      return;
    }
    case lts_eq_trace:
      detail::bisimulation_reduce(l,false);
      determinise(l);
      detail::bisimulation_reduce(l,false);
      return;
    case lts_eq_weak_trace:
    {
      detail::bisimulation_reduce(l,true,false);
      detail::tau_star_reduce(l);
      detail::bisimulation_reduce(l,false);
      determinise(l);
      detail::bisimulation_reduce(l,false);
      return;
    }
    case lts_red_tau_star:
    {
      detail::bisimulation_reduce(l,true,false);
      detail::tau_star_reduce(l);
      detail::bisimulation_reduce(l,false);
      return;
    }
    case lts_red_determinisation:
    {
      determinise(l);
      return;
    }
    default:
      throw mcrl2::runtime_error("Unknown reduction method.");
  }
}

template <class LTS_TYPE>
bool compare(const LTS_TYPE& l1, const LTS_TYPE& l2, const lts_equivalence eq, const bool generate_counter_examples)
{
  switch (eq)
  {
    case lts_eq_none:
      return false;
    default:
      LTS_TYPE l1_copy(l1);
      LTS_TYPE l2_copy(l2);
      return destructive_compare(l1_copy,l2_copy,eq,generate_counter_examples);
  }
  return false;
}

template <class LTS_TYPE>
bool compare(const LTS_TYPE& l1, const LTS_TYPE& l2, const lts_preorder pre)
{
  LTS_TYPE l1_copy(l1);
  LTS_TYPE l2_copy(l2);
  return destructive_compare(l1_copy,l2_copy,pre);
}

template <class LTS_TYPE>
bool destructive_compare(LTS_TYPE& l1, LTS_TYPE& l2, const lts_preorder pre)
{
  switch (pre)
  {
    case lts_pre_sim:
    {
      // Merge this LTS and l and store the result in this LTS.
      // In the resulting LTS, the initial state i of l will have the
      // state number i + N where N is the number of states in this
      // LTS (before the merge).
      const size_t init_l2 = l2.initial_state() + l1.num_states();
      detail::merge(l1,l2);

      // We no longer need l, so clear it to save memory
      l2.clear();

      // Run the partitioning algorithm on this merged LTS
      detail::sim_partitioner<LTS_TYPE> sp(l1);
      sp.partitioning_algorithm();

      return sp.in_preorder(l1.initial_state(),init_l2);
    }
    case lts_pre_trace:
    {
      // Preprocessing: reduce modulo strong bisimulation equivalence.
      // This is not strictly necessary, but may reduce time/memory
      // needed for determinisation.
      detail::bisimulation_reduce(l1,false);
      detail::bisimulation_reduce(l2,false);

      // Determinise both LTSes. As postprocessing, reduce modulo
      // strong bisimulation equivalence. This is not strictly
      // necessary, but may reduce time/memory needed for simulation
      // preorder checking.
      determinise(l1);
      detail::bisimulation_reduce(l1,false);

      determinise(l2);
      detail::bisimulation_reduce(l2,false);

      // Trace preorder now corresponds to simulation preorder
      return destructive_compare(l1,l2,lts_pre_sim);
    }
    case lts_pre_weak_trace:
    {
      // Eliminate silent steps of first LTS
      detail::bisimulation_reduce(l1,true,false);
      detail::tau_star_reduce(l1);

      // Eliminate silent steps of second LTS
      detail::bisimulation_reduce(l2,true,false);
      detail::tau_star_reduce(l2);

      // Weak trace preorder now corresponds to strong trace preorder
      return destructive_compare(l1,l2,lts_pre_trace);
    }
    default:
      mCRL2log(log::error) << "Comparison for this preorder is not available\n";
      return false;
  }
}


template <class LTS_TYPE>
bool is_deterministic(const LTS_TYPE& l)
{
  outgoing_transitions_per_state_action_t trans_lut=transitions_per_outgoing_state_action_pair(l.get_transitions());

  for (outgoing_transitions_per_state_action_t::const_iterator i=trans_lut.begin(); i!=trans_lut.end(); ++i)
  {
    outgoing_transitions_per_state_action_t::const_iterator i_next=i;
    i_next++;
    if (i_next!=trans_lut.end() && from(i)==from(i_next) && label(i)==label(i_next))
    {
      // found a pair <s,l,t> and <s,l,t'>, so l is not deterministic.
      return false;
    }
  }
  return true;
}


inline
bool compare_transition_label_to_from(const transition& t1, const transition& t2)
{
  if (t1.label() != t2.label())
  {
    return t1.label() < t2.label();
  }
  else if (t1.to() != t2.to())
  {
    return t1.to() < t2.to();
  }
  else
  {
    return t1.from() < t2.from();
  }
}

namespace detail
{
inline
void get_trans(std::multimap < transition::size_type, std::pair < transition::size_type, transition::size_type > > &begin,
                      tree_set_store* tss,
                      size_t d,
                      std::vector<transition> &d_trans)
{
  if (!tss->is_set_empty(d))
  {
    if (tss->is_set_empty(tss->get_set_child_right(d)))
    {
      for (std::multimap < transition::size_type, std::pair < transition::size_type, transition::size_type > > :: const_iterator
           j=begin.lower_bound(tss->get_set_child_left(d)); j!=begin.upper_bound(tss->get_set_child_left(d)); ++j)
      {
        d_trans.push_back(transition(j->first,j->second.first,j->second.second));
      }
    }
    else
    {
      get_trans(begin,tss,tss->get_set_child_left(d),d_trans);
      get_trans(begin,tss,tss->get_set_child_right(d),d_trans);
    }
  }
}
} // namespace detail


template <class LTS_TYPE>
void determinise(LTS_TYPE& l)
{
  using namespace std;
  using namespace mcrl2::core;

  tree_set_store* tss = new tree_set_store();

  vector<transition> d_transs;
  vector<ptrdiff_t> d_states;

  // create the initial state of the DLTS
  d_states.push_back(l.initial_state());
  ptrdiff_t d_id = tss->set_set_tag(tss->create_set(d_states));
  d_states.clear();

  std::multimap < transition::size_type, std::pair < transition::size_type, transition::size_type > >
  begin=transitions_per_outgoing_state(l.get_transitions());

  l.clear_transitions();
  l.clear_state_labels();
  size_t d_ntransitions = 0;
  std::vector < transition > d_transitions;

  size_t s;
  size_t i,to,lbl,n_t;

  while (d_id < tss->get_next_tag())
  {
    // collect the outgoing transitions of every state of DLTS state d_id in
    // the vector d_transs
    detail::get_trans(begin,tss,tss->get_set(d_id),d_transs);

    // sort d_transs by label and (if labels are equal) by destination
    sort(d_transs.begin(),d_transs.end(),compare_transition_label_to_from);

    n_t = d_transs.size();
    i = 0;
    for (lbl = 0; lbl < l.num_action_labels(); ++lbl)
    {
      // compute the destination of the transition with label lbl
      while (i < n_t && d_transs[i].label() < lbl)
      {
        ++i;
      }
      while (i < n_t && d_transs[i].label() == lbl)
      {
        to = d_transs[i].to();
        d_states.push_back(to);
        while (i < n_t && d_transs[i].label() == lbl &&
               d_transs[i].to() == to)
        {
          ++i;
        }
      }
      s = tss->create_set(d_states);

      // generate the transitions to each of the next states
      if (!tss->is_set_empty(s))
      {
        d_transitions.push_back(transition(d_id,lbl,tss->set_set_tag(s)));

        if (d_ntransitions%10000 == 0)
        {
          mCRL2log(log::debug) <<
            "generated " << tss->get_next_tag() << " states and " << d_ntransitions
                         << " transitions; explored " << d_id << " states" << std::endl;
        }
      }
      d_states.clear();
    }
    d_transs.clear();
    ++d_id;
  }

  delete tss;

  l.set_num_states(d_id,false); // remove the state values, and reset the number of states.
  l.set_initial_state(0);

  for (std::vector < transition > :: const_iterator i=d_transitions.begin(); i!=d_transitions.end(); ++i)
  {
    l.add_transition(*i);
  }
  assert(is_deterministic(l));
}

} // namespace lts
} // namespace mcrl2

#endif // MCRL2_LTS_LTS_ALGORITHM_H



