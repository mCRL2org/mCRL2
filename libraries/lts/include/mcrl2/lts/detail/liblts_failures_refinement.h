// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_failures_refinement.h

// This file contains an implementation of 
// T. Wang, S. Song, J. Sun, Y. Liu, J.S. Dong, X. Wang and S. Li.
// More Anti-Chain Based Refinement Checking. In proceedings ICFEM 2012, editors T. Aoki and K. Tagushi,
// Lecture Notes in Computer Science no 7635, pages 364-380, 2012.
//
// There are six algorithms. One for trace inclusion, one for failures inclusion and one for failures-divergence inclusion.
// All algorithms come in a variant with and without internal steps. 
// It is possible to generate a counter transition system in case the inclusion is answered by no.

#ifndef _LIBLTS_FAILURES_REFINEMENT_H
#define _LIBLTS_FAILURES_REFINEMENT_H

#include <mcrl2/lts/detail/liblts_bisim_gw.h>
#include <mcrl2/lts/detail/counter_example.h>

namespace mcrl2
{
namespace lts
{
namespace detail
{
  typedef size_t state_type;
  typedef size_t label_type;
  typedef std::set<state_type> set_of_states;
  typedef std::multimap <detail::state_type,detail::set_of_states> anti_chain_type;
  typedef std::set < label_type > action_label_set;

  template < class COUNTER_EXAMPLE_CONSTRUCTOR >
  class state_states_counter_example_index_triple
  {
    protected:
      detail::state_type m_state;
      detail::set_of_states m_states;
      typename COUNTER_EXAMPLE_CONSTRUCTOR::index_type m_counter_example_index;

    public:
      state_states_counter_example_index_triple()
      {}

      /// \brief Constructor.
      state_states_counter_example_index_triple(
              const state_type state, 
              const set_of_states& states, 
              const typename COUNTER_EXAMPLE_CONSTRUCTOR::index_type& counter_example_index)
       : m_state(state),
         m_states(states),
         m_counter_example_index(counter_example_index)
      {}

      /// \brief Get the state.
      state_type state() const
      {
        return m_state;
      }

      void swap(state_states_counter_example_index_triple& other)
      {
        std::swap(m_state,other.m_state);
        std::swap(m_states,other.m_states);
        std::swap(m_counter_example_index,other.m_counter_example_index);
      }

      /// \brief Get the set of states.
      const set_of_states& states() const
      {
        return m_states;
      }

      /// \brief Get the counter example index.
      const typename COUNTER_EXAMPLE_CONSTRUCTOR::index_type& counter_example_index() const
      {
        return m_counter_example_index;
      }
  };
 
  template <  class COUNTER_EXAMPLE_CONSTRUCTOR >
  inline void antichain_insert(
                  anti_chain_type& anti_chain, 
                  const state_states_counter_example_index_triple<COUNTER_EXAMPLE_CONSTRUCTOR>& impl_spec);
  template <  class COUNTER_EXAMPLE_CONSTRUCTOR >
  inline bool member_of_antichain(
                  const state_states_counter_example_index_triple<COUNTER_EXAMPLE_CONSTRUCTOR>& p, 
                  const anti_chain_type& anti_chain);


  // The class below recalls what the stable states and the states with a divergent
  // self loop of a transition system are, such that it does not have to be recalculated each time again.
  template < class LTS_TYPE >
  class lts_cache
  {
    protected:
      const LTS_TYPE& m_l;
      std::vector<std::vector<state_type> > m_tau_reachable_states;
      std::vector<std::vector<transition> > m_sorted_transitions;
      std::vector<bool> m_divergent;
      std::vector<action_label_set> m_enabled_actions;
      
    private:
      void calculate_weak_property_cache(const bool weak_reduction)
      {
        for(const transition& t: m_l.get_transitions()) 
        {
          assert(t.from()<m_l.num_states());
          if (m_l.is_tau(m_l.apply_hidden_label_map(t.label())) && weak_reduction)
          {
            m_tau_reachable_states[t.from()].push_back(t.to());  // There is an outgoing tau. 
          }
          m_sorted_transitions[t.from()].push_back(t);
          if (m_l.is_tau(m_l.apply_hidden_label_map(t.label())) && t.from()==t.to() && weak_reduction)
          {
            m_divergent[t.from()]=true;  // There is a self loop.
          }
          m_enabled_actions[t.from()].insert(m_l.apply_hidden_label_map(t.label()));
        }
      }

