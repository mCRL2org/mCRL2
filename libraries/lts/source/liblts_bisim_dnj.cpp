// Author(s): David N. Jansen, Institute of Software, Chinese Academy of
// Sciences, Beijing, PR China
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file lts/detail/liblts_bisim_dnj.cpp
///
/// \brief O(m log n)-time branching bisimulation algorithm
///
/// \details This file implements an efficient partition refinement algorithm
/// for labelled transition systems inspired by Groote / Jansen / Keiren / Wijs
/// and Valmari (2009) to calculate the branching bisimulation classes of a
/// labelled transition system.
///
/// Different from that article, it does not translate a LTS to a Kripke
/// structure but works on the LTS directly.
///
/// \author David N. Jansen, Institute of Software, Chinese Academy of
/// Sciences, Beijing, PR China


#include "mcrl2/lts/detail/stackless_coroutine.h"
#include "mcrl2/lts/detail/liblts_bisim_dnj.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
// #include "mcrl2/lts/transition.h"
// #include "mcrl2/lts/lts_utilities.h"

#include <algorithm>

namespace mcrl2
{
namespace lts
{
namespace detail
{
namespace bisim_dnj
{

// definitions of static members
block_bunch_const_iter_t block_bunch_slice_t::block_bunch_begin;
state_type block_t::nr_of_blocks;
bunch_t* bunch_t::first_nontrivial;

                                                                                #ifndef NDEBUG
                                                                                state_info_const_ptr state_info_entry::s_i_begin;
                                                                                state_info_const_ptr state_info_entry::s_i_end;
                                                                                permutation_iter_t block_t::perm_begin;
/* ************************************************************************* */ permutation_const_iter_t block_t::perm_end;
/*                                                                           */ action_block_const_iter_t action_block_entry::action_block_begin;
/*                   R E F I N A B L E   P A R T I T I O N                   */ const action_block_iter_t* action_block_entry::action_block_end;
/*                                                                           */ action_block_const_iter_t action_block_entry::action_block_orig_inert_begin;
/* ************************************************************************* */ const block_bunch_iter_t* block_bunch_slice_t::block_bunch_end;
                                                                                #endif




/// \brief refine the block
/// \details This function is called after a refinement function has found
/// where to split the block into unmarked (blue) and marked (red) states.
/// It creates a new block for the blue states.
/// \param   new_block_mode indicates whether the blue or the red block
///          should be the new one.
/// \returns pointer to the new block
block_t* block_t::split_off_block(enum new_block_mode_t const new_block_mode)
{                                                                               assert(0 < marked_size());  assert(0 < unmarked_bottom_size());
    // swap contents

    // The structure of a block is
    // |  unmarked  |   marked   |  unmarked  |   marked   |
    // |   bottom   |   bottom   | non-bottom | non-bottom |
    // We have to swap the marked bottom with the unmarked non-bottom states.
    //
    // It is not necessary to reset the nottoblue counters; these counters are
    // anyway only valid for the maybe-blue states.
    state_type swapcount = std::min(marked_bottom_size(),
                                                    unmarked_nonbottom_size());
    if (0 < swapcount)
    {
        // vector swap the states:
        permutation_iter_t pos1 = marked_bottom_begin,
                                                 pos2 = marked_nonbottom_begin; assert(pos1 < pos2);
        state_info_ptr const temp = *pos1;
        for (;;)
        {
            --pos2;                                                             assert(pos1 < pos2);
            *pos1 = *pos2;
            (*pos1)->pos = pos1;
            ++pos1;
            if (0 >= --swapcount)  break;                                       assert(pos1 < pos2);
            *pos2 = *pos1;
            (*pos2)-> pos = pos2;
        }
        *pos2 = temp;
        (*pos2)->pos = pos2;
    }

    // create a new block
    permutation_iter_t const splitpoint = marked_bottom_begin +
                                                     unmarked_nonbottom_size(); assert(begin < splitpoint);  assert(splitpoint < end);

    block_t* new_block;
    if (new_block_is_blue == new_block_mode)
    {                                                                           assert((state_type) (splitpoint - begin) <= size()/2);
        new_block = new block_t(begin, splitpoint);
        new_block->nonbottom_begin = marked_bottom_begin;

        // adapt the old block: it only keeps the red states
        begin = splitpoint;
        nonbottom_begin = marked_nonbottom_begin;
    }
    else
    {                                                                           assert(new_block_is_red == new_block_mode);
        new_block = new block_t(splitpoint, end);                               assert((state_type) (end - splitpoint) <= size()/2);
        new_block->nonbottom_begin = marked_nonbottom_begin;

        // adapt the old block: it only keeps the blue states
        end = splitpoint;
        nonbottom_begin = marked_bottom_begin;
    }

    // unmark all states in both blocks
    marked_nonbottom_begin = end;
    // new_block->marked_nonbottom_begin =  new_block->end;
    marked_bottom_begin = nonbottom_begin;
    new_block->marked_bottom_begin = new_block->nonbottom_begin;                assert(new_block->size() <= size());

    // set the block pointer of states is the new block
    for (permutation_iter_t s_iter = new_block->begin;
                                             s_iter < new_block->end; ++s_iter)
    {
        (*s_iter)->block = new_block;
    }

    return new_block;
}
                                                                                #ifndef NDEBUG
                                                                                /// \brief print a slice of the partition (typically a block)
                                                                                /// \details If the slice indicated by the parameters is not empty, the states
                                                                                /// in this slice will be printed.
                                                                                /// \param message text printed as a title if the slice is not empty
                                                                                /// \param B       block that is being printed (it is checked whether states
                                                                                ///                belong to this block)
                                                                                /// \param begin   iterator to the beginning of the slice
                                                                                /// \param end     iterator past the end of the slice
                                                                                void part_state_t::print_block(const char*const message, const block_t*const B,
                                                                                                                      permutation_const_iter_t begin,
                                                                                                                      permutation_const_iter_t const end) const
                                                                                {
                                                                                    if (0 == end - begin)  return;

                                                                                    mCRL2log(log::debug, "bisim_dnj") << '\t' << message
                                                                                                                         << (1 < end - begin ? "s:\n" : ":\n");
                                                                                    do
                                                                                    {
                                                                                        mCRL2log(log::debug, "bisim_dnj") << "\t\t" << (*begin)->debug_id();
                                                                                        if (B != (*begin)->block)
                                                                                        {
                                                                                            mCRL2log(log::debug,"bisim_dnj") << ", inconsistent: points to "
                                                                                                                                << (*begin)->block->debug_id();
                                                                                        }
                                                                                        if (begin != (*begin)->pos)
                                                                                        {
                                                                                            mCRL2log(log::debug, "bisim_dnj")
                                                                                                               << ", inconsistent pointer to state_info_entry";
                                                                                        }
                                                                                        mCRL2log(log::debug, "bisim_dnj") << '\n';
                                                                                    }
                                                                                    while (++begin < end);
                                                                                }

                                                                                /// \brief print the partition as a tree (per constellation and block)
                                                                                /// \details The function prints all constellations (in order); for each
                                                                                /// constellation it prints the blocks it consists of; and for each block, it
                                                                                /// lists its states, separated into nonbottom and bottom states.
                                                                                /// \param part_tr partition for the transitions
                                                                                void part_state_t::print_part() const
                                                                                {   assert(permutation.begin() < permutation.end());
                                                                                    const block_t* B = (*permutation.begin())->block;
                                                                                    do
                                                                                    {
                                                                                        mCRL2log(log::debug, "bisim_dnj") << B->debug_id() << ":\n";
                                                                                        print_block("Bottom state", B, B->begin, B->marked_bottom_begin);
                                                                                        print_block("Marked bottom state", B,
                                                                                                                   B->marked_bottom_begin, B->nonbottom_begin);
                                                                                        print_block("Non-bottom state", B,
                                                                                                                B->nonbottom_begin, B->marked_nonbottom_begin);
                                                                                        print_block("Marked non-bottom state", B,
                                                                                                                            B->marked_nonbottom_begin, B->end);
                                                                                    // go to next block
                                                                                    }
                                                                                    while (B->end < permutation.end() && (B = (*B->end)->block, true));
                                                                                }

