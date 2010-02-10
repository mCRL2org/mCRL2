// Author(s): Muck van Weerdenburg, Bert Lisser
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file liblts_bisim.cpp

#include <math.h>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lts/detail/liblts_bisim.h"
#include "mcrl2/lts/detail/liblts_scc.h"

using namespace mcrl2::core;
using namespace std;


namespace mcrl2
{
namespace lts
{
namespace detail
{
// Private methods of bisim_partitioner

  bisim_partitioner::bisim_partitioner(
                   mcrl2::lts::lts &l,
                   const bool branching,
                   const bool preserve_divergence)
             :aut(l), tau_label(l.num_labels())
  { assert( branching || !preserve_divergence); 
    if (core::gsVerbose)
    { std::cerr << (preserve_divergence?"Divergence preserving b)":"B") <<
                   (branching?"ranching b":"") << "isimulation partitioner created for " << l.num_states() << " states and " << 
             l.num_transitions() << " transitions\n";
    }
    create_initial_partition(branching,preserve_divergence);
    refine_partition_until_it_becomes_stable(preserve_divergence);
  }

  bisim_partitioner::~bisim_partitioner()
  {
  }
  
  void bisim_partitioner::replace_transitions(const bool branching, const bool preserve_divergences)
  { 
    // Put all the non inert transitions in a set. Add the transitions that form a self
    // loop. Such transitions only exist in case divergence preserving branching bisimulation is
    // used. A set is used to remove double occurrences of transitions. 
    std::set < transition > resulting_transitions;

    for(transition_iterator i=aut.get_transitions(); i.more(); ++i)
    { if (!branching ||
          !aut.is_tau(i.label()) || 
          block_index_of_a_state[i.from()]!=block_index_of_a_state[i.to()] || 
          (preserve_divergences && i.from()==i.to()))
      { resulting_transitions.insert(
                   transition(
                         block_index_of_a_state[i.from()],
                         i.label(),
                         block_index_of_a_state[i.to()]));
      }
    }
    // Remove the old transitions
    aut.set_transitions(NULL,0,0);

    // Copy the transitions from the set into a malloced block of memory, as the lts library
    // currently requires it in that form.
    transition* new_transitions=(transition *)malloc(sizeof(transition)*resulting_transitions.size());
    unsigned int counter=0;
    for(std::set < transition >::const_iterator i=resulting_transitions.begin();
          i!=resulting_transitions.end(); ++i)
    { new_transitions[counter]=transition(i->from,i->label,i->to);
      counter++;
    }

    // Set the resulting number of states and transitions.
    aut.set_transitions(new_transitions,resulting_transitions.size(),resulting_transitions.size());
  }

  unsigned int bisim_partitioner::num_eq_classes() const
  { return blocks.size();
  }

  unsigned int bisim_partitioner::get_eq_class(const unsigned int s) const
  { assert(s<block_index_of_a_state.size());
    return block_index_of_a_state[s];
  }

  bool bisim_partitioner::in_same_class(const unsigned int s, const unsigned int t) const
  { return get_eq_class(s)==get_eq_class(t);
  }

// Private methods of bisim_partitioner

  // Below there are two routines to order states on tau reachability.
            
  void bisim_partitioner::order_recursively_on_tau_reachability(
                                 const state_type s,
                                 std::map < state_type, std::vector < state_type > > &inert_transition_map,
                                 std::vector < non_bottom_state > &new_non_bottom_states,
                                 std::set < state_type > &visited)
  { if (inert_transition_map.count(s)>0) // The state s is a bottom state. We need not to investigate these.
    { if (visited.count(s)==0)            
      { visited.insert(s);
        std::vector < state_type> &inert_transitions=inert_transition_map[s];
        for(std::vector < state_type>::const_iterator j=inert_transitions.begin();
             j!=inert_transitions.end(); j++)
        { order_recursively_on_tau_reachability(*j,inert_transition_map,new_non_bottom_states,visited);
        }
        new_non_bottom_states.push_back(non_bottom_state(s));
        inert_transitions.swap(new_non_bottom_states.back().inert_transitions); 
      }
    }
  }