    public:
      
      lts_cache(const LTS_TYPE& l, const bool weak_reduction)
        : m_l(l),
          m_tau_reachable_states(l.num_states()),
          m_sorted_transitions(l.num_states()),
          m_divergent(l.num_states(),false),
          m_enabled_actions(l.num_states())
      { 
        calculate_weak_property_cache(weak_reduction);
      }

      bool stable(const state_type s) const
      {
        return m_tau_reachable_states[s].size()==0;
      }

      const std::vector<state_type>& tau_reachable_states(const state_type s) const
      {
        return m_tau_reachable_states[s];
      }

      const std::vector<transition>::const_iterator transitions_begin(const state_type s) const
      {
        assert(s<m_sorted_transitions.size());
        return m_sorted_transitions[s].begin();
      }

      const std::vector<transition>::const_iterator transitions_end(const state_type s) const
      {
        assert(s<m_sorted_transitions.size());
        return m_sorted_transitions[s].end();
      }

      bool diverges(const state_type s) const
      {
        return m_divergent[s];
      }

      const action_label_set& action_labels(const state_type s) const
      {
        return m_enabled_actions[s];
      }
  };

  template < class LTS_TYPE >
  set_of_states collect_reachable_states_via_taus(
                 const state_type s, 
                 const lts_cache<LTS_TYPE>& weak_property_cache,
                 const bool weak_reduction);

  template < class LTS_TYPE >
  set_of_states collect_reachable_states_via_an_action(
                 const state_type s, 
                 const label_type e,
                 const lts_cache<LTS_TYPE>& weak_property_cache,
                 const bool weak_reduction,
                 const LTS_TYPE& l);