                                                                                /// \brief asserts that the partition of states is consistent
                                                                                /// \details It also requires that no states are marked.
                                                                                void part_state_t::assert_consistency(bool const branching) const
                                                                                {
                                                                                    permutation_const_iter_t block_begin = permutation.begin();
                                                                                    while (block_begin < permutation.end())
                                                                                    {
                                                                                        const block_t* const block = (*block_begin)->block;
                                                                                        // block is consistent:
                                                                                        assert(block->begin == block_begin);
                                                                                        assert(block->begin <= block->marked_bottom_begin);
                                                                                        assert(block->marked_bottom_begin == block->nonbottom_begin);
                                                                                        assert(block->nonbottom_begin <= block->marked_nonbottom_begin);
                                                                                        assert(block->marked_nonbottom_begin == block->end);
                                                                                        assert(block->begin < block->nonbottom_begin);
                                                                                        assert(branching || block->nonbottom_begin == block->end);
                                                                                        assert(0 <= block->seqnr);
                                                                                        assert(block->seqnr < block_t::nr_of_blocks);

                                                                                        // states in the block are consistent:
                                                                                        for (permutation_const_iter_t perm_iter = block->begin;
                                                                                                                           perm_iter < block->end; ++perm_iter)
                                                                                        {
                                                                                            state_info_const_ptr const state = *perm_iter;
                                                                                            assert(state->pred.begin <= state->pred_inert.begin);
                                                                                            assert(state->pred_inert.begin <= state->pred_cend());
                                                                                            assert(state->succ.begin <= state->current_out_slice);
                                                                                            assert(state->current_out_slice <= state->succ_inert.begin);
                                                                                            if (perm_iter < block->nonbottom_begin)
                                                                                            {
                                                                                                assert(state->succ_inert.begin == state->succ_cend());
                                                                                            }
                                                                                            else
                                                                                            {
/* ************************************************************************* */                 assert(state->succ_inert.begin < state->succ_cend());
/*                                                                           */             }
/*                           T R A N S I T I O N S                           */             assert(block == state->block);
/*                                                                           */             assert(perm_iter == state->pos);
/* ************************************************************************* */         }
                                                                                        block_begin = block->end;
                                                                                    }
                                                                                    assert(permutation.end() == block_begin);
                                                                                }
                                                                                #endif // ifndef NDEBUG
/// \brief transition is moved to a new bunch
/// \details This (and the next function) have to be called after a
/// transition has changed its bunch.  The member function will adapt the
/// transition data structure.  It assumes that the transition is non-inert.
///
/// The work has to be done in two steps: We call the first step
/// first_move_transition_to_new_bunch() for each transition in the new bunch,
/// and then call second_move_transition_to_new_bunch() again for all these
/// transitions.  The reason is that some data structures need to be finalised
/// in the second phase.
///
/// The first phase moves all transitions to their correct position in the
/// out-slices and block_bunch-slices, but it doesn't yet create a fully
/// correct new out-slice and block_bunch-slice.  It adapts current_out_slice
/// of all states with a transition in the new bunch.
/// \param action_block_iter_iter     transition that has to be changed
/// \param first_transition_of_state  This is the first transition of the
///                                   state, so a new bunch_slice has to be
///                                   allocated.
void part_trans_t::first_move_transition_to_new_bunch(action_block_iter_t const
                       action_block_iter, bool const first_transition_of_state)
{

    /*  -  -  -  -  -  -  -  -  adapt part_tr.succ  -  -  -  -  -  -  -  -  */

    // adapt the outgoing transition array:
    // move the transition to the end
    succ_iter_t const old_succ_pos = action_block_iter->succ();
    succ_iter_t out_slice_begin = old_succ_pos->out_slice_begin();
    succ_iter_t const new_succ_pos = out_slice_begin->begin_or_before_end();    assert(out_slice_begin == new_succ_pos->begin_or_before_end());
                                                                                assert(new_succ_pos<old_succ_pos->block_bunch->pred->source->succ_inert.begin);
    // move the transition to the end of its out-slice
    if (old_succ_pos < new_succ_pos)
    {
        std::swap(old_succ_pos->block_bunch, new_succ_pos->block_bunch);
        old_succ_pos->block_bunch->pred->action_block->succ.set(old_succ_pos);  assert(action_block_iter == new_succ_pos->block_bunch->pred->action_block);
        action_block_iter->succ.set(new_succ_pos);
    }                                                                           else  assert(old_succ_pos == new_succ_pos);

    // adapt the old out-slice immediately
        // If the old out-slice becomes empty, then out_slice_begin ==
        // new_succ_pos, so the two following assignments will assign the same
        // variable.  The second assignment is the relevant one.
    out_slice_begin->begin_or_before_end.set(new_succ_pos - 1);

    // adapt the new out-slice, as far as is possible now:
        // make the begin_or_before_end pointers of the first and last
        // transition in the slice correct immediately.  The other
        // begin_or_before_end pointers need to be corrected after all
        // transitions in the new bunch have been positioned correctly.
    if (first_transition_of_state)
    {
        new_succ_pos->begin_or_before_end.set(new_succ_pos);
        new_succ_pos->block_bunch->pred->source->current_out_slice =
                                                               out_slice_begin;
    }
    else
    {
        succ_iter_t out_slice_before_end =
                                        new_succ_pos[1].out_slice_before_end(); assert(new_succ_pos < out_slice_before_end);
                                                                                assert(out_slice_before_end <
                                                                                                    new_succ_pos->block_bunch->pred->source->succ_inert.begin);
                                                                                assert(new_succ_pos+1 == out_slice_before_end->begin_or_before_end());
        out_slice_before_end->begin_or_before_end.set(new_succ_pos);            assert(out_slice_begin ==
                                                                                                   new_succ_pos->block_bunch->pred->source->current_out_slice);
        new_succ_pos->begin_or_before_end.set(out_slice_before_end);            assert(out_slice_before_end->bunch() == new_succ_pos->bunch());
    }

    /*  -  -  -  -  -  -  -  adapt part_tr.block_bunch  -  -  -  -  -  -  -  */
                                                                                assert(unstable_block_bunch.empty());
    block_bunch_iter_t const old_block_bunch_pos = new_succ_pos->block_bunch;   assert(new_succ_pos == action_block_iter->succ());
    block_bunch_slice_iter_t const old_block_bunch_slice =
                                                  old_block_bunch_pos->slice();
    block_bunch_iter_t const new_block_bunch_pos=old_block_bunch_slice->end-1;

    // move the transition to the end of its block_bunch-slice
    if (old_block_bunch_pos < new_block_bunch_pos)
    {
        std::swap(old_block_bunch_pos->pred, new_block_bunch_pos->pred);
        old_block_bunch_pos->pred->action_block->succ()->block_bunch =
                                                           old_block_bunch_pos; assert(new_succ_pos == new_block_bunch_pos->pred->action_block->succ());
        new_succ_pos->block_bunch = new_block_bunch_pos;
    }                                                                           else  assert(new_block_bunch_pos == old_block_bunch_pos);

    // adapt the old block_bunch-slice
    old_block_bunch_slice->end = new_block_bunch_pos;

    // (create and) adapt the new block_bunch-slice
        // All this work is done in the second phase.
}


/// \brief transition is moved to a new bunch, phase 2
/// \details This (and the previous function) have to be called after a
/// transition has changed its bunch.  The member function will adapt the
/// transition data structure.  It assumes that the transition is non-inert.
///
/// The work has to be done in two steps: We call the first step
/// first_move_transition_to_new_bunch() for each transition in the new bunch,
/// and then call second_move_transition_to_new_bunch() again for all these
/// transitions.  The reason is that some data structures need to be finalised
/// in the second phase.
///
/// The second phase finalizes the new out-slices and block_bunch-slices
/// that were left half-finished by the first phase.  It assumes that all
/// block_bunch-slices are registered as stable.
void part_trans_t::second_move_transition_to_new_bunch(
         action_block_iter_t const action_block_iter, bunch_t* const new_bunch)
{

    /*  -  -  -  -  -  -  -  -  adapt part_tr.succ  -  -  -  -  -  -  -  -  */

    succ_iter_t const new_succ_pos = action_block_iter->succ();
    succ_iter_t const new_begin_or_before_end =
                    new_succ_pos->begin_or_before_end()->begin_or_before_end();
    state_info_ptr const source = new_succ_pos->block_bunch->pred->source;

    if (new_begin_or_before_end < new_succ_pos)
    {
        new_succ_pos->begin_or_before_end.set(new_begin_or_before_end);
    }
                                                                                #ifndef NDEBUG
                                                                                else
                                                                                {   assert(new_begin_or_before_end == new_succ_pos);
                                                                                    succ_const_iter_t new_before_end =
                                                                                                                new_begin_or_before_end->begin_or_before_end();
                                                                                    if (new_begin_or_before_end <= new_before_end)
                                                                                    {   assert(source->current_out_slice <= new_begin_or_before_end);
                                                                                        // This is the first transition in the new out-slice.  Test
                                                                                        // whether it is sorted according to bunch order.
                                                                                        assert(new_begin_or_before_end == source->succ.begin ||
                                                                                                            *new_begin_or_before_end[-1].bunch() < *new_bunch);
                                                                                        assert(new_before_end + 1 == source->succ_inert.begin ||
                                                                                                                      *new_bunch < *new_before_end[1].bunch());
    /*  -  -  -  -  -  -  -  adapt part_tr.block_bunch  -  -  -  -  -  -  -  */     }
                                                                                }
                                                                                #endif
    block_bunch_iter_t const new_block_bunch_pos = new_succ_pos->block_bunch;
    block_bunch_slice_iter_t const old_block_bunch_slice =
                                                  new_block_bunch_pos->slice();
                                                                                #ifndef NDEBUG
                                                                                if (new_bunch == old_block_bunch_slice->bunch())
                                                                                {   // the block_bunch-slice has been updated in an earlier call.
                                                                                    assert(new_block_bunch_pos + 1 < old_block_bunch_slice->end);
                                                                                    assert(old_block_bunch_slice == new_block_bunch_pos[1].slice());
                                                                                }
                                                                                else
                                                                                {   assert(*old_block_bunch_slice->bunch() < *new_bunch);
                                                                                    assert(old_block_bunch_slice->end <= new_block_bunch_pos);
                                                                                }
                                                                                #endif
    if (new_block_bunch_pos + 1 < block_bunch_inert_begin &&
                       old_block_bunch_slice == new_block_bunch_pos[1].slice())
    {
        // This transition is not the last in the block_bunch-slice.
        return;
    }
                                                                                assert(old_block_bunch_slice->is_stable());
    // This transition is the last in the block_bunch-slice.  We do all
    // necessary work for this block_bunch-slice upon finding this
    // transition.  Because every block_bunch-slice has exactly one such
    // transition, all work is executed exactly once.
    if (old_block_bunch_slice->empty())
    {
        // The old block_bunch-slice is empty, reuse it as the new one.
        old_block_bunch_slice->end = new_block_bunch_pos + 1;
        old_block_bunch_slice->bunch = new_bunch;

        // Because now every bottom state has a transition in the new bunch,
        // and no state has a transition in the old bunch, there is no need
        // to refine this block.  So we don't make this block_bunch-slice
        // unstable.

        // unmark the states
        source->block->marked_nonbottom_begin =source->block->end;
        source->block->marked_bottom_begin = source->block->nonbottom_begin;

        return;
    }

    // Create a new block_bunch-slice and set the pointers accordingly.
    block_bunch_slice_iter_t new_block_bunch_slice;
    if (1 >= source->block->size())
    {
        // the new block_bunch-slice is stable.  Insert at the appropriate
        // position in the list of stable block_bunch-slices:  immediately
        // after the old block_bunch-slice.
        new_block_bunch_slice = source->block->stable_block_bunch.
                    emplace(std::next(old_block_bunch_slice),
                                     new_block_bunch_pos + 1, new_bunch, true);

        // set the pointers accordingly
        block_bunch_iter_t block_bunch_iter = old_block_bunch_slice->end;
        do
        {                                                                       assert(block_bunch_iter->slice() == old_block_bunch_slice);
            block_bunch_iter->slice.set(new_block_bunch_slice);
        }
        while (++block_bunch_iter <= new_block_bunch_pos);
                                                                                assert(source->block->marked_nonbottom_begin == source->block->end);
        // unmark the state
        source->block->marked_bottom_begin = source->block->nonbottom_begin;

        return;
    }

    // The old block_bunch-slice becomes unstable, and the new
    // block_bunch-slice is created unstable.

    // Move the old block_bunch-slice to the unstable slices.
    unstable_block_bunch.splice(unstable_block_bunch.begin(),
                     source->block->stable_block_bunch, old_block_bunch_slice);
    old_block_bunch_slice->make_unstable();
    unstable_block_bunch.emplace_front(new_block_bunch_pos+1, new_bunch,false);
    new_block_bunch_slice = unstable_block_bunch.begin();

    // set the pointers accordingly
    block_bunch_iter_t block_bunch_iter = old_block_bunch_slice->end;
    do
    {                                                                           assert(block_bunch_iter->slice() == old_block_bunch_slice);
        block_bunch_iter->slice.set(new_block_bunch_slice);
    }
    while (++block_bunch_iter <= new_block_bunch_pos);
}


/// \brief Adapt the non-inert transitions in an out-slice to a new block
/// \details After a block has been split, the outgoing transitions of the
/// new block need to move to the respective block_bunch-slice of the new
/// block.
///
/// This function handles all transitions in the out-slice just before
/// `out_slice_end`, as they all belong to the same block_bunch-slice and can
/// be moved together.
///
/// The function returns the beginning of this out-slice (which can become the
/// next out_slice_end).  It is meant to be called from the last out-slice back
/// to the first because it inserts stable block_bunch-slices at the beginning
/// of the list for the new block, so it would normally become ordered
/// according to the bunch.
succ_iter_t part_trans_t::move_out_slice_to_new_block(
                        succ_iter_t out_slice_end, block_t* const old_block,
                            block_bunch_slice_const_iter_t const last_splitter)
{
    state_info_ptr const source = out_slice_end[-1].block_bunch->pred->source;  assert(out_slice_end <= source->succ_inert.begin);
    succ_iter_t const out_slice_begin=out_slice_end[-1].begin_or_before_end();  assert(out_slice_begin < out_slice_end);
    block_t* new_block = source->block;                                         assert(source->succ.begin <= out_slice_begin);

    block_bunch_iter_t old_block_bunch_pos = out_slice_end[-1].block_bunch;
    block_bunch_slice_iter_t old_block_bunch_slice =
                                                  old_block_bunch_pos->slice();
    if (last_splitter == old_block_bunch_slice)  return out_slice_begin;

    block_bunch_iter_t after_new_block_bunch_pos = old_block_bunch_slice->end;
    block_bunch_slice_iter_t new_block_bunch_slice;
    if (after_new_block_bunch_pos >= block_bunch_inert_begin ||
           (new_block_bunch_slice = after_new_block_bunch_pos->slice(),
            new_block != after_new_block_bunch_pos->pred->source->block ||
             old_block_bunch_slice->bunch() != new_block_bunch_slice->bunch()))
    {
        // the new block_bunch-slice is not suitable; create a new one
        // and insert it into the correct list.
        if (old_block_bunch_slice->is_stable())
        {
            // In most cases, but not always, the source is a bottom state.
            new_block->stable_block_bunch.emplace_front(
               old_block_bunch_slice->end,old_block_bunch_slice->bunch(),true);
            new_block_bunch_slice = new_block->stable_block_bunch.begin();
        }
        else
        {
            new_block_bunch_slice = unstable_block_bunch.emplace(
               std::next(old_block_bunch_slice), old_block_bunch_slice->end,
                                        old_block_bunch_slice->bunch(), false);
        }
    }

    // move all transitions in this out-slice to the new block_bunch
    do
    {
        --out_slice_end;
        --after_new_block_bunch_pos;                                            assert(old_block_bunch_pos->slice()->bunch()==old_block_bunch_slice->bunch());
        after_new_block_bunch_pos->slice.set(new_block_bunch_slice);
        std::swap(old_block_bunch_pos->pred, after_new_block_bunch_pos->pred);
        old_block_bunch_pos->pred->action_block->succ()->block_bunch =
                                                           old_block_bunch_pos; assert(out_slice_end == after_new_block_bunch_pos->pred->action_block->succ());
        out_slice_end->block_bunch = after_new_block_bunch_pos;
    }
    while (out_slice_begin < out_slice_end &&
                  (old_block_bunch_pos = out_slice_end[-1].block_bunch, true));
    old_block_bunch_slice->end = after_new_block_bunch_pos;
    if (old_block_bunch_slice->empty())
    {
        if (old_block_bunch_slice->is_stable())
        {
            // If the new block is red, then the old (blue) block loses exactly
            // one stable block_bunch-slice, namely the one we just stabilised
            // for (last_splitter).  We could perhaps optimize this by moving
            // that slice as a whole to the new block -- perhaps later.
            //
            // If the new block is blue, then the old (red) block loses no
            // stable block_bunch-slices if it contains any bottom state.  If
            // it doesn't contain any bottom state, it will definitely keep
            // last_splitter, but nothing else can be guaranteed.
            //
            // So old_block_bunch_slice may be deleted, in particular if the
            // new block is blue, but not exclusively.
            old_block->stable_block_bunch.erase(old_block_bunch_slice);
        }
        else
        {                                                                       assert(unstable_block_bunch_postprocess_end != old_block_bunch_slice);
            unstable_block_bunch.erase(old_block_bunch_slice);
        }
    }
    return out_slice_begin;
}


/// \brief handle one transition after a block has been split
/// \details The main task of this method is to move the transition to the
/// correct place in the action_block slice.
///
/// This function handles phase 1.  Because the new action_block-slice cannot
/// be adapted completely until all transitions into the new block have been
/// handled through phase 1, the next function handles them again in phase 2.
void part_trans_t::first_move_transition_to_new_action_block(pred_iter_t const
                                                                     pred_iter)
{
    action_block_iter_t const old_action_block_pos = pred_iter->action_block;
    action_block_iter_t const action_block_slice_begin =
                              old_action_block_pos->action_block_slice_begin();
    action_block_iter_t const new_action_block_pos =
                               action_block_slice_begin->begin_or_before_end(); assert(action_block_slice_begin==new_action_block_pos->begin_or_before_end());

    // move the transition to the end of the action_block-slice
    if (old_action_block_pos < new_action_block_pos)
    {
        std::swap(old_action_block_pos->succ, new_action_block_pos->succ);
        old_action_block_pos->succ()->block_bunch->pred->action_block =
                                                          old_action_block_pos; assert(pred_iter == new_action_block_pos->succ()->block_bunch->pred);
        pred_iter->action_block = new_action_block_pos;
    }                                                                           else  assert(old_action_block_pos == new_action_block_pos);

    // adapt the old action_block-slice immediately
        // If the old action_block-slice becomes empty, then
        // action_block_slice_begin == new_action_block_pos, so the two
        // following assignments will assign the same variable.  The second
        // assignment is the relevant one.
    action_block_slice_begin->begin_or_before_end.set(new_action_block_pos-1);
                                                                                assert(pred_iter == new_action_block_pos->succ()->block_bunch->pred);
    // adapt the new action_block-slice, as far as is possible now
        // make the begin_or_before_end pointers of the first and last
        // transition in the slice correct immediately.  The other
        // begin_or_before_end pointers need to be corrected after all
        // transitions in the new bunch have been positioned correctly.
    if (new_action_block_pos + 1 >= action_block_inert_begin ||
        new_action_block_pos[1].succ.is_null() ||
        new_action_block_pos[1].succ()->bunch() !=
                                       new_action_block_pos->succ()->bunch() ||
        new_action_block_pos[1].succ()->block_bunch->pred->target->block !=
                                                      pred_iter->target->block)
    {
        // This is the first transition that moves to this new
        // action_block-slice.
        new_action_block_pos->begin_or_before_end.set(new_action_block_pos);
    }
    else
    {
        action_block_iter_t const action_block_slice_before_end =
                                 new_action_block_pos[1].begin_or_before_end(); assert(new_action_block_pos < action_block_slice_before_end);
                                                                                assert(new_action_block_pos + 1 ==
                                                                                                         action_block_slice_before_end->begin_or_before_end());
        action_block_slice_before_end->begin_or_before_end.set(
                                                         new_action_block_pos); assert(action_block_slice_before_end->succ()->block_bunch->
                                                                                                              pred->target->block == pred_iter->target->block);
        new_action_block_pos->begin_or_before_end.set(
                                                action_block_slice_before_end); assert(action_block_slice_before_end < action_block_inert_begin);
    }
}


/// \brief handle one transition after a block has been split, phase 2
/// \details Because the new action_block-slice cannot be adapted completely
/// until all transitions into the new block have been handled through phase 1
/// (see the previous function), this function handles them again in phase 2.
void part_trans_t::second_move_transition_to_new_action_block(pred_iter_t const
                                                                     pred_iter)
{
    action_block_iter_t const new_action_block_pos = pred_iter->action_block;
    action_block_iter_t const old_begin_or_before_end =
                                   new_action_block_pos->begin_or_before_end();
    action_block_iter_t const new_begin_or_before_end =
                                old_begin_or_before_end->begin_or_before_end();
    if (new_begin_or_before_end < new_action_block_pos)
    {                                                                           assert(old_begin_or_before_end ==
                                                                                                               new_begin_or_before_end->begin_or_before_end());
        new_action_block_pos->begin_or_before_end.set(new_begin_or_before_end); assert(new_action_block_pos <= old_begin_or_before_end);
        return;
    }                                                                           assert(new_begin_or_before_end == new_action_block_pos);
    if (old_begin_or_before_end < new_action_block_pos)  return;

    // this is the first transition in the new action_block-slice.
    // Check whether the bunch it belongs to has become nontrivial.
    bisim_dnj::bunch_t* bunch = new_action_block_pos->succ()->bunch();
    if (!bunch->is_trivial())  return;                                          assert(old_begin_or_before_end + 1 == bunch->end);
    if (bunch->begin < new_action_block_pos)
    {
        bunch->make_nontrivial();
    }
}


/// \brief adapt data structures for a transition that has become non-inert
/// \details If the action_block-slice and the block_bunch-slice that precede
/// the inert transitions in the respective arrays fit, the transition is added
/// to these arrays instead of creating a new one.  This only works if:
/// - the action_block-slice has the same target block and the same action as
///   old_pred_pos
/// - the block_bunch-slice has the same source block as old_pred_pos
/// - the bunch must contain the action_block-slice.
/// If only the last two conditions are fulfilled, we can start a new
/// action_block-slice in the same bunch.  (It would be best for this if the
/// red subblock's block_bunch-slice would be the new one, because that would
/// generally allow to add the new non-inert transitions to the last splitter.)
///
/// The state is only marked if is becomes a new bottom state.  Otherwise, the
/// marking/unmarking of the state is unchanged.
/// \returns true iff the state became a new bottom state
bool part_trans_t::make_noninert(pred_iter_t const old_pred_pos,
iterator_or_null<block_bunch_slice_iter_t>* const new_noninert_block_bunch_ptr)
{
    action_block_iter_t const old_action_block_pos=old_pred_pos->action_block;
    succ_iter_t const old_succ_pos = old_action_block_pos->succ();
    block_bunch_iter_t const old_block_bunch_pos = old_succ_pos->block_bunch;   assert(old_pred_pos == old_block_bunch_pos->pred);

    state_info_ptr const source = old_pred_pos->source;
    state_info_ptr const target = old_pred_pos->target;
    block_t* const source_block = source->block;                                assert(source_block->nonbottom_begin <= source->pos);

    pred_iter_t const new_pred_pos = target->pred_inert.begin++;
    action_block_iter_t new_action_block_pos = action_block_inert_begin++;
    succ_iter_t const new_succ_pos = source->succ_inert.begin++;
    block_bunch_iter_t const new_block_bunch_pos = block_bunch_inert_begin++;

    // adapt pred
    if (new_pred_pos < old_pred_pos)
    {
        *old_pred_pos = *new_pred_pos;                                          assert(old_pred_pos->action_block->succ()->block_bunch->pred == new_pred_pos);
        old_pred_pos->action_block->succ()->block_bunch->pred = old_pred_pos;
        new_pred_pos->source = source;
        new_pred_pos->target = target;
    }                                                                           else  assert(new_pred_pos == old_pred_pos);
    new_pred_pos->action_block = new_action_block_pos;

    /* adapt action_block */                                                    assert(new_action_block_pos->begin_or_before_end.is_null());
    if (new_action_block_pos < old_action_block_pos)
    {                                                                           assert(old_action_block_pos->begin_or_before_end.is_null());
        old_action_block_pos->succ = new_action_block_pos->succ;                assert(old_action_block_pos->succ()->block_bunch->pred->action_block ==
                                                                                                                                         new_action_block_pos);
        old_action_block_pos->succ()->block_bunch->pred->action_block =
                                                          old_action_block_pos;
    }                                                                           else  assert(new_action_block_pos == old_action_block_pos);
    new_action_block_pos->succ.set(new_succ_pos);
    // new_action_block_pos->begin_or_before_end.set(...); -- see below

    /* adapt succ */                                                            assert(new_succ_pos->begin_or_before_end.is_null());
    if (new_succ_pos < old_succ_pos)
    {                                                                           assert(old_succ_pos->begin_or_before_end.is_null());
        old_succ_pos->block_bunch = new_succ_pos->block_bunch;                  assert(old_succ_pos->block_bunch->pred->action_block->succ() == new_succ_pos);
        old_succ_pos->block_bunch->pred->action_block->succ.set(old_succ_pos);
    }                                                                           else  assert(new_succ_pos == old_succ_pos);
    new_succ_pos->block_bunch = new_block_bunch_pos;
    // new_succ_pos->begin_or_before_end.set(...); -- see below

    // make the state a bottom state if necessary
    bool became_bottom = false;
    if (source->succ_end() == source->succ_inert.begin)
    {
        // make the state a marked bottom state
        if (source->pos >= source_block->marked_nonbottom_begin)
        {
            iter_swap(source->pos, source_block->marked_nonbottom_begin++);
        }                                                                       assert(source->pos < source->block->marked_nonbottom_begin);
        iter_swap(source->pos, source_block->nonbottom_begin++);
        became_bottom = true;
    }

    /* adapt block_bunch */                                                     assert(new_block_bunch_pos->slice.is_null());
    if (new_block_bunch_pos < old_block_bunch_pos)
    {                                                                           assert(old_block_bunch_pos->slice.is_null());
        old_block_bunch_pos->pred = new_block_bunch_pos->pred;                  assert(old_block_bunch_pos->pred->action_block->succ()->block_bunch ==
                                                                                                                                          new_block_bunch_pos);
        old_block_bunch_pos->pred->action_block->succ()->block_bunch =
                                                           old_block_bunch_pos;
    }                                                                           else  assert(new_block_bunch_pos == old_block_bunch_pos);
    new_block_bunch_pos->pred = new_pred_pos;
    // new_block_bunch_pos->slice = ...; -- see below

    bunch_t* new_noninert_bunch;
    if (!new_noninert_block_bunch_ptr->is_null())
    {
        // There is already some new non-inert transition from this block.
        // So we can reuse this block_bunch and its bunch.
        // (However, it may be the case that the current transition goes to
        // another block; in the latter case, we have to create a new
        // action_block-slice.)

        // extend the bunch
        new_noninert_bunch = (*new_noninert_block_bunch_ptr)()->bunch();        assert(new_action_block_pos == new_noninert_bunch->end ||
                                                                                                        (new_action_block_pos[-1].succ.is_null() &&
                                                                                                         new_action_block_pos - 1 == new_noninert_bunch->end));
        new_noninert_bunch->end = action_block_inert_begin;
                                                                                assert((*new_noninert_block_bunch_ptr)()->is_stable());
        /* extend the block_bunch-slice */                                      assert((*new_noninert_block_bunch_ptr)()->end == new_block_bunch_pos);
        (*new_noninert_block_bunch_ptr)()->end = block_bunch_inert_begin;
        new_block_bunch_pos->slice = *new_noninert_block_bunch_ptr;

        /* adapt the action_block-slice */                                      assert(new_noninert_bunch->begin < new_action_block_pos);
        if (!new_action_block_pos[-1].succ.is_null() && target->block ==
             new_action_block_pos[-1].succ()->block_bunch->pred->target->block)
        {
            // the action_block-slice is suitable: extend it
            action_block_iter_t const action_block_slice_begin =
                                new_action_block_pos[-1].begin_or_before_end(); assert(new_action_block_pos - 1 ==
                                                                                                              action_block_slice_begin->begin_or_before_end());
            action_block_slice_begin->begin_or_before_end.set(
                                                         new_action_block_pos);
            new_action_block_pos->begin_or_before_end.set(
                                                     action_block_slice_begin);
        }
        else
        {
            // create a new action_block-slice
            new_action_block_pos->begin_or_before_end.set(
                                                         new_action_block_pos);
            if (new_noninert_bunch->is_trivial())
            {                                                                   // Only during initialisation, it may happen that we add new non-inert
                new_noninert_bunch->make_nontrivial();                          // transitions to a nontrivial bunch:
            }                                                                   else  assert(action_block.begin() == new_noninert_bunch->begin);
        }

        // adapt the out-slice
        if (source->succ.begin < new_succ_pos &&
                                new_succ_pos[-1].bunch() == new_noninert_bunch)
        {
            // the out-slice is suitable: extend it.
            succ_iter_t out_slice_begin=new_succ_pos[-1].begin_or_before_end(); assert(new_succ_pos - 1 == out_slice_begin->begin_or_before_end());
            out_slice_begin->begin_or_before_end.set(new_succ_pos);
            new_succ_pos->begin_or_before_end.set(out_slice_begin);
            return became_bottom;
        }
    }
    else
    {
        // create a new bunch for noninert transitions
        new_noninert_bunch = new bunch_t(new_action_block_pos,
                                  action_block_inert_begin,
                                  new_action_block_pos - action_block.begin());

        // create a new block_bunch-slice
        // (While it's technically unstable, we immediately stabilise for it
        // afterwards.  So we already insert it in the list of stable slices.)
        block_bunch_slice_iter_t new_noninert_block_bunch = source_block->
             stable_block_bunch.emplace(source_block->stable_block_bunch.end(),
                            block_bunch_inert_begin, new_noninert_bunch, true);
        new_block_bunch_pos->slice.set(new_noninert_block_bunch);
        new_noninert_block_bunch_ptr->set(new_noninert_block_bunch);

        // create a new action_block-slice
        new_action_block_pos->begin_or_before_end.set(new_action_block_pos);
    }                                                                           assert(source->succ.begin == new_succ_pos ||
    /* create a new out-slice */                                                                              *new_succ_pos[-1].bunch() < *new_noninert_bunch);
    new_succ_pos->begin_or_before_end.set(new_succ_pos);
    return became_bottom;
}


/// \brief Split all data structures after a new block has been created
/// \details This function splits the block_bunch- and action_block-slices to
/// reflect that some transitions now start or end in the new block NewB.  They
/// can no longer be in the same slice as the transitions that start or end in
/// the old block, respectively.  It also marks the transitions that have
/// become non-inert as such and finds new bottom states.
///
/// Its time complexity is O(1 + |in(NewB)| + |out(NewB)|).
void part_trans_t::adapt_transitions_for_new_block(block_t* const new_block,
    block_t* const old_block,
        iterator_or_null<block_bunch_slice_iter_t> new_noninert_block_bunch,
                        block_bunch_slice_const_iter_t const last_splitter,
                                    enum new_block_mode_t const new_block_mode)
{                                                                               assert(last_splitter->is_stable());
    // We begin with a bottom state so the new block gets a sorted list of
    // stable block_bunch-slices.
    permutation_iter_t s_iter = new_block->begin;                               assert(s_iter < new_block->end);
    do
    {
        state_info_ptr s = *s_iter;                                             assert(new_block == s->block);

        /*-  -  -  -  -  -  -  adapt part_tr.block_bunch  -  -  -  -  -  -  -*/

        for (succ_iter_t succ = s->succ_inert.begin; s->succ.begin < succ; )
        {
            succ = move_out_slice_to_new_block(succ, old_block, last_splitter);
        }

        /*  -  -  -  -  -  -  adapt part_tr.action_block  -  -  -  -  -  -  */

        for (pred_iter_t pred=s->pred.begin; pred<s->pred_inert.begin; ++pred)
        {
            first_move_transition_to_new_action_block(pred);
        }
    }
    while (++s_iter < new_block->end);
    if (new_block_is_red == new_block_mode)
    {                                                                           assert(last_splitter->source_block() == new_block);
        // The last_splitter slice moves completely from the old to the new
        // block.  We move it as a whole to the new block_bunch list.
        new_block->stable_block_bunch.splice(
                    new_block->stable_block_bunch.begin(),
                                 old_block->stable_block_bunch, last_splitter);
    }                                                                           else assert(last_splitter->source_block() == old_block);

