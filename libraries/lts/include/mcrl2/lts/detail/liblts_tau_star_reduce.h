// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_tau_star_reduce.h

#ifndef _LIBLTS_TAUSTARREDUCE_H
#define _LIBLTS_TAUSTARREDUCE_H

#include <cstdlib> // free
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

//Replace sequences tau* a tau* by a single action a.


enum t_reach { unknown, reached, explored };

template < class STATE_LABEL_T, class ACTION_LABEL_T >
void tau_star_reduce(lts<STATE_LABEL_T,ACTION_LABEL_T> &l)
// This method assumes there are no tau loops!
{
  using namespace std;
  l.sort_transitions();

  // Copy the transitions into a local set of transitions, to which we
  // have access.

  std::vector < transition > local_transitions=l.get_transitions();

  size_t* trans_lut = l.get_transition_indices();
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(new_trans_lut,size_t,l.num_states() + 1);


  new_trans_lut[0] = l.num_transitions();
  l.clear_transitions();

  // Calculate the tau* a transitive closure.
  for (size_t state = 0; state < l.num_states(); state++)
  {
    size_t t = trans_lut[state];
    while (t < trans_lut[state+1])
    {
      if (l.is_tau(local_transitions[t].label()) &&
          (local_transitions[t].from() != local_transitions[t].to()))
      {
        size_t to = local_transitions[t].to();
        size_t u = trans_lut[to];
        while (u < trans_lut[to+1])
        {
          if (!((to < state) && l.is_tau(local_transitions[u].label())))
          {
            local_transitions.push_back(transition(state,local_transitions[u].label(),local_transitions[u].to()));
          }
          u++;
        }
        if (to < state)
        {
          u = new_trans_lut[to];
          while (u < new_trans_lut[to+1])
          {
            if (!l.is_tau(local_transitions[u].label()))
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
    while (t < local_transitions.size())
    {
      if (l.is_tau(local_transitions[t].label()) &&
          (local_transitions[t].from() != local_transitions[t].to()))
      {
        size_t to = local_transitions[t].to();
        size_t u = trans_lut[to];
        while (u < trans_lut[to+1])
        {
          if (!((to < state) && l.is_tau(local_transitions[u].label())))
          {
            local_transitions.push_back(transition(state,local_transitions[u].label(),local_transitions[u].to()));
          }
          u++;
        }
        if (to < state)
        {
          u = new_trans_lut[to];
          while (u < new_trans_lut[to+1])
          {
            if (!l.is_tau(local_transitions[u].label()))
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

  free(trans_lut);

  // Add the newly generated transitions
  for(std::vector < transition >::const_iterator i=local_transitions.begin();
            i!=local_transitions.end(); ++i)
  {
    if (!l.is_tau(i->label()))
    {
      l.add_transition(*i);
    }
  }

  reachability_check(l, true); // Remove unreachable parts.
}

}
}
}
#endif // _LIBLTS_TAUSTARREDUCE_H
