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
#include "mcrl2/lts/liblts.h"
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
        type = lts_none;
        state_info = false;

        states_size = 0;
        nstates = 0;
        if (state_values != NULL)
        {
          ATunprotectArray(state_values);
          free(state_values);
          state_values = NULL;
        }          
        
        transitions_size = 0;
        ntransitions = 0;
        free(transitions);

        // Assign the reduced LTS
        transitions = sp.get_transitions(ntransitions,transitions_size);
        nstates = sp.num_eq_classes();
        init_state = sp.get_eq_class(init_state);

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
    case lts_eq_trace:
      {
        // Copy LTSs
        lts l1(*this);
        lts l2(l);

        // Determinise first LTS
        bisimulation_reduce(l1,false);
        l1.determinise();
        
        // Determinise second LTS
        bisimulation_reduce(l2,false);
        l2.determinise();

        // Trace equivalence now corresponds to bisimilarity
        return bisimulation_compare(l1,l2,false);
      }
    case lts_eq_weak_trace:
      {
        // Copy LTSs
        lts l1(*this);
        lts l2(l);

        // Eliminate silent steps and determinise first LTS
        bisimulation_reduce(l1,true,false,&opts.reduce.tau_actions);
        l1.tau_star_reduce();
        bisimulation_reduce(l1,false);
        l1.determinise();
        
        // Eliminate silent steps and determinise second LTS
        bisimulation_reduce(l2,true,false,&opts.reduce.tau_actions);
        l2.tau_star_reduce();
        bisimulation_reduce(l2,false);
        l2.determinise();

        // Weak trace equivalence now corresponds to bisimilarity
        return bisimulation_compare(l1,l2,false);
      }
    case lts_eq_isomorph:
    default:
      gsErrorMsg("comparision for this equivalence is not yet implemented\n");
      return false;
  }
}
 
}
}