    // We cannot join the loop above with the one below because transitions
    // in the action_block-slices need to be handled in two phases.

    for (permutation_iter_t s_iter = new_block->begin;
                                             s_iter < new_block->end; ++s_iter)
    {
        state_info_ptr s = *s_iter;
        for (pred_iter_t pred=s->pred.begin; pred<s->pred_inert.begin; ++pred)
        {
            second_move_transition_to_new_action_block(pred);
        }
    }

    /*-  -  -  -  -  -  - find new non-inert transitions -  -  -  -  -  -  -*/

    if (block_bunch_inert_begin == block_bunch.end())
    {                                                                           assert(action_block_inert_begin == action_block.end());
        // There are no inert transitions at all, so no inert transition can
        // become non-inert.
        return;
    }                                                                           assert(block_bunch_inert_begin < block_bunch.end());
                                                                                assert(block_bunch.end() - block_bunch_inert_begin ==
                                                                                                                action_block.end() - action_block_inert_begin);
                                                                                assert(0 == new_block->marked_size());
    if (new_block_is_blue == new_block_mode)
    {                                                                           assert(new_block->end < block_t::perm_end);
                                                                                assert(old_block == (*new_block->end)->block);
        permutation_iter_t target_iter = new_block->begin;                      assert(0 == old_block->marked_size());
        do
        {
            state_info_ptr target = *target_iter;
            // check all incoming inert transitions of s, whether they
            // still start in new_block
            for (pred_iter_t pred_iter = target->pred_inert.begin;
                                   pred_iter < target->pred_end(); ++pred_iter)
            {
                state_info_ptr source = pred_iter->source;
                if (new_block != source->block)
                {                                                               assert(old_block == source->block);
                    if (!make_noninert(pred_iter, &new_noninert_block_bunch))
                    {
                        old_block->mark_nonbottom(source);
                    }
                }
            }
        }
        while (++target_iter < new_block->end);                                 assert(0 < old_block->bottom_size());
    }
    else
    {                                                                           assert(new_block_is_red == new_block_mode);
        /* We have to be careful because make_noninert may make a state move */ assert(old_block == new_block->begin[-1]->block);
        /* either forward (to the marked states) or back (to the bottom */      assert(block_t::perm_begin < new_block->begin);
        /* states). */                                                          assert(0 < old_block->bottom_size());  assert(0 == old_block->marked_size());
        for (permutation_iter_t source_iter = new_block->nonbottom_begin;
                             source_iter < new_block->marked_nonbottom_begin; )
        {
            state_info_ptr source = *source_iter;
            // check all outgoing inert transitions of s, whether they still
            // end in new_block.
            bool dont_mark = true;
            succ_iter_t succ_iter = source->succ_inert.begin;                   assert(succ_iter < source->succ_end());
            do
            {
                if (new_block != succ_iter->block_bunch->pred->target->block)
                {                                                               assert(old_block == succ_iter->block_bunch->pred->target->block);
                    // the transition becomes non-inert
                    dont_mark = make_noninert(succ_iter->block_bunch->pred,
                                                    &new_noninert_block_bunch);
                }
            }
            while (++succ_iter < source->succ_end());
            if (dont_mark)  ++source_iter;
            else  new_block->mark_nonbottom(source);                            assert(new_block->nonbottom_begin <= source_iter);
        }
    }
}


} // end namespace bisim_dnj
                                                                                #ifndef NDEBUG
                                                                                /// \brief print all transitions
                                                                                /// \details Transitions are printed organised into bunches.
                                                                                template <class LTS_TYPE>
                                                                                void bisim_dnj::part_trans_t::print_trans(
                                                                                                      const bisim_partitioner_dnj<LTS_TYPE>& partitioner) const
                                                                                {
                                                                                    // for all bunches
                                                                                    action_block_const_iter_t bunch_begin = action_block.begin();
                                                                                    do
                                                                                    {
                                                                                        if (bunch_begin >= action_block.end())
                                                                                        {
                                                                                            mCRL2log(log::debug, "bisim_dnj") << "No transitions.\n";
                                                                                            return;
                                                                                        }
                                                                                    } while (bunch_begin->succ.is_null() &&
                                                                                       (assert(bunch_begin->begin_or_before_end.is_null()),
                                                                                        ++bunch_begin,                                      true));
                                                                                    do
                                                                                    {
                                                                                        action_block_const_iter_t bunch_end;
                                                                                        action_block_const_iter_t action_block_slice_end;
                                                                                        if (bunch_begin->succ()->block_bunch->slice.is_null())
                                                                                        {
                                                                                            mCRL2log(log::debug, "bisim_dnj") <<"Inert transition slice [";
                                                                                            action_block_slice_end = bunch_end = action_block.end();
                                                                                        }
                                                                                        else
                                                                                        {
                                                                                            const bunch_t* const bunch = bunch_begin->succ()->bunch();
                                                                                            mCRL2log(log::debug, "bisim_dnj") << bunch->debug_id_short()
                                                                                                                                << ":\n\taction_block-slice [";
                                                                                            bunch_end = bunch->end;
                                                                                            assert(bunch->begin == bunch_begin);
                                                                                            assert(bunch_begin < bunch_end);
                                                                                            assert(bunch_end <= action_block_inert_begin);

                                                                                            action_block_slice_end = bunch_begin->begin_or_before_end() + 1;
                                                                                        }
                                                                                        assert(action_block_slice_end <= bunch_end);
                                                                                        // for all transition sets in Bu
                                                                                        action_block_const_iter_t action_block_slice_begin = bunch_begin;
                                                                                        do
                                                                                        {
                                                                                            mCRL2log(log::debug,"bisim_dnj") << (action_block_slice_begin -
                                                                                                                 action_block_entry::action_block_begin) << ","
                                                                                                << (action_block_slice_end -
                                                                                                             action_block_entry::action_block_begin) << "):\n";
                                                                                            // for all transitions in Tr
                                                                                            for(action_block_const_iter_t tr_iter=action_block_slice_begin;
                                                                                                                   tr_iter < action_block_slice_end; ++tr_iter)
                                                                                            {
                                                                                                mCRL2log(log::debug, "bisim_dnj") << "\t\t"
                                                                                                            << tr_iter->succ()->block_bunch->
                                                                                                                           pred->debug_id(partitioner) << '\n';
                                                                                            }
                                                                                            // go to next transition set
                                                                                            action_block_slice_begin = action_block_slice_end;
                                                                                            while (action_block_slice_begin < bunch_end &&
                                                                                                                      action_block_slice_begin->succ.is_null())
                                                                                            {
                                                                                                assert(action_block_slice_begin->
                                                                                                                                begin_or_before_end.is_null());
                                                                                                ++action_block_slice_begin;
                                                                                                assert(action_block_slice_begin < bunch_end);
                                                                                            }
                                                                                            if (action_block_slice_begin >= bunch_end)  break;
                                                                                            action_block_slice_end =
                                                                                                           action_block_slice_begin->begin_or_before_end() + 1;
                                                                                            mCRL2log(log::debug, "bisim_dnj") << "\taction_block-slice [";
                                                                                        }
                                                                                        while (true);
                                                                                        // go to next bunch
                                                                                        bunch_begin = bunch_end;
                                                                                        while (bunch_begin < action_block.end() && bunch_begin->succ.is_null())
                                                                                        {
                                                                                            assert(bunch_begin->begin_or_before_end.is_null());
                                                                                            ++bunch_begin;
                                                                                        }
                                                                                    }
                                                                                    while (bunch_begin < action_block.end());
                                                                                }

