// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_tau_star.cpp

#include "mcrl2/core/messaging.h"
#include <mcrl2/lts/lts.h>

#include "workarounds.h" // DECL_A

using namespace std;

namespace mcrl2
{
namespace lts
{

void p_lts::tau_star_reduce()
  // This method assumes there are not tau loops!
{
  p_sort_transitions();
  unsigned int *trans_lut = p_get_transition_indices();
  DECL_A(new_trans_lut,unsigned int,nstates+1);

  new_trans_lut[0] = ntransitions;
  for (unsigned int state = 0; state < nstates; state++)
  {
    unsigned int t = trans_lut[state];
    while ( t < trans_lut[state+1] )
    {
      if ( taus[transitions[t].label] &&
          (transitions[t].from != transitions[t].to) )
      {
        unsigned int to = transitions[t].to;
        unsigned int u = trans_lut[to];
        while ( u < trans_lut[to+1] )
        {
          if ( !( (to < state) && taus[transitions[u].label] ) )
          {
            p_add_transition(state,transitions[u].label,transitions[u].to);
          }
          u++;
        }
        if ( to < state )
        {
          u = new_trans_lut[to];
          while ( u < new_trans_lut[to+1] )
          {
            if ( !taus[transitions[u].label] )
            {
              p_add_transition(state,transitions[u].label,transitions[u].to);
            }
            u++;
          }
        }
      }
      t++;
    }
    t = new_trans_lut[state];
    while ( t < ntransitions )
    {
      if ( taus[transitions[t].label] &&
          (transitions[t].from != transitions[t].to) )
      {
        unsigned int to = transitions[t].to;
        unsigned int u = trans_lut[to];
        while ( u < trans_lut[to+1] )
        {
          if ( !( (to < state) && taus[transitions[u].label] ) )
          {
            p_add_transition(state,transitions[u].label,transitions[u].to);
          }
          u++;
        }
        if ( to < state )
        {
          u = new_trans_lut[to];
          while ( u < new_trans_lut[to+1] )
          {
            if ( !taus[transitions[u].label] )
            {
              p_add_transition(state,transitions[u].label,transitions[u].to);
            }
            u++;
          }
        }
      }
      t++;
    }
    new_trans_lut[state+1] = ntransitions;
  }
  FREE_A(new_trans_lut);

  typedef enum { unknown, reached, explored } t_reach;
  DECL_A(reachable,t_reach,nstates);
  for (unsigned int i=0; i<nstates; i++)
  {
    reachable[i] = unknown;
  }
  reachable[init_state] = reached;
  bool notdone = true;
  while ( notdone )
  {
    notdone = false;
    for (unsigned int i=0; i<nstates; i++)
    {
      if ( reachable[i] == reached )
      {
        unsigned int t = trans_lut[i];
        while ( t < trans_lut[i+1] )
        {
          if ( reachable[transitions[t].to] == unknown )
          {
            reachable[transitions[t].to] = reached;
            if ( transitions[t].to < i )
            {
              notdone = true;
            }
          }
          t++;
        }
        t = new_trans_lut[i];
        while ( t < new_trans_lut[i+1] )
        {
          if ( reachable[transitions[t].to] == unknown )
          {
            reachable[transitions[t].to] = reached;
            if ( transitions[t].to < i )
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
  DECL_A(state_map,unsigned int,nstates);
  unsigned int new_nstates = 0;
  for (unsigned int i=0; i < nstates; i++)
  {
    if ( reachable[i] != unknown )
    {
      state_map[i] = new_nstates;
      if ( state_info )
      {
        state_values[new_nstates] = state_values[i];
      }
      new_nstates++;
    }
  }

  DECL_A(label_map,unsigned int,nlabels);
  unsigned int new_nlabels = 0;
  for (unsigned int i=0; i < nlabels; i++)
  {
    if ( !taus[i] )
    {
      label_map[i] = new_nlabels;
      if ( label_info )
      {
        label_values[new_nlabels] = label_values[i];
      }
      new_nlabels++;
    }
  }

  unsigned int new_ntransitions = 0;
  for (unsigned int i=0; i < ntransitions; i++)
  {
    if ( (reachable[transitions[i].from] != unknown) &&
         !taus[transitions[i].label] )
    {
      transition t = transitions[i];
      t.from = state_map[t.from];
      t.label = label_map[t.label];
      t.to = state_map[t.to];
      transitions[new_ntransitions] = t;
      new_ntransitions++;
    }
  }

  for ( unsigned int i=0; i < nlabels; i++)
  {
    taus[i] = false;
  }

  FREE_A(label_map);
  FREE_A(state_map);
  FREE_A(reachable);

  nstates = new_nstates;
  nlabels = new_nlabels;
  ntransitions = new_ntransitions;
}

}
}
