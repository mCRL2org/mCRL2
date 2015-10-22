// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/** \file
 *
 * \brief This file contains some utility functions to manipulate lts's
 * \details The function in this file typically allow to present the
 *          transitions of a state space differently.
 * \author Jan Friso Groote
 */

#ifndef MCRL2_LTS_LTS_UTILITIES_H
#define MCRL2_LTS_LTS_UTILITIES_H

// #include <string>
#include <map>
#include <set>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts.h"

namespace mcrl2
{

namespace lts
{
/// \brief Type for exploring transitions per state.
typedef std::multimap<transition::size_type, std::pair<transition::size_type, transition::size_type> >
outgoing_transitions_per_state_t;

/// \brief From state of an iterator exploring transitions per outgoing state.
inline size_t from(const outgoing_transitions_per_state_t::const_iterator& i)
{
  return i->first;
}

/// \brief Label of an iterator exploring transitions per outgoing state.
inline size_t label(const outgoing_transitions_per_state_t::const_iterator& i)
{
  return i->second.first;
}

/// \brief To state of an iterator exploring transitions per outgoing state.
inline size_t to(const outgoing_transitions_per_state_t::const_iterator& i)
{
  return i->second.second;
}

/// \brief Provide the transitions as a multimap accessible per outgoing state, useful
///        for for instance state space exploration.
inline outgoing_transitions_per_state_t transitions_per_outgoing_state(const std::vector<transition> &trans)
{
  outgoing_transitions_per_state_t result;
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    const transition t = *r;
    result.insert(std::pair<transition::size_type, std::pair<transition::size_type, transition::size_type> >(
                    t.from(), std::pair<transition::size_type, transition::size_type>(t.label(), t.to())));
  }
  return result;
}

/// \brief Provide the transitions as a multimap accessible per outgoing state, useful
///        for for instance state space exploration.
inline outgoing_transitions_per_state_t transitions_per_outgoing_state_reversed(const std::vector<transition> &trans)
{
  outgoing_transitions_per_state_t result;
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    const transition t =*r;
    result.insert(std::pair<transition::size_type, std::pair<transition::size_type, transition::size_type> >(
                    t.to(), std::pair<transition::size_type, transition::size_type>(t.label(), t.from())));
  }
  return result;
}

/// \brief Type for exploring transitions per state and action.
typedef std::multimap<std::pair<transition::size_type, transition::size_type>, transition::size_type>
outgoing_transitions_per_state_action_t;

/// \brief From state of an iterator exploring transitions per outgoing state and action.
inline size_t from(const outgoing_transitions_per_state_action_t::const_iterator& i)
{
  return i->first.first;
}

/// \brief Label of an iterator exploring transitions per outgoing state and action.
inline size_t label(const outgoing_transitions_per_state_action_t::const_iterator& i)
{
  return i->first.second;
}

/// \brief To state of an iterator exploring transitions per outgoing state and action.
inline size_t to(const outgoing_transitions_per_state_action_t::const_iterator& i)
{
  return i->second;
}

/// \brief Provide the transitions as a multimap accessible per from state and label.
inline outgoing_transitions_per_state_action_t transitions_per_outgoing_state_action_pair(const std::vector<transition> &trans)
{
  outgoing_transitions_per_state_action_t result;
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    const transition t = *r;
    result.insert(std::pair<std::pair<transition::size_type, transition::size_type>, transition::size_type>(
                    std::pair<transition::size_type, transition::size_type>(t.from(), t.label()), t.to()));
  }
  return result;
}

/// \brief Provide the transitions as a multimap accessible per from state and label, ordered backwardly.
inline outgoing_transitions_per_state_action_t transitions_per_outgoing_state_action_pair_reversed(
  const std::vector<transition> &trans)
{
  outgoing_transitions_per_state_action_t result;
  for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
  {
    const transition t = *r;
    result.insert(std::pair<std::pair<transition::size_type, transition::size_type>, transition::size_type>(
                    std::pair<transition::size_type, transition::size_type>(t.to(), t.label()), t.from()));
  }
  return result;
}

namespace detail
{

// Determine an index of the tau label. If it does not exist, and throw exception is true,
// throw an exception. Otherwise, a non existing index of an action label is returned.
template < class LTS_TYPE>
size_t determine_tau_label(const LTS_TYPE& l)
{
  // Set the tau_label to an existing label, if possible.
  // If nothing is found the tau label becomes l.num_action_labels,
  // but there will not be a tau anyhow in this case.
  size_t tau_label=l.num_action_labels();

  for (size_t i=0; i<l.num_action_labels(); ++i)
  {
    if (l.is_tau(i))
    {
      tau_label=i;
      break;
    }
  }
  if (tau_label==l.num_action_labels())
  {
    mCRL2log(mcrl2::log::debug) << "No tau label has been found.\n";
    return size_t(-1);
  }
  else
  {
    mCRL2log(mcrl2::log::debug) << "Using <" << pp(l.action_label(tau_label)) << "> as tau label.\n";
  }
  return tau_label;
}

// Make a new divergent_transition_label and replace each self loop with it.
// Return the number of the divergent transition label.
template < class LTS_TYPE >
size_t mark_explicit_divergence_transitions(LTS_TYPE& l)
{
  const typename LTS_TYPE::action_label_t lab; // nameless action label, representing divergence.
  size_t divergent_transition_label=l.add_action(lab);
  for(std::vector<transition>::iterator i=l.get_transitions().begin(); i!=l.get_transitions().end(); ++i)
  {
    if (l.is_tau(i->label()) && i->to()==i->from())
    {
      *i = transition(i->to(),divergent_transition_label,i->to());
    }
  }
  return divergent_transition_label;
}

// Replace each transition in a state that is an outgoing divergent_transition with a tau_loop in that state.
template < class LTS_TYPE >
void unmark_explicit_divergence_transitions(LTS_TYPE& l, const size_t divergent_transition_label)
{
  size_t tau_label=determine_tau_label(l);
  for(std::vector<transition>::iterator i=l.get_transitions().begin(); i!=l.get_transitions().end(); ++i)
  {
    if (i->label()==divergent_transition_label)
    { 
      assert(tau_label!=size_t(-1));
      *i = transition(i->from(),tau_label,i->from());
    }
  }
}

} // namespace detail

}
}

#endif // MCRL2_LTS_LTS_UTILITIES_H
