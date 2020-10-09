// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

#include "mcrl2/lts/lts_lts.h"

namespace mcrl2
{

namespace lts
{

/** \brief Sorts the transitions using a sort style.
 * \param[in/out] transitions A vector of transitions to be sorted. 
 * \param[in] hidden_label_set A set that tells which actions are to be interpreted as being hidden.
 *            Sorting takes place after mapping hidden actions to zero.
 * \param[in] ts The sort style to use.
 */

inline void sort_transitions(std::vector<transition>& transitions, 
                      const std::set<transition::size_type>& hidden_label_set,
                      transition_sort_style ts = src_lbl_tgt)
{
  switch (ts)
  {
    case lbl_tgt_src:
    {
      const detail::compare_transitions_lts compare(hidden_label_set);
      sort(transitions.begin(),transitions.end(),compare);
      break;
    }
    case src_lbl_tgt:
    default:
    {
      const detail::compare_transitions_slt compare(hidden_label_set);
      sort(transitions.begin(),transitions.end(),compare);
      break;
    }
  }
}

/** \brief Sorts the transitions using a sort style.
 * \param[in/out] transitions A vector of transitions to be sorted. 
 * \param[in] ts The sort style to use.
 */

inline void sort_transitions(std::vector<transition>& transitions, transition_sort_style ts = src_lbl_tgt)
{
  sort_transitions(transitions, std::set<transition::size_type>(), ts);
}


namespace detail
{

// An indexed sorted vector below contains the outgoing or incoming transitions per state,
// grouped per state. The input consists of a vector of transitions. The incoming/outcoming
// tau transitions are grouped by state in the m_states_with_outgoing_or_incoming_transition. 
// It is as long as the lts aut has transitions. The vector m_indices is as long as the number
// of states plus 1. For each state it contains the place in the other vector where its tau transitions
// start. So, the tau transitions reside at position indices[s] to indices[s+1]. These indices
// can be acquired using the functions lowerbound and upperbound. 
// This data structure is chosen due to its minimal memory and time footprint. 
template <class CONTENT>
class indexed_sorted_vector_for_transitions
{
  protected:
    typedef std::size_t state_type;
    typedef std::size_t label_type;
    typedef std::pair<label_type,state_type> label_state_pair;

    std::vector < CONTENT > m_states_with_outgoing_or_incoming_transition;
    std::vector <size_t> m_indices;

  public:

    indexed_sorted_vector_for_transitions(const std::vector < transition >& transitions , state_type num_states, bool outgoing)
     : m_indices(num_states+1,0)
    {
      // First count the number of outgoing transitions per state and put it in indices.
      for(const transition& t: transitions)
      {
        m_indices[outgoing?t.from():t.to()]++;
      }

      // Calculate the m_indices where the states with outgoing/incoming tau transition must be placed.
      // Put the starting index for state i at position i-1. When placing the transitions these indices
      // are decremented properly. 
      
      size_t sum=0;
      for(state_type& i: m_indices)  // The vector is changed. This must be a reference. 
      {
        sum=sum+i;
        i=sum;
      }

      // Now declare enough space for all transitions and store them in reverse order, while
      // at the same time decrementing the indices in m_indices. 
      m_states_with_outgoing_or_incoming_transition.resize(sum);
      for(const transition& t: transitions)
      {
        if (outgoing)
        {
          assert(t.from()<m_indices.size());
          assert(m_indices[t.from()]>0);
          m_indices[t.from()]--;
          assert(m_indices[t.from()] < m_states_with_outgoing_or_incoming_transition.size());
          m_states_with_outgoing_or_incoming_transition[m_indices[t.from()]]=label_state_pair(t.label(), t.to());
        }
        else
        {
          assert(t.to()<m_indices.size());
          assert(m_indices[t.to()]>0);
          m_indices[t.to()]--;
          assert(m_indices[t.to()] < m_states_with_outgoing_or_incoming_transition.size());
          m_states_with_outgoing_or_incoming_transition[m_indices[t.to()]]=label_state_pair(t.label(), t.from());
        }
      }
      assert(m_indices.at(num_states)==m_states_with_outgoing_or_incoming_transition.size());
    }

    // Get the indexed transitions. 
    const std::vector<CONTENT>& get_transitions() const
    {
      return m_states_with_outgoing_or_incoming_transition;
    }
  
    // Get the lowest index of incoming/outging transitions stored in m_states_with_outgoing_or_incoming_transition.
    size_t lowerbound(const state_type s) const
    {
      assert(s+1<m_indices.size());
      return m_indices[s];
    }

    // Get 1 beyond the higest index of incoming/outging transitions stored in m_states_with_outgoing_or_incoming_transition.
    size_t upperbound(const state_type s) const
    {
      assert(s+1<m_indices.size());
      return m_indices[s+1];
    }

