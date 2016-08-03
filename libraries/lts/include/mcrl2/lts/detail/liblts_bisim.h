// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts/detail/liblts_bisim.h

#ifndef _LIBLTS_BISIM_H
#define _LIBLTS_BISIM_H
#include <cmath>
#include <vector>
#include <map>
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/trace/trace.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_dot.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{

template < class LTS_TYPE>
class bisim_partitioner
{

  public:
    /** \brief Creates a bisimulation partitioner for an LTS.
     *  \details This bisimulation partitioner applies the algorithm
     *  defined in J.F. Groote and F.W. Vaandrager. An efficient algorithm for branching bisimulation and stuttering
     *  equivalence. In M.S. Paterson, editor, Proceedings 17th ICALP, Warwick, volume 443 of Lecture Notes in Computer
     *  Science, pages 626-638. Springer-Verlag, 1990. The only difference is that this algorithm uses actions labels
     *  on transitions. Therefore, each list of non_inert_transitions is grouped such that all transitions with the
     *  same label are grouped together. Tau transitions (which have as label index the number of labels) occur at the
     *  beginning of this list.
     *
     *  If branching is true, then branching bisimulation is used, otherwise strong bisimulation is applied.
     *  If preserve_divergence is true, then branching must be true. In this case states with an internal tau loop
     *  are considered to be different from states without a tau loop. In this way the divergences are preserved.
     *
     *  The input transition system is not allowed to contain tau loops, except that if preserve_divergence is true
     *  tau loops on a single state are allowed as they indicate divergences. Using the scc partitioner the tau
     *  loops must first be removed before applying this algorithm.
     *  \warning Note that when compiled with optimisations, bisimulation partitioning
     *  is much faster than compiled without any optimisation. The difference can go up to a factor 10.
     *  \param[in] l Reference to the LTS. The LTS l is only changed if \ref replace_transitions is called. */
    bisim_partitioner(
      LTS_TYPE& l,
      const bool branching=false,
      const bool preserve_divergence=false)
       : max_state_index(0), 
         aut(l), 
         tau_label(determine_tau_label(l))  // tau_label is size_t(-1) if label does not exist
    {
      assert(branching || !preserve_divergence);
      mCRL2log(log::verbose) << (preserve_divergence?"Divergence preserving b":"B") <<
                  (branching?"ranching b":"") << "isimulation partitioner created for "
                  << l.num_states() << " states and " <<
                  l.num_transitions() << " transitions\n";
      create_initial_partition(branching,preserve_divergence);
      refine_partition_until_it_becomes_stable(branching, preserve_divergence);
    }


    /** \brief Destroys this partitioner. */
    ~bisim_partitioner()
    {}

    /** \brief Replaces the transition relation of the current lts by the transitions
     *         of the bisimulation reduced transition system.
     * \details Each transition (s,l,s') is replaced by a transition (t,l,t') where
     * t and t' are the equivalence classes to which classes of the LTS. If the label l is
     * internal, which is detected using the function is_tau, then it is only returned
     * if t!=t' or preserve_divergence=true. This effectively removes all inert transitions.
     * Duplicates are removed from the transitions in the new lts.
     * Note that the number of states nor the initial state are not adapted by this method.
     * These must be set separately.
     *
     * \pre The bisimulation equivalence classes have been computed.
     * \param[in] branching Causes non internal transitions to be removed.
     * \param[in] preserve_divergences Preserves tau loops on states. */
    void replace_transitions(const bool branching, const bool preserve_divergences)
    {
      // Put all the non inert transitions in a set. Add the transitions that form a self
      // loop. Such transitions only exist in case divergence preserving branching bisimulation is
      // used. A set is used to remove double occurrences of transitions.
      std::set < transition > resulting_transitions;

      const std::vector<transition>& trans=aut.get_transitions();
      for (std::vector<transition>::const_iterator t=trans.begin(); t!=trans.end(); ++t)
      {
        const transition i=*t;
        if (!branching ||
            !aut.is_tau(i.label(aut.hidden_label_map())) ||
            get_eq_class(i.from())!=get_eq_class(i.to()) ||
            (preserve_divergences && i.from()==i.to()))
        {
          resulting_transitions.insert(
            transition(
              get_eq_class(i.from()),
              i.label(transition::default_label_map()),
              get_eq_class(i.to())));
        }
      }
      // Remove the old transitions
      aut.clear_transitions();

      // Copy the transitions from the set into the transition system.
      for (std::set < transition >::const_iterator i=resulting_transitions.begin();
           i!=resulting_transitions.end(); ++i)
      {
        aut.add_transition(*i);
      }
    }

    /** \brief Gives the number of bisimulation equivalence classes of the LTS.
     *  \return The number of bisimulation equivalence classes of the LTS.
     */
    size_t num_eq_classes() const
    {
      return max_state_index;
    }


    /** \brief Gives the bisimulation equivalence class number of a state.
     *  \param[in] s A state number.
     *  \return The number of the bisimulation equivalence class to which \e s belongs. */
    size_t get_eq_class(const size_t s) const
    {
      assert(s<block_index_of_a_state.size());
      return blocks[block_index_of_a_state[s]].state_index;
    }


    /** \brief Returns whether two states are in the same bisimulation equivalence class.
     *  \param[in] s A state number.
     *  \param[in] t A state number.
     *  \retval true if \e s and \e t are in the same bisimulation equivalence class;
     *  \retval false otherwise. */
    bool in_same_class(const size_t s, const size_t t) const
    {
      return get_eq_class(s)==get_eq_class(t);
    }


    /** \brief Returns a vector of counter traces.
     *  \details The states s and t are non bisimilar states. If they are
     *           bisimilar an exception is raised. A counter trace of the form sigma a is
     *           returned, which has the property that s-sigma->s'-a-> and t-sigma->t'-/a->,
     *           or vice versa, s-sigma->s'-/a-> and t-sigma->t'-a->. A vector of such
     *           counter traces is returned.
     *  \param[in] s A state number.
     *  \param[in] t A state number.
     *  \param[in] branching_bisimulation A boolean indicating whether the branching bisimulation partitioner has been used.
     *  \return A vector containing counter traces. */
    std::set < mcrl2::trace::Trace > counter_traces(const size_t s, const size_t t, const bool branching_bisimulation);

  private:

    typedef size_t block_index_type;
    typedef size_t state_type;
    typedef size_t label_type;

    state_type max_state_index;
    LTS_TYPE& aut;

    struct non_bottom_state
    {
      state_type state;
      std::vector < state_type > inert_transitions; // Only the target state is interesting.

      non_bottom_state(const state_type s)
        : state(s)
      {}
      non_bottom_state(const state_type s, const std::vector < state_type > &it)
        : state(s), inert_transitions(it)
      {}
    };

    struct block
    {
      state_type state_index;                   // The state number that represent the states in this block
      block_index_type block_index;             // The sequence number of this block.
      block_index_type parent_block_index;      // Index of the parent block.
      // If there is no parent block, this refers to the block
      // itself.
      std::pair < label_type, block_index_type > splitter;
      // The action and block that caused this block to split.
      // This information is only defined if the block has been split.
      std::vector < state_type > bottom_states; // The non bottom states must be ordered
      // on tau reachability. The deepest
      // states occur first in the vector.
      std::vector < non_bottom_state > non_bottom_states;
      // The non_inert transitions are grouped per label. The (non-inert) transitions
      // with tau labels are at the end of this vector.
      std::vector < transition > non_inert_transitions;

      void swap(block& b)
      {
        state_type state_index1=b.state_index;
        b.state_index=state_index;
        state_index=state_index1;

        block_index_type block_index1=b.block_index;
        b.block_index=block_index;
        block_index=block_index1;

        block_index_type parent_block_index1=b.parent_block_index;
        b.parent_block_index=parent_block_index;
        parent_block_index=parent_block_index1;

        std::pair < label_type, block_index_type > splitter1=b.splitter;
        b.splitter=splitter;
        splitter=splitter1;
        bottom_states.swap(b.bottom_states);
        non_bottom_states.swap(b.non_bottom_states);
        non_inert_transitions.swap(b.non_inert_transitions);
      }
    };

    std::vector < block > blocks;

    // std::vector < bool > block_is_active;       // Indicates whether this is still a block in the partition.
    // Blocks that are split become inactive.
    std::vector < state_type > block_index_of_a_state;
    std::vector < bool > block_flags;
    std::vector < bool > block_is_in_to_be_processed;
    std::vector < bool > state_flags;

    std::vector< block_index_type > to_be_processed;
    std::vector< block_index_type > BL;
    const label_type tau_label;

    void create_initial_partition(const bool branching,
                                  const bool preserve_divergences)

    {
      using namespace std;

      to_be_processed.clear();

      block initial_partition;

      // First store the bottom and non bottom states.
      aut.sort_transitions(mcrl2::lts::src_lbl_tgt);

      state_type last_non_stored_state_number=0;
      bool bottom_state=true;
      std::vector < state_type > current_inert_transitions;

      {
        // Reserve enough space, such that no reallocations of the vector are required when adding transitions.
        // For this purpose, first the number of inert transitions must be counted, to avoid reserving too much
        // space. This for instance leads to a waste of memory (terabytes for reducing 30M states), especially,
        // when calculating ia strong bisimulation reduction.
        size_t initial_partition_non_inert_counter=0;
        size_t current_inert_transition_counter=0;
        const std::vector<transition> & trans=aut.get_transitions();
        for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
        {
          const transition t= *r;
          if (branching && aut.is_tau(t.label(aut.hidden_label_map())))
          {
            if (preserve_divergences && t.from()==t.to())
            {
              initial_partition_non_inert_counter++;
            }
            else
            {
              current_inert_transition_counter++;
            }
          }
        }
        current_inert_transitions.reserve(initial_partition_non_inert_counter);
        initial_partition.non_inert_transitions.reserve(current_inert_transition_counter);
      }

      const std::vector<transition>& trans=aut.get_transitions();
      for (std::vector<transition>::const_iterator r=trans.begin(); r!=trans.end(); ++r)
      {
        const transition t=* r;

        if (branching && aut.is_tau(t.label(aut.hidden_label_map())))
        {
          if (preserve_divergences && t.from()==t.to())
          {
            assert(tau_label!=size_t(-1));
            initial_partition.non_inert_transitions.push_back(transition(t.from(),tau_label,t.to()));
          }
          else
          {
            current_inert_transitions.push_back(t.to());
            bottom_state=false;
          }
        }
        std::vector<transition>::const_iterator next_i=r;
        ++next_i;
        if (next_i==trans.end() || t.from()!=next_i->from())
        {
          // store the current from state
          for (; last_non_stored_state_number<t.from(); ++last_non_stored_state_number)
          {
            initial_partition.bottom_states.push_back(last_non_stored_state_number);
          }

          if (bottom_state)
          {
            initial_partition.bottom_states.push_back(t.from());
          }
          else
          {
            initial_partition.non_bottom_states.push_back(non_bottom_state(t.from()));
            current_inert_transitions.swap(initial_partition.non_bottom_states.back().inert_transitions);
            bottom_state=true;
          }
          assert(last_non_stored_state_number==t.from());
          last_non_stored_state_number++;
        }
      }

      for (; last_non_stored_state_number<aut.num_states(); ++last_non_stored_state_number)
      {
        initial_partition.bottom_states.push_back(last_non_stored_state_number);
      }

      // Sort the non-bottom states such that the deepest states occur first.
      // Raise an exception if there is a non trivial tau loop.

      order_on_tau_reachability(initial_partition.non_bottom_states);

      // Store the non-inert transitions (i.e. the non tau transitions)
      aut.sort_transitions(mcrl2::lts::lbl_tgt_src);
      const std::vector<transition> & trans1=aut.get_transitions();
      for (std::vector<transition>::const_iterator r=trans1.begin(); r!=trans1.end(); ++r)
      {
        const transition t= *r;
        if (!branching || !aut.is_tau(t.label(aut.hidden_label_map())))
        {
          // Note that by sorting the transitions first, the non_inert_transitions are grouped per label.
          initial_partition.non_inert_transitions.push_back(t);
        }
      }

      // block_is_active.push_back(true);
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

// Refine the partition until the partition has become stable
#ifndef NDEBUG
    void refine_partition_until_it_becomes_stable(const bool branching, const bool preserve_divergence)
#else
    void refine_partition_until_it_becomes_stable(const bool, const bool)
#endif
    {
#ifndef NDEBUG
      size_t consistency_check_counter=1;
      size_t consistency_check_barrier=1;
#endif
      bool partition_is_unstable=true; // This boolean indicates that the partition becomes unstable
      // because an inert transition becomes non inert.
      while (!to_be_processed.empty() || partition_is_unstable)
      {
#ifndef NDEBUG
        // Avoid checking too often. This is too time consuming, even in debug mode.
        consistency_check_counter++;
        if (consistency_check_counter>=consistency_check_barrier)
        {
          consistency_check_counter=0;
          consistency_check_barrier=consistency_check_barrier*2;
          check_internal_consistency_of_the_partitioning_data_structure(branching, preserve_divergence);
        }
#endif
        if (to_be_processed.empty() && partition_is_unstable)
        {
          // Put all blocks in to_be_processed;
          for (block_index_type i=0; i< blocks.size(); ++i)
          {
            to_be_processed.push_back(i);
          }
          block_is_in_to_be_processed=std::vector < bool >(blocks.size(),true);
          partition_is_unstable=false;
        }

        const block_index_type splitter_index=to_be_processed.back();
        // assert(block_is_in_to_be_processed[splitter_index]||!block_is_active[splitter_index]);
        to_be_processed.pop_back();
        block_is_in_to_be_processed[splitter_index]=false;

        // Split with the splitter block, unless it is to_be_processed as we have to reconsider it
        // completely anyhow at some later point.

        // Walk through the non_inert_transitions via a natural number, because
        // the blocks and the non_inert_transitions can move around in memory. This makes it
        // unsafe to use interators.

        for (size_t i=0; i<blocks[splitter_index].non_inert_transitions.size(); ++i)
        {
          // The flag of the starting state of *i is raised and its block is added to BL;

          assert(blocks[splitter_index].non_inert_transitions[i].from()<aut.num_states());
          state_flags[blocks[splitter_index].non_inert_transitions[i].from()]=true;
          const block_index_type marked_block_index=block_index_of_a_state[blocks[splitter_index].non_inert_transitions[i].from()];
          if (block_flags[marked_block_index]==false)
          {
            block_flags[marked_block_index]=true;
            BL.push_back(marked_block_index);
          }

          // If the label of the next action is different, we must carry out the splitting.
          if ((i+1)==blocks[splitter_index].non_inert_transitions.size() ||
              blocks[splitter_index].non_inert_transitions[i].label(aut.hidden_label_map())!=blocks[splitter_index].non_inert_transitions[i+1].label(aut.hidden_label_map()))
          {
            // We consider BL which contains references to all blocks from which a state from splitter
            // can be reached. If not all flags of the non bottom states in a block in BL are set, the
            // non flagged non bottom states are moved to a new block.

            split_the_blocks_in_BL(partition_is_unstable,blocks[splitter_index].non_inert_transitions[i].label(aut.hidden_label_map()),splitter_index);

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



    void refine_partion_with_respect_to_divergences(void);

    void split_the_blocks_in_BL(
      bool& partition_is_unstable,
      const label_type splitter_label,
      const block_index_type splitter_block)
    {
      using namespace std;
      for (std::vector < block_index_type > :: const_iterator i1=BL.begin();
           i1!=BL.end(); ++i1)
      {
// std::cerr << "Splitter label " << splitter_label << "   "  << aut.action_label(splitter_label) << "\n";
        // assert(block_is_active[*i1]);
        block_flags[*i1]=false;
        std::vector < state_type > flagged_states;
        std::vector < state_type > non_flagged_states;
        std::vector < state_type > i1_bottom_states;
        i1_bottom_states.swap(blocks[*i1].bottom_states);

        for (std::vector < state_type >::const_iterator j=i1_bottom_states.begin();
             j!=i1_bottom_states.end(); ++j)
        {
          if (state_flags[*j])
          {
            // state is flagged.
            flagged_states.push_back(*j);
          }
          else
          {
            // state is not flagged. It will be moved to a new block.
            non_flagged_states.push_back(*j);
            block_index_of_a_state[*j]=blocks.size();
          }
        }
        assert(!flagged_states.empty()||!blocks[*i1].non_bottom_states.empty()||i1_bottom_states.empty());
        block_index_type reset_state_flags_block=*i1;

        if (!non_flagged_states.empty())
        {
          // There are flagged and non flagged states. So, the block must be split.
          // Move the unflagged states to the new block.

          if (mCRL2logEnabled(log::debug))
          {
            const size_t m = static_cast<size_t>(std::pow(10.0, std::floor(std::log10(static_cast<double>((blocks.size()+1)/2)))));
            if ((blocks.size()+1)/2 % m==0)
            {
              mCRL2log(log::debug) << "Bisimulation partitioner: create block " << (blocks.size()+1)/2 << std::endl;
            }
          }
          // Record how block *i1 is split, to use this to generate counter examples.
          blocks[*i1].splitter=pair< label_type, block_index_type > (splitter_label,splitter_block);

          // Create a first new block.
          blocks.push_back(block());
          block_index_type new_block1=blocks.size()-1;
          // block_is_active.push_back(true);
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
          // block_is_active.push_back(true);
          blocks.back().state_index=blocks[*i1].state_index;
          blocks.back().block_index=new_block2;
          reset_state_flags_block=new_block2;
          blocks.back().parent_block_index=*i1;

          // Move the flagged states to the second block, and let the block index of these states refer to this block.
          flagged_states.swap(blocks.back().bottom_states);
          std::vector < state_type > &reference_to_flagged_states_of_block2=blocks.back().bottom_states;
          for (std::vector < state_type >::const_iterator j=reference_to_flagged_states_of_block2.begin();
               j!=reference_to_flagged_states_of_block2.end(); ++j)
          {
            block_index_of_a_state[*j]=new_block2;
          }

          // Put the indices of second split block to to_be_processed.
          to_be_processed.push_back(blocks.back().block_index);
          block_is_in_to_be_processed.push_back(true);

          // reset the flag of block *i1, which is being split.
          block_is_in_to_be_processed[*i1]=false;
          // block_is_active[*i1]=false;

          // The flag fields of the new blocks is set to false;
          block_flags.push_back(false);
          block_flags.push_back(false);

          // Declare already some space for transitions, such that we can
          // put inert transitions that become non inert in there when investigating
          // the non bottom states. After investigating the non bottom states,
          // the transitions are split over the vectors below.

          std::vector < transition > flagged_non_inert_transitions;
          std::vector < transition > non_flagged_non_inert_transitions;
          // Reserve enough space for transitions to be copied. Otherwise, resizing may lead to
          // lot of unneccesary copying...
          flagged_non_inert_transitions.reserve(blocks[*i1].non_inert_transitions.size());
          non_flagged_non_inert_transitions.reserve(blocks[*i1].non_inert_transitions.size());

          // Next we scan the non-bottom states of *i1. If for some non-bottom state the flag is not raised
          // and if none of the outgoing P-inert transitions leads to a state in the old block then this
          // state becomes a non bottom state of B2.

          std::vector < non_bottom_state > flagged_non_bottom_states;
          std::vector < non_bottom_state > non_flagged_non_bottom_states;
          std::vector < non_bottom_state > i1_non_bottom_states;
          i1_non_bottom_states.swap(blocks[*i1].non_bottom_states);
          for (typename std::vector < non_bottom_state >::iterator k=i1_non_bottom_states.begin();
               k!=i1_non_bottom_states.end(); ++k)
          {
            const std::vector < state_type > &inert_transitions=k->inert_transitions;
            if (!state_flags[k->state])
            {
              bool all_transitions_end_in_unflagged_block=true;
              for (std::vector < state_type > :: const_iterator l=inert_transitions.begin();
                   all_transitions_end_in_unflagged_block && l!=inert_transitions.end(); ++l)
              {
                if (block_index_of_a_state[*l]!= new_block1)
                {
                  block_index_of_a_state[*l]=new_block2;
                  all_transitions_end_in_unflagged_block=false;
                }
              }
              if (all_transitions_end_in_unflagged_block)
              {
                // Move *k to the non flagged block. Swap the inert transitions to avoid copying.
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
            for (std::vector < state_type > :: const_iterator l=inert_transitions.begin();
                 l!=inert_transitions.end(); ++l)
            {
              if (block_index_of_a_state[*l]==new_block1)
              {
                // The transition *l (*k,tau_label,*l) becomes a non inert transition in the new
                // block.
                assert(tau_label!=size_t(-1));
                non_flagged_non_inert_transitions.push_back(transition(k->state,tau_label,*l));
              }
              else
              {
                // The transition represented by *l remains an inert transition.
                block_index_of_a_state[*l]=new_block2;
                remaining_inert_transitions.push_back(*l);
              }
            }
            if (remaining_inert_transitions.empty()) // The last outgoing inert tau transition just became non inert.
              // k->state has become a bottom state. Otherwise it remains
              // a non bottom state.
            {
              blocks[new_block2].bottom_states.push_back(k->state);
              block_index_of_a_state[k->state]=new_block2;
              partition_is_unstable=true;
            }
            else
            {
              flagged_non_bottom_states.push_back(non_bottom_state(k->state,remaining_inert_transitions));
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
          for (std::vector < transition >::iterator k=i1_non_inert_transitions.begin();
               k!=i1_non_inert_transitions.end(); ++k)
          {
            if (block_index_of_a_state[k->to()]==new_block1)
            {
              non_flagged_non_inert_transitions.push_back(*k);
            }
            else
            {
              block_index_of_a_state[k->to()]=new_block2;
              flagged_non_inert_transitions.push_back(*k);
            }
          }

          // Only put the parts that we need to store. Avoid that reserved space for
          // current_inert_transition_counter and flagged_non_inert_transitions is stored
          // in the non_inert_transitions in both blocks. Therefore, we do not use a copy
          // constructor, but put elements in there one by one.
          // The two lines below were old code, wasting memory bandwidth.
          // non_flagged_non_inert_transitions.swap(blocks[new_block1].non_inert_transitions);
          // flagged_non_inert_transitions.swap(blocks[new_block2].non_inert_transitions);

          blocks[new_block1].non_inert_transitions.reserve(non_flagged_non_inert_transitions.size());
          for(std::vector < transition > ::const_iterator i=non_flagged_non_inert_transitions.begin();
                    i!=non_flagged_non_inert_transitions.end(); i++)
          {
            blocks[new_block1].non_inert_transitions.push_back(*i);
          }

          blocks[new_block2].non_inert_transitions.reserve(flagged_non_inert_transitions.size());
          for(std::vector < transition > ::const_iterator i=flagged_non_inert_transitions.begin();
                    i!=flagged_non_inert_transitions.end(); i++)
          {
            blocks[new_block2].non_inert_transitions.push_back(*i);
          }
        }
        else
        {
          // Nothing changed, so put the bottom states back again.
          i1_bottom_states.swap(blocks[*i1].bottom_states);
        }
        // reset the state flags
        std::vector < state_type > &flagged_states_to_be_unflagged=blocks[reset_state_flags_block].bottom_states;
        for (std::vector < state_type >::const_iterator j=flagged_states_to_be_unflagged.begin();
             j!=flagged_states_to_be_unflagged.end(); ++j)
        {
          state_flags[*j]=false;
        }

        std::vector < non_bottom_state > &flagged_states_to_be_unflagged1=blocks[reset_state_flags_block].non_bottom_states;
        for (typename std::vector < non_bottom_state >::const_iterator j=flagged_states_to_be_unflagged1.begin();
             j!=flagged_states_to_be_unflagged1.end(); ++j)
        {
          state_flags[j->state]=false;
        }
      }
      BL.clear();
    }
    void order_recursively_on_tau_reachability(
      const state_type s,
      std::map < state_type, std::vector < state_type > > &inert_transition_map,
      std::vector < non_bottom_state > &new_non_bottom_states,
      std::set < state_type > &visited)
    {
      if (inert_transition_map.count(s)>0) // The state s is a bottom state. We need not to investigate these.
      {
        if (visited.count(s)==0)
        {
          visited.insert(s);
          std::vector < state_type> &inert_transitions=inert_transition_map[s];
          for (std::vector < state_type>::const_iterator j=inert_transitions.begin();
               j!=inert_transitions.end(); j++)
          {
            order_recursively_on_tau_reachability(*j,inert_transition_map,new_non_bottom_states,visited);
          }
          new_non_bottom_states.push_back(non_bottom_state(s));
          inert_transitions.swap(new_non_bottom_states.back().inert_transitions);
        }
      }
    }

    void order_on_tau_reachability(std::vector < non_bottom_state > &non_bottom_states)
    {
      std::set < state_type > visited;
      std::map < state_type, std::vector < state_type > > inert_transition_map;
      for (typename std::vector < non_bottom_state >::iterator i=non_bottom_states.begin();
           i!=non_bottom_states.end(); ++i)
      {
        i->inert_transitions.swap(inert_transition_map[i->state]);
      }
      std::vector < non_bottom_state > new_non_bottom_states;

      for (typename std::vector < non_bottom_state >::const_iterator i=non_bottom_states.begin();
           i!=non_bottom_states.end(); ++i)
      {
        order_recursively_on_tau_reachability(i->state, inert_transition_map, new_non_bottom_states, visited);
      }
      new_non_bottom_states.swap(non_bottom_states);
    }

    std::set < mcrl2::trace::Trace > counter_traces_aux(
      const state_type s,
      const state_type t,
      const mcrl2::lts::outgoing_transitions_per_state_action_t& outgoing_transitions,
      const bool branching_bisimulation) const
    {
      // First find the smallest block containing both states s and t.
      // Find all blocks containing s.
      std::set < block_index_type > blocks_containing_s;
      block_index_type b_s=block_index_of_a_state[s];
      blocks_containing_s.insert(b_s);
      while (blocks[b_s].parent_block_index!=b_s)
      {
        b_s=blocks[b_s].parent_block_index;
        blocks_containing_s.insert(b_s);
      }

      // Find the first smallest block containing t and s
      block_index_type b_C=block_index_of_a_state[t];
      while (blocks_containing_s.count(b_C)==0)
      {
        assert(blocks[b_C].parent_block_index!=b_C);
        b_C=blocks[b_C].parent_block_index;
      }

      // Now b_C is the smallest block containing both s and t.

      const label_type l=blocks[b_C].splitter.first;
      const block_index_type B__=blocks[b_C].splitter.second;
// std::cerr << "splitter label counter example" << l << "   " << aut.action_label(l) << "\n";
      std::set < state_type > l_reachable_states_for_s;
      std::set < state_type > visited1;
      reachable_states_in_block_s_via_label_l(s,b_C,l,outgoing_transitions, l_reachable_states_for_s,visited1,branching_bisimulation);

      std::set < state_type> B_s_reacha;
      std::set < state_type> B_s_nonreacha;
      for (const state_type i: l_reachable_states_for_s)
      {
        block_index_type b=block_index_of_a_state[i];
        bool reached=b==B__;
        do
        {
          b=blocks[b].parent_block_index;
          if (b==B__)
          {
            reached=true;
          }
        }
        while (!reached && b!=blocks[b].parent_block_index);

        if (reached)
        {
          B_s_reacha.insert(i);
        }
        else
        {
          B_s_nonreacha.insert(i);
        }
      }

      std::set < state_type > l_reachable_states_for_t;
      std::set < state_type > visited2;
      reachable_states_in_block_s_via_label_l(t,b_C,l,outgoing_transitions, l_reachable_states_for_t,visited2,branching_bisimulation);

      std::set < state_type> B_t_reacha;
      std::set < state_type> B_t_nonreacha;
      for (std::set < state_type >::const_iterator i=l_reachable_states_for_t.begin();
           i!=l_reachable_states_for_t.end(); ++i)
      {
        block_index_type b=block_index_of_a_state[*i];
        bool reached=b==B__;
        do
        {
          b=blocks[b].parent_block_index;
          if (b==B__)
          {
            reached=true;
          }
        }
        while (!reached && b!=blocks[b].parent_block_index);

        if (reached)
        {
          B_t_reacha.insert(*i);
        }
        else
        {
          B_t_nonreacha.insert(*i);
        }
      }
// std::cerr << "ASSERTION FAILURE " << B_s_reacha.empty() << "   " << B_t_reacha.empty() << "\n";
      assert((B_s_reacha.empty() && !B_t_reacha.empty()) ||
             (!B_s_reacha.empty() && B_t_reacha.empty()));

      std::set < mcrl2::trace::Trace > resulting_counter_traces;

      if (B_s_reacha.empty())
      {
        B_s_reacha.swap(B_t_reacha);
        B_s_nonreacha.swap(B_t_nonreacha);
      }

      assert(!B_s_reacha.empty());


      if (B_t_nonreacha.empty())
      {
        // The counter trace is simply the label l.
        mcrl2::trace::Trace counter_trace;
        counter_trace.addAction(mcrl2::lps::multi_action(mcrl2::process::action(
                                mcrl2::process::action_label(core::identifier_string(mcrl2::lts::pp(aut.action_label(l))),mcrl2::data::sort_expression_list()),
                                mcrl2::data::data_expression_list())));
        resulting_counter_traces.insert(counter_trace);
      }
      else
      {
        for (std::set < state_type>::const_iterator i_s=B_s_reacha.begin();
             i_s!=B_s_reacha.end(); ++i_s)
        {
          for (std::set < state_type>::const_iterator i_t=B_t_nonreacha.begin();
               i_t!=B_t_nonreacha.end(); ++i_t)
          {
            const std::set < mcrl2::trace::Trace > counter_traces=
              counter_traces_aux(*i_s,*i_t,outgoing_transitions,branching_bisimulation);
            // Add l to these traces and add them to resulting_counter_traces
            for (std::set< mcrl2::trace::Trace >::const_iterator j=counter_traces.begin();
                 j!=counter_traces.end(); ++j)
            {
               mcrl2::trace::Trace new_counter_trace;
              new_counter_trace.addAction(mcrl2::lps::multi_action(mcrl2::process::action(
                                mcrl2::process::action_label(core::identifier_string(mcrl2::lts::pp(aut.action_label(l))),mcrl2::data::sort_expression_list()),
                                mcrl2::data::data_expression_list())));
              mcrl2::trace::Trace old_counter_trace=*j;
              old_counter_trace.resetPosition();
              for (size_t k=0 ; k< old_counter_trace.number_of_actions(); k++)
              {
                new_counter_trace.addAction(old_counter_trace.currentAction());
                old_counter_trace.increasePosition();
              }
              resulting_counter_traces.insert(new_counter_trace);
            }
          }
        }

      }
      return resulting_counter_traces;
    }



    void reachable_states_in_block_s_via_label_l(
      const state_type s,
      const block_index_type block_index_for_bottom_state,
      const label_type l,
      const mcrl2::lts::outgoing_transitions_per_state_action_t& outgoing_transitions,
      std::set < state_type > &result_set,
      std::set < state_type > &visited,
      const bool branching_bisimulation) const
    {
      using namespace std;
      // This function yields a set of states that are reachable via a sequence of tau steps
      // in block block_index_for_bottom_state, followed by an l step.
      // The technique used is to search through tau transitions from s, until a bottom state
      // in the current class is found. From this state all reachable states are put in the result set.
      if (visited.count(s)>0)
      {
        return;
      }

      visited.insert(s);
      // Put all l reachable states in the result set.
      for (outgoing_transitions_per_state_action_t::const_iterator i1=outgoing_transitions.lower_bound(pair<state_type,label_type>(s,l));
           i1!=outgoing_transitions.upper_bound(pair<state_type, label_type>(s,l)); ++i1)
      {
        result_set.insert(to(i1));
      }

      // Search for tau reachable states that are still in the block with block_index_for_bottom_state.
      if (branching_bisimulation)
      {
        for (label_type lab=0; lab<aut.num_action_labels(); ++lab)
        {
          if (aut.is_tau(aut.apply_hidden_label_map(lab)))
          {
            for (outgoing_transitions_per_state_action_t::const_iterator i=outgoing_transitions.lower_bound(pair<state_type,label_type>(s,lab));
                 i!=outgoing_transitions.upper_bound(pair<state_type, label_type>(s,lab)); ++i)
            {
              // Now find out whether the block index of to(i) is part of the block with index block_index_for_bottom_state.
              block_index_type b=block_index_of_a_state[to(i)];
              while (b!=block_index_for_bottom_state && blocks[b].parent_block_index!=b)
              {
                assert(blocks[b].parent_block_index!=b);
                b=blocks[b].parent_block_index;
              }
              if (b==block_index_for_bottom_state)
              {
                reachable_states_in_block_s_via_label_l(
                  to(i),
                  block_index_for_bottom_state,
                  l,
                  outgoing_transitions,
                  result_set,
                  visited,
                  branching_bisimulation);
              }
            }
          }
        }
      }
    }


#ifndef NDEBUG
    // The method below is intended to check the consistency of the internal data
    // structure. Its sole purpose is to detect programming errors. It has no
    // side effects on the data structure. If a problem occurs, execution halts with
    // an assert.

    void check_internal_consistency_of_the_partitioning_data_structure(
      const bool branching,
      const bool preserve_divergence) const
    {
      state_type total_number_of_states=0;
      size_t total_number_of_transitions=0;

      assert(!blocks.empty());
      std::set < block_index_type > block_indices;

      assert(block_index_of_a_state.size()==aut.num_states());
      for (typename std::vector < block >::const_iterator i=blocks.begin();
           i!=blocks.end(); ++i)
      {
        // Check the block_index.
        assert(i->block_index<blocks.size());
        assert(block_indices.count(i->block_index)==0);
        block_indices.insert(i->block_index);

        // Check the bottom states.
        const std::vector < state_type > &i_bottom_states=i->bottom_states;

        for (std::vector < state_type >::const_iterator j=i_bottom_states.begin();
             j!=i_bottom_states.end(); ++j)
        {
          total_number_of_states++;
          assert(*j<aut.num_states());
          // Check that the block number of the state is maintained properly.
          assert(block_index_of_a_state[*j]==i->block_index);
        }

        // Check the non bottom states. In particular check that there is no tau loop
        // in these non bottom states.
        const std::vector < non_bottom_state > &i_non_bottom_states=i->non_bottom_states;
        std::set < state_type > visited;
        std::set < state_type > local_bottom_states;
        for (typename std::vector < non_bottom_state >::const_iterator j=i_non_bottom_states.begin();
             j!=i_non_bottom_states.end(); ++j)
        {
          local_bottom_states.insert(j->state);
        }

        for (typename std::vector < non_bottom_state >::const_iterator j=i_non_bottom_states.begin();
             j!=i_non_bottom_states.end(); ++j)
        {
          total_number_of_states++;
          assert(j->state<aut.num_states());
          // Check that the block number of the state is maintained properly.
          assert(block_index_of_a_state[j->state]==i->block_index);
          const std::vector < state_type > &j_inert_transitions=j->inert_transitions;
          for (std::vector < state_type >::const_iterator k=j_inert_transitions.begin();
               k!=j_inert_transitions.end(); k++)
          {
            total_number_of_transitions++;
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
        for (std::vector < transition >::const_iterator j=i_non_inert_transitions.begin();
             j!=i_non_inert_transitions.end(); ++j)
        {
          total_number_of_transitions++;
          assert(j->to()<aut.num_states());
          assert(j->from()<aut.num_states());

          // Check proper grouping of action labels.
          std::vector < transition >::const_iterator j_next=j;
          j_next++;
          if (j_next==i_non_inert_transitions.end() || (j->label(aut.hidden_label_map())!=j_next->label(aut.hidden_label_map())))
          {
            assert(observed_action_labels.count(j->label(aut.hidden_label_map()))==0);
            observed_action_labels.insert(j->label(aut.hidden_label_map()));
          }

          // Check whether tau transition in non inert transition vector is inert.
          if (!preserve_divergence && branching && aut.is_tau(j->label(aut.hidden_label_map())))
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
      for (std::vector < state_type >::const_iterator i=block_index_of_a_state.begin();
           i!=block_index_of_a_state.end(); ++i)
      {
        assert(blocks[*i].block_index== *i);
      }

      // Check block_flags that the block flags are all set to false
      for (std::vector < bool >::const_iterator i=block_flags.begin();
           i!=block_flags.end(); ++i)
      {
        assert(!*i);
      }

      // Check that state_flags are all false.
      for (std::vector < bool >::const_iterator i=state_flags.begin();
           i!=state_flags.end(); ++i)
      {
        assert(!*i);
      }

      // Check to_be_processed
      // Check block_is_in_to_be_processed
      std::vector < bool > temporary_block_is_in_to_be_processed(blocks.size(),false);

      for (std::vector< block_index_type > ::const_iterator i=to_be_processed.begin();
           i!=to_be_processed.end(); ++i)
      {
        temporary_block_is_in_to_be_processed[*i]=true;
      }
      for (state_type i=0; i<blocks.size(); ++i)
      {
        assert(!block_is_in_to_be_processed[i] || temporary_block_is_in_to_be_processed[i]);
      }

      // Check that BL is empty.
      assert(BL.empty());

      // Check that tau_label is smaller or equal to the number of labels.
      // If no tau label is used, it is equal to the number of labels, which
      // is a number of labels that is not used.
      assert(tau_label<=aut.num_action_labels() || tau_label==size_t(-1));
    }

#endif // not NDEBUG

};


/** \brief Reduce transition system l with respect to strong or (divergence preserving) branching bisimulation.
 * \param[in/out] l The transition system that is reduced.
 * \param[in] branching If true branching bisimulation is applied, otherwise strong bisimulation.
 * \param[in] preserve_divergences Indicates whether loops of internal actions on states must be preserved. If false
 *            these are removed. If true these are preserved.  */
template < class LTS_TYPE>
void bisimulation_reduce(
  LTS_TYPE& l,
  const bool branching = false,
  const bool preserve_divergences = false);


/** \brief Checks whether the two initial states of two lts's are strong or branching bisimilar.
 * \details This lts and the lts l2 are not usable anymore after this call.
 *          The space consumption is O(n) and time is O(nm). It uses the branching bisimulation
 *          algorithm by Groote and Vaandrager from 1990.
 * \param[in/out] l1 A first transition system.
 * \param[in/out] l2 A second transistion system.
 * \param[branching] If true branching bisimulation is used, otherwise strong bisimulation is applied.
 * \param[preserve_divergences] If true and branching is true, preserve tau loops on states.
 * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
template < class LTS_TYPE>
bool destructive_bisimulation_compare(
  LTS_TYPE& l1,
  LTS_TYPE& l2,
  const bool branching=false,
  const bool preserve_divergences=false,
  const bool generate_counter_examples = false);


/** \brief Checks whether the two initial states of two lts's are strong or branching bisimilar.
 *  \details The current transitions system and the lts l2 are first duplicated and subsequently
 *           reduced modulo bisimulation. If memory space is a concern, one could consider to
 *           use destructive_bisimulation_compare. This routine uses the Groote-Vaandrager
 *           branching bisimulation routine. It runs in O(mn) and uses O(n) memory where n is the
 *           number of states and m is the number of transitions.
 * \param[in/out] l1 A first transition system.
 * \param[in/out] l2 A second transistion system.
 * \param[branching] If true branching bisimulation is used, otherwise strong bisimulation is applied.
 * \param[preserve_divergences] If true and branching is true, preserve tau loops on states.
 * \retval True iff the initial states of the current transition system and l2 are (divergence preserving) (branching) bisimilar */
template < class LTS_TYPE>
bool bisimulation_compare(
  const LTS_TYPE& l1,
  const LTS_TYPE& l2,
  const bool branching=false,
  const bool preserve_divergences=false,
  const bool generate_counter_examples = false);





template < class LTS_TYPE>
std::set < mcrl2::trace::Trace > bisim_partitioner<LTS_TYPE>::counter_traces(
  const size_t s,
  const size_t t,
  const bool branching_bisimulation)
{
  if (get_eq_class(s)==get_eq_class(t))
  {
    throw mcrl2::runtime_error("Requesting a counter trace for two bisimilar states. Such a trace is not useful.");
  }

  const outgoing_transitions_per_state_action_t outgoing_transitions=transitions_per_outgoing_state_action_pair(aut.get_transitions(),transition::default_label_map());
  return counter_traces_aux(s,t,outgoing_transitions,branching_bisimulation);
}


template < class LTS_TYPE>
void bisimulation_reduce(LTS_TYPE& l,
                         const bool branching /*=false */,
                         const bool preserve_divergences /*=false */)
{
  // First, remove tau loops in case of branching bisimulation.
  if (branching)
  {
    scc_reduce(l,preserve_divergences);
  }

  // Secondly, apply the branching bisimulation reduction algorithm. If there are no tau's,
  // this will automatically yield strong bisimulation.
  detail::bisim_partitioner<LTS_TYPE> bisim_part(l, branching, preserve_divergences);

  // Clear the state labels of the LTS l
  l.clear_state_labels();

  // Assign the reduced LTS
  l.set_num_states(bisim_part.num_eq_classes());
  l.set_initial_state(bisim_part.get_eq_class(l.initial_state()));
  bisim_part.replace_transitions(branching,preserve_divergences);
}

template < class LTS_TYPE>
bool bisimulation_compare(
  const LTS_TYPE& l1,
  const LTS_TYPE& l2,
  const bool branching /* =false*/,
  const bool preserve_divergences /*=false*/,
  const bool generate_counter_examples /*= false*/)
{
  LTS_TYPE l1_copy(l1);
  LTS_TYPE l2_copy(l2);
  return destructive_bisimulation_compare(l1_copy,l2_copy,branching,preserve_divergences,
                                          generate_counter_examples);
}

template < class LTS_TYPE>
bool destructive_bisimulation_compare(
  LTS_TYPE& l1,
  LTS_TYPE& l2,
  const bool branching /* =false*/,
  const bool preserve_divergences /*=false*/,
  const bool generate_counter_examples /* = false */)
{
  size_t init_l2 = l2.initial_state() + l1.num_states();
  mcrl2::lts::detail::merge(l1,l2);
  l2.clear(); // No use for l2 anymore.


  // First remove tau loops in case of branching bisimulation.
  if (branching)
  {
    detail::scc_partitioner<LTS_TYPE> scc_part(l1);
    scc_part.replace_transitions(preserve_divergences);
    l1.set_num_states(scc_part.num_eq_classes());
    l1.set_initial_state(scc_part.get_eq_class(l1.initial_state()));
    init_l2 = scc_part.get_eq_class(init_l2);
  }

  detail::bisim_partitioner<LTS_TYPE> bisim_part(l1, branching, preserve_divergences);
  if (generate_counter_examples && !bisim_part.in_same_class(l1.initial_state(),init_l2))
  {
    std::set < mcrl2::trace::Trace > counter_example_traces=bisim_part.counter_traces(l1.initial_state(),init_l2,branching);
    size_t count=0;
    for (std::set < mcrl2::trace::Trace >::const_iterator i=counter_example_traces.begin();
         i!=counter_example_traces.end(); ++i,++count)
    {
      std::stringstream filename_s;
      filename_s << "Counterexample" << count << ".trc";
      const std::string filename(filename_s.str());
      mcrl2::trace::Trace i_trace= *i;
      i_trace.save(filename,mcrl2::trace::tfPlain);
      mCRL2log(mcrl2::log::info) << "Saved counterexample to: \"" << filename << "\"" << std::endl;
    }
  }
  return bisim_part.in_same_class(l1.initial_state(),init_l2);
}

}
}
}
#endif
