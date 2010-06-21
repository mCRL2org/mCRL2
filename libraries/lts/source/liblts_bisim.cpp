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
#include <iostream>
#include <sstream>
#include "mcrl2/core/messaging.h"
#include "mcrl2/lts/detail/liblts_bisim.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/lts_algorithm.h"

using namespace mcrl2::core;
using namespace std;


namespace mcrl2
{
namespace lts
{
namespace detail
{
// Private methods of bisim_partitioner

  static unsigned int determine_tau_label(const lts &l)
  {
    // Set the tau_label to an existing label, if possible.
    // Preference goes to a label which has name "tau".
    // So first find an arbitrary tau, and then let this tau
    // label be superseded by "tau". If nothing is found the tau
    // label becomes l.num_labels, but there will not be a tau
    // anyhow in this case.
    unsigned int tau_label=l.num_labels();
    for(unsigned int i=0; i<l.num_labels(); ++i)
    { if (l.is_tau(i))
      { tau_label=i;
        break;
      }
    }
    for(unsigned int i=0; i<l.num_labels(); ++i)
    { 
      if (l.has_label_info() && 
          l.is_tau(i) &&
          l.label_value_str(i)==std::string("tau"))
      { 
        tau_label=i;
        break;
      }
    }
    return tau_label;
  }

  bisim_partitioner::bisim_partitioner(
                   mcrl2::lts::lts &l,
                   const bool branching,
                   const bool preserve_divergence)
             :max_state_index(0), aut(l), tau_label(determine_tau_label(l))
  { 
    assert( branching || !preserve_divergence); 
    if (core::gsVerbose)
    { std::cerr << (preserve_divergence?"Divergence preserving b)":"B") <<
                   (branching?"ranching b":"") << "isimulation partitioner created for " 
                          << l.num_states() << " states and " << 
             l.num_transitions() << " transitions\n";
    }
    create_initial_partition(branching,preserve_divergence);
    refine_partition_until_it_becomes_stable(branching, preserve_divergence);
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

    for(transition_const_range t=aut.get_transitions(); !t.empty(); t.advance_begin(1))
    { const transition i=t.front();
      if (!branching ||
          !aut.is_tau(i.label()) || 
          get_eq_class(i.from())!=get_eq_class(i.to()) || 
          (preserve_divergences && i.from()==i.to()))
      { resulting_transitions.insert(
                   transition(
                         get_eq_class(i.from()),
                         i.label(),
                         get_eq_class(i.to())));
      }
    }
    // Remove the old transitions
    aut.clear_transitions();

    // Copy the transitions from the set into a malloced block of memory, as the lts library
    // currently requires it in that form.
    for(std::set < transition >::const_iterator i=resulting_transitions.begin();
          i!=resulting_transitions.end(); ++i)
    { aut.add_transition(transition(i->from(),i->label(),i->to()));
    }

  }

  unsigned int bisim_partitioner::num_eq_classes() const
  { return max_state_index;
  }