    // Drastically clear the vectors by resetting its memory usage to minimal. 
    void clear()   
    {
      std::vector <state_type>().swap(m_states_with_outgoing_or_incoming_transition);
      std::vector <size_t>().swap(m_indices);
      
    }
};

} // end namespace detail 

//
/// \brief Type for exploring transitions per state.
typedef std::pair<transition::size_type, transition::size_type> outgoing_pair_t;

typedef detail::indexed_sorted_vector_for_transitions < outgoing_pair_t > outgoing_transitions_per_state_t;

/// \brief Label of a pair of a label and target state. 
inline std::size_t label(const outgoing_pair_t& p)
{
  return p.first;
}

/// \brief Target state of a label state pair. 
inline std::size_t to(const outgoing_pair_t& p)
{
  return p.second;
}

/// \brief Type for exploring transitions per state and action.
// It can be considered to replace this function with an unordered_multimap.
// This may increase memory requirements, but would allow for constant versus logarithmic access times
// of elements. 
typedef std::multimap<std::pair<transition::size_type, transition::size_type>, transition::size_type>
                     outgoing_transitions_per_state_action_t;

/// \brief From state of an iterator exploring transitions per outgoing state and action.
inline std::size_t from(const outgoing_transitions_per_state_action_t::const_iterator& i)
{
  return i->first.first;
}

/// \brief Label of an iterator exploring transitions per outgoing state and action.
inline std::size_t label(const outgoing_transitions_per_state_action_t::const_iterator& i)
{
  return i->first.second;
}

/// \brief To state of an iterator exploring transitions per outgoing state and action.
inline std::size_t to(const outgoing_transitions_per_state_action_t::const_iterator& i)
{
  return i->second;
}

/// \brief Provide the transitions as a multimap accessible per from state and label.
inline outgoing_transitions_per_state_action_t transitions_per_outgoing_state_action_pair(const std::vector<transition>& trans)
{
  outgoing_transitions_per_state_action_t result;
  for (const transition& t: trans)
  {
    result.insert(std::pair<std::pair<transition::size_type, transition::size_type>, transition::size_type>(
                    std::pair<transition::size_type, transition::size_type>(t.from(), t.label()), t.to()));
  }
  return result;
}

/// \brief Provide the transitions as a multimap accessible per from state and label.
inline outgoing_transitions_per_state_action_t transitions_per_outgoing_state_action_pair(
                 const std::vector<transition>& trans, 
                 const std::set<transition::size_type>& hide_label_set)
{
  outgoing_transitions_per_state_action_t result;
  for (const transition& t: trans)
  {
    result.insert(std::pair<std::pair<transition::size_type, transition::size_type>, transition::size_type>(
                    std::pair<transition::size_type, transition::size_type>(t.from(), detail::apply_hidden_labels(t.label(),hide_label_set)), t.to()));
  }
  return result;
} 

/// \brief Provide the transitions as a multimap accessible per from state and label, ordered backwardly.
inline outgoing_transitions_per_state_action_t transitions_per_outgoing_state_action_pair_reversed(const std::vector<transition>& trans)
{
  outgoing_transitions_per_state_action_t result;
  for (const transition& t: trans)
  {
    result.insert(std::pair<std::pair<transition::size_type, transition::size_type>, transition::size_type>(
                    std::pair<transition::size_type, transition::size_type>(t.to(), t.label()), t.from()));
  }
  return result;
}

/// \brief Provide the transitions as a multimap accessible per from state and label, ordered backwardly.
inline outgoing_transitions_per_state_action_t transitions_per_outgoing_state_action_pair_reversed(
                const std::vector<transition>& trans,
                const std::set<transition::size_type>& hide_label_set)
{
  outgoing_transitions_per_state_action_t result;
  for (const transition& t: trans)
  {
    result.insert(std::pair<std::pair<transition::size_type, transition::size_type>, transition::size_type>(
                    std::pair<transition::size_type, transition::size_type>(t.to(), detail::apply_hidden_labels(t.label(),hide_label_set)), t.from()));
  }
  return result;
} 

namespace detail
{
// Yields a label with an obscure name referring to divergence.

template < class LABEL_TYPE >
// LABEL_TYPE make_divergence_label(const std::string& s, const LABEL_TYPE& l)
LABEL_TYPE make_divergence_label(const std::string& s)
{
  return LABEL_TYPE(s);
}

template <>
inline mcrl2::lts::action_label_lts make_divergence_label<mcrl2::lts::action_label_lts>(const std::string& s)
{
  return action_label_lts(lps::multi_action(process::action(process::action_label(core::identifier_string(s),
                                                                                  data::sort_expression_list()),
                                                            data::data_expression_list())));
}

// Make a new divergent_transition_label and replace each self loop with it.
// Return the number of the divergent transition label.
template < class LTS_TYPE >
std::size_t mark_explicit_divergence_transitions(LTS_TYPE& l)
{
  // Below we create an odd action label, representing divergence.
  const typename LTS_TYPE::action_label_t lab=make_divergence_label<typename LTS_TYPE::action_label_t>("!@*&divergence&*@!"); 
  std::size_t divergent_transition_label=l.add_action(lab);
  assert(divergent_transition_label+1==l.num_action_labels());
  for(transition& t: l.get_transitions())
  {
    if (l.is_tau(l.apply_hidden_label_map(t.label())) && t.to()==t.from())
    {
      t = transition(t.to(),divergent_transition_label,t.to());
    }
  }
  return divergent_transition_label;
}

// Replace each transition in a state that is an outgoing divergent_transition with a tau_loop in that state.
template < class LTS_TYPE >
void unmark_explicit_divergence_transitions(LTS_TYPE& l, const std::size_t divergent_transition_label)
{
  for(transition& t: l.get_transitions())
  {
    if (t.label()==divergent_transition_label)
    { 
      t = transition(t.from(),l.tau_label_index(),t.from());
    }
  }
}

} // namespace detail

}
}

#endif // MCRL2_LTS_LTS_UTILITIES_H
