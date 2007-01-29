#include <lts/liblts.h>

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
  unsigned int new_trans_lut[nstates+1];

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
          }
          u++;
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

  enum { unknown, reached, explored } reachable[nstates];
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
  unsigned int state_map[nstates];
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

  unsigned int label_map[nlabels];
  unsigned int new_nlabels = 0;
  for (unsigned int i=0; i < nlabels; i++)
  {
    if ( !taus[i] )
    {
      label_map[i] = new_nlabels;
      taus[new_nlabels] = false;
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
         !taus[label_map[transitions[i].label]] )
    {
      transition t = transitions[i];
      t.from = state_map[t.from];
      t.label = label_map[t.label];
      t.to = state_map[t.to];
      transitions[new_ntransitions] = t;
      new_ntransitions++;
    }
  }

  nstates = new_nstates;
  nlabels = new_nlabels;
  ntransitions = new_ntransitions;
}

}
}
