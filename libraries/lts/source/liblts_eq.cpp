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
#include "mcrl2/lts/lts.h"
#include "mcrl2/lts/detail/liblts_bisim.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_sim.h"

using namespace std;
using namespace mcrl2::core;

namespace mcrl2
{
namespace lts
{

lts_eq_options::lts_eq_options()
{
  reduce.add_class_to_state = false;
  reduce.tau_actions.clear();
}

lts_eq_options lts_eq_no_options = lts_eq_options();

void lts_reduce_add_tau_actions(lts_eq_options& opts, std::string const& act_names)
{
  string::size_type lastpos = 0, pos;
  while ( (pos = act_names.find(',',lastpos)) != string::npos )
  {
    opts.reduce.tau_actions.push_back(act_names.substr(lastpos,pos-lastpos));
    lastpos = pos+1;
  }
  opts.reduce.tau_actions.push_back(act_names.substr(lastpos));
}

bool lts::reduce(lts_equivalence eq, lts_eq_options const&opts)
{
  // set_tau_actions(&opts.reduce.tau_actions); TODO. This might not be properly dealt with now.

  switch ( eq )
  {
    case lts_eq_none:
    case lts_eq_isomorph:
      return true;
    case lts_eq_bisim:
      { bisimulation_reduce(false,false);
        return true;
      }
    case lts_eq_branching_bisim:
      { bisimulation_reduce(true,false);
        return true;
      } 
    case lts_eq_divergence_preserving_branching_bisim:
      { bisimulation_reduce(true,true);
        return true;
      }
    case lts_eq_sim:
      {
        // Run the partitioning algorithm on this LTS
        sim_partitioner sp(this);
        sp.partitioning_algorithm();

        // Clear this LTS, but keep the labels
        clear_type();
        clear_states();
        clear_transitions();

        // Assign the reduced LTS
        nstates = sp.num_eq_classes();
        init_state = sp.get_eq_class(init_state);
        transitions = sp.get_transitions(ntransitions,transitions_size);

        // Remove unreachable parts
        sort_transitions();
        reachability_check(true);

        return true;
      }
    case lts_eq_trace:
      bisimulation_reduce(false);
      determinise();
      bisimulation_reduce(false);
      return true;
    case lts_eq_weak_trace:
      bisimulation_reduce(true,false);
      tau_star_reduce();
      bisimulation_reduce(false);
      determinise();
      bisimulation_reduce(false);
      return true;
    default:
      return false;
  }
}

bool lts::compare(const lts &l, const lts_equivalence eq, const lts_eq_options &opts) const
{ switch ( eq )
  { case lts_eq_none:
      return false;
    default:
      lts l_copy(l);
      lts this_copy(*this);
      return this_copy.destructive_compare(l_copy,eq,opts);
  }
  return false;
} 

bool lts::destructive_compare(lts &l, const lts_equivalence eq, lts_eq_options const&opts)
{
  // Merge this LTS and l and store the result in this LTS.
  // In the resulting LTS, the initial state i of l will have the
  // state number i + N where N is the number of states in this
  // LTS (before the merge).

  // set_tau_actions(&opts.reduce.tau_actions); TODO. This might not be properly dealt with now.

  switch ( eq )
  {
    case lts_eq_none:
      return false;
    case lts_eq_bisim:
      { 
        unsigned int init_l = l.initial_state() + nstates;
        merge(&l);
        l.clear(); // l is not needed anymore.
        detail::bisim_partitioner bisim_part(*this);
        return bisim_part.in_same_class(init_state,init_l);
      }
    case lts_eq_branching_bisim:
      { 
        unsigned int init_l = l.initial_state() + nstates;
        merge(&l);
        l.clear(); // l is not needed anymore.
        detail::scc_partitioner scc_part(*this);
        scc_part.replace_transitions(false);
        clear_type();
        clear_states();
        nstates=scc_part.num_eq_classes();
        init_state=scc_part.get_eq_class(init_state);
        init_l=scc_part.get_eq_class(init_l);

        detail::bisim_partitioner bisim_part(*this,true);
        return bisim_part.in_same_class(init_state,init_l);
      } 
    case lts_eq_divergence_preserving_branching_bisim:
      { 
        unsigned int init_l = l.initial_state() + nstates;
        merge(&l);
        l.clear(); // l is not needed anymore.
        detail::scc_partitioner scc_part(*this);
        scc_part.replace_transitions(true);
        clear_type();
        clear_states();
        nstates=scc_part.num_eq_classes();
        init_state=scc_part.get_eq_class(init_state);
        init_l=scc_part.get_eq_class(init_l);

        detail::bisim_partitioner bisim_part(*this,true,true);
        return bisim_part.in_same_class(init_state,init_l);
      } 
    case lts_eq_sim:
      {
        // Run the partitioning algorithm on this merged LTS
        unsigned int init_l = l.initial_state() + nstates;
        merge(&l);
        l.clear(); // l is not needed anymore.
        sim_partitioner sp(this);
        sp.partitioning_algorithm();

        return sp.in_same_class(init_state,init_l);
      }
    case lts_eq_trace:
      {
        // Determinise first LTS
        bisimulation_reduce(false);
        this->determinise();

        // Determinise second LTS
        l.bisimulation_reduce(false);
        l.determinise();

        // Trace equivalence now corresponds to bisimilarity
        return bisimulation_compare(l,false);
      }
    case lts_eq_weak_trace:
      {
        // Eliminate silent steps and determinise first LTS
        bisimulation_reduce(true,false);
        this->tau_star_reduce();
        bisimulation_reduce(false);
        this->determinise();

        // Eliminate silent steps and determinise second LTS
        l.bisimulation_reduce(true,false);
        l.tau_star_reduce();
        l.bisimulation_reduce(false);
        l.determinise();

        // Weak trace equivalence now corresponds to bisimilarity
        return bisimulation_compare(l,false);
      }
    case lts_eq_isomorph:
    default:
      gsErrorMsg("comparison for this equivalence is not available\n");
      return false;
  }
}

bool lts::compare(const lts &l, const lts_preorder pre, lts_eq_options const&opts) const
{ lts l_copy(l);
  lts this_copy(*this);
  return this_copy.destructive_compare(l_copy,pre,opts); 
}

bool lts::destructive_compare(lts &l, const lts_preorder pre, lts_eq_options const&opts)
{
  // set_tau_actions(&opts.reduce.tau_actions); TODO. This might not be properly dealt with now.
  switch ( pre )
  {
    case lts_pre_sim:
      {
        // Merge this LTS and l and store the result in this LTS.
        // In the resulting LTS, the initial state i of l will have the
        // state number i + N where N is the number of states in this
        // LTS (before the merge).
        unsigned int init_l = l.initial_state() + nstates;
        merge(&l);

        // We no longer need l, so clear it to save memory
        l.clear();

        // Run the partitioning algorithm on this merged LTS
        sim_partitioner sp(this);
        sp.partitioning_algorithm();

        return sp.in_preorder(init_state,init_l);
      }
    case lts_pre_trace:
      {
        // Preprocessing: reduce modulo strong bisimulation equivalence.
        // This is not strictly necessary, but may reduce time/memory
        // needed for determinisation.
        bisimulation_reduce(false);
        l.bisimulation_reduce(false);

        // Determinise both LTSes. As postprocessing, reduce modulo
        // strong bisimulation equivalence. This is not strictly
        // necessary, but may reduce time/memory needed for simulation
        // preorder checking.
        this->determinise();
        bisimulation_reduce(false);

        l.determinise();
        l.bisimulation_reduce(false);

        // Trace preorder now corresponds to simulation preorder
        return this->compare(l,lts_pre_sim);
      }
    case lts_pre_weak_trace:
      {
        // Eliminate silent steps of first LTS
        l.bisimulation_reduce(true,false);
        this->tau_star_reduce();

        // Eliminate silent steps of second LTS
        l.bisimulation_reduce(true,false);
        l.tau_star_reduce();

        // Weak trace preorder now corresponds to strong trace preorder
        return this->compare(l,lts_pre_trace);
      }
    default:
      std::cerr << "Comparison for this preorder is not available\n";
      return false;
  }
}

}
}