  template < class LTS_TYPE >
  bool refusals_contained_in(
              const state_type impl, 
              const set_of_states& spec, 
              const lts_cache<LTS_TYPE>& weak_property_cache);
} // namespace detail

enum refinement_type { trace, failures, failures_divergence };

/* This function checks using algorithms in the paper mentioned above that
 * whether transition system l1 is included in transition system l2, in the
 * sense of trace inclusions, failures inclusion and divergence failures 
 * inclusion. If the bool weak_reduction is set, it will do so where tau's 
 * are included. When generate_counter_example is set, a labelled transition 
 * system is generated that can act as a counterexample. It consists of a 
 * trace, followed by outgoing transitions representing a refusal set. */

template < class LTS_TYPE, class COUNTER_EXAMPLE_CONSTRUCTOR = detail::dummy_counter_example_constructor >
bool destructive_refinement_checker(
                        LTS_TYPE& l1, 
                        LTS_TYPE& l2, 
                        const refinement_type refinement, 
                        const bool weak_reduction, 
                        COUNTER_EXAMPLE_CONSTRUCTOR generate_counter_example = detail::dummy_counter_example_constructor())
{
  size_t init_l2 = l2.initial_state() + l1.num_states();
  mcrl2::lts::detail::merge(l1,l2);
  l2.clear(); // No use for l2 anymore.
  // For weak-failures and failures-divergence, the existence of tau loops make a difference.
  // Therefore, we apply bisimulation reduction preserving divergences.
  // A typical example is a.(b+c) which is not weak-failures included n a.tau*.(b+c). The lhs has failure pairs
  // <a,{a}>, <a,{}> while the rhs has only failure pairs <a,{}>, as the state after the a is not stable.
  
  const bool allow_the_use_of_bisimulation_as_preprocessing=false;
  if (allow_the_use_of_bisimulation_as_preprocessing)
  {
    detail::bisim_partitioner_gw<LTS_TYPE> bisim_part(l1,weak_reduction,weak_reduction && (refinement!=trace));
    l1.clear_state_labels();
    
    // Assign the reduced LTS, and set init_l2.
    l1.set_num_states(bisim_part.num_eq_classes());
    l1.set_initial_state(bisim_part.get_eq_class(l1.initial_state()));
    init_l2=bisim_part.get_eq_class(init_l2);
    bisim_part.replace_transitions(weak_reduction,weak_reduction && (refinement!=trace));
  }

  const detail::lts_cache<LTS_TYPE> weak_property_cache(l1,weak_reduction);
  std::deque< detail::state_states_counter_example_index_triple < COUNTER_EXAMPLE_CONSTRUCTOR > > 
              working(  // let working be a stack containg the triple (init1,{s|init2-->s},root_index);
                    { detail::state_states_counter_example_index_triple< COUNTER_EXAMPLE_CONSTRUCTOR >(
                                  l1.initial_state(), 
                                  detail::collect_reachable_states_via_taus(init_l2,weak_property_cache,weak_reduction),
                                  generate_counter_example.root_index() ) });
                                                      // let antichain := emptyset;
  detail::anti_chain_type anti_chain;
  while (working.size()>0)                            // while working!=empty
  {
    detail::state_states_counter_example_index_triple < COUNTER_EXAMPLE_CONSTRUCTOR > impl_spec;   // pop (impl,spec) from working;
    impl_spec.swap(working.front());  
    working.pop_front();
    detail::antichain_insert(anti_chain,impl_spec);   // antichain := antichain united with (impl,spec);
                                                      // refusals(impl) not included in refusals(spec);

    if (refinement==failures_divergence && weak_property_cache.diverges(impl_spec.state()))
                                                      // if impl diverges
    {
      bool spec_diverges=false;
      for(const detail::state_type s: impl_spec.states())       // if spec does not diverge
      {
        if (weak_property_cache.diverges(s))
        {
          spec_diverges=true;
          break;
        }
      }
      if (!spec_diverges)
      {
        generate_counter_example.save_counter_example(impl_spec.counter_example_index(),l1);
        return false;                                 // return false; 
      }
    }
    else 
    {
      if (refinement==failures || refinement==failures_divergence)
      { 
        if (!detail::refusals_contained_in(impl_spec.state(),impl_spec.states(),weak_property_cache))
        {
          generate_counter_example.save_counter_example(impl_spec.counter_example_index(),l1);
          return false;                               // return false; 
        }
      }
      
      for(std::vector<transition>::const_iterator i=weak_property_cache.transitions_begin(impl_spec.state());
               i!=weak_property_cache.transitions_end(impl_spec.state()); ++i)
      {
        const transition& t=*i;
        const typename COUNTER_EXAMPLE_CONSTRUCTOR::index_type new_counterexample_index=
               generate_counter_example.add_transition(t.label(),impl_spec.counter_example_index());
        detail::set_of_states spec_prime;
        if (l1.is_tau(l1.apply_hidden_label_map(t.label())) && weak_reduction)                   // if e=tau then
        {
          spec_prime=impl_spec.states();        // spec' := spec;
        }
        else
        {                                           // spec' := {s' | exists s in spec. s-e->s'};
          for(const detail::state_type s: impl_spec.states())  
          {
            detail::set_of_states reachable_states_from_s_via_e=
                    detail::collect_reachable_states_via_an_action(s,l1.apply_hidden_label_map(t.label()),weak_property_cache,weak_reduction,l1);
            spec_prime.insert(reachable_states_from_s_via_e.begin(),reachable_states_from_s_via_e.end());
          }
        }
        if (spec_prime.empty())                     // if spec'={} then
        {
          generate_counter_example.save_counter_example(new_counterexample_index,l1);
          return false;                             //    return false;  
        }
                                                      // if (impl',spec') in antichain is not true then
        if (!detail::member_of_antichain(detail::state_states_counter_example_index_triple < COUNTER_EXAMPLE_CONSTRUCTOR >
                                     (t.to(),spec_prime,new_counterexample_index),anti_chain))
        {
                                                      // push(impl,spec') into working;
            working.push_back(detail::state_states_counter_example_index_triple < COUNTER_EXAMPLE_CONSTRUCTOR >(t.to(),spec_prime,new_counterexample_index));
        }
      }
    }
  }
  return true;                                      // return true;
}


namespace detail
{
  /* This function generates the set of states reachable from s within labelled
     transition system l1 by internal transitions, provided weak_reduction is true.
     Otherwise it generates a set with only state s in it.
  */
  template < class LTS_TYPE >
  set_of_states collect_reachable_states_via_taus(
              const set_of_states& s, 
              const lts_cache<LTS_TYPE>& weak_property_cache, 
              const bool weak_reduction)
  {
    set_of_states result(s);
    if (!weak_reduction)
    {
      return result;
    }
    set_of_states visited(s);
    std::deque<state_type> todo_stack(s.begin(),s.end());
    while (todo_stack.size()>0)
    {
      state_type current_state=todo_stack.front();
      todo_stack.pop_front();
      for(const state_type s: weak_property_cache.tau_reachable_states(current_state)) 
      {
        if (visited.insert(s).second)  // The element has been inserted.
        {
          todo_stack.push_back(s);
          result.insert(s);
        }
      }
    }
    
    return result;
  }

