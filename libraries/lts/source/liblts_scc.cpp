// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_scc.cpp

#include "mcrl2/core/messaging.h"
#include "mcrl2/lts/detail/liblts_scc.h"

using namespace mcrl2::core;
using namespace std;

namespace mcrl2
{
namespace lts
{
namespace detail
{
  scc_partitioner::scc_partitioner(mcrl2::lts::lts &l) 
             :aut(l)
  { if (core::gsDebug)
    {  std::cerr << "Tau loop (SCC) partitioner created for " << l.num_states() << " states and " << 
             l.num_transitions() << " transitions\n";
    }
    // read and store tau transitions.
    std::map < state_type, std::vector < state_type > > src_tgt;
    for(transition_iterator i=aut.get_transitions(); i.more(); ++i)
    { 
      if (aut.is_tau(i.label()))
      { src_tgt[i.from()].push_back(i.to());
      }
    }
    // Initialise the data structures
    std::vector<bool> visited(aut.num_states(),false);

    for(state_type i=0; i<aut.num_states(); ++i)
    { dfs_numbering(i,src_tgt,visited);
    }
    src_tgt.clear();

    std::map < state_type, std::vector < state_type > > tgt_src;
    for(transition_iterator i=aut.get_transitions(); i.more(); ++i)
    { if (aut.is_tau(i.label()))
      { tgt_src[i.to()].push_back(i.from());
      }
    }
    equivalence_class_index=0;
    block_index_of_a_state=std::vector < state_type >(aut.num_states(),0);
    for(std::vector < state_type >::reverse_iterator i=dfsn2state.rbegin();
                 i!=dfsn2state.rend(); ++i)
    { if (visited[*i])  // Visited is used inversely here.
      { group_components(*i,equivalence_class_index,tgt_src,visited);
        equivalence_class_index++;
      }
    }
    dfsn2state.clear();
  }
 
  scc_partitioner::~scc_partitioner()
  {
  }
  
  void scc_partitioner::replace_transitions(const bool preserve_divergence_loops)
  { // Put all the non inert transitions in a set. Add the transitions that form a self
    // loop. Such transitions only exist in case divergence preserving branching bisimulation is
    // used. A set is used to remove double occurrences of transitions. 
    std::set < transition > resulting_transitions;
    for(transition_iterator i=aut.get_transitions(); i.more(); ++i)
    { 
      if (!aut.is_tau(i.label()) || 
          preserve_divergence_loops ||
          block_index_of_a_state[i.from()]!=block_index_of_a_state[i.to()])
      { 
        resulting_transitions.insert(
                   transition(
                         block_index_of_a_state[i.from()],
                         i.label(),
                         block_index_of_a_state[i.to()]));
      }
    }

    aut.set_transitions(NULL,0,0);
    // Copy the transitions from the set into a malloced block of memory, as the lts library
    // currently requires it in that form.
    transition* new_transitions=(transition *)malloc(sizeof(transition)*resulting_transitions.size());
    unsigned int counter=0;
    for(std::set < transition >::const_iterator i=resulting_transitions.begin();
          i!=resulting_transitions.end(); ++i)
    { 
      new_transitions[counter++]=transition(i->from,i->label,i->to);
    }

    aut.set_transitions(new_transitions,resulting_transitions.size(),resulting_transitions.size());
  }

  unsigned int scc_partitioner::num_eq_classes() const
  { return equivalence_class_index;
  }

  unsigned int scc_partitioner::get_eq_class(const unsigned int s) const
  { 
    return block_index_of_a_state[s];
  }

  bool scc_partitioner::in_same_class(const unsigned int s, const unsigned int t) const
  { 
    return get_eq_class(s)==get_eq_class(t);
  }

// Private methods of scc_partitioner

  void scc_partitioner::group_components(
                          const state_type t, 
                          const state_type equivalence_class_index, 
                          const std::map < state_type, std::vector < state_type > > &tgt_src,
                          std::vector < bool > &visited) 
  { 
    if (!visited[t]) return;
    { 
      visited[t] = false;
      if (tgt_src.count(t)>0)
      { const std::vector < state_type > &sources = tgt_src.find(t)->second;
        for (std::vector < state_type >::const_iterator i=sources.begin();
                i!=sources.end(); ++i) 
        { 
          group_components(*i,equivalence_class_index,tgt_src,visited);
        }
      }
      block_index_of_a_state[t]=equivalence_class_index;
    }
  }

  void scc_partitioner::dfs_numbering(
                 const state_type t,
                 const std::map < state_type, std::vector < state_type > > &src_tgt,
                 std::vector < bool > &visited) 
  { 
    if (visited[t]) return;
    visited[t] = true;
    if (src_tgt.count(t)>0)
    { const std::vector < state_type > &targets = src_tgt.find(t)->second;
      for(std::vector < state_type >::const_iterator i=targets.begin();
               i!=targets.end() ; ++i)
      { dfs_numbering(*i,src_tgt,visited);
      }
    }
    dfsn2state.push_back(t); 
  }

} // namespace detail

  void scc_reduce(lts &l,const bool preserve_divergence_loops /* = false */)
  { 
    detail::scc_partitioner scc_part(l);
  
    // Clear this LTS, but keep the labels
    l.clear_type();
    l.clear_states();
    
    // Set the resulting number of states and transitions.
    l.set_num_states(scc_part.num_eq_classes());
    l.set_initial_state(scc_part.get_eq_class(l.initial_state()));

    scc_part.replace_transitions(preserve_divergence_loops);
  }
}
}