  unsigned int bisim_partitioner::get_eq_class(const unsigned int s) const
  { assert(s<block_index_of_a_state.size());
    return blocks[block_index_of_a_state[s]].state_index;
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
  void bisim_partitioner::create_initial_partition(const bool branching, 
                                                   const bool preserve_divergences)
  { 
    blocks.reserve(aut.num_states()); // Reserving blocks is done to avoid
                                      // messing around with blocks in memory,
                                      // which can be very time consuming.
    block_is_active.reserve(aut.num_states());
    to_be_processed.clear();
    
    block initial_partition;

    // First store the bottom and non bottom states.
    aut.sort_transitions(mcrl2::lts::src_lbl_tgt);

    state_type last_non_stored_state_number=0;
    bool bottom_state=true;
    std::vector < state_type > current_inert_transitions;
    for(transition_const_range r=aut.get_transitions(); !r.empty(); r.advance_begin(1))
    { const transition t=r.front();

      if (branching && aut.is_tau(t.label()))
      { if (preserve_divergences && t.from()==t.to()) 
        { 
          initial_partition.non_inert_transitions.push_back(transition(t.from(),tau_label,t.to()));
        } 
        else
        { current_inert_transitions.push_back(t.to());
          bottom_state=false;
        }
      }
      transition_const_range next_i=r;
      next_i.advance_begin(1);
      if (next_i.empty() || t.from()!=next_i.front().from())
      { // store the current from state
        for( ; last_non_stored_state_number<t.from(); ++last_non_stored_state_number)
        { initial_partition.bottom_states.push_back(last_non_stored_state_number);
        } 

        if (bottom_state)
        { initial_partition.bottom_states.push_back(t.from());
        }
        else
        { initial_partition.non_bottom_states.push_back(non_bottom_state(t.from()));
          current_inert_transitions.swap(initial_partition.non_bottom_states.back().inert_transitions);
          bottom_state=true;
        }
        assert(last_non_stored_state_number==t.from());
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
    for(transition_const_range r=aut.get_transitions(); !r.empty(); r.advance_begin(1))
    { 
      const transition t=r.front();
      if (!branching || !aut.is_tau(t.label()))
      { // Note that by sorting the transitions first, the non_inert_transitions are grouped per label.
          initial_partition.non_inert_transitions.push_back(transition(t.from(),t.label(),t.to()));
      }
    }
    
    block_is_active.push_back(true);
    initial_partition.block_index=0;
    initial_partition.state_index=0;
    max_state_index=1;
    initial_partition.parent_block_index=0;
    initial_partition.splitter=pair< label_type, block_index_type > (0,0);
    blocks.push_back(block());
    blocks.back().swap(initial_partition);
    block_index_of_a_state=std::vector < block_index_type >(aut.num_states(),0);
    block_flags.push_back(false);
    state_flags=std::vector < bool >(aut.num_states(),false);
    block_is_in_to_be_processed.push_back(false);
    to_be_processed.clear();
    BL.clear();
  } // end create_initial_partition


  void bisim_partitioner::split_the_blocks_in_BL(bool &partition_is_unstable,
                                                 const label_type splitter_label,
                                                 const block_index_type splitter_block)
  { 
    for(std::vector < block_index_type > :: const_iterator i1=BL.begin();
              i1!=BL.end(); ++i1)
    { 
      assert(block_is_active[*i1]);
      block_flags[*i1]=false;
      std::vector < state_type > flagged_states;
      std::vector < state_type > non_flagged_states;
      std::vector < state_type > i1_bottom_states;
      i1_bottom_states.swap(blocks[*i1].bottom_states);
      
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
      assert(!flagged_states.empty()||!blocks[*i1].non_bottom_states.empty()||i1_bottom_states.empty());
      block_index_type reset_state_flags_block=*i1;

      if (!non_flagged_states.empty())
      { // There are flagged and non flagged states. So, the block must be split. 
        // Move the unflagged states to the new block.
          
        if (core::gsVerbose)
        { const unsigned int m=pow(10,floor(log10( static_cast<double>( (blocks.size()+1)/2))));
          if ((blocks.size()+1)/2 % m==0) 
          { std::cerr << "Bisimulation partitioner: create block " << (blocks.size()+1)/2 << "\n";
          }
        }
        // Record how block *i1 is split, to use this to generate counter examples.
        blocks[*i1].splitter=pair< label_type, block_index_type > (splitter_label,splitter_block); 

        // Create a first new block.
        blocks.push_back(block());
        block_index_type new_block1=blocks.size()-1;
        block_is_active.push_back(true);
        blocks.back().state_index=max_state_index;
        max_state_index++;
        blocks.back().block_index=new_block1;
        blocks.back().parent_block_index=*i1;
        non_flagged_states.swap(blocks.back().bottom_states);
        // Put the indices of first split block to to_be_processed.
        to_be_processed.push_back(blocks.back().block_index);
        block_is_in_to_be_processed.push_back(true);
        
        // Create a second new block.
        blocks.push_back(block());
        block_index_type new_block2=blocks.size()-1;
        block_is_active.push_back(true);
        blocks.back().state_index=blocks[*i1].state_index;
        blocks.back().block_index=new_block2;
        reset_state_flags_block=new_block2;
        blocks.back().parent_block_index=*i1;
       
        // Move the flagged states to the second block, and let the block index of these states refer to this block.
        flagged_states.swap(blocks.back().bottom_states);
        std::vector < state_type > &reference_to_flagged_states_of_block2=blocks.back().bottom_states;
        for(std::vector < state_type >::const_iterator j=reference_to_flagged_states_of_block2.begin();
          j!=reference_to_flagged_states_of_block2.end(); ++j)
        { 
          block_index_of_a_state[*j]=new_block2;
        }

        // Put the indices of second split block to to_be_processed.
        to_be_processed.push_back(blocks.back().block_index);
        block_is_in_to_be_processed.push_back(true);
        
        // reset the flag of block *i1, which is being split.
        block_is_in_to_be_processed[*i1]=false;
        block_is_active[*i1]=false;

        // The flag fields of the new blocks is set to false;
        block_flags.push_back(false);
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
        std::vector < non_bottom_state > i1_non_bottom_states;
        i1_non_bottom_states.swap(blocks[*i1].non_bottom_states);
        for(std::vector < non_bottom_state >::iterator k=i1_non_bottom_states.begin();
                 k!=i1_non_bottom_states.end(); ++k)
        { const std::vector < state_type > &inert_transitions=k->inert_transitions;
          if (!state_flags[k->state])
          { bool all_transitions_end_in_unflagged_block=true;
            for(std::vector < state_type > :: const_iterator l=inert_transitions.begin();
                     all_transitions_end_in_unflagged_block && l!=inert_transitions.end(); ++l)
            { if (block_index_of_a_state[*l]!= new_block1)
              { block_index_of_a_state[*l]=new_block2;
                all_transitions_end_in_unflagged_block=false;
              }
            }
            if (all_transitions_end_in_unflagged_block)
            { // Move *k to the non flagged block. Swap the inert transitions to avoid copying.
              non_bottom_state s(k->state);
              s.inert_transitions.swap(k->inert_transitions);
              non_flagged_non_bottom_states.push_back(s);
              block_index_of_a_state[k->state]=new_block1;
              continue;
            }
          }
          // Move *k to the flagged block; note that the transitions can have become
          // non-inert. So, investigate them separately.
          std::vector < state_type > remaining_inert_transitions;
          for(std::vector < state_type > :: const_iterator l=inert_transitions.begin();
                     l!=inert_transitions.end(); ++l)
          { if (block_index_of_a_state[*l]==new_block1)
            { // The transition *l (*k,tau_label,*l) becomes a non inert transition in the new
              // block.
              non_flagged_non_inert_transitions.push_back(transition(k->state,tau_label,*l));
            }
            else
            { // The transition represented by *l remains an inert transition.
              block_index_of_a_state[*l]=new_block2;
              remaining_inert_transitions.push_back(*l);
            }
          }
          if (remaining_inert_transitions.empty()) // The last outgoing inert tau transition just became non inert. 
                                                   // k->state has become a bottom state. Otherwise it remains
                                                   // a non bottom state.
          { blocks[new_block2].bottom_states.push_back(k->state);
            block_index_of_a_state[k->state]=new_block2;
            partition_is_unstable=true;
          }
          else
          { flagged_non_bottom_states.push_back(non_bottom_state(k->state,remaining_inert_transitions));
            block_index_of_a_state[k->state]=new_block2;
          }
        }
        non_flagged_non_bottom_states.swap(blocks[new_block1].non_bottom_states);
        flagged_non_bottom_states.swap(blocks[new_block2].non_bottom_states);

        // Finally the non-inert transitions are distributed over both blocks in the obvious way.
        // Note that this must be done after all states are properly put into a new block.

        assert(*i1 < blocks.size());
        std::vector < transition > i1_non_inert_transitions;
        i1_non_inert_transitions.swap(blocks[*i1].non_inert_transitions);
        for(std::vector < transition >::iterator k=i1_non_inert_transitions.begin();
                 k!=i1_non_inert_transitions.end(); ++k )
        { if (block_index_of_a_state[k->to()]==new_block1)
          { non_flagged_non_inert_transitions.push_back(*k);
          }
          else
          { block_index_of_a_state[k->to()]=new_block2;
            flagged_non_inert_transitions.push_back(*k);
          }
        }
        
        non_flagged_non_inert_transitions.swap(blocks[new_block1].non_inert_transitions);
        flagged_non_inert_transitions.swap(blocks[new_block2].non_inert_transitions);
      }
      else
      { // Nothing changed, so put the bottom states back again.
        i1_bottom_states.swap(blocks[*i1].bottom_states);
      }
      // reset the state flags
      std::vector < state_type > &flagged_states_to_be_unflagged=blocks[reset_state_flags_block].bottom_states;
      for(std::vector < state_type >::const_iterator j=flagged_states_to_be_unflagged.begin();
                 j!=flagged_states_to_be_unflagged.end(); ++j)
      { state_flags[*j]=false;
      }
    
      std::vector < non_bottom_state > &flagged_states_to_be_unflagged1=blocks[reset_state_flags_block].non_bottom_states;
      for(std::vector < non_bottom_state >::const_iterator j=flagged_states_to_be_unflagged1.begin();
                 j!=flagged_states_to_be_unflagged1.end(); ++j)
      { state_flags[j->state]=false;
      }
    }
    BL.clear();
  }


// Refine the partition until the partition has become stable
  void  bisim_partitioner::refine_partition_until_it_becomes_stable(const bool branching, const bool preserve_divergence)
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
      // Avoid checking too often. This is too time consuming, even in debug mode.
      consistency_check_counter++;
      if (consistency_check_counter>=consistency_check_barrier)
      { consistency_check_counter=0;
        consistency_check_barrier=consistency_check_barrier*2;
        check_internal_consistency_of_the_partitioning_data_structure(branching, preserve_divergence);
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
      assert(block_is_in_to_be_processed[splitter_index]||!block_is_active[splitter_index]);
      to_be_processed.pop_back();
      block_is_in_to_be_processed[splitter_index]=false;
     
      // Split with the splitter block, unless it is to_be_processed as we have to reconsider it 
      // completely anyhow at some later point.
      const std::vector <transition> &splitter_non_inert_transitions=blocks[splitter_index].non_inert_transitions;
      for(std::vector <transition>::const_iterator i=splitter_non_inert_transitions.begin(); 
                  block_is_active[splitter_index] &&
                  i!=splitter_non_inert_transitions.end(); ++i) 
      { // The flag of the starting state of *i is raised and its block is added to BL;

        assert(i->from()<aut.num_states());
        state_flags[i->from()]=true;
        const block_index_type marked_block_index=block_index_of_a_state[i->from()];
        if (block_flags[marked_block_index]==false)
        { block_flags[marked_block_index]=true;
          BL.push_back(marked_block_index);
        }
        
        // If the label of the next action is different, we must carry out the splitting.
        std::vector <transition>::const_iterator i_next=i;
        i_next++;
        if (i_next==splitter_non_inert_transitions.end() || 
                        i->label()!=i_next->label())
        { // We consider BL which contains references to all blocks from which a state from splitter
          // can be reached. If not all flags of the non bottom states in a block in BL are set, the
          // non flagged non bottom states are moved to a new block.

          split_the_blocks_in_BL(partition_is_unstable,i->label(),splitter_index);
    
        }
      }
    }
#ifndef NDEBUG
    check_internal_consistency_of_the_partitioning_data_structure(branching, preserve_divergence);
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
  
  void bisim_partitioner::check_internal_consistency_of_the_partitioning_data_structure(
                            const bool branching,
                            const bool preserve_divergence) const
  { 
    state_type total_number_of_states=0;
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
        assert(j->to()<aut.num_states());
        assert(j->from()<aut.num_states());
        
        // Check proper grouping of action labels.
        std::vector < transition >::const_iterator j_next=j;
        j_next++;
        if (j_next==i_non_inert_transitions.end() || (j->label()!=j_next->label()))
        { assert(observed_action_labels.count(j->label())==0);
          observed_action_labels.insert(j->label());
        }

        // Check whether tau transition in non inert transition vector is inert.
        if (!preserve_divergence && branching && aut.is_tau(j->label()))
        { 
          assert(j->to()!=j->from());
        }

        // Check whether the target state of the transition is in the current block.
        assert(block_index_of_a_state[j->to()]==i->block_index);
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
    { 
      assert(!block_is_in_to_be_processed[i] || temporary_block_is_in_to_be_processed[i]);
    }
    
    // Check that BL is empty.
    assert(BL.empty());

    // Check that tau_label is smaller or equal to the number of labels.
    // If no tau label is used, it is equal to the number of labels, which
    // is a number of labels that is not used.
    assert(tau_label<=aut.num_labels());
  }

#endif // not NDEBUG

  std::vector < mcrl2::trace::Trace > 
            bisim_partitioner::counter_traces_aux(
                           const unsigned int s,
                           const unsigned int t,
                           const mcrl2::lts::outgoing_transitions_per_state_action_t &outgoing_transitions) const
  {
    // First find the smallest block containing both states s and t.
    // Find all blocks containing s.
    std::set < bisim_partitioner::block_index_type > blocks_containing_s;
    bisim_partitioner::block_index_type b_s=block_index_of_a_state[s];
    blocks_containing_s.insert(b_s);
    while (blocks[b_s].parent_block_index!=b_s)
    { b_s=blocks[b_s].parent_block_index;
      blocks_containing_s.insert(b_s);
    }

    // Find the first smallest block containing t and s
    bisim_partitioner::block_index_type b_C=block_index_of_a_state[t];
    while (blocks_containing_s.count(b_C)==0)
    { 
      assert(blocks[b_C].parent_block_index!=b_C);
      b_C=blocks[b_C].parent_block_index;
    }
    
    // Now b_C is the smallest block containing both s and t.
    const label_type l=blocks[b_C].splitter.first;
    const bisim_partitioner::block_index_type B__=blocks[b_C].splitter.second;
  
    std::set < state_type> B_s_reacha;
    std::set < state_type> B_s_nonreacha;
    for(outgoing_transitions_per_state_action_t::const_iterator i=outgoing_transitions.lower_bound(pair<state_type,label_type>(s,l));
                 i!=outgoing_transitions.upper_bound(pair<state_type,label_type>(s,l)); ++i)
    { const state_type s_=to(i);
      block_index_type b=block_index_of_a_state[s_]; 
      bool reached=b==B__;
      do 
      { 
        b=blocks[b].parent_block_index;
        if (b==B__)
        { 
          reached=true;
        }
      } while (!reached && b!=blocks[b].parent_block_index);
      
      if (reached)
      { 
        B_s_reacha.insert(s_);
      }
      else
      { 
        B_s_nonreacha.insert(s_);
      }
    }
    
    std::set < state_type> B_t_reacha;
    std::set < state_type> B_t_nonreacha;
    for(outgoing_transitions_per_state_action_t::const_iterator i=outgoing_transitions.lower_bound(pair<state_type,label_type>(t,l));
                 i!=outgoing_transitions.upper_bound(pair<state_type,label_type>(t,l)); ++i)
    { const state_type t_=to(i);
      block_index_type b=block_index_of_a_state[t_]; 
      bool reached=b==B__;
      do 
      { 
        b=blocks[b].parent_block_index;
        if (b==B__)
        { 
          reached=true;
        }
      } while (!reached && b!=blocks[b].parent_block_index);
      
      if (reached)
      { 
        B_t_reacha.insert(t_);
      }
      else
      { 
        B_t_nonreacha.insert(t_);
      }
    }

    assert((B_s_reacha.empty() && !B_t_reacha.empty()) || 
           (!B_s_reacha.empty() && B_t_reacha.empty()));

    std::vector < mcrl2::trace::Trace > resulting_counter_traces;

    if (B_s_reacha.empty())
    { B_s_reacha.swap(B_t_reacha);
      B_s_nonreacha.swap(B_t_nonreacha);
    }

    assert(!B_s_reacha.empty()); 


    if (B_t_nonreacha.empty())
    { // The counter trace is simply the label l.
      mcrl2::trace::Trace counter_trace;
      counter_trace.addAction((ATermAppl)aut.label_value(l));
      resulting_counter_traces.push_back(counter_trace);
    }
    else
    { 
      for(std::set < state_type>::const_iterator i_s=B_s_reacha.begin();
             i_s!=B_s_reacha.end(); ++i_s)
      { 
        for(std::set < state_type>::const_iterator i_t=B_t_nonreacha.begin();
             i_t!=B_t_nonreacha.end(); ++i_t)
        { 
          const std::vector < mcrl2::trace::Trace > counter_traces=counter_traces_aux(*i_s,*i_t,outgoing_transitions);
          // Add l to these traces and add them to resulting_counter_traces
          for(std::vector < mcrl2::trace::Trace >::const_iterator j=counter_traces.begin();
                    j!=counter_traces.end(); ++j)
          { 
            mcrl2::trace::Trace new_counter_trace;
            new_counter_trace.addAction((ATermAppl)aut.label_value(l));
            mcrl2::trace::Trace old_counter_trace=*j; 
            old_counter_trace.resetPosition();
            for(unsigned int k=0 ; k< old_counter_trace.getLength(); k++)
            { 
              new_counter_trace.addAction(old_counter_trace.nextAction());
            }
            resulting_counter_traces.push_back(new_counter_trace);
          }
        }
      }
    
    }
    return resulting_counter_traces; 
  }
    
  std::vector < mcrl2::trace::Trace > bisim_partitioner::counter_traces(const unsigned int s, const unsigned int t)
  { 
    if (get_eq_class(s)==get_eq_class(t))
    { throw mcrl2::runtime_error("Requesting a counter trace for two bisimilar states. Such a trace is not useful.");
    }
    
    const outgoing_transitions_per_state_action_t outgoing_transitions=transitions_per_outgoing_state_action_pair(aut.get_transitions());
    return counter_traces_aux(s,t,outgoing_transitions);
  }


  void bisimulation_reduce(lts &l,
                           const bool branching /*=false */,
                           const bool preserve_divergences /*=false */)
  { // First remove tau loops in case of branching bisimulation.
    if (branching)
    { 
      detail::scc_partitioner scc_part(l);
      scc_part.replace_transitions(preserve_divergences);
      l.set_num_states(scc_part.num_eq_classes());
      l.set_initial_state(scc_part.get_eq_class(l.initial_state()));
    }

    // Second apply the branching bisimulation reduction algorithm. If there are no tau's,
    // this will automatically yield strong bisimulation.
    detail::bisim_partitioner bisim_part(l, branching, preserve_divergences);
  
    // Clear LTS l, but keep the labels
    l.clear_type();
    l.clear_states();
    
    // Assign the reduced LTS
    l.set_num_states(bisim_part.num_eq_classes());
    l.set_initial_state(bisim_part.get_eq_class(l.initial_state()));
    bisim_part.replace_transitions(branching,preserve_divergences);
  }
  
  bool bisimulation_compare(
              const lts &l1,
              const lts &l2, 
              const bool branching /* =false*/, 
              const bool preserve_divergences /*=false*/,
              const bool generate_counter_examples /*= false*/ )
  { lts l1_copy(l1);
    lts l2_copy(l2);
    return destructive_bisimulation_compare(l1_copy,l2_copy,branching,preserve_divergences,
                                            generate_counter_examples);
  }

  bool destructive_bisimulation_compare(
              lts &l1, 
              lts &l2,
              const bool branching /* =false*/, 
              const bool preserve_divergences /*=false*/,
              const bool generate_counter_examples /* = false */)
  { unsigned int init_l2 = l2.initial_state() + l1.num_states();
    merge(l1,l2);
    l2.clear(); // No use for l2 anymore.

    // First remove tau loops in case of branching bisimulation.
    if (branching)
    { detail::scc_partitioner scc_part(l1);
      scc_part.replace_transitions(preserve_divergences);
      l1.set_num_states(scc_part.num_eq_classes());
      l1.set_initial_state(scc_part.get_eq_class(l1.initial_state()));
      init_l2 = scc_part.get_eq_class(init_l2);
    }
    
    detail::bisim_partitioner bisim_part(l1, branching, preserve_divergences);
    if (generate_counter_examples && !bisim_part.in_same_class(l1.initial_state(),init_l2))
    { std::vector < mcrl2::trace::Trace > counter_example_traces=bisim_part.counter_traces(l1.initial_state(),init_l2);
      unsigned int count=0;
      for(std::vector < mcrl2::trace::Trace >::iterator i=counter_example_traces.begin();
                 i!=counter_example_traces.end(); ++i,++count)
      { std::stringstream filename_s;
        filename_s << "Counterexample" << count << ".trc";
        const std::string filename(filename_s.str());
        i->save(filename,mcrl2::trace::tfPlain);
      }
    }
    return bisim_part.in_same_class(l1.initial_state(),init_l2);
  }

} // namespace detail
}
}
