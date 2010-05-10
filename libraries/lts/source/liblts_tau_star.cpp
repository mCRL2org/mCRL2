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
  unsigned int *trans_lut = l.get_transition_indices();
  boost::scoped_array< unsigned int > new_trans_lut(new unsigned int[l.num_states() + 1]);

  new_trans_lut[0] = l.num_transitions();
  for (unsigned int state = 0; state < l.num_states(); state++)
  {
    unsigned int t = trans_lut[state];
    while ( t < trans_lut[state+1] )
    {
      if ( l.taus[l.transitions[t].label] &&
          (l.transitions[t].from != l.transitions[t].to) )
      {
        unsigned int to = l.transitions[t].to;
        unsigned int u = trans_lut[to];
        while ( u < trans_lut[to+1] )
        {
          if ( !( (to < state) && l.taus[l.transitions[u].label] ) )
          {
            l.add_transition(state,l.transitions[u].label,l.transitions[u].to);
          }
          u++;
        }
        if ( to < state )
        {
          u = new_trans_lut[to];
          while ( u < new_trans_lut[to+1] )
          {
            if ( !l.taus[l.transitions[u].label] )
            {
              l.add_transition(state,l.transitions[u].label,l.transitions[u].to);
            }
            u++;
          }
        }
      }
      t++;
    }
    t = new_trans_lut[state];
    while ( t < l.num_transitions() )
    {
      if ( l.taus[l.transitions[t].label] &&
          (l.transitions[t].from != l.transitions[t].to) )
      {
        unsigned int to = l.transitions[t].to;
        unsigned int u = trans_lut[to];
        while ( u < trans_lut[to+1] )
        {
          if ( !( (to < state) && l.taus[l.transitions[u].label] ) )
          {
            l.add_transition(state,l.transitions[u].label,l.transitions[u].to);
          }
          u++;
        }
        if ( to < state )
        {
          u = new_trans_lut[to];
          while ( u < new_trans_lut[to+1] )
          {
            if ( !l.taus[l.transitions[u].label] )
            {
              l.add_transition(state,l.transitions[u].label,l.transitions[u].to);
            }
            u++;
          }
        }
      }
      t++;
    }
    new_trans_lut[state+1] = l.num_transitions();
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
          if ( reachable[l.transitions[t].to] == unknown )
          {
            reachable[l.transitions[t].to] = reached;
            if ( l.transitions[t].to < i )
            {
              notdone = true;
            }
          }
          t++;
        }
        t = new_trans_lut[i];
        while ( t < new_trans_lut[i+1] )
        {
          if ( reachable[l.transitions[t].to] == unknown )
          {
            reachable[l.transitions[t].to] = reached;
            if ( l.transitions[t].to < i )
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
      if ( l.state_info )
      {
        l.state_values[new_nstates] = l.state_values[i];
      }
      new_nstates++;
    }
  }

  boost::scoped_array< unsigned int > label_map(new unsigned int[l.num_labels()]);
  unsigned int new_nlabels = 0;
  for (unsigned int i=0; i < l.num_labels(); i++)
  {
    if ( !l.taus[i] )
    {
      label_map[i] = new_nlabels;
      if ( l.label_info )
      {
        l.label_values[new_nlabels] = l.label_values[i];
      }
      new_nlabels++;
    }
  }

  unsigned int new_ntransitions = 0;
  for (unsigned int i=0; i < l.num_transitions(); i++)
  {
    if ( (reachable[l.transitions[i].from] != unknown) &&
         !l.taus[l.transitions[i].label] )
    {
      transition t = l.transitions[i];
      t.from = state_map[t.from];
      t.label = label_map[t.label];
      t.to = state_map[t.to];
      l.transitions[new_ntransitions] = t;
      new_ntransitions++;
    }
  }

  for ( unsigned int i=0; i < l.num_labels(); i++)
  {
    l.taus[i] = false;
  }

  l.set_num_states(new_nstates);
  l.set_num_labels(new_nlabels);
  l.set_num_transitions(new_ntransitions);
}

}
/// \endcond
}
}