  template < class LTS_TYPE >
  set_of_states collect_reachable_states_via_taus(
                  const state_type s, 
                  const lts_cache<LTS_TYPE>& weak_property_cache, 
                  const bool weak_reduction)
  {
    set_of_states set_with_s({s});
    return collect_reachable_states_via_taus(set_with_s, weak_property_cache, weak_reduction);
  }

  template < class LTS_TYPE >
  set_of_states collect_reachable_states_via_an_action(
                 const state_type s,
                 const label_type e,  // This is already the hidden action.
                 const lts_cache<LTS_TYPE>& weak_property_cache,
                 const bool weak_reduction,
                 const LTS_TYPE& l)
  {
    const set_of_states set_before_action_e=collect_reachable_states_via_taus(s,weak_property_cache,weak_reduction);
    set_of_states states_reachable_via_e;
    for(const state_type s: set_before_action_e)
    {
      for(std::vector<transition>::const_iterator i=weak_property_cache.transitions_begin(s);
               i!=weak_property_cache.transitions_end(s); ++i)
      {
        const transition& t=*i;
        {
          if (l.apply_hidden_label_map(t.label())==e)
          { 
            assert(set_before_action_e.count(t.from())>0);
            states_reachable_via_e.insert(t.to());
          }
        }
      }
    }
    return collect_reachable_states_via_taus(states_reachable_via_e, weak_property_cache, weak_reduction);
  }

  /* This function implements the insertion of <p,state(), p.states()> in the anti_chain.
     Concretely, this means that p.states() is inserted among the sets s1,...,sn associated to p.state().
     It is important that an anti_chain contains for each state a set of states of which 
     no set is a subset of another. The idea is that if such two sets occur, it is enough
     to keep the smallest.
     If p.states() is smaller than a set si associated to p.state(), this set is removed.
     If p.states() is larger than a set si, there is no need to add p.states(), as a better candidate
     is already there. 
   */
  template <  class COUNTER_EXAMPLE_CONSTRUCTOR >
  inline void antichain_insert(
                  anti_chain_type& anti_chain, 
                  const state_states_counter_example_index_triple<COUNTER_EXAMPLE_CONSTRUCTOR>& impl_spec)
  {
    // First check whether there is a set in the antichain for impl_spec.state() which is smaller than impl_spec.states().
    // If so, impl_spec.states() does not have to be inserted in the anti_chain.
    for(anti_chain_type::const_iterator i=anti_chain.lower_bound(impl_spec.state()); i!=anti_chain.upper_bound(impl_spec.state()); ++i)
    {
      const set_of_states s=i->second;
      if (std::includes(s.begin(),s.end(),impl_spec.states().begin(),impl_spec.states().end()))  
      {
        return;
      }
    }

    // Here impl_spec.states() must be inserted in the antichain. Moreover, all sets in the antichain that 
    // are a superset of impl_spec.states() must be removed.
    
    for(anti_chain_type::iterator i=anti_chain.lower_bound(impl_spec.state()); i!=anti_chain.upper_bound(impl_spec.state()); )
    {
      const set_of_states s=i->second;
      if (std::includes(impl_spec.states().begin(),impl_spec.states().end(),s.begin(),s.end()))  
      {
        // set s must be removed. 
        i=anti_chain.erase(i);
      }
      else
      {
        ++i;
      }
    }
    anti_chain.emplace(impl_spec.state(),impl_spec.states());
  }
  
  /* This function checks whether p is contained in anti_chain. This is rather subtle.
     It is important that an anti_chain contains for each state a set of states of which 
     no set is a subset of another. The idea is that if such two sets occur, it is enough
     to keep the smallest.

     What is checked is that for the sets s1,...,sn in the antichain belonging to p.state(),
     p.states() is a superset of one of these si's. If so, it is considered that p.states() is
     in the set, or there is a smaller set than p.states(), which is even better. */
  template <  class COUNTER_EXAMPLE_CONSTRUCTOR >
  inline bool member_of_antichain(
                   const state_states_counter_example_index_triple<COUNTER_EXAMPLE_CONSTRUCTOR>& p, 
                   const anti_chain_type& anti_chain)
  {
    for(anti_chain_type::const_iterator i=anti_chain.lower_bound(p.state()); i!=anti_chain.upper_bound(p.state()); ++i)
    {
      const set_of_states s=i->second;
      if (std::includes(s.begin(),s.end(),p.states().begin(),p.states().end()))  // Check that there is a set s in anti_chain which is contained in p.states().
      {
        return true;
      }
    }
    return false;
  }

