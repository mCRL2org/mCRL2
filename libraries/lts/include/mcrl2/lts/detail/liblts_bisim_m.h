// Author(s): Jan Martens
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
/// \file lts/detail/liblts_bisim_m.h
///
/// \brief Partition refinement algorithm for guaruanteed minimal depth
/// counter-examples.
///
/// \details 
#ifndef _LIBLTS_BISIM_M
#define _LIBLTS_BISIM_M

#include <fstream>
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/lts/lts_utilities.h"
#include "mcrl2/lts/detail/liblts_scc.h"
#include "mcrl2/lts/detail/liblts_merge.h"
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
class bisim_partitioner_counter_example {
public:
    /** \brief Creates a bisimulation partitioner for an LTS.
    *  \details TODO.
    *  \warning Might be less efficient than other implementations, focussed on generating minimal depth counter-examples.
    *  \param[in] l Reference to the LTS. */
    bisim_partitioner_counter_example(LTS_TYPE& l, const std::size_t init_l2)
        : max_state_index(0),
        aut(l),
        initial_l2(init_l2)
    {
        create_initial_partition();
        mCRL2log(mcrl2::log::info) << "Created initial partition.\n";
        bool splitted = true; 
        level_type lvl = 1;
        while (splitted && block_index_of_a_state[initial_l2] == block_index_of_a_state[aut.initial_state()]) {
            splitted = refine_partition(lvl);
            lvl++; 
            for (std::vector<block>::reverse_iterator i = blocks.rbegin();
                i != blocks.rend() && (*i).level == lvl - 1; ++i) {
                to_be_processed.push_back((*i).block_index);
            }
        }

        mCRL2log(mcrl2::log::info) << "Refining done blocks, doing some post-processing.\n";
        for (state_type i = 0; i < aut.num_states(); i++) {
            block_index_type bid = block_index_of_a_state[i];
            if (!block_flags[bid]) {
                block_flags[bid] = true; 
                partition.insert(bid);
            }
        }
        mCRL2log(mcrl2::log::info) << "Partition refinement done, partition contains: " << partition.size() << " blocks, the history contains "<< lvl-1 << " levels.\n";
        transId(aut.initial_state());
    }
    /** \brief Destroys this partitioner. */
    ~bisim_partitioner_counter_example() = default;

    /** \brief Creates a state formula that distinguishes state s from state t.
     *  \details The states s and t are non bisimilar states. If they are
     *           bisimilar an exception is raised. A counter state formula phi is
     *           returned, which has the property that s |= phi and not t |= phi.
     *           Based on "Computing Distinguishing Formulas for Branching Bisimulation", 1991 by Henri Korver
     *  \param[in] s The state number for which the resulting formula should be true
     *  \param[in] t The state number for which the resulting formula should be false
     *  \return A distinguishing state formula. */
    mcrl2::state_formulas::state_formula counter_formula_mindepth();


private:
    typedef std::size_t block_index_type;
    typedef std::size_t state_type;
    typedef std::size_t level_type;
    typedef std::size_t formula_index_type;
    typedef std::size_t label_type;
    state_type initial_l2;

    state_type max_state_index;
    LTS_TYPE& aut;

    struct block
    {
        state_type state_index;                   // The state number that represent the states in this block
        block_index_type block_index;             // The sequence number of this block.
        block_index_type parent_block_index;      // Index of the parent block.
        level_type level;


        // If there is no parent block, this refers to the block itself.
        std::vector < state_type > states;
        std::vector < transition > transitions;
        std::set<std::pair < label_type, block_index_type >> outgoing_transitions;
        void swap(block& b)
        {
            state_type state_index1 = b.state_index;
            b.state_index = state_index;
            state_index = state_index1;

            block_index_type block_index1 = b.block_index;
            b.block_index = block_index;
            block_index = block_index1;

            block_index_type parent_block_index1 = b.parent_block_index;
            b.parent_block_index = parent_block_index;
            parent_block_index = parent_block_index1;

            level_type level1 = b.level;
            b.level = level;
            level = level1;

            states.swap(b.states);
            transitions.swap(b.transitions);
        }
    };

    struct formula 
    {
        formula_index_type index;
        label_type label;
        bool negated;
        std::vector <formula> conjunctions;

        int depth() {
            int max_depth = 0;
            for (formula f : conjunctions) {
                if (f.depth() > max_depth) {
                    max_depth = f.depth() + 1;
                }
            }
            return max_depth; 
        }