  void bisim_partitioner::order_on_tau_reachability(std::vector < non_bottom_state > &non_bottom_states)
  { std::set < state_type > visited;
    std::map < state_type, std::vector < state_type > > inert_transition_map;
    for(std::vector < non_bottom_state >::iterator i=non_bottom_states.begin();
             i!=non_bottom_states.end(); ++i)
    { i->inert_transitions.swap(inert_transition_map[i->state]);
    }
    std::vector < non_bottom_state > new_non_bottom_states;

    for(std::vector < non_bottom_state >::const_iterator i=non_bottom_states.begin();
             i!=non_bottom_states.end(); ++i)
    { order_recursively_on_tau_reachability(i->state, inert_transition_map, new_non_bottom_states, visited);
    }
    new_non_bottom_states.swap(non_bottom_states);
  }

  // Put the labelled transition system into the basic data structure for the
  // bisimulation algorithm where all states are put in one block.
  void bisim_partitioner::create_initial_partition(const bool branching, const bool preserve_divergences)
  { 
    blocks.reserve(aut.num_states()); // Reserving blocks is done to avoid
                                      // messing around with blocks in memory.
                                      // This can be very time consuming, esp. for lists.
    to_be_processed.clear();
    
    block initial_partition;

    // First store the bottom and non bottom states.
    aut.sort_transitions(mcrl2::lts::src_lbl_tgt);
    transition_iterator i=aut.get_transitions();

    state_type last_non_stored_state_number=0;
    bool bottom_state=true;
    std::vector < state_type > current_inert_transitions;
    for( ; i.more(); ++i)
    { if (branching && aut.is_tau(i.label()))
      { if (preserve_divergences && i.from()==i.to()) 
        { 
          initial_partition.non_inert_transitions.push_back(transition(i.from(),tau_label,i.to()));
        } 
        else
        { current_inert_transitions.push_back(i.to());
          bottom_state=false;
        }
      }
      transition_iterator next_i=i;
      ++next_i;
      if (!next_i.more() || i.from()!=next_i.from())
      { // store the current from state
        for( ; last_non_stored_state_number<i.from(); ++last_non_stored_state_number)
        { initial_partition.bottom_states.push_back(last_non_stored_state_number);
        } 

        if (bottom_state)
        { initial_partition.bottom_states.push_back(i.from());
        }
        else
        { initial_partition.non_bottom_states.push_back(non_bottom_state(i.from()));
          current_inert_transitions.swap(initial_partition.non_bottom_states.back().inert_transitions);
          bottom_state=true;
        }
        assert(last_non_stored_state_number==i.from());
        last_non_stored_state_number++;
      }
    }

    for( ; last_non_stored_state_number<aut.num_states(); ++last_non_stored_state_number)
    { initial_partition.bottom_states.push_back(last_non_stored_state_number);
    }

    // Sort the non-bottom states such that the deepest states occur first.
    // Raise an exception if there is a non trivial tau loop.
    
    order_on_tau_reachability(initial_partition.non_bottom_states);

    // Store the non-inert transitions (i.e. the non tau transitions)
    aut.sort_transitions(mcrl2::lts::lbl_tgt_src);
    for(transition_iterator i=aut.get_transitions(); i.more(); ++i)
    { if (!branching || !aut.is_tau(i.label()))
      { // Note that by sorting the transitions first, the non_inert_transitions are grouped per label.
          initial_partition.non_inert_transitions.push_back(transition(i.from(),i.label(),i.to()));
      }
    }
    
    initial_partition.block_index=0;
    blocks.push_back(block());
    blocks.back().swap(initial_partition);
    block_index_of_a_state=std::vector < block_index_type >(aut.num_states(),0);
    block_flags.push_back(false);
    state_flags=std::vector < bool >(aut.num_states(),false);
    block_is_in_to_be_processed.push_back(false);
    to_be_processed.clear();
    BL.clear();
  } // end create_initial_partition


