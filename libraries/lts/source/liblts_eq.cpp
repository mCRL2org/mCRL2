// Author(s): Muck van Weerdenburg
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
  switch ( eq )
  {
    case lts_eq_none:
    case lts_eq_isomorph:
      return true;
    case lts_eq_bisim:
      return bisimulation_reduce(*this,false,opts.reduce.add_class_to_state,&opts.reduce.tau_actions);
    case lts_eq_branching_bisim:
      return bisimulation_reduce(*this,true,opts.reduce.add_class_to_state,&opts.reduce.tau_actions);
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
      bisimulation_reduce(*this,false);
      determinise();
      return bisimulation_reduce(*this,false);
    case lts_eq_weak_trace:
      bisimulation_reduce(*this,true,false,&opts.reduce.tau_actions);
      tau_star_reduce();
      bisimulation_reduce(*this,false);
      determinise();
      return bisimulation_reduce(*this,false);
    default:
      return false;
  }
}

bool lts::compare(lts &l, lts_equivalence eq, lts_eq_options const&opts)
{
  switch ( eq )
  {
    case lts_eq_none:
      return false;
    case lts_eq_bisim:
      return bisimulation_compare(*this,l,false,&opts.reduce.tau_actions);
    case lts_eq_branching_bisim:
      return bisimulation_compare(*this,l,true,&opts.reduce.tau_actions);
    case lts_eq_sim:
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

        return sp.in_same_class(init_state,init_l);
      }
    case lts_eq_trace:
      {
        // Determinise first LTS
        bisimulation_reduce(*this,false);
        this->determinise();

        // Determinise second LTS
        bisimulation_reduce(l,false);
        l.determinise();

        // Trace equivalence now corresponds to bisimilarity
        return bisimulation_compare(*this,l,false);
      }
    case lts_eq_weak_trace:
      {
        // Eliminate silent steps and determinise first LTS
        bisimulation_reduce(*this,true,false,&opts.reduce.tau_actions);
        this->tau_star_reduce();
        bisimulation_reduce(*this,false);
        this->determinise();

        // Eliminate silent steps and determinise second LTS
        bisimulation_reduce(l,true,false,&opts.reduce.tau_actions);
        l.tau_star_reduce();
        bisimulation_reduce(l,false);
        l.determinise();

        // Weak trace equivalence now corresponds to bisimilarity
        return bisimulation_compare(*this,l,false);
      }
    case lts_eq_isomorph:
    default:
      gsErrorMsg("comparison for this equivalence is not available\n");
      return false;
  }
}

bool lts::compare(lts &l, lts_preorder pre, lts_eq_options const&opts)
{
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
        bisimulation_reduce(*this,false);
        bisimulation_reduce(l,false);

        // Determinise both LTSes. As postprocessing, reduce modulo
        // strong bisimulation equivalence. This is not strictly
        // necessary, but may reduce time/memory needed for simulation
        // preorder checking.
        this->determinise();
        bisimulation_reduce(*this,false);

        l.determinise();
        bisimulation_reduce(l,false);

        // Trace preorder now corresponds to simulation preorder
        return this->compare(l,lts_pre_sim);
      }
    case lts_pre_weak_trace:
      {
        // Eliminate silent steps of first LTS
        bisimulation_reduce(*this,true,false,&opts.reduce.tau_actions);
        this->tau_star_reduce();

        // Eliminate silent steps of second LTS
        bisimulation_reduce(l,true,false,&opts.reduce.tau_actions);
        l.tau_star_reduce();

        // Weak trace preorder now corresponds to strong trace preorder
        return this->compare(l,lts_pre_trace);
      }
    default:
      gsErrorMsg("comparison for this preorder is not available\n");
      return false;
  }
}

}
}