                                                                                /// \brief assert that the data structure is consistent and stable
                                                                                /// \details The data structure is tested against a large number of
                                                                                /// assertions to ensure that everything is consistent, e. g. pointers that
                                                                                /// should point to successors of state s actually point to a transition
                                                                                /// that starts in s.
                                                                                ///
                                                                                /// Additionally, it is asserted that the partition is stable. i. e. every
                                                                                /// bottom state in every block can reach exactly every bunch in the list
                                                                                /// of bunches that should be reachable from it, and every nonbottom state
                                                                                /// can reach a subset of them.
                                                                                template<class LTS_TYPE>
                                                                                void bisim_partitioner_dnj<LTS_TYPE>::assert_stability() const
                                                                                {
                                                                                    part_st.assert_consistency(branching);

                                                                                    assert(part_tr.succ.size() == part_tr.block_bunch.size() + 1);
                                                                                    assert(part_tr.pred.size() == part_tr.block_bunch.size() + 1);
                                                                                    assert(part_tr.action_block.size() ==
                                                                                                         part_tr.block_bunch.size() + action_label.size() - 1);
                                                                                    if (part_tr.block_bunch.empty())  return;

                                                                                    assert(part_tr.unstable_block_bunch.empty());
                                                                                    for (bisim_dnj::block_bunch_slice_const_iter_t block_bunch =
                                                                                                                          part_tr.unstable_block_bunch.begin();
                                                                                              part_tr.unstable_block_bunch.end() != block_bunch; ++block_bunch)
                                                                                    {
                                                                                        assert(!block_bunch->is_stable());
                                                                                    }

                                                                                    // for all blocks
                                                                                    for (bisim_dnj::permutation_const_iter_t block_begin =
                                                                                                                part_st.permutation.begin();
                                                                                                                     block_begin < part_st.permutation.end(); )
                                                                                    {
                                                                                        const bisim_dnj::block_t* const block = (*block_begin)->block;
                                                                                        // iterators have no predefined hash, so we store pointers:
                                                                                        std::unordered_set<const bisim_dnj::block_bunch_slice_t*>
                                                                                                                                         block_bunch_check_set;
                                                                                        block_bunch_check_set.reserve(block->stable_block_bunch.size());

                                                                                        // for all stable block_bunch-slices of the block
                                                                                        for (bisim_dnj::block_bunch_slice_const_iter_t block_bunch =
                                                                                                                             block->stable_block_bunch.begin();
                                                                                             block_bunch != block->stable_block_bunch.end(); ++block_bunch)
                                                                                        {
                                                                                            assert(block_bunch->source_block() == block);
                                                                                            assert(block_bunch->is_stable());
                                                                                            block_bunch_check_set.insert(&*block_bunch);
                                                                                        }

                                                                                        // for all states in the block
                                                                                        for (bisim_dnj::permutation_const_iter_t perm_begin = block->begin;
                                                                                                                         perm_begin < block->end; ++perm_begin)
                                                                                        {
                                                                                            bisim_dnj::state_info_const_ptr const state = *perm_begin;
                                                                                            bisim_dnj::succ_const_iter_t out_slice_begin = state->succ.begin;
                                                                                            trans_type block_bunch_count = 0;
                                                                                            // for all out-slices of the state
                                                                                            if (out_slice_begin < state->succ_inert.begin)
                                                                                            {
                                                                                                const bisim_dnj::bunch_t* bunch;
                                                                                                bisim_dnj::succ_const_iter_t out_slice_end;
                                                                                                do
                                                                                                {
                                                                                                    bisim_dnj::block_bunch_slice_const_iter_t
                                                                                                                    block_bunch_slice =
                                                                                                                         out_slice_begin->block_bunch->slice();
                                                                                                    bunch = block_bunch_slice->bunch();
                                                                                                    assert(block == block_bunch_slice->source_block());
                                                                                                    if (block_bunch_slice->is_stable())
                                                                                                    {
                                                                                                        assert(1 == block_bunch_check_set.count(
                                                                                                                                         &*block_bunch_slice));
                                                                                                        ++block_bunch_count;
                                                                                                    }
                                                                                                    else  assert(0); //< all block_bunch-slices should be
                                                                                                                     //  stable
                                                                                                    out_slice_end = out_slice_begin->begin_or_before_end() + 1;
                                                                                                    assert(out_slice_begin < out_slice_end);

                                                                                                    // for all transitions in the out-slice
                                                                                                    bisim_dnj::succ_const_iter_t succ_iter = out_slice_end;
                                                                                                    do
                                                                                                    {
                                                                                                        --succ_iter;
                                                                                                        if (1 != block_bunch_slice->end-succ_iter->block_bunch)
                                                                                                        {
                                                                                                            assert(0 < block_bunch_slice->end -
                                                                                                                                       succ_iter->block_bunch);
                                                                                                            assert(block_bunch_slice ==
                                                                                                                            succ_iter->block_bunch[1].slice());
                                                                                                        }
                                                                                                        assert(bunch->begin <=
                                                                                                                   succ_iter->block_bunch->pred->action_block);
                                                                                                        assert(succ_iter->block_bunch->pred->action_block <
                                                                                                                                                   bunch->end);
                                                                                                        assert(block_bunch_slice ==
                                                                                                                              succ_iter->block_bunch->slice());
                                                                                                    }
                                                                                                    while (out_slice_begin < succ_iter &&
                                                                                                           (assert(out_slice_begin ==
                                                                                                                      succ_iter->begin_or_before_end()),
                                                                                                            assert(succ_iter!=state->current_out_slice),true));
                                                                                                }
                                                                                                while (out_slice_end < state->succ_inert.begin &&
                                                                                                   (assert(bunch->end-bunch->begin+bunch->sort_id <=
                                                                                                       out_slice_end->block_bunch->slice()->bunch()->sort_id),
                                                                                                                       out_slice_begin = out_slice_end, true));
                                                                                                assert(out_slice_end == state->succ_inert.begin);
                                                                                            }
                                                                                            if (perm_begin < block->nonbottom_begin)
                                                                                            {
                                                                                                assert(block_bunch_check_set.size() == block_bunch_count);
                                                                                            }
                                                                                        }
                                                                                        block_begin = block->end;
                                                                                    }
                                                                                    assert(part_tr.action_block.begin() <= part_tr.action_block_inert_begin);
                                                                                    assert(part_tr.block_bunch.begin() <= part_tr.block_bunch_inert_begin);
                                                                                    if (branching)
                                                                                    {
                                                                                        assert(part_tr.action_block_inert_begin <= part_tr.action_block.end());
                                                                                        assert(part_tr.block_bunch_inert_begin<=part_tr.block_bunch.end());
                                                                                        assert(part_tr.block_bunch.end()-part_tr.block_bunch_inert_begin ==
                                                                                                part_tr.action_block.end() - part_tr.action_block_inert_begin);

                                                                                        // for all inert transitions
                                                                                        for (bisim_dnj::action_block_const_iter_t action_block =
                                                                                                                              part_tr.action_block_inert_begin;
                                                                                                     action_block < part_tr.action_block.end(); ++action_block)
                                                                                        {
                                                                                            assert(action_block->begin_or_before_end.is_null());
                                                                                            assert(action_block->succ()->block_bunch->slice.is_null());
                                                                                            bisim_dnj::pred_const_iter_t const pred_iter =
                                                                                                                       action_block->succ()->block_bunch->pred;
                                                                                            assert(action_block == pred_iter->action_block);
                                                                                            assert(part_tr.block_bunch_inert_begin <=
                                                                                                                            action_block->succ()->block_bunch);
                                                                                            assert(pred_iter->source != pred_iter->target);
                                                                                            assert(pred_iter->source->block == pred_iter->target->block);
                                                                                            assert(pred_iter->source->succ_inert.begin<=action_block->succ());
                                                                                            assert(action_block->succ() < pred_iter->source->succ_cend());
                                                                                            assert(pred_iter->target->pred_inert.begin <= pred_iter);
                                                                                            assert(pred_iter < pred_iter->target->pred_cend());
                                                                                        }
                                                                                    }
                                                                                    else
                                                                                    {
                                                                                        assert(!preserve_divergence);
                                                                                        assert(part_tr.action_block_inert_begin == part_tr.action_block.end());
                                                                                        assert(part_tr.block_bunch_inert_begin == part_tr.block_bunch.end());
                                                                                    }
                                                                                    bisim_dnj::action_block_const_iter_t action_slice_end =
                                                                                                                              part_tr.action_block_inert_begin;
                                                                                    // for all action labels and bunches
                                                                                    label_type label = 0;
                                                                                    const bisim_dnj::bunch_t* previous_bunch = nullptr;
                                                                                    do
                                                                                    {        
                                                                                        assert(part_tr.action_block.begin() <= action_label[label].begin);
                                                                                        assert(action_label[label].begin <= action_slice_end);
                                                                                        assert(action_slice_end <= part_tr.action_block_inert_begin);
                                                                                        // for all action_block slices
                                                                                        for (bisim_dnj::action_block_const_iter_t action_block_slice_end =
                                                                                                                                             action_slice_end;
                                                                                                          action_label[label].begin < action_block_slice_end; )
                                                                                        {
                                                                                            bisim_dnj::action_block_const_iter_t const
                                                                                                    action_block_slice_begin =
                                                                                                              action_block_slice_end[-1].begin_or_before_end();
                                                                                            assert(action_block_slice_begin < action_block_slice_end);
                                                                                            assert(action_block_slice_end <= action_slice_end);
                                                                                            const bisim_dnj::block_t* const target_block =
                                                                                                                action_block_slice_begin->succ()->
                                                                                                                              block_bunch->pred->target->block;
                                                                                            const bisim_dnj::bunch_t* const bunch = action_block_slice_begin->
                                                                                                                         succ()->block_bunch->slice()->bunch();
                                                                                            if (previous_bunch != bunch)
                                                                                            {
                                                                                                assert(nullptr == previous_bunch);
                                                                                                previous_bunch = bunch;
                                                                                                assert(bunch->end == action_block_slice_end);
                                                                                                if (bunch->begin == action_block_slice_begin)
                                                                                                {
                                                                                                    // Perhaps this does not always hold; sometimes, an
                                                                                                    // action_block slice disappears but the bunch cannot be
                                                                                                    // made trivial.
                                                                                                    assert(bunch->is_trivial());
                                                                                                }
                                                                                                else  assert(!bunch->is_trivial());
                                                                                            }
                                                                                            if(bunch->begin==action_block_slice_begin)  previous_bunch=nullptr;
                                                                                            else  assert(bunch->begin < action_block_slice_begin);

                                                                                            assert(action_block_slice_begin->begin_or_before_end() + 1 ==
                                                                                                                                       action_block_slice_end);
                                                                                            // for all transitions in the action_block slice
                                                                                            bisim_dnj::action_block_const_iter_t action_block =
                                                                                                                                        action_block_slice_end;
                                                                                            do
                                                                                            {
                                                                                                --action_block;
                                                                                                bisim_dnj::pred_const_iter_t const pred_iter =
                                                                                                                       action_block->succ()->block_bunch->pred;
                                                                                                assert(action_block == pred_iter->action_block);
                                                                                                assert(action_block->succ()->block_bunch <
                                                                                                                              part_tr.block_bunch_inert_begin);
                                                                                                assert(!branching || !aut.is_tau(label) ||
                                                                                                    pred_iter->source->block != pred_iter->target->block ||
                                                                                                                     (preserve_divergence &&
                                                                                                                      pred_iter->source == pred_iter->target));
                                                                                                assert(pred_iter->source->succ.begin <= action_block->succ());
                                                                                                assert(action_block->succ() <
                                                                                                                          pred_iter->source->succ_inert.begin);
                                                                                                assert(pred_iter->target->pred.begin <= pred_iter);
                                                                                                assert(pred_iter < pred_iter->target->pred_inert.begin);
                                                                                                assert(target_block == pred_iter->target->block);
                                                                                                assert(bunch ==
                                                                                                          action_block->succ()->block_bunch->slice()->bunch());
                                                                                            }
                                                                                            while (action_block_slice_begin < action_block &&
                                                                                               (// some properties only need to be checked for states that
                                                                                                // are not the first one:
/* ************************************************************************* */                 assert(action_block->begin_or_before_end() ==
/*                                                                           */                                              action_block_slice_begin), true));
/*                            A L G O R I T H M S                            */             action_block_slice_end = action_block_slice_begin;
/*                                                                           */         }
/* ************************************************************************* */         if (action_slice_end < part_tr.action_block_inert_begin)
                                                                                        {
                                                                                            // there is a dummy transition between action labels
                                                                                            assert(action_slice_end->succ.is_null());
                                                                                            assert(action_slice_end->begin_or_before_end.is_null());
                                                                                        }
/*===========================================================================*/     }
/*                              initialisation                               */     while (++label < action_label.size() &&
/*===========================================================================*/                  (action_slice_end = action_label[label - 1].begin - 1, true));
                                                                                    assert(nullptr == previous_bunch);
                                                                                }
                                                                                #endif
/// \brief Create a partition satisfying the main invariant
/// \details Before the actual bisimulation minimisation can start, this
/// function needs to be called to create a partition that satisfies the
/// main invariant of the efficient O(m log n) branching bisimulation
/// minimisation.
///
/// It puts all non-inert transitions into a single bunch, containing one
/// action_block-slice for each action label.  It creates a single block
/// (or possibly two, if there are states that never will do any visible
/// action).  As a side effect, it deletes all transitions from the LTS
/// that is stored with the partitioner;  information about the transitions
/// is kept in data structures that are suitable for the efficient
/// algorithm.
///
/// For divergence-preserving branching bisimulation, we only need to treat
/// tau-self-loops as non-inert transitions.  In other texts, this is
/// sometimes described as temporarily renaming the tau-self-loops to
/// self-loops with a special label.  However, as there are no other
/// non-inert tau transitions, we can simply put them in their own
/// action_block-slice, separate from the inert tau transitions.  (It would
/// be an error to mix the inert transitions with the self-loops in the
/// same slice.)
template <class LTS_TYPE>
void bisim_partitioner_dnj<LTS_TYPE>::create_initial_partition()
{
    // log::mcrl2_logger::set_reporting_level(log::debug, "bisim_dnj");

    mCRL2log(log::verbose, "bisim_dnj") << "Strictly O(m log n) "
                   << (branching ? (preserve_divergence
                                           ? "divergence-preserving branching "
                                           : "branching ")
                                 : "")
                 << "bisimulation partitioner created for " << aut.num_states()
                 << " states and " << aut.num_transitions();

    // create one block for all states

    bisim_dnj::block_t* B = new bisim_dnj::block_t(
                       part_st.permutation.begin(), part_st.permutation.end());

    // Iterate over the transitions to count how to order them in part_trans_t

    // counters for the non-inert outgoing and incoming transitions per state
    // are provided in part_st.state_info.  These counters have been
    // initialised to zero in the constructor of part_state_t.
    // counters for the non-inert transition per label are stored in
    // action_label.
                                                                                assert(action_label.size() == aut.num_action_labels());
    // counter for the total number of inert transitions:
    trans_type inert_transitions = 0;
    for (const transition& t: aut.get_transitions())
    {
        if (branching && aut.is_tau(aut.apply_hidden_label_map(t.label())) &&   ((
                                                            t.from() != t.to())   || (assert(preserve_divergence), false)))
        {
            // The transition is inert.
            part_st.state_info[t.from()].succ_inert.count++;
            part_st.state_info[t.to()].pred_inert.count++;
            ++inert_transitions;

            // The source state should become non-bottom:
            if (part_st.state_info[t.from()].pos < B->nonbottom_begin)
            {
                iter_swap(part_st.state_info[t.from()].pos,
                                                         --B->nonbottom_begin);
                // we do not yet update the marked_bottom_begin pointer
            }
        }
        else
        {
            // The transition is non-inert.  (It may be a self-loop).
            part_st.state_info[t.from()].succ.count++;
            part_st.state_info[t.to()].pred.count++;

            ++action_label[aut.apply_hidden_label_map(t.label())].count;
        }
    }
    // Now we update the marked_bottom_begin pointer:
    B->marked_bottom_begin = B->nonbottom_begin;

    // set the pointers to transition slices in the state info entries

    // We set them all to the end of the respective slice here.  Then, with
    // every transition, the pointer will be reduced by one, so that after
    // placing all transitions it will point to the beginning of the slice.

    bisim_dnj::pred_iter_t next_pred_begin = part_tr.pred.begin();
    bisim_dnj::succ_iter_t next_succ_begin = part_tr.succ.begin();
    bisim_dnj::state_info_ptr state_iter;
    for (state_iter = &*part_st.state_info.begin();
                      state_iter < &part_st.state_info.end()[-1]; ++state_iter)
    {
        state_iter->block = B;
        // state_iter->notblue = 0;

        next_pred_begin += state_iter->pred.count;
        state_iter->pred.begin = next_pred_begin;
        next_pred_begin += state_iter->pred_inert.count;
        state_iter->pred_inert.begin = next_pred_begin;

        state_iter->current_out_slice = next_succ_begin;
            // create slice descriptors in part_tr.succ for each state with
            // outgoing transitions.
        state_iter->succ.begin = next_succ_begin + state_iter->succ.count;
        if (next_succ_begin < state_iter->succ.begin)
        {
            next_succ_begin->begin_or_before_end.set(state_iter->succ.begin-1);
            bisim_dnj::succ_iter_t out_slice_begin = next_succ_begin;
            while (++next_succ_begin < state_iter->succ.begin)
            {
                next_succ_begin->begin_or_before_end.set(out_slice_begin);
            }

            // Mark state as a state with a non-inert transition
            B->mark(state_iter);
        }
        state_iter->succ_inert.begin = next_succ_begin +
                                                  state_iter->succ_inert.count;
        while (next_succ_begin < state_iter->succ_inert.begin)
        {
            next_succ_begin->begin_or_before_end.clear();
            ++next_succ_begin;
        }
    }
    state_iter->pred.begin = next_pred_begin;
    state_iter->succ.begin = next_succ_begin;

    // create a single bunch containing all non-inert transitions

    part_tr.action_block_inert_begin =
                                part_tr.action_block.end() - inert_transitions;
    part_tr.block_bunch_inert_begin =
                                 part_tr.block_bunch.end() - inert_transitions;

    bisim_dnj::bunch_t* bunch = nullptr;

    // create a single block_bunch entry for all non-inert transitions
    if (part_tr.action_block.begin() < part_tr.action_block_inert_begin)
    {
        bunch = new bisim_dnj::bunch_t(part_tr.action_block.begin(),
                                          part_tr.action_block_inert_begin, 0);
        B->stable_block_bunch.emplace_front(part_tr.block_bunch_inert_begin,
                                                                  bunch, true);
    }

    // create slice descriptors in part_tr.action_block for each label

    // The action_block array shall have the tau transitions at the end: first
    // the non-inert tau transitions (during initialisation, that are only the
    // tau self-loops), then the tau transitions that have become non-inert
    // and finally the inert transitions.
    // Transitions with other labels are placed from beginning to end.
    // Every such transition block except the last one ends with a dummy entry.
    /* If there are transition labels without transitions, multiple dummy */    assert(part_tr.action_block.size() ==
    /* entries will be placed side-by-side. */                                                                aut.num_transitions() + action_label.size() - 1);
    bisim_dnj::action_block_iter_t next_action_label_begin =
                                                  part_tr.action_block.begin();
    label_type num_labels_with_transitions = 0;
    label_type label = action_label.size();
    do
    {
        --label;
        if (0 < action_label[label].count)
        {
            if (++num_labels_with_transitions == 2)
            {
                // This is the second action_block-slice, so the bunch is not
                // yet marked as nontrivial but it should be.
                bunch->make_nontrivial();
            }
            // initialise begin_or_before_end pointers for this
            // action_block-slice
            action_label[label].begin = next_action_label_begin +
                                                     action_label[label].count;
            next_action_label_begin->begin_or_before_end.set(
                                                action_label[label].begin - 1);
            bisim_dnj::action_block_iter_t action_block_slice_begin =
                                                       next_action_label_begin;
            while (++next_action_label_begin < action_label[label].begin)
            {
                next_action_label_begin->begin_or_before_end.set(
                                                     action_block_slice_begin);
            }
        }
        else  action_label[label].begin = next_action_label_begin;
    }
    while (0 < label &&
               (/* insert a dummy entry */                                      assert(next_action_label_begin < part_tr.action_block_inert_begin),
                next_action_label_begin->succ.clear(),
                next_action_label_begin->begin_or_before_end.clear(),
                ++next_action_label_begin,                              true)); assert(next_action_label_begin == part_tr.action_block_inert_begin);

    // distribute the transitions over the data structures

    bisim_dnj::block_bunch_iter_t next_block_bunch=part_tr.block_bunch.begin();
    for (const transition& t: aut.get_transitions())
    {                                                                           assert(part_st.state_info[t.from()].block == B);
        bisim_dnj::succ_iter_t succ_pos;
        bisim_dnj::block_bunch_iter_t block_bunch_pos;
        bisim_dnj::pred_iter_t pred_pos;
        bisim_dnj::action_block_iter_t action_block_pos;

        if (branching && aut.is_tau(aut.apply_hidden_label_map(t.label())) &&   ((
                                                            t.from() != t.to())   || (assert(preserve_divergence), false)))
        {
            // It is a (normal) inert transition: place near the end of the
            // respective pred/succ slices, just before the other inert
            // transitions.
            succ_pos = --part_st.state_info[t.from()].succ_inert.begin;         assert(succ_pos->begin_or_before_end.is_null());
            block_bunch_pos = part_tr.block_bunch.end() - inert_transitions;    assert(block_bunch_pos >= part_tr.block_bunch_inert_begin);
            pred_pos = --part_st.state_info[t.to()].pred_inert.begin;
            action_block_pos = part_tr.action_block.end() - inert_transitions;  assert(action_block_pos >= part_tr.action_block_inert_begin);
            --inert_transitions;
            action_block_pos->begin_or_before_end.clear();
            block_bunch_pos->slice.clear();
        }
        else
        {
            // It is a non-inert transition (possibly a self-loop): place at
            // the end of the respective pred/succ slices
            succ_pos = --part_st.state_info[t.from()].succ.begin;               assert(succ_pos->begin_or_before_end() <= succ_pos ||
                                                                                           succ_pos->begin_or_before_end()->begin_or_before_end() == succ_pos);
            block_bunch_pos = next_block_bunch++;                               assert(block_bunch_pos < part_tr.block_bunch_inert_begin);
            pred_pos = --part_st.state_info[t.to()].pred.begin;
            action_block_pos =
                   --action_label[aut.apply_hidden_label_map(t.label())].begin; assert(action_block_pos->begin_or_before_end() <= action_block_pos ||
                                                                                                action_block_pos->begin_or_before_end()->
                                                                                                                    begin_or_before_end() == action_block_pos);
            block_bunch_pos->slice.set(B->stable_block_bunch.begin());          assert(action_block_pos < part_tr.action_block_inert_begin);
        }
        succ_pos->block_bunch = block_bunch_pos;
        block_bunch_pos->pred = pred_pos;
        pred_pos->action_block = action_block_pos;
        pred_pos->source = &part_st.state_info[t.from()];                       assert(pred_pos->source->current_out_slice <= succ_pos);
        pred_pos->target = &part_st.state_info[t.to()];
        action_block_pos->succ.set(succ_pos);
    }                                                                           assert(0 == inert_transitions);
    /* delete transitions already -- they are no longer needed.  We will add */ assert(next_block_bunch == part_tr.block_bunch_inert_begin);
    // new transitions at the end of minimisation.
    aut.clear_transitions();

    if (nullptr != bunch)
    {
        while (bunch->begin->succ.is_null())
        {
            ++bunch->begin;                                                     assert(bunch->begin < bunch->end);
        }
        while (bunch->end[-1].succ.is_null())
        {
            --bunch->end;                                                       assert(bunch->begin < bunch->end);
        }
    }

    mCRL2log(log::verbose, "bisim_dnj") << " transitions\n";

    // now split the block (if it has marked states) into the part with
    // non-inert transitions and the part without.
    if (0 < B->marked_size())
    {
        if (1 < B->size())
        {
                                                                                #ifndef NDEBUG
                                                                                part_st.print_part();  part_tr.print_trans(*this);
                                                                                #endif
            B = refine(B,
                /* splitter block_bunch */ B->stable_block_bunch.begin(),
                     bisim_dnj::extend_from_state_markings_for_postprocessing);
            // We can ignore possible new non-inert transitions, as every red
            // bottom state already has a transition in bunch.
        }
        B->marked_nonbottom_begin = B->end;
        B->marked_bottom_begin = B->nonbottom_begin;
    }
}


/// \brief Adapt the LTS after minimisation
/// \details After the efficient branching bisimulation minimisation, the
/// information about the quotient LTS is only stored in the partition data
/// structure of the partitioner object.  This function exports the
/// information back to the LTS by adapting its states and transitions:  it
/// updates the number of states and adds those transitions that are
/// mandated by the partition data structure.
///
/// The parameter and return value are implicit with this function: a
/// reference to the LTS was stored in the object by the constructor.
template <class LTS_TYPE>
void bisim_partitioner_dnj<LTS_TYPE>::replace_transition_system()
{                                                                               assert(sizeof(label_type) <= sizeof(const void*));
    // store the labels with the action_block-slices
    // We misuse the field succ()->begin_or_before_end of the first entry
    // of each action_block-slice for this purpose.
    bisim_dnj::action_block_const_iter_t action_block_iter_end =
                                              part_tr.action_block_inert_begin;
    label_type label = 0;
    do
    {
        for (bisim_dnj::action_block_iter_t
              action_block_iter = action_label[label].begin;
              action_block_iter < action_block_iter_end;
              action_block_iter = action_block_iter->begin_or_before_end() + 1)
        {                                                                       assert(!action_block_iter->succ.is_null());
            action_block_iter->succ()->begin_or_before_end.set_label(label);    assert(action_block_iter <= action_block_iter->begin_or_before_end());
        }
    }
    while (++label < action_label.size() &&
            (action_block_iter_end = action_label[label - 1].begin - 1, true));

    // for all blocks
    bisim_dnj::permutation_const_iter_t s_iter = part_st.permutation.begin();
    do
    {
        const bisim_dnj::block_t* const B = (*s_iter)->block;
        // for all block_bunch-slices of the block
        for(bisim_dnj::block_bunch_slice_const_iter_t
                    trans_iter = B->stable_block_bunch.begin();
                       B->stable_block_bunch.end() != trans_iter; ++trans_iter)
        {                                                                       assert(trans_iter->is_stable());  assert(!trans_iter->empty());
            bisim_dnj::pred_const_iter_t const pred = trans_iter->end[-1].pred;
            // add a transition from the source block to the goal block with
            // the indicated label.
            label_type label = pred->action_block->action_block_slice_begin()->
                                       succ()->begin_or_before_end.get_label(); assert(0 <= label);  assert(label < action_label.size());
            aut.add_transition(transition(B->seqnr, label,
                                                  pred->target->block->seqnr));
        }
        s_iter = B->end;
    }
    while (s_iter < part_st.permutation.end());

    // Merge the states, by setting the state labels of each state to the
    // concatenation of the state labels of its equivalence class.

    if (aut.has_state_info())   /* If there are no state labels
                                                    this step can be ignored */
    {
      /* Create a vector for the new labels */
      bisim_gjkw::fixed_vector<typename LTS_TYPE::state_label_t> new_labels(
                                             bisim_dnj::block_t::nr_of_blocks);

      for(std::size_t i=aut.num_states(); i>0; )
      {
        --i;
        const std::size_t new_index = part_st.state_info[i].block->seqnr;
        /* i. e. new_index = get_eq_class(i) */
        new_labels[new_index]=aut.state_label(i)+new_labels[new_index];
      }

      aut.set_num_states(bisim_dnj::block_t::nr_of_blocks);
      for(std::size_t i=0; i < bisim_dnj::block_t::nr_of_blocks; ++i)
      {
        aut.set_state_label(i,new_labels[i]);
      }
    }
    else
    {
      aut.set_num_states(bisim_dnj::block_t::nr_of_blocks);
    }

    aut.set_initial_state(part_st.state_info[aut.initial_state()].
                                                                 block->seqnr);
}



/*=============================================================================
=                               main algorithm                                =
=============================================================================*/



/// \brief Run (branching) bisimulation minimisation in time O(m log n)
/// \details This function assumes that the partitioner object stores a LTS
/// with a partition satisfying the invariant:
///
/// If a state contains a transition in a bunch, then every bottom state in
/// the same block contains a transition in that bunch.
///
/// The function runs the efficient O(m log n) algorithm for branching
/// bisimulation minimisation on the LTS that has been stored in the
/// partitioner:  As long as there are nontrivial bunches, it selects one,
/// subdivides it into two bunches and then stabilises the partition for
/// these bunches.  As a result, the partition stored in the partitioner
/// will become stable.
///
/// Parameters and return value are implicit with this function:  the LTS,
/// the partition and the flags of the bisimulation algorithm are all
/// stored in the partitioner object.
template <class LTS_TYPE>
void bisim_partitioner_dnj<LTS_TYPE>::
                                     refine_partition_until_it_becomes_stable()
{
    // while there is a nontrivial bunch
    for (;;)
    {
                                                                                #ifndef NDEBUG
        /*------------------------- find a splitter -------------------------*/ if (mCRL2logEnabled(log::debug, "bisim_dnj"))
                                                                                {   part_st.print_part();  part_tr.print_trans(*this);  }
        /* select a nontrivial bunch splitter_bunch */                          assert_stability();
                                                                                #endif
        bisim_dnj::bunch_t* const splitter_bunch =
                                     bisim_dnj::bunch_t::get_some_nontrivial();
        if (nullptr == splitter_bunch)  break;
                                                                                #ifndef NDEBUG
        /* select a small action_block-slice splitter_slice in splitter_bunch*/ mCRL2log(log::debug, "bisim_dnj") << "Refining "
        /* move splitter_slice from splitter_bunch to a new bunch */                                                     << splitter_bunch->debug_id() << '\n';
                                                                                #endif
        bisim_dnj::bunch_t* const new_bunch =
                          splitter_bunch->split_off_small_action_block_slice();
                                                                                #ifndef NDEBUG
                                                                                mCRL2log(log::debug, "bisim_dnj") << "Splitting off "
        /*----------------- find predecessors of new_bunch ------------------*/                                               << new_bunch->debug_id() << '\n';
                                                                                #endif
        // for all transitions in new_bunch
        for (bisim_dnj::action_block_iter_t splitter_iter = new_bunch->begin;
                               splitter_iter < new_bunch->end; ++splitter_iter)
        {
            bisim_dnj::state_info_entry* const source =
                              splitter_iter->succ()->block_bunch->pred->source;
            // mark source of the transition
            bool const first_transition_of_state = source->block->mark(source);
            // register whether source still has transitions to
            // splitter_bunch
            part_tr.first_move_transition_to_new_bunch(splitter_iter,
                                                    first_transition_of_state);
        }

        // We cannot join the loop above with the loop below!

        // for all transitions in new_bunch
        for (bisim_dnj::action_block_iter_t splitter_iter = new_bunch->begin;
                               splitter_iter < new_bunch->end; ++splitter_iter)
        {
            part_tr.second_move_transition_to_new_bunch(splitter_iter,
                                                                    new_bunch);
                //< this function also moves the block_bunch-slice of the new
                //  and of the old bunch to the unstable block_bunch-slices.
        }

        /*------------------- stabilise the partition again -----------------*/

        // for all unstable block_bunch-slices
        while (!part_tr.unstable_block_bunch.empty())
        {
            // The first element of the unstable block_bunch-slices list should
            // be a small splitter of a block that is marked accordingly.

            bisim_dnj::block_bunch_slice_iter_t splitter =
                                          part_tr.unstable_block_bunch.begin(); assert(!splitter->is_stable());  assert(splitter->bunch() == new_bunch);
            bisim_dnj::block_t* refine_block = splitter->source_block();        assert(1 < refine_block->size());
            // make the splitter stable again:  Insert it
            // in the stable block_bunch-slice list of refine_block.
            refine_block->stable_block_bunch.splice(
                    refine_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
            splitter->make_stable();                                            // test whether the next splitter actually belongs to this block:
            /* assume that refine_block has state markings corresponding to */  assert(*part_tr.unstable_block_bunch.front().bunch() < *splitter->bunch());
            /* splitter.  Split: */                                             assert(part_tr.unstable_block_bunch.front().source_block() == refine_block);
            bisim_dnj::permutation_iter_t refine_block_begin =
                                                           refine_block->begin;
            bisim_dnj::block_t* red_block = refine(refine_block, splitter,
                                   bisim_dnj::extend_from_state_markings_only);
            if (refine_block_begin < red_block->begin)
            {
                bisim_dnj::block_t* const blue_block =
                                                  (*refine_block_begin)->block; assert(blue_block->end == red_block->begin);
                bisim_dnj::block_bunch_slice_iter_t blue_splitter =
                                          part_tr.unstable_block_bunch.begin(); assert(0 == blue_block->marked_size());
                if (part_tr.unstable_block_bunch.end() != blue_splitter &&
                    (blue_splitter->source_block() == blue_block ||
                     (++blue_splitter != part_tr.unstable_block_bunch.end() &&
                                 blue_splitter->source_block() == blue_block)))
                {                                                               assert(!blue_splitter->is_stable());
                    // The next ``unstable'' block_bunch is the blue subblock's
                    /* slice.  Actually that is already stable. */              assert(blue_splitter->bunch() == splitter_bunch);
                    blue_block->stable_block_bunch.splice(
                            blue_block->stable_block_bunch.end(),
                                  part_tr.unstable_block_bunch, blue_splitter);
                    blue_splitter->make_stable();
                }
            }
            // postprocess new non-inert transitions
            if (0 < red_block->marked_size())
            {
                red_block = postprocess_new_noninert(red_block, splitter);
                if (nullptr == red_block)  continue;
            }
            // Now the first element of the unstable block_bunch-slices
            // list should be a large splitter of red_block, and it should
            // be handled using the current_out_slice pointers.
            splitter = part_tr.unstable_block_bunch.begin();
            if (part_tr.unstable_block_bunch.end() == splitter)  break;         assert(!splitter->is_stable());
            if (red_block != splitter->source_block())  continue;
                                                                                assert(splitter->bunch() == splitter_bunch);
            // make the splitter stable again: Insert it in the correct
            // place in the stable block_bunch-slice list of red_block.
            red_block->stable_block_bunch.splice(
                            red_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
            splitter->make_stable();

            if (1 >= red_block->size())  continue;
            red_block = refine(red_block, splitter,
                                          bisim_dnj::extend_from_FromRed_only);
            // postprocess new non-inert transitions
            if (0 < red_block->marked_size())
            {
                postprocess_new_noninert(red_block, splitter);
            }
        }
    }
    // return P
        // (this happens implicitly, through the bisim_partitioner_dnj object
        // data)
}



/*=============================================================================
=                                   Refine                                    =
=============================================================================*/



/// \brief Split a block according to a splitter
/// \details The function splits `refine_block` into the red part (states
/// with a transition in `splitter`) and the blue part (states without a
/// transition in `splitter`).  Depending on `mode`, the states are primed
/// as follows:
///
/// - If `mode == extend_from_state_markings_only`, then all states with a
///   transition must have been marked already.
/// - If `mode == extend_from_state_markings_for_postprocessing`, states
///   are marked as above.  The only difference is the handling of new
///   non-inert transitions.
/// - If `mode == extend_from_FromRed_only`, then no states must be marked;
///   the initial states with a transition in `splitter` are searched by
///   `refine()` itself.
/// - If `mode == extend_from_bottom_state_markings_and_FromRed`, then
///   bottom states with a transition must have been marked already, but
///   there may be non-bottom states that also have a transition, which are
///   searched by `refine()`.
///
/// The  function  will  also  adapt  all  data  structures  and  determine
/// which  transitions  have  changed  from  inert  to  non-inert.   States
/// with  a  new  non-inert  transition  will  be  marked  upon  returning.
/// Normally,  the  new  non-inert  transitions  are  moved  to  a  new
/// bunch,  which  will  be  specially  created.   However,  if  `mode ==
/// extend_from_state_markings_for_postprocessing`, then the new non-inert
/// transitions will be added to `splitter` (which must hold transitions
/// that have just become non-inert before this call to `refine()`).  If
/// the resulting block contains marked states, the caller has to call
/// `postprocess_new_noninert()` to stabilise the block because the new
/// bunch may make the block unstable.
/// \param refine_block  block that needs to be refined
/// \param splitter      transition set that makes the block unstable
/// \param mode          indicates how to find states with a transition in
///                      `splitter`, as described above
/// \returns (a pointer to) the red subblock.  It is an error to call the
/// function with settings that lead to an empty red subblock.  (An empty
/// blue subblock is ok.)
template <class LTS_TYPE>
bisim_dnj::block_t* bisim_partitioner_dnj<LTS_TYPE>::refine(
      bisim_dnj::block_t* const refine_block,
      bisim_dnj::block_bunch_slice_iter_t const splitter,
      enum bisim_dnj::refine_mode_t const mode)
{                                                                               assert(refine_block == splitter->source_block());
                                                                                #ifndef NDEBUG
                                                                                mCRL2log(log::debug,"bisim_dnj") <<"refine(" <<refine_block->debug_id() << ','
                                                                                    << splitter->debug_id()
                                                                                    << (bisim_dnj::extend_from_state_markings_only == mode
                                                                                        ? ",extend_from_state_markings_only)\n"
                                                                                        : (bisim_dnj::extend_from_state_markings_for_postprocessing == mode
                                                                                           ? ",extend_from_state_markings_for_postprocessing)\n"
                                                                                           : (bisim_dnj::extend_from_FromRed_only == mode
                                                                                              ? ",extend_from_FromRed_only)\n"
                                                                                              : (bisim_dnj::extend_from_bottom_state_markings_and_FromRed==mode
                                                                                                 ? ",extend_from_bottom_state_markings_and_FromRed)\n"
                                                                                                 : ",UNKNOWN MODE)\n"))));
                                                                                #endif
                                                                                assert(1 < refine_block->size());
                                                                                assert(0 < refine_block->marked_size() ||
                                                                                             bisim_dnj::extend_from_FromRed_only == mode ||
                                                                                             bisim_dnj::extend_from_bottom_state_markings_and_FromRed == mode);
    bisim_dnj::block_t* red_block;                                              assert(0 == refine_block->marked_size() ||
                                                                                                                  bisim_dnj::extend_from_FromRed_only != mode);
    COROUTINES_SECTION
        // shared variables of both coroutines
        bisim_dnj::permutation_iter_t notblue_initialised_end =
                                                 refine_block->nonbottom_begin;
        bool FromRed_is_handled =
              bisim_dnj::extend_from_state_markings_only == mode ||
              bisim_dnj::extend_from_state_markings_for_postprocessing == mode;

        // variable declarations of the blue coroutine
        bisim_dnj::permutation_iter_t blue_visited_end;
        bisim_dnj::permutation_iter_t blue_blue_nonbottom_end;
        bisim_dnj::pred_iter_t blue_pred_iter;
        bisim_dnj::pred_const_iter_t blue_pred_inert_end;
        bisim_dnj::state_info_ptr blue_source;
        bisim_dnj::succ_const_iter_t blue_begin, blue_end;

        // variable declarations of the red coroutine
        union R
        {
            bisim_dnj::block_bunch_iter_t fromred;
            bisim_dnj::permutation_iter_t block;
            R(){}  ~R(){}  // constructor and destructor are required
        } red_visited_begin;
        bisim_dnj::pred_iter_t red_pred_iter;
        bisim_dnj::pred_const_iter_t red_pred_inert_end;

        COROUTINE_LABELS(   (REFINE_RED_PREDECESSOR_HANDLED)
                            (REFINE_BLUE_PREDECESSOR_HANDLED)
                            (REFINE_RED_STATE_HANDLED)
                            (REFINE_BLUE_STATE_HANDLED)
                            (REFINE_BLUE_TESTING)
                            (REFINE_RED_COLLECT_FROMRED)
                            (REFINE_BLUE_COLLECT_BOTTOM))

        /*------------------------ find blue states -------------------------*/

        COROUTINE(1)
            if (bisim_dnj::extend_from_FromRed_only == mode)
            {
                // we have to decide which unmarked bottom states are blue.  So
                // we walk over all of them and check whether they have a
                // transition to the bunch of FromRed or not.
                // 3.3: ..., Blue := {}
                blue_visited_end = refine_block->begin;
                // 3.5l: whenever |Blue| > |RfnB|/2 do  Abort this coroutine
                    // nothing needs to be done now, as |Blue| = 0 here.

                /*  -  -  -  -  - collect blue bottom states -  -  -  -  -  */

                // 3.6l: while Test is not empty and FromRed is not empty do
                    // We use the variable blue_visited_ptr in this loop
                    // to indicate the boundary between blue states (those
                    // in the interval [refine_block->bottom_begin,
                    // blue_visited_ptr) ) and Test states (those in
                    // [blue_visited_ptr, refine_block->end) ).
                COROUTINE_WHILE (REFINE_BLUE_COLLECT_BOTTOM,
                        blue_visited_end < refine_block->marked_bottom_begin &&
                                                           !FromRed_is_handled)
                {
                    // 3.7l: Choose s in Test
                    bisim_dnj::state_info_ptr s = *blue_visited_end;
                    // 3.8l: if s --> SpBu then
                    if (s->surely_has_transition_to(splitter->bunch()))
                    {
                        // 3.9l: Move s from Test to Red
                        // The state s is not blue.  Move it to the slice of
                        // non-blue bottom states.
                        iter_swap(blue_visited_end,
                                          --refine_block->marked_bottom_begin);
                        // 3.5r: whenever |Red| > |RfnB|/2 do
                        //          Abort the red coroutine
                        if (refine_block->marked_size()>refine_block->size()/2)
                        {
                            ABORT_OTHER_COROUTINE();
                        }
                    // 3.10l: else
                    }
                    else
                    {                                                           assert(s->surely_has_no_transition_to(splitter->bunch()));
                        // 3.11l: Move s from Test to Blue
                        ++blue_visited_end;
                        // 3.5l: whenever |Blue| > |RfnB|/2 do
                        //          Abort this coroutine
                        if((state_type)(blue_visited_end-refine_block->begin) >
                                                      refine_block->size() / 2)
                        {
                            ABORT_THIS_COROUTINE();
                        }
                    // 3.12l: end if
                    }
                // 3.13l: end while
                }
                END_COROUTINE_WHILE;
            }

            // 3.14l: Blue := Blue union Test
                // done implicitly: we now regard all unmarked bottom states as
                // blue, i. e. the whole interval [refine_block->bottom_begin,
                // refine_block->marked_bottom_begin).
            // 3.5l: whenever |Blue| > |RfnB|/2 do  Abort this coroutine
            if (refine_block->unmarked_bottom_size() > refine_block->size()/2)
            {
                ABORT_THIS_COROUTINE();
            }
            if (0 == refine_block->unmarked_bottom_size())
            {
                // all bottom states are red, so there cannot be any blue
                // states.  Unmark all states, as there are no transitions that
                // have become non-inert.
                refine_block->marked_nonbottom_begin = refine_block->end;
                refine_block->marked_bottom_begin =
                                                 refine_block->nonbottom_begin;

                red_block = refine_block;
                TERMINATE_COROUTINE_SUCCESSFULLY();
            }

             /*  -  -  -  -  -  -  - visit blue states -  -  -  -  -  -  -  */

            // 3.15l: while Blue contains unvisited states do
            blue_visited_end = refine_block->begin;
            blue_blue_nonbottom_end = refine_block->nonbottom_begin;
            COROUTINE_DO_WHILE (REFINE_BLUE_STATE_HANDLED,
                                    blue_visited_end < blue_blue_nonbottom_end)
            {
                // 3.16l: Choose an unvisited s in Blue
                blue_pred_iter = (*blue_visited_end)->pred_inert.begin,
                blue_pred_inert_end = (*blue_visited_end)->pred_cend();
                // 3.17l: Mark s as visited
                ++blue_visited_end;
                // 3.18l: for all s_prime in inert_in(s) \ Red do
                COROUTINE_FOR (REFINE_BLUE_PREDECESSOR_HANDLED, (void) 0,
                        blue_pred_iter < blue_pred_inert_end, ++blue_pred_iter)
                {
                    blue_source = blue_pred_iter->source;                       assert(refine_block->nonbottom_begin <= blue_source->pos);
                                                                                assert(blue_source->pos < refine_block->end);
                    if (refine_block->marked_nonbottom_begin<=blue_source->pos)
                    {
                        continue;
                    }
                    // 3.19l: if notblue(s_prime) undefined then
                    if (notblue_initialised_end <= blue_source->pos)
                    {
                        // 3.20l: notblue(s_prime) := |inert_out(s_prime)|
                        blue_source->notblue = blue_source->succ_end() -
                                                 blue_source->succ_inert.begin;
                        iter_swap(blue_source->pos, notblue_initialised_end++);
                    // 3.21l: end if
                    }
                    // 3.22l: notblue(s_prime) := notblue(s_prime) - 1
                    // 3.23l: if notblue(s_prime) == 0 && ...
                    if (--blue_source->notblue > 0)  continue;
                    // 3.23l: ... && (FromRed == {} ||
                    //         out_noninert(s_prime) intersect SpBu == {}) then
                    if (!FromRed_is_handled)
                    {
                        if (blue_source->surely_has_transition_to(
                                                            splitter->bunch()))
                        {
                            continue;
                        }
                        if (!blue_source->surely_has_no_transition_to(
                                                            splitter->bunch()))
                        {
                            // It is not yet known whether blue_source has a
                            // transition in splitter->bunch or not.  Execute
                            // the slow test now.
                            blue_begin = blue_source->succ.begin;
                            blue_end = blue_source->succ_inert.begin;
                            COROUTINE_WHILE (REFINE_BLUE_TESTING,
                                                         blue_begin < blue_end)
                            {
                                // binary search for transitions from
                                // blue_source in bunch splitter->bunch.
                                bisim_dnj::succ_const_iter_t const mid =
                                      blue_begin + (blue_end - blue_begin) / 2;
                                if (*splitter->bunch() <= *mid->bunch())
                                {
                                    blue_end = mid->out_slice_begin();
                                }
                                if (*mid->bunch() <= *splitter->bunch())
                                {
                                    blue_begin = mid->out_slice_before_end()+1;
                                }
                            }
                            END_COROUTINE_WHILE;
                            if (blue_begin > blue_end)  continue;
                        }
                    }                                                           assert(blue_blue_nonbottom_end <= blue_source->pos);
                    /* 3.24l: Blue := Blue union {s_prime} */                   assert(blue_source->pos < notblue_initialised_end);
                    iter_swap(blue_source->pos, blue_blue_nonbottom_end++);
                    // 3.5l: whenever |Blue| > |RfnB|/2 do Abort this coroutine
                    if (blue_blue_nonbottom_end-refine_block->nonbottom_begin +
                                    refine_block->unmarked_bottom_size() >
                                                      refine_block->size() / 2)
                    {
                        ABORT_THIS_COROUTINE();
                    }
                    // 3.25l: end if
                        // this is implicit in the `continue` statements above.
                // 3.26l: end for
                }
                END_COROUTINE_FOR;
            // 3.27l: end while
                if (refine_block->marked_bottom_begin == blue_visited_end)
                {
                    blue_visited_end = refine_block->nonbottom_begin;
                }
            }
            END_COROUTINE_DO_WHILE;

            /* -  -  -  -  -  -  - split off blue block -  -  -  -  -  -  - */

            // 3.28l: Abort the other coroutine
            ABORT_OTHER_COROUTINE();
            // All non-blue states are red.
            // 3.29l: Move Blue to a new block NewB
            // and
            // 3.30l: Destroy all temporary data
            refine_block->marked_nonbottom_begin = blue_blue_nonbottom_end;
            red_block = refine_block;
            bisim_dnj::block_t* const blue_block =
                   refine_block->split_off_block(bisim_dnj::new_block_is_blue);
            part_tr.adapt_transitions_for_new_block(blue_block, refine_block,
                 bisim_dnj::extend_from_state_markings_for_postprocessing==mode
                        ? bisim_dnj::iterator_or_null
                                <bisim_dnj::block_bunch_slice_iter_t>(splitter)
                        : bisim_dnj::iterator_or_null
                                <bisim_dnj::block_bunch_slice_iter_t>(nullptr),
                                       splitter, bisim_dnj::new_block_is_blue);
        END_COROUTINE

        /*------------------------- find red states -------------------------*/

        COROUTINE(2)
            if (refine_block->marked_size() > refine_block->size() / 2)
            {
                ABORT_THIS_COROUTINE();
            }

            /* -  -  -  -  -  - collect states from FromRed -  -  -  -  -  - */

            // 3.6r: while FromRed != {} do
            if (!FromRed_is_handled)
            {
                red_visited_begin.fromred = splitter->end;
                COROUTINE_DO_WHILE(REFINE_RED_COLLECT_FROMRED,
                    part_tr.block_bunch.begin() < red_visited_begin.fromred &&
                             splitter == red_visited_begin.fromred[-1].slice())
                {
                    // 3.10r: FromRed := FromRed \ {s --> t}
                    --red_visited_begin.fromred;
                    // 3.7r: Choose s --> t in FromRed
                    bisim_dnj::state_info_ptr source =
                                       red_visited_begin.fromred->pred->source; assert(source->block == refine_block);
                    // 3.8r: Test := Test \ {s}
                    // and
                    // 3.9r: Red := Red union {s}
                    if (refine_block->nonbottom_begin <= source->pos &&
                                         source->pos < notblue_initialised_end)
                    {
                        // The non-bottom state has a transition to a blue
                        // state, so notblue is initialised; however, now it is
                        // revealed to be red anyway.
                        iter_swap(source->pos, --notblue_initialised_end);
                    }
                    if (refine_block->mark(source) &&
                    // 3.5r: whenever |Red| > |RfnB|/2 do  Abort this coroutine
                          refine_block->marked_size() > refine_block->size()/2)
                    {
                        ABORT_THIS_COROUTINE();
                    }
            // 3.13r: end while
                }
                END_COROUTINE_DO_WHILE;

                // The shared variable FromRed_is_handled is set to true as
                // soon as the transitions splitter have been completed.  (From
                // that moment on, no slow tests are needed any more.)
                FromRed_is_handled = true;
            }

            /*-  -  -  -  -  -  -  - visit red states -  -  -  -  -  -  -  -*/
                                                                                assert(0 != refine_block->marked_size());
            red_visited_begin.block = refine_block->nonbottom_begin;
            if (refine_block->marked_bottom_begin == red_visited_begin.block)
            {
                // It may happen that all found states are non-bottom states.
                // (In that case, some of these states will become new bottom
                // states.)
                red_visited_begin.block = refine_block->end;
            }
            // 3.15r: while Red contains unvisited states do
            COROUTINE_DO_WHILE(REFINE_RED_STATE_HANDLED,
               refine_block->marked_nonbottom_begin != red_visited_begin.block)
            {
                // 3.17r (order of lines changed): Mark s as visited
                --red_visited_begin.block;
                // 3.16r: Choose an unvisited s in Red
                red_pred_inert_end = (*red_visited_begin.block)->pred_cend();
                // 3.18r: for all s_prime in inert_in(s) do
                COROUTINE_FOR (REFINE_RED_PREDECESSOR_HANDLED,
                  red_pred_iter = (*red_visited_begin.block)->pred_inert.begin,
                           red_pred_iter < red_pred_inert_end, ++red_pred_iter)
                {
                    bisim_dnj::state_info_ptr const source =
                                                         red_pred_iter->source; assert(refine_block->nonbottom_begin <= source->pos);
                    /* 3.24r: Red := Red union {s_prime} */                     assert(source->pos < refine_block->end);
                    if (// refine_block->nonbottom_begin <= source->pos &&
                                         source->pos < notblue_initialised_end)
                    {
                        // The state has a transition to a blue state, so
                        // notblue is initialised; however, now it is revealed
                        // to be red anyway.
                        iter_swap(source->pos, --notblue_initialised_end);
                    }
                    if (refine_block->mark_nonbottom(source) &&
                    // 3.5r: whenever |Red| > |RfnB|/2 do  Abort this coroutine
                          refine_block->marked_size() > refine_block->size()/2)
                    {
                        ABORT_THIS_COROUTINE();
                    }
                // 3.26r: end for
                }
                END_COROUTINE_FOR;
            // 3.27r: end while
                if(refine_block->marked_bottom_begin==red_visited_begin.block&&
                       red_visited_begin.block < refine_block->nonbottom_begin)
                {
                    red_visited_begin.block = refine_block->end;
                }
            }
            END_COROUTINE_DO_WHILE;

            /*  -  -  -  -  -  -  - split off red block -  -  -  -  -  -  -  */

            // 3.28r: Abort the other coroutine
            ABORT_OTHER_COROUTINE();
            // All non-red states are blue.
            // 3.29r: Move Red to a new block NewB
            // and
            // 3.30r: Destroy all temporary data
            red_block =
                    refine_block->split_off_block(bisim_dnj::new_block_is_red);
            part_tr.adapt_transitions_for_new_block(red_block, refine_block,
                 bisim_dnj::extend_from_state_markings_for_postprocessing==mode
                        ? bisim_dnj::iterator_or_null
                                <bisim_dnj::block_bunch_slice_iter_t>(splitter)
                        : bisim_dnj::iterator_or_null
                                <bisim_dnj::block_bunch_slice_iter_t>(nullptr),
                                        splitter, bisim_dnj::new_block_is_red);
        END_COROUTINE
    END_COROUTINES_SECTION
    return red_block;
}



/*=============================================================================
=                        Postprocess new bottom states                        =
=============================================================================*/



/// \brief Internal functional to compare new bottom states
/// \details The functions in this class are used to sort and search through
/// the new bottom states.  For efficiency, they are ordered according to their
/// current out-slice (containing transitions to a specific bunch).  That means
/// that new bottom states can quickly be separated into those that can reach
/// that bunch and those that cannot.
///
/// The order is reversed (it is a greater-operator while std::sort and
/// std::lower_bound expect a less-operator) so that states with the smallest
///current out-slice come last in the order.
static struct {
    /// \brief Comparison operator for the current out-slice of new bottom
    /// states
    /// \details This variant can be used for std::lower_bound, to compare a
    /// current out-slice pointer with a bunch directly.
    bool operator()(bisim_dnj::state_info_const_ptr p2,
                                                  const bisim_dnj::bunch_t* p1)
    {
        if (p2->current_out_slice >= p2->succ_inert.begin)  return true;
        return *p1 < *p2->current_out_slice->bunch();
    }

    /// \brief Comparison operator for the current out-slice of new bottom
    /// states
    /// \details This variant can be used for std::sort, to compare the curent
    /// out-slices of two states.
    bool operator()(bisim_dnj::state_info_const_ptr p2,
                                            bisim_dnj::state_info_const_ptr p1)
    {
        if (p1->current_out_slice >= p1->succ_inert.begin)  return false;
        return operator()(p2, p1->current_out_slice->bunch());
    }
} current_out_slice_greater;


/// \brief Prepare a block for postprocesing
/// \details When this function starts, it assumes that the states with a
/// new non-inert transition in refine_block are marked.  It is an error if
/// it does not contain any marked states.
///
/// The function separates the states with new non-inert transitions from
/// those without;  as a result, the red subblock (which contains states
/// with new non-inert transitions) will contain at least one new bottom
/// state (and no old bottom states).  It then sorts the new bottom states
/// according to the first bunch in which they have transitions and marks
/// all block_bunch-slices of refine_block as unstable.
/// \param refine_block   block containing states with new non-inert
///                       transitions that need to be stabilised
/// \param last_splitter  splitter of the last separation before, i. e. the
///                       splitter that made these transitions non-inert
///                       (refine_block should already be stable w. r. t.
///                       last_splitter).
/// \param first_preparation  If true, then the function also makes sure
///                           that all unstable block_bunch-slice of
///                           refine_block are before
///                           unstable_block_bunch_postprocess_end.
/// \returns the block containing the old bottom states (and every state in
///          refine_block that cannot reach any new non-inert transition),
///          i. e. the blue subblock of the separation
template <class LTS_TYPE>
bisim_dnj::block_t*bisim_partitioner_dnj<LTS_TYPE>::prepare_for_postprocessing(
                            bisim_dnj::block_t* refine_block,
                            bisim_dnj::block_bunch_slice_iter_t last_splitter,
                                                        bool first_preparation)
{                                                                               assert(refine_block == last_splitter->source_block());
    bisim_dnj::block_t* blue_block;                                             assert(last_splitter->is_stable());
                                                                                assert(part_tr.block_bunch.begin() < part_tr.block_bunch_inert_begin);
    bisim_dnj::block_bunch_slice_iter_t new_noninert_block_bunch =
                                   part_tr.block_bunch_inert_begin[-1].slice(); assert(refine_block == new_noninert_block_bunch->source_block());
    // The noninert block_bunch-slice is already stable, so we do not need to
    /* move it to the stable block_bunch-slices. */                             assert(new_noninert_block_bunch->is_stable());
    // First, split into the states that have a new noninert transition and
    /* those that haven't. */                                                   assert(0 < refine_block->marked_size());
    if (0 < refine_block->unmarked_bottom_size())
    {
        refine_block = refine(refine_block, new_noninert_block_bunch,
                     bisim_dnj::extend_from_state_markings_for_postprocessing); assert(part_st.permutation.begin() < refine_block->begin);
        blue_block = refine_block->begin[-1]->block;
        // If more new noninert transitions are found, we do not need to handle
        // them, as every bottom state already has a transition in
        // new_noninert_block_bunch->bunch.
        if (0 < refine_block->marked_bottom_size())
        {
            // New non-inert transitions have been found.  In that case, we
            // also have to refine w. r. t. the last splitter.  We set the
            // variable `last_splitter` to new_noninert_block_bunch so it won't
            // disturb in the test below.
            last_splitter = new_noninert_block_bunch;
        }
    }
    else
    {
        blue_block = nullptr;
    }
    refine_block->marked_bottom_begin = refine_block->nonbottom_begin;
    refine_block->marked_nonbottom_begin = refine_block->end;

    // if this is the first call to prepare_for_postprocessing in the beginning
    // of postprocess_new_noninert, then unstable_block_bunch_postprocess_end
    // may point to a slice of the red subblock; as we stabilise for
    // all subblocks of it during postprocessing, we pull it before
    // unstable_block_bunch_postprocess_end.
    if (first_preparation)
    {
        // 4.2: Create an empty list R of refinable bunches
        bisim_dnj::block_bunch_slice_iter_t red_splitter =
                        part_tr.unstable_block_bunch_postprocess_end =
                                          part_tr.unstable_block_bunch.begin();

        if (part_tr.unstable_block_bunch.end() != red_splitter)
        {
            if (red_splitter->source_block() == refine_block)
            {
                ++part_tr.unstable_block_bunch_postprocess_end;
            }
            else if (++red_splitter != part_tr.unstable_block_bunch.end() &&
                                  red_splitter->source_block() == refine_block)
            {
                part_tr.unstable_block_bunch.splice(
                    part_tr.unstable_block_bunch_postprocess_end,
                                   part_tr.unstable_block_bunch, red_splitter);
            }
        }
    }
                                                                                #ifndef NDEBUG
                                                                                // assert that the remainder of the unstable block_bunch slice list after
                                                                                // part_tr.unstable_block_bunch_postprocess_end does not contain any
                                                                                // block_bunch slice whose source_block() is refine_block.
                                                                                #endif
    if (1 >= refine_block->size())  return blue_block;

    bisim_dnj::permutation_iter_t s_iter = refine_block->begin;                 assert(s_iter < refine_block->nonbottom_begin);
    do
    {
        bisim_dnj::state_info_ptr s = *s_iter;
        s->current_out_slice = s->succ.begin;
    }
    while (++s_iter < refine_block->nonbottom_begin);

    // Sort from last to first bunch
    std::sort(refine_block->begin, refine_block->nonbottom_begin,
                                                    current_out_slice_greater);
    s_iter = refine_block->begin;                                               assert(s_iter < refine_block->nonbottom_begin);
    do
    {                                                                           assert(s_iter == refine_block->begin ||
                                                                                                              !current_out_slice_greater(*s_iter, s_iter[-1]));
        (*s_iter)->pos = s_iter;
    }
    while (++s_iter < refine_block->nonbottom_begin);
    // make all stable block_bunch-slices of refine_block unstable
    // However, the bunch of new noninert transitions and the bunch that was
    // the last splitter do not need to be handled (as long as there are no
    // further new bottom states).
    for (bisim_dnj::block_bunch_slice_iter_t block_bunch_iter =
            refine_block->stable_block_bunch.begin();
                  refine_block->stable_block_bunch.end() != block_bunch_iter; )
    {                                                                           assert(block_bunch_iter->is_stable());
        bisim_dnj::block_bunch_slice_iter_t const next_block_bunch_iter =
                                                   std::next(block_bunch_iter);
        if (block_bunch_iter != last_splitter &&
                                  block_bunch_iter != new_noninert_block_bunch)
        {
            part_tr.unstable_block_bunch.splice(
                    part_tr.unstable_block_bunch_postprocess_end,
                           refine_block->stable_block_bunch, block_bunch_iter);
            block_bunch_iter->make_unstable();
        }
        block_bunch_iter = next_block_bunch_iter;
    }
    return blue_block;
}


/// \brief Split a block with new non-inert transitions as needed
/// \details The function splits refine_block by stabilising for all
/// bunches in which it contains transitions.
///
/// When this function starts, it assumes that the states with a new
/// non-inert transition in refine_block are marked.  It is an error if it
/// does not contain any marked states.
///
/// The function first separates the states with new non-inert transitions
/// from those without;  as a result, the red subblock (which contains
/// states with new non-inert transitions) will contain at least one new
/// bottom state.  Then it walks through all the bunches that can be
/// reached from this subblock to separate it into smaller, stable
/// subblocks.
/// \param refine_block   block containing states with new non-inert
///                       transitions that need to be stabilised
/// \param last_splitter  splitter of the last separation before, i. e. the
///                       splitter that made these transitions non-inert
///                       (refine_block should already be stable w. r. t.
///                       last_splitter).
/// \returns the block containing the old bottom states (and every state in
///          refine_block that cannot reach any new non-inert transition),
///          i. e. the blue subblock of the first separation above
template <class LTS_TYPE>
bisim_dnj::block_t* bisim_partitioner_dnj<LTS_TYPE>::postprocess_new_noninert(
                             bisim_dnj::block_t* refine_block,
                             bisim_dnj::block_bunch_slice_iter_t last_splitter)
{

    /*---------------- collect reachable block_bunch-slices -----------------*/

    bisim_dnj::block_t* const result_block =
                 prepare_for_postprocessing(refine_block, last_splitter, true);

    /*------------ stabilise w. r. t. found action_block-slices -------------*/

    bisim_dnj::block_bunch_slice_iter_t splitter;
    while ((splitter = part_tr.unstable_block_bunch.begin())
                               != part_tr.unstable_block_bunch_postprocess_end)
    {                                                                           assert(!splitter->is_stable());
        refine_block = splitter->source_block();                                assert(0 == refine_block->marked_size());
                                                                                assert(refine_block->begin < refine_block->nonbottom_begin);
        if (1 >= refine_block->size())
        {
            // make the splitter stable and do nothing else.
            refine_block->stable_block_bunch.splice(
                            refine_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
            splitter->make_stable();
            continue;
        }
        // check if we first have to split w. r. t. the splitter suggested
        // by the new bottom states instead:
        // (That should be another unstable block_bunch.)
        if (*splitter->bunch() <
             *refine_block->nonbottom_begin[-1]->current_out_slice->bunch() ||
            (// Yes, we have to use this splitter first:
             splitter = refine_block->nonbottom_begin[-1]->
                                     current_out_slice->block_bunch->slice(),   assert(part_tr.unstable_block_bunch_postprocess_end != splitter),
             !splitter->is_stable() &&
             (refine_block->marked_bottom_begin = std::lower_bound(
                   refine_block->begin, refine_block->nonbottom_begin - 1,
                              splitter->bunch(), current_out_slice_greater),    assert((*refine_block->marked_bottom_begin)->current_out_slice->bunch() ==
                                                                                                                                            splitter->bunch()),
                                                                        true)))
        {                                                                       assert(refine_block->begin == refine_block->marked_bottom_begin ||
                                                                                        *splitter->bunch() < *refine_block->
            /* make the splitter stable. */                                                               marked_bottom_begin[-1]->current_out_slice->bunch());
            refine_block->stable_block_bunch.splice(
                            refine_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
            splitter->make_stable();

            refine_block = refine(refine_block, splitter,
                     bisim_dnj::extend_from_bottom_state_markings_and_FromRed);
            // check whether there are even more new non-inert transitions
            if (0 < refine_block->marked_size())
            {
                // Prepare the slices of outgoing transitions of RedB similar
                // to Lines 4.4-4...
                refine_block = prepare_for_postprocessing(refine_block,
                                                              splitter, false);
                if (nullptr == refine_block)  continue;
            }
        }
        // advance the current_out_slice pointers of the (remaining) red
        // subblock
        bisim_dnj::permutation_iter_t s_iter = refine_block->begin;             assert(s_iter < refine_block->nonbottom_begin);
        do
        {
            bisim_dnj::state_info_ptr s = *s_iter;                              assert(s->succ.begin <= s->current_out_slice);
                                                                                assert(s->current_out_slice < s->succ_inert.begin);
                                                                                assert(s->current_out_slice->block_bunch->slice() == splitter);
            s->current_out_slice =
                              s->current_out_slice->out_slice_before_end() + 1;
        }
        while (++s_iter < refine_block->nonbottom_begin);
        // sort the bottom states of the red subblock according to their
        // new current_out_slice
        std::sort(refine_block->begin, refine_block->nonbottom_begin,
                                                    current_out_slice_greater);
        s_iter = refine_block->begin;                                           assert(s_iter < refine_block->nonbottom_begin);
        do
        {                                                                       assert(s_iter == refine_block->begin ||
                                                                                                              !current_out_slice_greater(*s_iter, s_iter[-1]));
            (*s_iter)->pos = s_iter;
        }
        while (++s_iter < refine_block->nonbottom_begin);
    // 4.25: end for
    }
    // 4.26: Destroy all temporary data
//        We somewhere need to reset:
//            SpSl->begin_from_non_bottom = SpSl->begin;
    return result_block;
}



/*=============================================================================
=                       explicit instantiation requests                       =
=============================================================================*/



template class bisim_partitioner_dnj<lts_lts_t>;
template class bisim_partitioner_dnj<lts_aut_t>;
template class bisim_partitioner_dnj<lts_fsm_t>;

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2