  void bisim_partitioner::split_the_blocks_in_BL(bool &partition_is_unstable)
  { 
    
    for(std::vector < block_index_type > :: const_iterator i1=BL.begin();
              i1!=BL.end(); ++i1)
    { block_flags[*i1]=false;
      std::vector < state_type > flagged_states;
      std::vector < state_type > non_flagged_states;
      const std::vector < state_type > &i1_bottom_states=blocks[*i1].bottom_states;
      assert(i1_bottom_states.size()>0);
      for(std::vector < state_type >::const_iterator j=i1_bottom_states.begin();
          j!=i1_bottom_states.end(); ++j)
      { if (state_flags[*j])
        { // state is flagged. 
          flagged_states.push_back(*j);
        }
        else
        { // state is not flagged. It will be moved to a new block.
          non_flagged_states.push_back(*j);
          block_index_of_a_state[*j]=blocks.size();
        }
      }
      assert(!flagged_states.empty()||!blocks[*i1].non_bottom_states.empty());
      if (!non_flagged_states.empty())
      { // There are flagged and non flagged states. So, the block must be split. 
        // Move the unflagged states to the new block.
          
        if (core::gsVerbose)
        { const unsigned int m=pow(10,floor(log10( static_cast<double>( blocks.size()))));
          if (blocks.size() % m==0)
          { std::cerr << "Bisimulation partitioner: create block " << blocks.size() << "\n";
          }
        }
        blocks.push_back(block());
        blocks.back().block_index=blocks.size()-1;
        non_flagged_states.swap(blocks.back().bottom_states);
        
        // Move the flagged states to the old block.
        flagged_states.swap(blocks[*i1].bottom_states);
        
        // Put the indices of both split blocks to to_be_processed.
        to_be_processed.push_back(blocks.back().block_index);
        block_is_in_to_be_processed.push_back(true);
        
        if (!block_is_in_to_be_processed[*i1])
        { to_be_processed.push_back(*i1);
          block_is_in_to_be_processed[*i1]=true;
        }

        // The flag fields of the new block is set to false;
        block_flags.push_back(false);

        // Declare already some space for transitions, such that we can 
        // put inert transitions that become non inert in there when investigating
        // the non bottom states. After investigating the non bottom states, 
        // the transitions are split over the vectors below.
        
        std::vector < transition > flagged_non_inert_transitions;
        std::vector < transition > non_flagged_non_inert_transitions;
        // Next we scan the non-bottom states of *i1. If for some non-bottom state the flag is not raised
        // and if none of the outgoing P-inert transitions leads to a state in the old block then this 
        // state becomes a non bottom state of B2. 

        std::vector < non_bottom_state > flagged_non_bottom_states;
        std::vector < non_bottom_state > non_flagged_non_bottom_states;
        std::vector < non_bottom_state > &i1_non_bottom_states=blocks[*i1].non_bottom_states;
        for(std::vector < non_bottom_state >::iterator k=i1_non_bottom_states.begin();
                 k!=i1_non_bottom_states.end(); ++k)
        { const std::vector < state_type > &inert_transitions=k->inert_transitions;
          if (!state_flags[k->state])
          { bool all_transitions_end_in_unflagged_block=true;
            for(std::vector < state_type > :: const_iterator l=inert_transitions.begin();
                     all_transitions_end_in_unflagged_block && l!=inert_transitions.end(); ++l)
            { if (block_index_of_a_state[*l]!= blocks.size()-1)
              { assert(block_index_of_a_state[*l]==*i1);
                all_transitions_end_in_unflagged_block=false;
              }
            }
            if (all_transitions_end_in_unflagged_block)
            { // Move *k to the non flagged block. Swap the inert transitions to avoid copying.
              non_bottom_state s(k->state);
              s.inert_transitions.swap(k->inert_transitions);
              non_flagged_non_bottom_states.push_back(s);
              block_index_of_a_state[k->state]=blocks.size()-1;
              continue;
            }
          }
          // Move *k to the flagged block; note that the transitions can have become
          // non-inert. So, investigate them separately.
          std::vector < state_type > remaining_inert_transitions;
          for(std::vector < state_type > :: const_iterator l=inert_transitions.begin();
                     l!=inert_transitions.end(); ++l)
          { if (block_index_of_a_state[*l]==blocks.size()-1)
            { // The transition *l (*k,tau_label,*l) becomes a non inert transition in the new
              // block.
              non_flagged_non_inert_transitions.push_back(transition(k->state,tau_label,*l));
            }
            else
            { // The transition represented by *l remains an inert transition.
              assert(block_index_of_a_state[*l]==*i1);
              remaining_inert_transitions.push_back(*l);
            }
          }
          if (remaining_inert_transitions.empty()) // The last outgoing inert tau transition just became non inert. 
                                                   // k->state has become a bottom state. Otherwise it remains
                                                   // a non bottom state.
          { blocks[*i1].bottom_states.push_back(k->state);
            partition_is_unstable=true;
          }
          else
          { flagged_non_bottom_states.push_back(non_bottom_state(k->state,remaining_inert_transitions));
          }
        }
        non_flagged_non_bottom_states.swap(blocks.back().non_bottom_states);
        flagged_non_bottom_states.swap(blocks[*i1].non_bottom_states);

        // Finally the non-inert transitions are distributed over both blocks in the obvious way.
        // Note that this must be done after all states are properly put into a new block.

        assert(*i1 < blocks.size());
        std::vector < transition > &i1_non_inert_transitions=blocks[*i1].non_inert_transitions;
        for(std::vector < transition >::iterator k=i1_non_inert_transitions.begin();
                 k!=i1_non_inert_transitions.end(); ++k )
        { if (block_index_of_a_state[k->to]==blocks.size()-1)
          { non_flagged_non_inert_transitions.push_back(*k);
          }
          else
          { assert(block_index_of_a_state[k->to]==*i1);
            flagged_non_inert_transitions.push_back(*k);
          }
        }
        
        non_flagged_non_inert_transitions.swap(blocks.back().non_inert_transitions);
        flagged_non_inert_transitions.swap(i1_non_inert_transitions);
        flagged_non_inert_transitions.clear();
      }
      // reset the state flags
      std::vector < state_type > &flagged_states_to_be_unflagged=blocks[*i1].bottom_states;
      for(std::vector < state_type >::const_iterator j=flagged_states_to_be_unflagged.begin();
                 j!=flagged_states_to_be_unflagged.end(); ++j)
      { state_flags[*j]=false;
      }
    
      std::vector < non_bottom_state > &flagged_states_to_be_unflagged1=blocks[*i1].non_bottom_states;
      for(std::vector < non_bottom_state >::const_iterator j=flagged_states_to_be_unflagged1.begin();
                 j!=flagged_states_to_be_unflagged1.end(); ++j)
      { state_flags[j->state]=false;
      }
    }
    BL.clear();
  }


// Refine the partition until the partition has become stable
  void  bisim_partitioner::refine_partition_until_it_becomes_stable(const bool preserve_divergence)
  { 
#ifndef NDEBUG
    unsigned int consistency_check_counter=1;
    unsigned int consistency_check_barrier=1;
#endif
    bool partition_is_unstable=true; // This boolean indicates that the partition becomes unstable
                                      // because an inert transition becomes non inert.
    while (!to_be_processed.empty() || partition_is_unstable)
    { 
#ifndef NDEBUG
      // Avoid checking too often. This too time consuming, even in debug mode.
      consistency_check_counter++;
      if (consistency_check_counter>=consistency_check_barrier)
      { consistency_check_counter=0;
        consistency_check_barrier=consistency_check_barrier*2;
        check_internal_consistency_of_the_partitioning_data_structure(preserve_divergence);
      }
#endif
      if (to_be_processed.empty() && partition_is_unstable)
      { // Put all blocks in to_be_processed;
        for(block_index_type i=0; i< blocks.size(); ++i)
        { to_be_processed.push_back(i);
        }
        block_is_in_to_be_processed=std::vector < bool >(blocks.size(),true);
        partition_is_unstable=false;
      }

      const block_index_type splitter_index=to_be_processed.back();
      to_be_processed.pop_back();
      block_is_in_to_be_processed[splitter_index]=false;
     
      // Split with the splitter block, unless it is to_be_processed as we have to reconsider it 
      // completely anyhow at some later point.
      std::vector <transition> &splitter_non_inert_transitions=blocks[splitter_index].non_inert_transitions;
      for(std::vector <transition>::const_iterator i=splitter_non_inert_transitions.begin(); 
                     !block_is_in_to_be_processed[splitter_index] && i!=splitter_non_inert_transitions.end(); ++i) 
      { // The flag of the starting state of *i is raised and its block is added to BL;

        const std::vector < transition > &i_non_inert_transitions=blocks[splitter_index].non_inert_transitions;
        
        state_flags[i->from]=true;
        const block_index_type marked_block_index=block_index_of_a_state[i->from];
        if (block_flags[marked_block_index]==false)
        { block_flags[marked_block_index]=true;
          BL.push_back(marked_block_index);
        }
        
        // If the label of the next action is different, we must carry out the splitting.
        std::vector <transition>::const_iterator i_next=i;
        i_next++;
        if (i_next==i_non_inert_transitions.end() || 
                        i->label!=i_next->label)
        { // We consider BL which contains references to all blocks from which a state from splitter
          // can be reached. If not all flags of the non bottom states in a block in BL are set, the
          // non flagged non bottom states are moved to a new block.

          split_the_blocks_in_BL(partition_is_unstable);
    
        }
      }
    }
#ifndef NDEBUG
    check_internal_consistency_of_the_partitioning_data_structure(preserve_divergence);
#endif
    block_flags.clear();
    block_is_in_to_be_processed.clear();
    state_flags.clear();
    to_be_processed.clear();
    BL.clear();
  }

#ifndef NDEBUG  

