// Author(s): Muck van Weerdenburg, Bas Ploeger, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_eq.cpp

#include <vector>
#include <set>
#include <stack>
#include <string>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/detail/liblts_bisim.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_sim.h"
#include "mcrl2/lts/detail/liblts_tau_star_reduce.h"

using namespace std;
using namespace mcrl2::core;

namespace mcrl2
{
namespace lts
{

lts_eq_options::lts_eq_options()
{
  reduce.add_class_to_state = false;
}

lts_eq_options lts_eq_no_options = lts_eq_options();


bool reduce(lts &l,lts_equivalence eq, lts_eq_options const&opts)
{

  switch ( eq )
  {
    case lts_eq_none:
      return true;
    case lts_eq_bisim:
      { detail::bisimulation_reduce(l,false,false);
        return true;
      }
    case lts_eq_branching_bisim:
      { detail::bisimulation_reduce(l,true,false);
        return true;
      } 
    case lts_eq_divergence_preserving_branching_bisim:
      { detail::bisimulation_reduce(l,true,true);
        return true;
      }
    case lts_eq_sim:
      {
        // Run the partitioning algorithm on this LTS
        sim_partitioner sp(l);
        sp.partitioning_algorithm();

        // Clear this LTS, but keep the labels
        l.clear_type();
        l.clear_states();
        l.clear_transitions();

        // Assign the reduced LTS
        l.set_num_states(sp.num_eq_classes());
        l.set_initial_state(sp.get_eq_class(l.initial_state()));
        unsigned int ntrans=l.num_transitions();
        transition *trans=sp.get_transitions(ntrans,l.transitions_size);
        l.set_transitions(trans,ntrans,l.transitions_size);
        // Remove unreachable parts
        l.sort_transitions();
        reachability_check(l,true);

        return true;
      }
    case lts_eq_trace:
      detail::bisimulation_reduce(l,false);
      determinise(l);
      detail::bisimulation_reduce(l,false);
      return true;
    case lts_eq_weak_trace:
      detail::bisimulation_reduce(l,true,false);
      detail::tau_star_reduce(l);
      detail::bisimulation_reduce(l,false);
      determinise(l);
      detail::bisimulation_reduce(l,false);
      return true;
    default:
      return false;
  }
}

bool compare(const lts &l1, const lts &l2, const lts_equivalence eq, const lts_eq_options &opts)
{ 
  switch ( eq )
  { case lts_eq_none:
      return false;
    default:
      lts l1_copy(l1);
      lts l2_copy(l2);
      return destructive_compare(l1_copy,l2_copy,eq,opts);
  }
  return false;
} 

bool destructive_compare(lts &l1, lts &l2, const lts_equivalence eq, const lts_eq_options &opts)
{
  // Merge this LTS and l and store the result in this LTS.
  // In the resulting LTS, the initial state i of l will have the
  // state number i + N where N is the number of states in this
  // LTS (before the merge).

  switch ( eq )
  {
    case lts_eq_none:
      return false;
    case lts_eq_bisim:
      { 
        unsigned int init_l2 = l2.initial_state() + l1.num_states();
        l1.merge(l2);
        l2.clear(); // l is not needed anymore.
        detail::bisim_partitioner bisim_part(l1);
        return bisim_part.in_same_class(l1.initial_state(),init_l2);
      }
    case lts_eq_branching_bisim:
      { 
        unsigned int init_l2 = l2.initial_state() + l1.num_states();
        l1.merge(l2);
        l2.clear(); // l is not needed anymore.
        detail::scc_partitioner scc_part(l1);
        scc_part.replace_transitions(false);
        l1.clear_type();
        l1.clear_states();
        l1.set_num_states(scc_part.num_eq_classes());
        l1.set_initial_state(scc_part.get_eq_class(l1.initial_state()));
        init_l2=scc_part.get_eq_class(init_l2);

        detail::bisim_partitioner bisim_part(l1,true);
        return bisim_part.in_same_class(l1.initial_state(),init_l2);
      } 
    case lts_eq_divergence_preserving_branching_bisim:
      { 
        unsigned int init_l2 = l2.initial_state() + l1.num_states();
        l1.merge(l2);
        l2.clear(); // l is not needed anymore.
        detail::scc_partitioner scc_part(l1);
        scc_part.replace_transitions(true);
        l1.clear_type();
        l1.clear_states();
        l1.set_num_states(scc_part.num_eq_classes());
        l1.set_initial_state(scc_part.get_eq_class(l1.initial_state()));
        init_l2=scc_part.get_eq_class(init_l2);

        detail::bisim_partitioner bisim_part(l1,true,true);
        return bisim_part.in_same_class(l1.initial_state(),init_l2);
      } 
    case lts_eq_sim:
      {
        // Run the partitioning algorithm on this merged LTS
        unsigned int init_l2 = l2.initial_state() + l1.num_states();
        l1.merge(l2);
        l2.clear(); // l is not needed anymore.
        sim_partitioner sp(l1);
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
        return detail::destructive_bisimulation_compare(l1,l2,false);
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
        return detail::destructive_bisimulation_compare(l1,l2,false);
      }
    default:
      gsErrorMsg("comparison for this equivalence is not available\n");
      return false;
  }
}

bool compare(const lts &l1, const lts &l2, const lts_preorder pre, lts_eq_options const&opts)
{ 
  lts l1_copy(l1);
  lts l2_copy(l2);
  return destructive_compare(l1_copy,l2_copy,pre,opts); 
}

bool destructive_compare(lts &l1, lts &l2, const lts_preorder pre, lts_eq_options const&opts)
{
  switch ( pre )
  {
    case lts_pre_sim:
      {
        // Merge this LTS and l and store the result in this LTS.
        // In the resulting LTS, the initial state i of l will have the
        // state number i + N where N is the number of states in this
        // LTS (before the merge).
        const unsigned int init_l2 = l2.initial_state() + l1.num_states();
        l1.merge(l2);

        // We no longer need l, so clear it to save memory
        l2.clear();

        // Run the partitioning algorithm on this merged LTS
        sim_partitioner sp(l1);
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
      std::cerr << "Comparison for this preorder is not available\n";
      return false;
  }
}

}
}
