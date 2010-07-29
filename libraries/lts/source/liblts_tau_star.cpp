// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_tau_star.cpp

#include <cstdlib> // free
#include "boost/scoped_array.hpp"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/core/messaging.h"

using namespace std;

namespace mcrl2
{
namespace lts
{
/// \cond INTERNAL_DOCS
namespace detail
{
  enum t_reach { unknown, reached, explored };

void tau_star_reduce(lts &l)
  // This method assumes there are not tau loops!
{
  l.sort_transitions();

  // Copy the transitions into a local set of transitions, to which we 
  // have access. 

  const transition_const_range r=l.get_transitions();
  std::vector < transition > local_transitions(r.begin(),r.end());
  // for(transition_const_range r=get_transitions(); !r.empty(); r.advance_begin(1))
  // { local_transitions.push_back(r.front());
  // }


  unsigned int *trans_lut = l.get_transition_indices();
  boost::scoped_array< unsigned int > new_trans_lut(new unsigned int[l.num_states() + 1]);

  new_trans_lut[0] = l.num_transitions();
  for (unsigned int state = 0; state < l.num_states(); state++)
  {
    unsigned int t = trans_lut[state];
    while ( t < trans_lut[state+1] )
    {
      if ( l.is_tau(local_transitions[t].label()) &&
          (local_transitions[t].from() != local_transitions[t].to()) )
      {
        unsigned int to = local_transitions[t].to();
        unsigned int u = trans_lut[to];
        while ( u < trans_lut[to+1] )
        {
          if ( !( (to < state) && l.is_tau(local_transitions[u].label()) ) )
          {
            local_transitions.push_back(transition(state,local_transitions[u].label(),local_transitions[u].to()));
          }
          u++;
        }
        if ( to < state )
        {
          u = new_trans_lut[to];
          while ( u < new_trans_lut[to+1] )
          {
            if ( !l.is_tau(local_transitions[u].label()) )
            {
              local_transitions.push_back(transition(state,local_transitions[u].label(),local_transitions[u].to()));
            }
            u++;
          }
        }
      }
      t++;
    }
    t = new_trans_lut[state];
    while ( t < local_transitions.size() )
    {
      if ( l.is_tau(local_transitions[t].label()) &&
          (local_transitions[t].from() != local_transitions[t].to()) )
      {
        unsigned int to = local_transitions[t].to();
        unsigned int u = trans_lut[to];
        while ( u < trans_lut[to+1] )
        {
          if ( !( (to < state) && l.is_tau(local_transitions[u].label()) ) )
          {
            local_transitions.push_back(transition(state,local_transitions[u].label(),local_transitions[u].to()));
          }
          u++;
        }
        if ( to < state )
        {
          u = new_trans_lut[to];
          while ( u < new_trans_lut[to+1] )
          {
            if ( !l.is_tau(local_transitions[u].label()) )
            {
              local_transitions.push_back(transition(state,local_transitions[u].label(),local_transitions[u].to()));
            }
            u++;
          }
        }
      }
      t++;
    }
    new_trans_lut[state+1] = local_transitions.size();
  }

  using namespace mcrl2::lts::detail;

  boost::scoped_array< t_reach > reachable(new t_reach[l.num_states()]);
  for (unsigned int i=0; i<l.num_states(); i++)
  {
    reachable[i] = unknown;
  }
  reachable[l.initial_state()] = reached;
  bool notdone = true;
  while ( notdone )
  {
    notdone = false;
    for (unsigned int i=0; i<l.num_states(); i++)
    {
      if ( reachable[i] == reached )
      {
        unsigned int t = trans_lut[i];
        while ( t < trans_lut[i+1] )
        {
          if ( reachable[local_transitions[t].to()] == unknown )
          {
            reachable[local_transitions[t].to()] = reached;
            if ( local_transitions[t].to() < i )
            {
              notdone = true;
            }
          }
          t++;
        }
        t = new_trans_lut[i];
        while ( t < new_trans_lut[i+1] )
        {
          if ( reachable[local_transitions[t].to()] == unknown )
          {
            reachable[local_transitions[t].to()] = reached;
            if ( local_transitions[t].to() < i )
            {
              notdone = true;
            }
          }
          t++;
        }
        reachable[i] = explored;
      }
    }
  }

  free(trans_lut);

  boost::scoped_array< unsigned int > state_map(new unsigned int[l.num_states()]);
  unsigned int new_nstates = 0;
  for (unsigned int i=0; i < l.num_states(); i++)
  {
    if ( reachable[i] != unknown )
    {
      state_map[i] = new_nstates;
      if ( l.has_state_info() )
      {
        l.set_state_value(new_nstates,l.state_value(i));
      }
      new_nstates++;
    }
  }

  boost::scoped_array< unsigned int > label_map(new unsigned int[l.num_labels()]);
  unsigned int new_nlabels = 0;
  for (unsigned int i=0; i < l.num_labels(); i++)
  {
    if ( !l.is_tau(i) )
    {
      label_map[i] = new_nlabels;
      if ( l.has_label_info() )
      {
        l.set_label_value(new_nlabels,l.label_value(i));
      }
      new_nlabels++;
    }
  }

  std::set < transition > new_transitions;
  for (std::vector < transition >::const_iterator i=local_transitions.begin(); i!=local_transitions.end(); ++i)
  { const transition t=*i;
    if ( (reachable[t.from()] != unknown) &&
         !l.is_tau(t.label()) )
    {
      new_transitions.insert(transition(state_map[t.from()],label_map[t.label()],state_map[t.to()]));
    }
  }

  l.clear_transitions();
  for(std::set < transition >::const_iterator i=new_transitions.begin();
       i!=new_transitions.end(); ++i)
  { l.add_transition(*i);
  }

  for ( unsigned int i=0; i < l.num_labels(); i++)
  {
    l.set_tau(i,false);
  }

  l.set_num_states(new_nstates);
  l.set_num_labels(new_nlabels);
}

}
/// \endcond
}
}