  // The method below is intended to check the consistency of the internal data
  // structure. Its sole purpose is to detect programming errors. It has no
  // side effects on the data structure. If a problem occurs, execution halts with
  // an assert.
  
  void bisim_partitioner::check_internal_consistency_of_the_partitioning_data_structure(const bool preserve_divergence) const
  { state_type total_number_of_states=0;
    unsigned int total_number_of_transitions=0;

    assert(!blocks.empty());
    std::set < block_index_type > block_indices;
    
    assert(block_index_of_a_state.size()==aut.num_states());
    for(std::vector < block >::const_iterator i=blocks.begin();
         i!=blocks.end(); ++i)
    { // Check the block_index.
      assert(i->block_index<blocks.size());
      assert(block_indices.count(i->block_index)==0);
      block_indices.insert(i->block_index);

      // Check the bottom states.
      const std::vector < state_type > &i_bottom_states=i->bottom_states;

      for(std::vector < state_type >::const_iterator j=i_bottom_states.begin();
              j!=i_bottom_states.end(); ++j)
      { total_number_of_states++;
        assert(*j<aut.num_states());
        // Check that the block number of the state is maintained properly.
        assert(block_index_of_a_state[*j]==i->block_index);
      }

      // Check the non bottom states. In particular check that there is no tau loop
      // in these non bottom states.
      const std::vector < non_bottom_state > &i_non_bottom_states=i->non_bottom_states;
      std::set < state_type > visited;
      std::set < state_type > local_bottom_states;
      for(std::vector < non_bottom_state >::const_iterator j=i_non_bottom_states.begin();
              j!=i_non_bottom_states.end(); ++j)
      { local_bottom_states.insert(j->state);
      }

      for(std::vector < non_bottom_state >::const_iterator j=i_non_bottom_states.begin();
              j!=i_non_bottom_states.end(); ++j)
      { total_number_of_states++;
        assert(j->state<aut.num_states());
        // Check that the block number of the state is maintained properly.
        assert(block_index_of_a_state[j->state]==i->block_index);
        const std::vector < state_type > &j_inert_transitions=j->inert_transitions;
        for(std::vector < state_type >::const_iterator k=j_inert_transitions.begin();
              k!=j_inert_transitions.end(); k++)
        { total_number_of_transitions++;
          assert(*k<aut.num_states());
          // Check that the inert transitions are well ordered.
          assert(visited.count(*k)>0 || local_bottom_states.count(*k)==0); 
        }
        visited.insert(j->state);
      }

      // Check the non_inert_transitions. It is required that the transitions
      // are grouped per label, and that tau transitions must be inert.

      const std::vector < transition > &i_non_inert_transitions=i->non_inert_transitions;
      std::set < label_type > observed_action_labels;
      for(std::vector < transition >::const_iterator j=i_non_inert_transitions.begin();
              j!=i_non_inert_transitions.end(); ++j)
      { total_number_of_transitions++;
        assert(j->to<aut.num_states());
        assert(j->from<aut.num_states());
        
        // Check proper grouping of action labels.
        std::vector < transition >::const_iterator j_next=j;
        j_next++;
        if (j_next==i_non_inert_transitions.end() || (j->label!=j_next->label))
        { assert(observed_action_labels.count(j->label)==0);
          observed_action_labels.insert(j->label);
        }

        // Check whether tau transition in non inert transition vector is inert.
        if (!preserve_divergence && j->label==tau_label)
        { assert(j->to!=j->from);
        }

        // Check whether the target state of the transition is in the current block.
        assert(block_index_of_a_state[j->to]==i->block_index);
      }
    }

    // Check total number of states and transitions.
    assert(total_number_of_states==aut.num_states());
    assert(total_number_of_transitions==aut.num_transitions());

    // Check block_index_of_a_state
    assert(block_index_of_a_state.size()==aut.num_states());
    for(std::vector < state_type >::const_iterator i=block_index_of_a_state.begin();
              i!=block_index_of_a_state.end(); ++i)
    { assert(blocks[*i].block_index== *i);
    }

    // Check block_flags that the block flags are all set to false
    for(std::vector < bool >::const_iterator i=block_flags.begin(); 
                 i!=block_flags.end(); ++i)
    { assert(!*i);
    }
    
    // Check that state_flags are all false.
    for(std::vector < bool >::const_iterator i=state_flags.begin(); 
                 i!=state_flags.end(); ++i)
    { assert(!*i);
    }
    
    // Check to_be_processed
    // Check block_is_in_to_be_processed
    std::vector < bool > temporary_block_is_in_to_be_processed(blocks.size(),false);
    
    for(std::vector< block_index_type > ::const_iterator i=to_be_processed.begin();
                i!=to_be_processed.end(); ++i)
    { temporary_block_is_in_to_be_processed[*i]=true;
    }
    for(state_type i=0; i<blocks.size(); ++i)
    { assert(temporary_block_is_in_to_be_processed[i]==block_is_in_to_be_processed[i]);
    }
    
    // Check that BL is empty.
    assert(BL.empty());

    // Check that tau_label is smaller or equal to the number of labels.
    // If no tau label is used, it is equal to the number of labels, which
    // is a number of labels that is not used.
    assert(tau_label==aut.num_labels());
  }

#endif // not NDEBUG
} // namespace detail