        void set_truths() {
            std::set <block_index_type> image_truths;
            std::set <block_index_type> pre_image_truths;
            std::set<block_index_type> intersection;

            image_truths = partition.copy();
            for (formula f : conjunctions) {
                std::set_intersection(image_truths.begin(), image_truths.end(), truths[f].begin(), truths[f].end(),
                        std::inserter(intersection, intersection.begin()));
                image_truths.swap(intersection);
                intersection.clear();
            }

            //Now compute preimage according to label
            for (block_index_type B : image_truths) {
                for (transition t : blocks[B].transitions) {
                    if (t.label() == label && !pre_image_truths.contains(block_index_of_a_state[t.from()])) {
                        pre_image_truths.insert(block_index_of_a_state[t.from()]);
                    }
                }
            }
           
            if (negated) {
                image_truths.swap(pre_image_truths);
                pre_image_truths.clear();
                std::set_difference(partition.begin(), partition.end(), image_truths.begin(), image_truths.end(),
                    std::inserter(pre_image_truths.begin(), pre_image_truths.end()));
            }
            truths[f] = pre_image_truths;
        }
    };

    std::vector < block > blocks;
    std::set <block_index_type> partition;
    std::map < formula, std::set <block>> truths;
    // Blocks that are split become inactive.
    std::vector < state_type > block_index_of_a_state;
    std::vector < bool > block_flags;
    std::vector < bool > state_flags;

    std::vector< block_index_type > to_be_processed;
    std::vector< block_index_type > BL;

    // map from source state and action to target state, makes generating counterexample info easier
    mcrl2::lts::outgoing_transitions_per_state_action_t transitions_outgoing;

    void transId(state_type s) {
        return; 
    }

    void create_initial_partition() {
        to_be_processed.clear();
        // First store the bottom and non bottom states.
        transitions_outgoing = mcrl2::lts::transitions_per_outgoing_state_action_pair(aut.get_transitions());

        block initial_block;

        state_type last_non_stored_state_number = 0;
        initial_block = block();
        for (state_type i = 0; i < aut.num_states(); i++) {
            initial_block.states.push_back(i);
        }
        sort_transitions(aut.get_transitions(), mcrl2::lts::lbl_tgt_src);
        const std::vector<transition>& trans = aut.get_transitions();
        for (std::vector<transition>::const_iterator r = trans.begin(); r != trans.end(); ++r)
        {
            initial_block.transitions.push_back(*r);
        }
        initial_block.block_index = 0;
        initial_block.state_index = 0;
        max_state_index = 1;
        initial_block.parent_block_index = 0;
        initial_block.level = 0;
        blocks.push_back(block());
        blocks.back().swap(initial_block);
        block_index_of_a_state = std::vector < block_index_type >(aut.num_states(), 0);
        block_flags.push_back(false);
        state_flags=std::vector < bool >(aut.num_states(),false);
        to_be_processed.push_back(0);
        BL.clear();
    } // end create_initial_partition

     // Refine the partition to a certain lvl.
    bool refine_partition(level_type lvl) {
        if (to_be_processed.empty()) {
            return false;
        }
        block_index_type splitter_index;
        while (!to_be_processed.empty()) {
            splitter_index = to_be_processed.front();

            to_be_processed.erase(to_be_processed.begin());
            if (blocks[splitter_index].level == lvl) {
                return true;
            }

            std::vector < transition > transitions_to_process;
            transitions_to_process = blocks[splitter_index].transitions;
            for (std::vector < transition > ::const_iterator i = transitions_to_process.begin();
                i != transitions_to_process.end(); ++i)
            {
                transition t = *i;
                state_flags[t.from()] = true;
                const block_index_type marked_block_index = block_index_of_a_state[t.from()];
                if (block_flags[marked_block_index] == false) {
                    block_flags[marked_block_index] = true;
                    BL.push_back(marked_block_index);
                }
                // If the label of the next action is different, we must carry out the splitting.
                if ((i != transitions_to_process.end() && next(i) == transitions_to_process.end())||
                    t.label() != (*next(i)).label())
                {
                    split_BL(t.label(), splitter_index, lvl);
                    BL.clear();
                }
            }
            //blocks[splitter_index].transitions.clear();
        }
        return true;
    }