  /* Calculate the enabled sets of actions of states reachable via tau's */

  template < class LTS_TYPE >
  std::vector < action_label_set > calculate_stable_enabled_sets(
        const set_of_states& s, 
        const lts_cache<LTS_TYPE>& weak_property_cache)
  {
    set_of_states visited;
    std::deque < state_type > todo_stack(s.begin(), s.end());
    std::vector < action_label_set > result;

    while (todo_stack.size()>0)
    {
      const state_type current_state=todo_stack.front();
      todo_stack.pop_front();
      if (weak_property_cache.stable(current_state))
      {
        // Put the outgoing action labels in a set and put these in the result.
        result.push_back(weak_property_cache.action_labels(current_state));
      }
      else
      {
        // Put the states reachable in one tau step onto the todo stack, if they have not 
        // been visited yet. 
        for(const state_type s: weak_property_cache.tau_reachable_states(current_state)) 
                                                      // This should be done per state.
        {
          if (visited.insert(s).second) // t.to() is a new state.
          {
            todo_stack.push_back(s);
          }
        }
      }
    }
    return result;
  }

  /* This function checks that the refusals(impl) are contained in the refusals of spec, where
     the refusals of spec are defined by { r | exists s in spec. r in refusals(s) }.
     This is equivalent to saying that for all stable states s' reachable via tau's from impl there is a stable t'
     reachable via tau's from some t in spec, enable(t') is contained in enable(s'). The last expression is calculated below. */
  template < class LTS_TYPE >
  bool refusals_contained_in(
              const state_type impl, 
              const set_of_states& spec, 
              const lts_cache<LTS_TYPE>& weak_property_cache)
  {
    // This function calculates whether refusals(impl) are not included in the refusals(spec).
    // This is equivalent to:
    // There is a tau-reachable stable state s' from impl, such that for each tau-reachable stable state s''
    // from any of the states in spec: enable(s'')\enable(s') is not empty.

    // First calculate the refusal sets reachable from spec.
    
    std::vector < action_label_set > enabled_stable_sets_of_specification=calculate_stable_enabled_sets(spec,weak_property_cache);

    // Now walk through the tau-reachable stable states s' of impl.
    set_of_states visited;
    std::deque < state_type > todo_stack;
    todo_stack.push_back(impl);

    while (todo_stack.size()>0)
    {
      const state_type current_state=todo_stack.front();
      todo_stack.pop_front();
      if (weak_property_cache.stable(current_state))
      {
        // Put the outgoing action labels in a set and put these in the result.
       
        const action_label_set& impl_enabled_action_set=weak_property_cache.action_labels(current_state);

        bool success=false;
        // Compare the obtained enable set of s' with all those of the specification.
        for(action_label_set spec_action_labels: enabled_stable_sets_of_specification)
        {
          // Check whether the enabled actions of spec are included in the enabled actions of impl.
          // This is equivalent to checking that all spec_action_labels are in the impl_enabled_action_set.
          bool inclusion_success=true;
          for(const label_type a: spec_action_labels)
          {
            if (impl_enabled_action_set.count(a)==0) // action in spec_actions_labels is not in this implementation set. This is not ok.
            {
              inclusion_success=false;
              break;
            }
          }
          if (inclusion_success)
          {
            success=true;
            break;
          }
        }
        if (!success)
        {
          return false;
        }
      }
      else
      {
        // Put the states reachable in one tau step onto the todo stack, if they have not 
        // been visited yet. 
        for(const state_type s: weak_property_cache.tau_reachable_states(current_state)) 
        {
          if (visited.insert(s).second) // s is a new state.
          {
            todo_stack.push_back(s);
          }
        }

      }
    }

    return true;   
  }
  
  
} // namespace detail
} // namespace lts
} // namespace mcrl2

#endif //  _LIBLTS_FAILURES_REFINEMENT_H