  void lts::bisimulation_reduce(const bool branching /*=false */,
                                const bool preserve_divergences /*=false */)
  { // First remove tau loops in case of branching bisimulation.
    if (branching)
    { detail::scc_partitioner scc_part(*this);
      scc_part.replace_transitions(preserve_divergences);
      nstates=scc_part.num_eq_classes();
      init_state = scc_part.get_eq_class(init_state);
    }

    // Second apply the branching bisimulation reduction algorithm. If there are no tau's,
    // this will automatically yield strong bisimulation.
    
    detail::bisim_partitioner bisim_part(*this, branching, preserve_divergences);
  
    // Clear this LTS, but keep the labels
    clear_type();
    clear_states();
    
    // Assign the reduced LTS
    nstates = bisim_part.num_eq_classes();
    init_state = bisim_part.get_eq_class(init_state);
    bisim_part.replace_transitions(branching,preserve_divergences);
  }
  
  bool lts::bisimulation_compare(
              const lts &l, 
              const bool branching /* =false*/, 
              const bool preserve_divergences /*=false*/) const
  { lts this_copy(*this);
    lts l_copy(l);
    return this_copy.destructive_bisimulation_compare(l_copy,branching,preserve_divergences);
  }

  bool lts::destructive_bisimulation_compare(
              lts &l, 
              const bool branching /* =false*/, 
              const bool preserve_divergences /*=false*/)
  { unsigned int init_l = l.initial_state() + nstates;
    merge(&l);
    l.clear(); // No use for l anymore.

    // First remove tau loops in case of branching bisimulation.
    if (branching)
    { detail::scc_partitioner scc_part(*this);
      scc_part.replace_transitions(preserve_divergences);
      nstates=scc_part.num_eq_classes();
      init_state = scc_part.get_eq_class(init_state);
      init_l = scc_part.get_eq_class(init_l);
    }
    
    detail::bisim_partitioner bisim_part(*this, branching, preserve_divergences);
    return bisim_part.in_same_class(init_state,init_l);
  }
}
}
