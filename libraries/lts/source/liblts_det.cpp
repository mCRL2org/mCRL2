// Author(s): Bas Ploeger, Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_det.cpp

#include <algorithm>
#include <vector>
#include <boost/scoped_array.hpp>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/exception.h"
#include "mcrl2/lts/detail/tree_set.h"

using namespace std;
using namespace mcrl2::core;

namespace mcrl2
{
namespace lts
{

bool is_deterministic(const lts &l)   
{ 
  outgoing_transitions_per_state_action_t trans_lut=transitions_per_outgoing_state_action_pair(l.get_transitions());

  for(outgoing_transitions_per_state_action_t::const_iterator i=trans_lut.begin(); i!=trans_lut.end(); ++i)
  { 
    outgoing_transitions_per_state_action_t::const_iterator i_next=i;
    i_next++;
    if (i_next!=trans_lut.end() && from(i)==from(i_next) && label(i)==label(i_next))
    { // found a pair <s,l,t> and <s,l,t'>, so l is not deterministic.
      return false;
    }
  }
  return true;
}


    static bool compare_transition_label_to_from(const transition &t1, const transition &t2)
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

static void get_trans(std::multimap < transition::size_type, std::pair < transition::size_type, transition::size_type > > &begin,
                      tree_set_store *tss,
                      unsigned int d,
                      vector<transition> &d_trans) 
{
  if (!tss->is_set_empty(d)) 
  {
    if (tss->is_set_empty(tss->get_set_child_right(d))) 
    {
      for(std::multimap < transition::size_type, std::pair < transition::size_type, transition::size_type > > :: const_iterator
                j=begin.lower_bound(tss->get_set_child_left(d)); j!=begin.upper_bound(tss->get_set_child_left(d)); ++j)
      {
        d_trans.push_back(transition(j->first,j->second.first,j->second.second));
      }
    }
    else {
      get_trans(begin,tss,tss->get_set_child_left(d),d_trans);
      get_trans(begin,tss,tss->get_set_child_right(d),d_trans);
    }
  }
}

void determinise(lts &l) 
{
  tree_set_store *tss = new tree_set_store();

  vector<transition> d_transs;
  vector<unsigned int> d_states;

  // create the initial state of the DLTS
  d_states.push_back(l.initial_state());
  unsigned int d_id = tss->set_set_tag(tss->create_set(d_states));
  d_states.clear();

  std::multimap < transition::size_type, std::pair < transition::size_type, transition::size_type > >
             begin=transitions_per_outgoing_state(l.get_transitions());
  l.clear_transitions();
  unsigned int d_ntransitions = 0;
  std::vector < transition > d_transitions;

  int s;
  unsigned int i,to,lbl,n_t;

  while (d_id < tss->get_next_tag()) 
  {
    // collect the outgoing transitions of every state of DLTS state d_id in
    // the vector d_transs
    get_trans(begin,tss,tss->get_set(d_id),d_transs);   

    // sort d_transs by label and (if labels are equal) by destination
    sort(d_transs.begin(),d_transs.end(),compare_transition_label_to_from);

    n_t = d_transs.size();
    i = 0;
    for (lbl = 0; lbl < l.num_labels(); ++lbl) 
    {
      // compute the destination of the transition with label lbl
      while (i < n_t && d_transs[i].label() < lbl) 
      {
        ++i;
      }
      while (i < n_t && d_transs[i].label() == lbl) {
        to = d_transs[i].to();
        d_states.push_back(to);
        while (i < n_t && d_transs[i].label() == lbl &&
            d_transs[i].to() == to) {
          ++i;
        }
      }
      s = tss->create_set(d_states);

      // generate the transitions to each of the next states
      if (!tss->is_set_empty(s)) 
      {
        d_transitions.push_back(transition(d_id,lbl,tss->set_set_tag(s)));

        if (d_ntransitions%10000 == 0) {
          gsVerboseMsg(
              "generated %d states and %d transitions; explored %d states\n",
              tss->get_next_tag(),d_ntransitions,d_id);
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
  l.clear_transitions();

  for(std::vector < transition > :: const_iterator i=d_transitions.begin(); i!=d_transitions.end(); ++i )
  { 
    l.add_transition(*i);
  }
  assert(is_deterministic(l));
}
}
}