    void split_BL(
        const label_type splitter_label,
        const block_index_type splitter_block,
        level_type lvl
    )
    {
        for (block_index_type Bsplit : BL) {
            block_flags[Bsplit] = false;
            std::vector < state_type > flagged_states;
            std::vector < state_type > non_flagged_states;
            std::vector < state_type > Bsplit_states;
            Bsplit_states.swap(blocks[Bsplit].states);
            for (state_type s : Bsplit_states) {
                if (state_flags[s]) {
                    // state is flagged.
                    flagged_states.push_back(s);
                }
                else {
                    // state is not flagged. It will be moved to a new block.
                    non_flagged_states.push_back(s);
                    block_index_of_a_state[s] = blocks.size();
                }
            }
            block_index_type Bparent = Bsplit;
            // Set the correct parent
            while (blocks[Bparent].level == lvl) {
                Bparent = blocks[Bparent].parent_block_index;
            }

            block_index_type reset_state_flags_block = Bsplit;
            if (!non_flagged_states.empty()) {
                // There are flagged and non flagged states. So, the block must be split.
                // Move the unflagged states to the new block.
                if (mCRL2logEnabled(log::debug)) {
                    const std::size_t m = static_cast<std::size_t>(std::pow(10.0, std::floor(std::log10(static_cast<double>((blocks.size() + 1) / 2)))));
                    if ((blocks.size() + 1) / 2 % m == 0) {
                        mCRL2log(log::debug) << "Bisimulation partitioner: create block " << (blocks.size() + 1) / 2 << std::endl;
                    }
                }
                // Create a first new block.
                blocks.push_back(block());
                block_index_type new_block1 = blocks.size() - 1;
                blocks.back().state_index = max_state_index;
                max_state_index++;
                blocks.back().block_index = new_block1;
                blocks.back().level = lvl;

                blocks.back().parent_block_index = Bparent;
                non_flagged_states.swap(blocks.back().states);

                // The flag fields of the new blocks is set to false;
                block_flags.push_back(false);
                // distribute the transitions
                // Finally the non-inert transitions are distributed over both blocks in the obvious way.
                // Note that this must be done after all states are properly put into a new block.
                std::vector < transition > old_transitions;
                std::vector < transition > flagged_transitions;
                std::vector < transition > non_flagged_transitions;

                old_transitions = blocks[Bsplit].transitions;
                for (std::vector < transition >::iterator k = old_transitions.begin();
                    k != old_transitions.end(); ++k)
                {
                    if (state_flags[(*k).to()]) {
                        flagged_transitions.push_back(*k);
                    } else {
                        non_flagged_transitions.push_back(*k);
                    }
                }

                // Create a second new block.
                block BlockLeft = block();

                // block_is_active.push_back(true);
                BlockLeft.state_index = blocks[Bsplit].state_index;
                BlockLeft.level = lvl;
                BlockLeft.parent_block_index = Bparent;
                BlockLeft.transitions.swap(flagged_transitions);
                BlockLeft.states.swap(flagged_states);
                if (blocks[Bsplit].level == lvl) {
                    BlockLeft.block_index = Bsplit;
                    BlockLeft.state_index = blocks[Bsplit].state_index;
                    blocks[Bsplit].swap(BlockLeft);
                }
                else {
                    BlockLeft.block_index = blocks.size();
                    BlockLeft.state_index = max_state_index;
                    max_state_index++;
                    blocks.push_back(BlockLeft);
                    block_flags.push_back(false);
                    for (state_type s : BlockLeft.states) {
                        block_index_of_a_state[s] = BlockLeft.block_index;
                    } 
                }

                blocks[new_block1].transitions.swap(non_flagged_transitions);
                reset_state_flags_block = BlockLeft.block_index;

                if (BlockLeft.block_index != Bsplit) {
                    std::vector < state_type >& reference_to_flagged_states_of_block2 = blocks.back().states;
                    for (std::vector < state_type >::const_iterator j = reference_to_flagged_states_of_block2.begin();
                        j != reference_to_flagged_states_of_block2.end(); ++j)
                    {
                        block_index_of_a_state[*j] = BlockLeft.block_index;
                    }
                }

            } else {
                reset_state_flags_block = Bsplit;
                blocks[Bsplit].states.swap(flagged_states);
            }
            // reset the state flags
            std::vector < state_type >& flagged_states_to_be_unflagged = blocks[reset_state_flags_block].states;
            for (state_type s : flagged_states_to_be_unflagged) {
                state_flags[s] = false;
            }
            BL.clear();
        };
    };
};
}
}
}
#endif