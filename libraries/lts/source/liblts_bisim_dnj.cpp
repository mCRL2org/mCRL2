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

#ifndef NDEBUG
    state_info_const_ptr state_info_entry::s_i_begin;
    state_info_const_ptr state_info_entry::s_i_end;
    permutation_const_iter_t block_t::perm_begin;
    permutation_const_iter_t block_t::perm_end;
    action_block_const_iter_t action_block_entry::action_block_begin;
    const action_block_iter_t* action_block_entry::action_block_end;
    block_bunch_const_iter_t block_bunch_slice_t::block_bunch_end;
#endif
block_bunch_const_iter_t block_bunch_slice_t::block_bunch_begin;
state_type block_t::nr_of_blocks;
bunch_t* bunch_t::first_nontrivial;






/* ************************************************************************* */
/*                                                                           */
/*                   R E F I N A B L E   P A R T I T I O N                   */
/*                                                                           */
/* ************************************************************************* */





/// \brief refine the block
/// \details This function is called after a refinement function has found
/// where to split the block into unmarked (blue) and marked (red) states.
/// It creates a new block for the blue states.
/// \param   new_block_mode indicates whether the blue or the red block
///          should be the new one.
/// \returns pointer to the new block
block_t* block_t::split_off_block(enum new_block_mode_t const new_block_mode)
{
    // swap contents
    assert(0 < marked_size());
    assert(0 < unmarked_bottom_size());

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
                                                 pos2 = marked_nonbottom_begin;
        state_info_ptr const temp = *pos1;
        for (;;)
        {
            --pos2;
            *pos1 = *pos2;
            (*pos1)->pos = pos1;
            ++pos1;
            if (0 >= --swapcount)  break;
            *pos2 = *pos1;
            (*pos2)-> pos = pos2;
        }
        *pos2 = temp;
        (*pos2)->pos = pos2;
    }

    // create a new block
    permutation_iter_t const splitpoint = marked_bottom_begin +
                                                     unmarked_nonbottom_size();
    assert(begin < splitpoint);
    assert(splitpoint < end);

    block_t* new_block;
    if (new_block_is_blue == new_block_mode)
    {
        assert((state_type) (splitpoint - begin) <= size()/2);
        new_block = new block_t(begin, splitpoint);
        new_block->nonbottom_begin = marked_bottom_begin;

        // adapt the old block: it only keeps the red states
        begin = splitpoint;
        nonbottom_begin = marked_nonbottom_begin;
    }
    else
    {
        assert(new_block_is_red == new_block_mode);
        assert((state_type) (end - splitpoint) <= size()/2);

        new_block = new block_t(splitpoint, end);
        new_block->nonbottom_begin = marked_nonbottom_begin;

        // adapt the old block: it only keeps the blue states
        end = splitpoint;
        nonbottom_begin = marked_bottom_begin;
    }

    // unmark all states in both blocks
    marked_nonbottom_begin = end;
    // new_block->marked_nonbottom_begin =  new_block->end;
    marked_bottom_begin = nonbottom_begin;
    new_block->marked_bottom_begin = new_block->nonbottom_begin;

    // set the block pointer of states is the new block
    assert(new_block->size() <= size() / 2);
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
{
    assert(permutation.begin() < permutation.end());
    const block_t* B = (*permutation.begin())->block;
    for (;;)
    {
        mCRL2log(log::debug, "bisim_dnj") << B->debug_id() << ":\n";
        print_block("Bottom state", B, B->begin, B->nonbottom_begin);
        print_block("Non-bottom state", B, B->nonbottom_begin, B->end);
        // go to next block
        if (permutation.end() == B->end)  break;
        B = (*B->end)->block;
    }
}

#endif // ifndef NDEBUG





/* ************************************************************************* */
/*                                                                           */
/*                           T R A N S I T I O N S                           */
/*                                                                           */
/* ************************************************************************* */





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
    assert(!action_block_iter->succ.is_null());
    succ_iter_t const old_succ_pos = action_block_iter->succ.iter;
    succ_iter_t out_slice_begin = old_succ_pos->out_slice_begin();
    assert(!out_slice_begin->begin_or_before_end.is_null());
    succ_iter_t const new_succ_pos = out_slice_begin->begin_or_before_end.iter;
    assert(!new_succ_pos->begin_or_before_end.is_null());
    assert(out_slice_begin == new_succ_pos->begin_or_before_end.iter);
    assert(new_succ_pos <
                    old_succ_pos->block_bunch->pred->source->succ_inert.begin);

    // move the transition to the end of its out-slice
    if (old_succ_pos < new_succ_pos)
    {
        std::swap(old_succ_pos->block_bunch, new_succ_pos->block_bunch);
        old_succ_pos->block_bunch->pred->action_block->succ.iter=old_succ_pos;
        assert(action_block_iter ==
                                new_succ_pos->block_bunch->pred->action_block);
        action_block_iter->succ.iter = new_succ_pos;
    }
    else  assert(old_succ_pos == new_succ_pos);

    // adapt the old out-slice immediately
        // If the old out-slice becomes empty, then out_slice_begin ==
        // new_succ_pos, so the two following assignments will assign the same
        // variable.  The second assignment is the relevant one.
    out_slice_begin->begin_or_before_end.iter = new_succ_pos - 1;

    // adapt the new out-slice, as far as is possible now:
        // make the begin_or_before_end pointers of the first and last
        // transition in the slice correct immediately.  The other
        // begin_or_before_end pointers need to be corrected after all
        // transitions in the new bunch have been positioned correctly.
    if (first_transition_of_state)
    {
        new_succ_pos->begin_or_before_end.iter = new_succ_pos;
        new_succ_pos->block_bunch->pred->source->current_out_slice =
                                                               out_slice_begin;
    }
    else
    {
        succ_iter_t out_slice_before_end =
                                        new_succ_pos[1].out_slice_before_end();
        assert(new_succ_pos < out_slice_before_end);
        assert(out_slice_before_end <
                    new_succ_pos->block_bunch->pred->source->succ_inert.begin);
        assert(!out_slice_before_end->begin_or_before_end.is_null());
        assert(new_succ_pos+1==out_slice_before_end->begin_or_before_end.iter);
        assert(out_slice_begin ==
                   new_succ_pos->block_bunch->pred->source->current_out_slice);
        assert(out_slice_before_end->bunch() == new_succ_pos->bunch());
        out_slice_before_end->begin_or_before_end.iter = new_succ_pos;
        new_succ_pos->begin_or_before_end.iter = out_slice_before_end;
    }

    /*  -  -  -  -  -  -  -  adapt part_tr.block_bunch  -  -  -  -  -  -  -  */

    assert(unstable_block_bunch.empty());
    assert(new_succ_pos == action_block_iter->succ.iter);
    block_bunch_iter_t const old_block_bunch_pos = new_succ_pos->block_bunch;
    assert(!old_block_bunch_pos->slice.is_null());
    block_bunch_slice_iter_t const old_block_bunch_slice =
                                               old_block_bunch_pos->slice.iter;
    block_bunch_iter_t const new_block_bunch_pos=old_block_bunch_slice->end-1;

    // move the transition to the end of its block_bunch-slice
    if (old_block_bunch_pos < new_block_bunch_pos)
    {
        std::swap(old_block_bunch_pos->pred, new_block_bunch_pos->pred);
        old_block_bunch_pos->pred->action_block->succ.iter->block_bunch =
                                                           old_block_bunch_pos;
        assert(new_succ_pos ==
                           new_block_bunch_pos->pred->action_block->succ.iter);
        new_succ_pos->block_bunch = new_block_bunch_pos;
    }
    else  assert(new_block_bunch_pos == old_block_bunch_pos);

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

    assert(!action_block_iter->succ.is_null());
    succ_iter_t const new_succ_pos = action_block_iter->succ.iter;
    assert(!new_succ_pos->begin_or_before_end.is_null());
    assert(!new_succ_pos->begin_or_before_end.iter->
                                                begin_or_before_end.is_null());
    succ_iter_t const new_begin_or_before_end =
              new_succ_pos->begin_or_before_end.iter->begin_or_before_end.iter;
    state_info_ptr const source = new_succ_pos->block_bunch->pred->source;

    if (new_begin_or_before_end < new_succ_pos)
    {
        new_succ_pos->begin_or_before_end.iter = new_begin_or_before_end;
    }
    #ifndef NDEBUG
        else
        {
            assert(new_begin_or_before_end == new_succ_pos);
            assert(!new_begin_or_before_end->begin_or_before_end.is_null());
            succ_const_iter_t new_before_end =
                             new_begin_or_before_end->begin_or_before_end.iter;
            if (new_begin_or_before_end <= new_before_end)
            {
                assert(source->current_out_slice <= new_begin_or_before_end);
                // This is the first transition in the new out-slice.  Test
                // whether it is sorted according to bunch order.
                assert(new_begin_or_before_end == source->succ.begin ||
                            *new_begin_or_before_end[-1].bunch() < *new_bunch);
                assert(new_before_end == source->succ_cend() ||
                                      *new_bunch < *new_before_end[1].bunch());
            }
        }
    #endif

    /*  -  -  -  -  -  -  -  adapt part_tr.block_bunch  -  -  -  -  -  -  -  */

    block_bunch_iter_t const new_block_bunch_pos = new_succ_pos->block_bunch;
    assert(!new_block_bunch_pos->slice.is_null());
    block_bunch_slice_iter_t const old_block_bunch_slice =
                                               new_block_bunch_pos->slice.iter;
    #ifndef NDEBUG
        if (new_bunch == old_block_bunch_slice->bunch)
        {
            // the block_bunch-slice has been updated in an earlier call.
            assert(new_block_bunch_pos + 1 < old_block_bunch_slice->end);
            assert(!new_block_bunch_pos[1].slice.is_null());
            assert(old_block_bunch_slice == new_block_bunch_pos[1].slice.iter);
        }
        else
        {
            assert(*old_block_bunch_slice->bunch < *new_bunch);
            assert(old_block_bunch_slice->end <= new_block_bunch_pos);
        }
    #endif
    if (new_block_bunch_pos + 1 < block_bunch_inert_begin &&
                  (assert(!new_block_bunch_pos[1].slice.is_null()),
                   old_block_bunch_slice == new_block_bunch_pos[1].slice.iter))
    {
        // This transition is not the last in the block_bunch-slice.
        return;
    }

    // This transition is the last in the block_bunch-slice.  We do all
    // necessary work for this block_bunch-slice upon finding this
    // transition.  Because every block_bunch-slice has exactly one such
    // transition, all work is executed exactly once.
    assert(old_block_bunch_slice->is_stable());
    if (old_block_bunch_slice->empty())
    {
        // The old block_bunch-slice is empty, reuse it as the new one.
        old_block_bunch_slice->end = new_block_bunch_pos + 1;
        old_block_bunch_slice->bunch = new_bunch;

        // Because now every bottom state has a transition in the new bunch,
        // and no state has a transition in the old bunch, there is no need
        // to refine this block.  So we don't make this block_bunch-slice
        // unstable.
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
        {
            assert(block_bunch_iter->slice.iter == old_block_bunch_slice);
            block_bunch_iter->slice.iter = new_block_bunch_slice;
        }
        while (++block_bunch_iter <= new_block_bunch_pos);

        return;
    }

    // The old block_bunch-slice becomes unstable, and the new
    // block_bunch-slice is created unstable.

    // Move the old block_bunch-slice to the unstable slices.
    unstable_block_bunch.splice(unstable_block_bunch.begin(),
                     source->block->stable_block_bunch, old_block_bunch_slice);
    unstable_block_bunch.emplace_front(new_block_bunch_pos+1, new_bunch,false);
    new_block_bunch_slice = unstable_block_bunch.begin();

    // set the pointers accordingly
    block_bunch_iter_t block_bunch_iter = old_block_bunch_slice->end;
    do
    {
        assert(block_bunch_iter->slice.iter == old_block_bunch_slice);
        block_bunch_iter->slice.iter = new_block_bunch_slice;
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
succ_iter_t part_trans_t::move_out_slice_to_new_block(succ_iter_t const
                                       out_slice_end, block_t* const old_block)
{
    state_info_ptr const source = out_slice_end[-1].block_bunch->pred->source;
    assert(out_slice_end <= source->succ_inert.begin);
    assert(!out_slice_end[-1].begin_or_before_end.is_null());
    succ_iter_t out_slice_begin = out_slice_end[-1].begin_or_before_end.iter;
    assert(out_slice_begin < out_slice_end);
    assert(source->succ.begin <= out_slice_begin);
    block_t* new_block = source->block;

    block_bunch_iter_t old_block_bunch_pos = out_slice_begin->block_bunch;
    assert(!old_block_bunch_pos->slice.is_null());
    block_bunch_slice_iter_t old_block_bunch_slice =
                                               old_block_bunch_pos->slice.iter;
    block_bunch_iter_t after_new_block_bunch_pos = old_block_bunch_slice->end;
    block_bunch_slice_iter_t new_block_bunch_slice;
    if (after_new_block_bunch_pos >= block_bunch_inert_begin ||
           (assert(!after_new_block_bunch_pos->slice.is_null()),
            new_block_bunch_slice = after_new_block_bunch_pos->slice.iter,
            new_block != after_new_block_bunch_pos->pred->source->block ||
                 old_block_bunch_slice->bunch != new_block_bunch_slice->bunch))
    {
        // the new block_bunch-slice is not suitable; create a new one
        // and insert it into the correct list.
        if (old_block_bunch_slice->is_stable())
        {
            // source is (normally) a bottom state:
            assert(source->pos < new_block->nonbottom_begin);
            new_block->stable_block_bunch.emplace_front(
               old_block_bunch_slice->end, old_block_bunch_slice->bunch, true);
            new_block_bunch_slice = new_block->stable_block_bunch.begin();
        }
        else
        {
            new_block_bunch_slice = unstable_block_bunch.emplace(
               std::next(old_block_bunch_slice), old_block_bunch_slice->end,
                                          old_block_bunch_slice->bunch, false);
        }
    }

    // move all transitions in this out-slice to the new block_bunch
    do
    {
        --after_new_block_bunch_pos;
        assert(!old_block_bunch_pos->slice.is_null());
        assert(old_block_bunch_pos->slice.iter->bunch ==
                                                 old_block_bunch_slice->bunch);
        after_new_block_bunch_pos->slice.iter = new_block_bunch_slice;
        std::swap(old_block_bunch_pos->pred, after_new_block_bunch_pos->pred);
        assert(!old_block_bunch_pos->pred->action_block->succ.is_null());
        old_block_bunch_pos->pred->action_block->
                                  succ.iter->block_bunch = old_block_bunch_pos;
        assert(!after_new_block_bunch_pos->pred->action_block->succ.is_null());
        assert(out_slice_begin ==
                     after_new_block_bunch_pos->pred->action_block->succ.iter);
        out_slice_begin->block_bunch = after_new_block_bunch_pos;
    }
    while (++out_slice_begin < out_slice_end &&
                   (old_block_bunch_pos = out_slice_begin->block_bunch, true));
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
        {
            assert(unstable_block_bunch_postprocess_end !=
                                                        old_block_bunch_slice);
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
    assert(!action_block_slice_begin->begin_or_before_end.is_null());
    action_block_iter_t const new_action_block_pos =
                            action_block_slice_begin->begin_or_before_end.iter;
    assert(!new_action_block_pos->begin_or_before_end.is_null());
    assert(action_block_slice_begin ==
                               new_action_block_pos->begin_or_before_end.iter);

    // move the transition to the end of the action_block-slice
    if (old_action_block_pos < new_action_block_pos)
    {
        std::swap(old_action_block_pos->succ.iter,
                                              new_action_block_pos->succ.iter);
        old_action_block_pos->succ.iter->block_bunch->pred->action_block =
                                                          old_action_block_pos;
        assert(pred_iter==new_action_block_pos->succ.iter->block_bunch->pred);
        pred_iter->action_block = new_action_block_pos;
    }
    else  assert(old_action_block_pos == new_action_block_pos);

    // adapt the old action_block-slice immediately
        // If the old action_block-slice becomes empty, then
        // action_block_slice_begin == new_action_block_pos, so the two
        // following assignments will assign the same variable.  The second
        // assignment is the relevant one.
    action_block_slice_begin->begin_or_before_end.iter=new_action_block_pos-1;

    // adapt the new action_block-slice, as far as is possible now
        // make the begin_or_before_end pointers of the first and last
        // transition in the slice correct immediately.  The other
        // begin_or_before_end pointers need to be corrected after all
        // transitions in the new bunch have been positioned correctly.
    assert(pred_iter == new_action_block_pos->succ.iter->block_bunch->pred);
    if (new_action_block_pos + 1 >= action_block_inert_begin ||
        new_action_block_pos[1].succ.is_null() ||
        new_action_block_pos[1].succ.iter->block_bunch->pred->target->block !=
                                                      pred_iter->target->block)
    {
        // This is the first transition that moves to this new
        // action_block-slice.
        new_action_block_pos->begin_or_before_end.iter = new_action_block_pos;
    }
    else
    {
        action_block_iter_t const action_block_slice_before_end =
                       new_action_block_pos[1].action_block_slice_before_end();
        assert(new_action_block_pos < action_block_slice_before_end);
        assert(action_block_slice_before_end < action_block_inert_begin);
        assert(!action_block_slice_before_end->succ.is_null());
        assert(action_block_slice_before_end->succ.iter->block_bunch->
                              pred->target->block == pred_iter->target->block);
        assert(!action_block_slice_before_end->begin_or_before_end.is_null());
        assert(new_action_block_pos + 1 ==
                      action_block_slice_before_end->begin_or_before_end.iter);
        action_block_slice_before_end->begin_or_before_end.iter =
                                                          new_action_block_pos;
        new_action_block_pos->begin_or_before_end.iter =
                                                 action_block_slice_before_end;
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
    assert(!new_action_block_pos->begin_or_before_end.is_null());
    assert(!new_action_block_pos->
                      begin_or_before_end.iter->begin_or_before_end.is_null());
    action_block_iter_t const new_begin_or_before_end = new_action_block_pos->
                            begin_or_before_end.iter->begin_or_before_end.iter;
    if (new_begin_or_before_end < new_action_block_pos)
    {
        assert(new_action_block_pos->begin_or_before_end ==
                                 new_begin_or_before_end->begin_or_before_end);
        assert(new_action_block_pos <=
                               new_action_block_pos->begin_or_before_end.iter);
        new_action_block_pos->begin_or_before_end.iter=new_begin_or_before_end;
    }
    else   assert(new_begin_or_before_end == new_action_block_pos);
}


/// \brief adapt data structures for a transition that has become non-inert
/// \returns true iff the state became a new bottom state
bool part_trans_t::make_noninert(pred_iter_t const old_pred_pos,
                                            bunch_t** const new_noninert_bunch)
{
    action_block_iter_t const old_action_block_pos=old_pred_pos->action_block;
    assert(!old_action_block_pos->succ.is_null());
    succ_iter_t const old_succ_pos = old_action_block_pos->succ.iter;
    block_bunch_iter_t const old_block_bunch_pos = old_succ_pos->block_bunch;
    assert(old_pred_pos == old_block_bunch_pos->pred);

    state_info_ptr source = old_pred_pos->source;
    assert(source->pos >= source->block->nonbottom_begin);
    state_info_ptr target = old_pred_pos->target;

    pred_iter_t const new_pred_pos = target->pred_inert.begin;
    action_block_iter_t new_action_block_pos = action_block_inert_begin++;
    succ_iter_t const new_succ_pos = source->succ_inert.begin;
    block_bunch_iter_t const new_block_bunch_pos = block_bunch_inert_begin++;

    // adapt pred
    assert(new_pred_pos <= old_pred_pos);
    *old_pred_pos = *new_pred_pos;
    assert(!old_pred_pos->action_block->succ.is_null());
    old_pred_pos->action_block->succ.iter->block_bunch->pred = old_pred_pos;
    new_pred_pos->action_block = new_action_block_pos;
    new_pred_pos->source = source;
    new_pred_pos->target = target;
    target->pred_inert.begin = new_pred_pos + 1;

    // adapt action_block
    assert(new_action_block_pos <= old_action_block_pos);
    old_action_block_pos->succ = new_action_block_pos->succ;
    assert(old_action_block_pos->begin_or_before_end.is_null());
    assert(!old_action_block_pos->succ.is_null());
    old_action_block_pos->succ.iter->block_bunch->pred->action_block =
                                                          old_action_block_pos;
    new_action_block_pos->succ.iter = new_succ_pos;
    // new_action_block_pos->begin_or_before_end.iter = ...; -- see below

    // adapt succ
    assert(new_succ_pos <= old_succ_pos);
    old_succ_pos->block_bunch = new_succ_pos->block_bunch;
    assert(old_succ_pos->begin_or_before_end.is_null());
    new_succ_pos->block_bunch = new_block_bunch_pos;
    // new_succ_pos->begin_or_before_end.iter = ...; -- see below
    source->succ_inert.begin = new_succ_pos + 1;

    // mark the state and make it a bottom state if necessary
    bool became_bottom = false;
    if (source->succ_end() == source->succ_inert.begin)
    {
        // make the state a marked bottom state
        if (source->pos >= source->block->marked_nonbottom_begin)
        {
            swap(source->pos, source->block->marked_nonbottom_begin++);
        }
        assert(source->pos < source->block->marked_nonbottom_begin);
        swap(source->pos, source->block->nonbottom_begin++);
        became_bottom = true;
    }
    else  source->block->mark_nonbottom(source);

    // adapt block_bunch
    assert(new_block_bunch_pos <= old_block_bunch_pos);
    old_block_bunch_pos->pred = new_block_bunch_pos->pred;
    assert(!old_block_bunch_pos->slice.is_null());
    // new_block_bunch_pos->slice = ...; -- see below

    if (nullptr != *new_noninert_bunch)
    {
        assert((*new_noninert_bunch)->begin < new_action_block_pos);
        assert(new_action_block_pos == (*new_noninert_bunch)->end);
        (*new_noninert_bunch)->end = action_block_inert_begin;

        // even if there is a bunch, it may be necessary to create a new
        // action_block-slice!  Namely if the goal block is different.
        assert(action_block.begin() < new_action_block_pos);
        if (new_action_block_pos[-1].succ.is_null() ||
            new_action_block_pos[-1].succ.iter->
                             block_bunch->pred->target->block != target->block)
        {
            // create a new action_block-slice
            new_action_block_pos->begin_or_before_end.iter =
                                                          new_action_block_pos;

            assert((*new_noninert_bunch)->is_trivial());
            (*new_noninert_bunch)->make_nontrivial();
        }
        else
        {
            // enlarge the action_block-slice
            assert(!new_action_block_pos[-1].begin_or_before_end.is_null());
            action_block_iter_t const action_block_slice_begin =
                         new_action_block_pos[-1].begin_or_before_end.iter;
            assert(!action_block_slice_begin->begin_or_before_end.is_null());
            assert(new_action_block_pos - 1 ==
                           action_block_slice_begin->begin_or_before_end.iter);
            action_block_slice_begin->begin_or_before_end.iter =
                                                          new_action_block_pos;
            new_action_block_pos->begin_or_before_end.iter =
                                                      action_block_slice_begin;
        }

        // adapt out-slice
        if (new_succ_pos == source->succ.begin ||
                               new_succ_pos[-1].bunch() != *new_noninert_bunch)
        {
            // the out-slice is unsuitable.
            block_bunch_slice_iter_t new_block_bunch_slice =
                                                  unstable_block_bunch.begin();
            assert(unstable_block_bunch.end() != new_block_bunch_slice);
            assert(!new_block_bunch_slice->is_stable());
            if (new_block_bunch_slice->bunch == *new_noninert_bunch)
            {
                // The block_bunch-slice is suitable.  Enlarge it.
                new_block_bunch_pos->slice.iter = new_block_bunch_slice;
                assert(new_block_bunch_slice->end == new_block_bunch_pos);
                new_block_bunch_slice->end = new_block_bunch_pos + 1;

                // create a new out-slice
                assert(new_succ_pos == source->succ.begin ||
                             *new_succ_pos[-1].bunch() < **new_noninert_bunch);
                new_succ_pos->begin_or_before_end.iter = new_succ_pos;
                return became_bottom;
            }
        }
        else
        {
            // the last out-slice is already in new_noninert_bunch
            // Copy the block_bunch-slice from there and enlarge it.
            new_block_bunch_pos->slice = new_succ_pos[-1].block_bunch->slice;
            assert(!new_block_bunch_pos->slice.is_null());
            assert(unstable_block_bunch.begin() ==
                                              new_block_bunch_pos->slice.iter);
            assert(!new_block_bunch_pos->slice.iter->is_stable());
            assert(new_block_bunch_pos->slice.iter->end==new_block_bunch_pos);
            new_block_bunch_pos->slice.iter->end = new_block_bunch_pos + 1;

            // enlarge the out-slice
            assert(!new_succ_pos[-1].begin_or_before_end.is_null());
            succ_iter_t out_slice_begin =
                                     new_succ_pos[-1].begin_or_before_end.iter;
            assert(!out_slice_begin->begin_or_before_end.is_null());
            assert(new_succ_pos-1==out_slice_begin->begin_or_before_end.iter);
            out_slice_begin->begin_or_before_end.iter = new_succ_pos;
            new_succ_pos->begin_or_before_end.iter = out_slice_begin;
            return became_bottom;
        }
    }
    else
    {
        // create a new bunch for noninert transitions
        *new_noninert_bunch = new bunch_t(new_action_block_pos,
                                  action_block_inert_begin,
                                  new_action_block_pos - action_block.begin());

        // create a new action_block-slice
        new_action_block_pos->begin_or_before_end.iter = new_action_block_pos;
    }

    // create a new block_bunch-slice
    unstable_block_bunch.emplace_front(block_bunch_inert_begin,
                                                   *new_noninert_bunch, false);
    new_block_bunch_pos->slice.iter = unstable_block_bunch.begin();

    // create a new out-slice
    assert(new_succ_pos == source->succ.begin ||
                             *new_succ_pos[-1].bunch() < **new_noninert_bunch);
    new_succ_pos->begin_or_before_end.iter = new_succ_pos;
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
                        block_t* const old_block, bunch_t* new_noninert_bunch,
                                    enum new_block_mode_t const new_block_mode)
{
    // We begin with a bottom state so the new block gets a sorted list of
    // stable block_bunch-slices.
    permutation_iter_t s_iter = new_block->begin;
    assert(s_iter < new_block->end);
    do
    {
        state_info_ptr s = *s_iter;
        assert(new_block == s->block);

        /*-  -  -  -  -  -  -  adapt part_tr.block_bunch  -  -  -  -  -  -  -*/

        for (succ_iter_t succ = s->succ_inert.begin; s->succ.begin < succ; )
        {
            succ = move_out_slice_to_new_block(succ, old_block);
        }
        // If new_block_is_red == new_block_mode, then the last_splitter slice
        // moves completely from the old to the new block.  We could optimize
        // the above loop and move last_splitter to a suitable place in the
        // stable block_bunch-slices of new_block -- perhaps later.

        /*  -  -  -  -  -  -  adapt part_tr.action_block  -  -  -  -  -  -  */

        for (pred_iter_t pred=s->pred.begin; pred<s->pred_inert.begin; ++pred)
        {
            first_move_transition_to_new_action_block(pred);
        }
    }
    while (++s_iter < new_block->end);

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
    {
        // There are no inert transitions at all, so no inert transition can
        // become non-inert.
        assert(action_block_inert_begin == action_block.end());
        return;
    }
    assert(block_bunch_inert_begin < block_bunch.end());
    assert(block_bunch.end() - block_bunch_inert_begin ==
                                action_block.end() - action_block_inert_begin);

    assert(0 == new_block->marked_size());
    if (new_block_is_blue == new_block_mode)
    {
        #ifndef NDEBUG
            assert(new_block->end < block_t::perm_end);
            block_t* red_block = (*new_block->end)->block;
            assert(0 == red_block->marked_size());
        #endif
        permutation_iter_t target_iter = new_block->begin;
        do
        {
            state_info_ptr target = *target_iter;
            // check all incoming inert transitions of s, whether they
            // still start in new_block
            for (pred_iter_t pred_iter = target->pred_inert.begin;
                                   pred_iter < target->pred_end(); ++pred_iter)
            {
                if (new_block != pred_iter->source->block)
                {
                    assert(red_block == pred_iter->source->block);
                    make_noninert(pred_iter, &new_noninert_bunch);
                }
            }
        }
        while (++target_iter < new_block->end);
        assert(0 < red_block->bottom_size());
    }
    else
    {
        assert(new_block_is_red == new_block_mode);
        #ifndef NDEBUG
            assert(block_t::perm_begin < new_block->begin);
            block_t* blue_block = new_block->begin[-1]->block;
            assert(0 < blue_block->bottom_size());
            assert(0 == blue_block->marked_size());
        #endif

        // We have to be careful because make_noninert may make a state move
        // either forward (to the marked states) or back (to the bottom
        // states).
        for (permutation_iter_t source_iter = new_block->end;
                                    new_block->nonbottom_begin < source_iter; )
        {
            --source_iter;
            state_info_ptr source = *source_iter;
            // check all outgoing inert transitions of s, whether they still
            // end in new_block.
            succ_iter_t succ_iter = source->succ_inert.begin;
            assert(succ_iter < source->succ_end());
            do
            {
                if (new_block != succ_iter->block_bunch->pred->target->block)
                {
                    assert(blue_block ==
                                  succ_iter->block_bunch->pred->target->block);
                    // the transition becomes non-inert
                    if (make_noninert(succ_iter->block_bunch->pred,
                                                          &new_noninert_bunch))
                    {
                        // the state becomes a bottom state and is moved to the
                        // beginning; in the place of source_iter, now there is
                        // another state.
                        assert(source != *source_iter);
                        ++source_iter;
                        assert(source_iter<=new_block->marked_nonbottom_begin);
                        // This can only happen upon handling the last
                        // non-inert transition.
                        assert(succ_iter + 1 == source->succ_end());
                        break;
                    }
                }
            }
            while (++succ_iter < source->succ_end());
        }
    }
}

#ifndef NDEBUG
    /// \brief print all transitions
    /// \details Transitions are printed organised into bunches.
    template <class LTS_TYPE>
    void part_trans_t::print_trans(
                      const bisim_partitioner_dnj<LTS_TYPE>& partitioner) const
    {
        if (action_block.begin() == action_block.end())
        {
            mCRL2log(log::debug, "bisim_dnj") << "No transitions.\n";
            return;
        }
        // for all bunches
        action_block_const_iter_t bunch_begin = action_block.begin();
        assert(!bunch_begin->succ.is_null());
        for (;;)
        {
            const bunch_t* const bunch = bunch_begin->succ.iter->bunch();
            action_block_const_iter_t bunch_end;
            action_block_const_iter_t action_block_slice_end;
            if (nullptr == bunch)
            {
                mCRL2log(log::debug, "bisim_dnj") <<"Inert transition slice [";
                action_block_slice_end = bunch_end = action_block.end();
            }
            else
            {
                mCRL2log(log::debug, "bisim_dnj") << bunch->debug_id_short()
                                                << ":\n\taction_block-slice [";
                bunch_end = bunch->end;
                assert(bunch->begin == bunch_begin);
                assert(bunch_begin < bunch_end);
                assert(bunch_end <= action_block_inert_begin);

                assert(!bunch_begin->begin_or_before_end.is_null());
                action_block_slice_end=bunch_begin->begin_or_before_end.iter+1;
            }
            // for all transition sets in Bu
            action_block_const_iter_t action_block_slice_begin = bunch_begin;
            for (;;)
            {
                mCRL2log(log::debug,"bisim_dnj") << (action_block_slice_begin -
                                 action_block_entry::action_block_begin) << ","
                    << (action_block_slice_end -
                             action_block_entry::action_block_begin) << "):\n";
                // for all transitions in Tr
                for(action_block_const_iter_t tr_iter=action_block_slice_begin;
                                   tr_iter < action_block_slice_end; ++tr_iter)
                {
                    assert(!tr_iter->succ.is_null());
                    mCRL2log(log::debug, "bisim_dnj") << "\t\t"
                                << tr_iter->succ.iter->block_bunch->
                                           pred->debug_id(partitioner) << '\n';
                }
                // go to next transition set
                if (bunch_end <= action_block_slice_end)  break;
                action_block_slice_begin = action_block_slice_end;
                while (action_block_slice_begin->succ.is_null())
                {
                    assert(action_block_slice_begin->
                                                begin_or_before_end.is_null());
                    ++action_block_slice_begin;
                    assert(action_block_slice_begin < bunch_end);
                }
                assert(!action_block_slice_begin->
                                                begin_or_before_end.is_null());
                action_block_slice_end =
                        action_block_slice_begin->begin_or_before_end.iter + 1;
                mCRL2log(log::debug, "bisim_dnj") << "\taction_block-slice [";
            }
            // go to next bunch
            if (action_block.end() <= bunch_end)  break;
            bunch_begin = bunch_end;
            while (bunch_begin->succ.is_null())
            {
                assert(bunch_begin->begin_or_before_end.is_null());
                ++bunch_begin;
                assert(bunch_begin < action_block.end());
            }
        }
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
    void part_trans_t::assert_stability(bool const branching,
                        bool const preserve_divergence,
                        const part_state_t& part_st, const LTS_TYPE& aut) const
    {
        assert(0);
        // avoid warnings
        (void) branching;
        (void) preserve_divergence;
        (void) part_st;
        (void) aut;
    }
#endif

} // end namespace bisim_dnj





/* ************************************************************************* */
/*                                                                           */
/*                            A L G O R I T H M S                            */
/*                                                                           */
/* ************************************************************************* */





/*=============================================================================
=                               initialisation                                =
=============================================================================*/



/// \brief initialise the whole data structure for the partition refinement
/// \details The partition should always satisfy the invariant:  ``If a state s
/// has a (non-inert) transition in a bunch Bu, then every bottom state in the
/// block of s has a non-inert transition in bunch Bu.''
///
/// To this end, we have to separate the states with a non-inert transition
/// from those without.  Non-inert transitions are visible transitions (and, if
/// the bisimulation is divergence-preserving, tau-self-loops).  We mark all
/// states with such a transition and split the block.  If we find new bottom
/// states, we have to mark them as bottom states but needn't do anything else
/// because there is only one bunch and every new bottom state has a transition
/// in this bunch, so it must be stable.
///
/// Note that for divergence-preserving branching bisimulation, we only need to
/// treat the tau-self-loops as non-inert transitions.  In other texts, this
/// is sometimes described as temporarily renaming the tau-self-loops to
/// self-loops with a special label.  However, as there are no other non-inert
/// tau transitions, we can simply put them in their own slice, separate from
/// the inert tau transitions.  (It would be an error to mix the inert
/// transitions with the self-loops in the same slice.)
template <class LTS_TYPE>
void bisim_partitioner_dnj<LTS_TYPE>::create_initial_partition()
{
    log::mcrl2_logger::set_reporting_level(log::debug, "bisim_dnj");

    mCRL2log(log::verbose, "bisim_dnj") << "Strictly O(m log n) "
                   << (branching ? (preserve_divergence
                                           ? "divergence-preserving branching "
                                           : "branching ")
                                 : "")
                 << "bisimulation partitioner created for " << aut.num_states()
                 << " states and " << aut.num_transitions();

    // create one block for all states

    bisim_dnj::block_t* const B = new bisim_dnj::block_t(
                       part_st.permutation.begin(), part_st.permutation.end());

    // Iterate over the transitions to count how to order them in part_trans_t

    // counters for the non-inert outgoing and incoming transitions per state
    // are provided in part_st.state_info.  These counters have been
    // initialised to zero in the constructor of part_state_t.

    // counters for the non-inert transition per label:
    assert(action_label.size() == aut.num_action_labels());

    // counter for the total number of inert transitions:
    trans_type inert_transitions = 0;

    for (const transition& t: aut.get_transitions())
    {
        if (branching && aut.is_tau(aut.apply_hidden_label_map(t.label()))
               && (t.from() != t.to() || (assert(preserve_divergence), false)))
        {
            // The transition is inert.
            part_st.state_info[t.from()].succ_inert.count++;
            part_st.state_info[t.to()].pred_inert.count++;
            ++inert_transitions;

            // The source state should become non-bottom:
            if (part_st.state_info[t.from()].pos < B->nonbottom_begin)
            {
                swap(part_st.state_info[t.from()].pos, --B->nonbottom_begin);
                // we do not yet update the marked_bottom_begin pointer
            }
        }
        else
        {
            // The transition is non-inert.  (It may be a self-loop).
            part_st.state_info[t.from()].succ.count++;
            part_st.state_info[t.to()].pred.count++;

            assert(0 <= aut.apply_hidden_label_map(t.label()));
            assert(aut.apply_hidden_label_map(t.label())<action_label.size());
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
            next_succ_begin->begin_or_before_end.iter=state_iter->succ.begin-1;
            bisim_dnj::succ_iter_t out_slice_begin = next_succ_begin;
            while (++next_succ_begin < state_iter->succ.begin)
            {
                next_succ_begin->begin_or_before_end.iter = out_slice_begin;
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

    // create a single bunch containing all transitions

    part_tr.action_block_inert_begin =
                                part_tr.action_block.end() - inert_transitions;
    part_tr.block_bunch_inert_begin =
                                 part_tr.block_bunch.end() - inert_transitions;

    bisim_dnj::bunch_t* const bunch = new bisim_dnj::bunch_t(
            part_tr.action_block.begin(), part_tr.action_block_inert_begin, 0);

    // create a single block_bunch entry for all non-inert transitions
    B->stable_block_bunch.emplace_front(part_tr.block_bunch_inert_begin, bunch,
                                                                         true);

    // create slice descriptors in part_tr.action_block for each label

    // The action_block array shall have the tau transitions at the end: first
    // the non-inert tau transitions (during initialisation, that are only the
    // tau self-loops), then the tau transitions that have become non-inert
    // and finally the inert transitions.
    // Transitions with other labels are placed from beginning to end.
    // Every such transition block except the last one ends with a dummy entry.
    // If there are transition labels without transitions, multiple dummy
    // entries will be placed side-by-side.
    assert(part_tr.action_block.size() ==
                              aut.num_transitions() + action_label.size() - 1);

    bisim_dnj::action_block_iter_t next_action_label_begin =
                                                  part_tr.action_block.begin();
    label_type num_labels_with_transitions = 0;
    for (label_type i = 0; i < action_label.size(); ++i)
    {
        bisim_dnj::action_block_iter_t this_action_label =
                                                       next_action_label_begin;
        if (branching && aut.is_tau(aut.apply_hidden_label_map(i)))
        {
            // This is the hidden label.  Place it at the end.
            this_action_label =
                      part_tr.action_block_inert_begin - action_label[i].count;
        }
        if (0 < action_label[i].count)
        {
            if (++num_labels_with_transitions == 2)
            {
                // This is the second action_block-slice, so the bunch is not
                // yet marked as nontrivial but it should be.
                bunch->make_nontrivial();
            }
            // initialise begin_or_before_end pointers for this
            // action_block-slice
            action_label[i].begin = this_action_label + action_label[i].count;
            this_action_label->begin_or_before_end.iter =
                                                     action_label[i].begin - 1;
            bisim_dnj::action_block_iter_t action_block_slice_begin =
                                                             this_action_label;
            while (++this_action_label < action_label[i].begin)
            {
                this_action_label->begin_or_before_end.iter =
                                                      action_block_slice_begin;
            }
        }
        if (this_action_label < part_tr.action_block_inert_begin)
        {
            // insert a dummy entry
            this_action_label->succ.clear();
            this_action_label->begin_or_before_end.clear();
            ++this_action_label;
            next_action_label_begin = this_action_label;
        }
        else if (i + 1 < action_label.size())
        {
            assert(branching);
            assert(aut.is_tau(aut.apply_hidden_label_map(i)));
        }
    }
    assert(next_action_label_begin == part_tr.action_block_inert_begin -
                  (branching ? action_label[aut.tau_label_index()].count : 0));

    // distribute the transitions over the data structures

    bisim_dnj::block_bunch_iter_t next_block_bunch=part_tr.block_bunch.begin();
    for (const transition& t: aut.get_transitions())
    {
        assert(part_st.state_info[t.from()].block == B);

        bisim_dnj::succ_iter_t succ_pos;
        bisim_dnj::block_bunch_iter_t block_bunch_pos;
        bisim_dnj::pred_iter_t pred_pos;
        bisim_dnj::action_block_iter_t action_block_pos;

        if (branching && aut.is_tau(aut.apply_hidden_label_map(t.label()))
               && (t.from() != t.to() || (assert(preserve_divergence), false)))
        {
            // It is a (normal) inert transition: place near the end of the
            // respective pred/succ slices, just before the other inert
            // transitions.
            succ_pos = --part_st.state_info[t.from()].succ_inert.begin;
            block_bunch_pos = part_tr.block_bunch.end() - inert_transitions;
            pred_pos = --part_st.state_info[t.to()].pred_inert.begin;
            action_block_pos = part_tr.action_block.end() - inert_transitions;
            --inert_transitions;

            assert(block_bunch_pos >= part_tr.block_bunch_inert_begin);
            assert(action_block_pos >= part_tr.action_block_inert_begin);
            assert(succ_pos->begin_or_before_end.is_null());

            action_block_pos->begin_or_before_end.clear();
            block_bunch_pos->slice.clear();
        }
        else
        {
            // It is a non-inert transition (possibly a self-loop): place at
            // the end of the respective pred/succ slices
            succ_pos = --part_st.state_info[t.from()].succ.begin;
            block_bunch_pos = next_block_bunch++;
            pred_pos = --part_st.state_info[t.to()].pred.begin;
            action_block_pos =
                   --action_label[aut.apply_hidden_label_map(t.label())].begin;

            assert(succ_pos->out_slice_begin() <= succ_pos);
            assert(succ_pos <= succ_pos->out_slice_before_end());
            assert(action_block_pos->action_block_slice_begin() <=
                                                             action_block_pos);
            assert(action_block_pos <=
                            action_block_pos->action_block_slice_before_end());
            assert(action_block_pos < part_tr.action_block_inert_begin);

            block_bunch_pos->slice.iter = B->stable_block_bunch.begin();
        }

        succ_pos->block_bunch = block_bunch_pos;
        block_bunch_pos->pred = pred_pos;
        pred_pos->action_block = action_block_pos;
        pred_pos->source = &part_st.state_info[t.from()];
        pred_pos->target = &part_st.state_info[t.to()];
        action_block_pos->succ.iter = succ_pos;

        assert(pred_pos->source->current_out_slice <= succ_pos);
    }
    assert(0 == inert_transitions);
    assert(next_block_bunch == part_tr.block_bunch_inert_begin);

    // delete transitions already -- they are no longer needed.  We will add
    // new transitions at the end.
    aut.clear_transitions();

    mCRL2log(log::verbose, "bisim_dnj") << " transitions\n";

    // now split the blocks (if it has marked states) into the part with
    // non-inert transitions and the part without.
    if (0 < B->marked_size() && 1 < B->size())
    {
        bisim_dnj::block_t* const red_block = refine(B,
                /* splitter block_bunch */ B->stable_block_bunch.begin(),
                /* bunch for new non-inert transitions == */ bunch,
                                   bisim_dnj::extend_from_state_markings_only);
        // We can ignore possible new non-inert transitions, as every red
        // bottom state already has a transition in Bu.
        red_block->marked_nonbottom_begin = red_block->end;
        red_block->marked_bottom_begin = red_block->nonbottom_begin;
    }
}


/// replace_transition_system() replaces the transitions of the LTS stored
/// here by those of its bisimulation quotient.  It also updates the number of
/// states, the state labels and the initial state.
template <class LTS_TYPE>
void bisim_partitioner_dnj<LTS_TYPE>::replace_transition_system()
{
    // store the labels with the action_block-slices
    // We misuse the field succ.null of the first entry of each
    // action_block-slice for this purpose.

    assert(sizeof(label_type) <= sizeof(const void*));
    for (label_type i = 0; i + 1 <= action_label.size(); ++i)
    {
        bisim_dnj::action_block_const_iter_t action_block_iter_end =
                      i + 1 >= action_label.size() ? part_tr.action_block.end()
                                                   : action_label[i + 1].begin;
        for (bisim_dnj::action_block_iter_t
           action_block_iter = action_label[i].begin;
           action_block_iter < action_block_iter_end;
           action_block_iter = action_block_iter->begin_or_before_end.iter + 1)
        {
            assert(!action_block_iter->succ.is_null());
            assert(!action_block_iter->begin_or_before_end.is_null());
            action_block_iter->succ.null =
                                   reinterpret_cast<const void*>((intptr_t) i);
            assert(action_block_iter <=
                                  action_block_iter->begin_or_before_end.iter);
        }
    }

    // for all blocks
    bisim_dnj::permutation_const_iter_t s_iter = part_st.permutation.begin();
    do
    {
        const bisim_dnj::block_t* const B = (*s_iter)->block;
        // for all block_bunch-slices of the block
        for(bisim_dnj::block_bunch_slice_const_iter_t
                    trans_iter = B->stable_block_bunch.begin();
                       B->stable_block_bunch.end() != trans_iter; ++trans_iter)
        {
            assert(trans_iter->is_stable());
            assert(!trans_iter->empty());
            bisim_dnj::pred_const_iter_t const pred = trans_iter->end[-1].pred;
            // add a transition from the source block to the goal block with
            // the indicated label.
            label_type label = reinterpret_cast<intptr_t>(
                    pred->action_block->action_block_slice_begin()->succ.null);
            assert(0 <= label);
            assert(label < action_label.size());
            aut.add_transition(transition(B->seqnr, label,
                                                  pred->target->block->seqnr));
        }
        s_iter = B->end;
    }
    while (s_iter < part_st.permutation.end());

    // Merge the states, by setting the state labels of each state to the concatenation of the state labels of its
    // equivalence class.

    if (aut.has_state_info())   /* If there are no state labels this step can be ignored */
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



template <class LTS_TYPE>
void bisim_partitioner_dnj<LTS_TYPE>::
                                     refine_partition_until_it_becomes_stable()
{
    // while there is a nontrivial bunch
    for (;;)
    {
        #ifndef NDEBUG
            if (mCRL2logEnabled(log::debug, "bisim_dnj"))
            {
                part_st.print_part();
                part_tr.print_trans(*this);
            }

            part_tr.assert_stability(branching, preserve_divergence, part_st,
                                                                          aut);
        #endif

        /*------------------------- find a splitter -------------------------*/

        // select a nontrivial bunch splitter_bunch
        bisim_dnj::bunch_t* const splitter_bunch =
                                     bisim_dnj::bunch_t::get_some_nontrivial();
        if (nullptr == splitter_bunch)  break;

        #ifndef NDEBUG
            mCRL2log(log::debug, "bisim_dnj") << "Refining "
                                         << splitter_bunch->debug_id() << '\n';
        #endif

        // select a small action_block-slice splitter_slice in splitter_bunch
        // move splitter_slice from splitter_bunch to a new bunch
        bisim_dnj::bunch_t* const new_bunch =
                          splitter_bunch->split_off_small_action_block_slice();
        #ifndef NDEBUG
            mCRL2log(log::debug, "bisim_dnj") << "Splitting off "
                                              << new_bunch->debug_id() << '\n';
        #endif

        /*----------------- find predecessors of new_bunch ------------------*/

        // for all transitions in new_bunch
        for (bisim_dnj::action_block_iter_t splitter_iter = new_bunch->begin;
                               splitter_iter < new_bunch->end; ++splitter_iter)
        {
            assert(!splitter_iter->succ.is_null());
            bisim_dnj::state_info_entry* const source =
                           splitter_iter->succ.iter->block_bunch->pred->source;
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
                                          part_tr.unstable_block_bunch.begin();
            assert(!splitter->is_stable());
            assert(splitter->bunch == new_bunch);
            bisim_dnj::block_t* refine_block = splitter->source_block();
            assert(1 < refine_block->size());
            // make the splitter stable again:  Insert it
            // in the stable block_bunch-slice list of refine_block.
            refine_block->stable_block_bunch.splice(
                    refine_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
            splitter->make_stable();
            // a few assertions to test whether the next splitter actually
            // belongs to this one:
            assert(*part_tr.unstable_block_bunch.front().bunch <
                                                             *splitter->bunch);
            assert(part_tr.unstable_block_bunch.front().source_block() ==
                                                                 refine_block);

            // assume that refine_block has state markings corresponding to
            // splitter.  Split:
            bisim_dnj::permutation_iter_t refine_block_begin =
                                                           refine_block->begin;
            bisim_dnj::block_t* red_block = refine(refine_block, splitter,
                          nullptr, bisim_dnj::extend_from_state_markings_only);
            bisim_dnj::block_t* const blue_block =
                        refine_block_begin < red_block->begin
                                      ? (*refine_block_begin)->block : nullptr;
            assert(nullptr==blue_block || blue_block->end == red_block->begin);
            assert(nullptr == blue_block || 0 == blue_block->marked_size());

            // postprocess new non-inert transitions
            if (0 == red_block->marked_size() ||
                (red_block = postprocess_new_noninert(red_block, splitter)) !=
                                                                       nullptr)
            {
                // Now the first element of the unstable block_bunch-slices
                // list should be a large splitter of red_block, and it should
                // be handled using the current_out_slice pointers.
                splitter = part_tr.unstable_block_bunch.begin();
                if (part_tr.unstable_block_bunch.end() == splitter)  break;
                assert(!splitter->is_stable());
                assert(splitter->bunch == splitter_bunch);
                if (blue_block == splitter->source_block())
                {
                    // Oops... first comes the blue subblock's slice.  That is
                    // already stable.
                    blue_block->stable_block_bunch.splice(
                                blue_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
                    splitter->make_stable();
                    splitter = part_tr.unstable_block_bunch.begin();
                    if (part_tr.unstable_block_bunch.end() == splitter)  break;
                    assert(!splitter->is_stable());
                    assert(splitter->bunch == splitter_bunch);
                }
                if (red_block == splitter->source_block())
                {
                    // make the splitter stable again: Insert it in the correct
                    // place in the stable block_bunch-slice list of red_block.
                    red_block->stable_block_bunch.splice(
                                red_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
                    splitter->make_stable();

                    if (1 < red_block->size())
                    {
                        red_block = refine(red_block, splitter, nullptr,
                                          bisim_dnj::extend_from_FromRed_only);
                        // postprocess new non-inert transitions
                        if (0 < red_block->marked_size())
                        {
                            postprocess_new_noninert(red_block, splitter);
                        }
                    }
                }
            }
            // skip a possible unstable block_bunch-slice of the old bunch for
            // the blue subblock.  This slice is known to be stable already.
            splitter = part_tr.unstable_block_bunch.begin();
            if (part_tr.unstable_block_bunch.end() == splitter)  break;
            assert(!splitter->is_stable());
            if (blue_block == splitter->source_block())
            {
                assert(splitter->bunch == splitter_bunch);
                blue_block->stable_block_bunch.splice(
                            blue_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
                splitter->make_stable();
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



template <class LTS_TYPE>
bisim_dnj::block_t* bisim_partitioner_dnj<LTS_TYPE>::refine(
      bisim_dnj::block_t* const refine_block,
      bisim_dnj::block_bunch_slice_iter_t const splitter,
      bisim_dnj::bunch_t* new_noninert_bunch,
      enum bisim_dnj::refine_mode_t const mode)
{
    #ifndef NDEBUG
        mCRL2log(log::debug,"bisim_dnj") <<"refine(" <<refine_block->debug_id()
            <<',' << splitter->debug_id() << ','
            << (nullptr == new_noninert_bunch ? std::string("nullptr")
                                        : new_noninert_bunch->debug_id_short())
            << (bisim_dnj::extend_from_state_markings_only == mode
                  ? ",extend_from_state_markings_only)\n"
                  : (bisim_dnj::extend_from_FromRed_only == mode
                       ? ",extend_from_FromRed_only)\n"
                       : ",extend_from_bottom_state_markings_and_FromRed)\n"));
    #endif

    assert(1 < refine_block->size());
    assert(refine_block == splitter->source_block());
    assert(0 < refine_block->marked_size() ||
                           bisim_dnj::extend_from_state_markings_only != mode);
    assert(nullptr == new_noninert_bunch ||
                                  bisim_dnj::extend_from_FromRed_only != mode);

    bisim_dnj::block_t* red_block;
    COROUTINES_SECTION
        // shared variables of both coroutines
        bisim_dnj::permutation_iter_t notblue_initialised_end =
                                                 refine_block->nonbottom_begin;
        bool FromRed_is_handled =
                            bisim_dnj::extend_from_state_markings_only == mode;

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

                /*-  -  -  -  -  -  - collect blue bottom states -  -  -  -  -  -  -*/

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
#ifndef NDEBUG
mCRL2log(log::debug, "bisim_dnj") << s->debug_id();
#endif
                    // 3.8l: if s --> SpBu then
                    if (s->surely_has_transition_to(splitter->bunch))
                    {
mCRL2log(log::debug, "bisim_dnj") << " is not blue\n";
                        // 3.9l: Move s from Test to Red
                        // The state s is not blue.  Move it to the slice of
                        // non-blue bottom states.
                        swap(blue_visited_end,
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
                    {
mCRL2log(log::debug, "bisim_dnj") << " is blue\n";
                        assert(s->surely_has_no_transition_to(splitter->
                                                                       bunch));
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
                    blue_source = blue_pred_iter->source;
                    assert(refine_block->nonbottom_begin <= blue_source->pos);
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
                        swap(blue_source->pos, notblue_initialised_end++);
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
                                                              splitter->bunch))
                        {
                            continue;
                        }
                        if (!blue_source->surely_has_no_transition_to(
                                                              splitter->bunch))
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
                                if (*splitter->bunch <= *mid->bunch())
                                {
                                    blue_end = mid->out_slice_begin();
                                }
                                if (*mid->bunch() <= *splitter->bunch)
                                {
                                    blue_begin = mid->out_slice_before_end()+1;
                                }
                            }
                            END_COROUTINE_WHILE;
                            if (blue_begin > blue_end)  continue;
                        }
                    }
                    // 3.24l: Blue := Blue union {s_prime}
                    assert(blue_blue_nonbottom_end <= blue_source->pos);
                    assert(blue_source->pos < notblue_initialised_end);
                    swap(blue_source->pos, blue_blue_nonbottom_end++);
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

mCRL2log(log::debug, "bisim_dnj") << "The blue subblock is smaller.\n";
            // 3.28l: Abort the other coroutine
            ABORT_OTHER_COROUTINE();
            // All non-blue states are red.
            // 3.29l: Move Blue to a new block NewB
            // and
            // 3.30l: Destroy all temporary data
            refine_block->marked_nonbottom_begin = blue_blue_nonbottom_end;
            bisim_dnj::block_t* const blue_block =
                   refine_block->split_off_block(bisim_dnj::new_block_is_blue);
            part_tr.adapt_transitions_for_new_block(blue_block, refine_block,
                             new_noninert_bunch, bisim_dnj::new_block_is_blue);

            red_block = refine_block;
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
                          splitter == red_visited_begin.fromred[-1].slice.iter)
                {
                    // 3.10r: FromRed := FromRed \ {s --> t}
                    --red_visited_begin.fromred;
                    // 3.7r: Choose s --> t in FromRed
                    bisim_dnj::state_info_ptr source =
                                       red_visited_begin.fromred->pred->source;
                    assert(source->block == refine_block);
#ifndef NDEBUG
mCRL2log(log::debug, "bisim_dnj") << source->debug_id() << " is red\n";
#endif
                    // 3.8r: Test := Test \ {s}
                    // and
                    // 3.9r: Red := Red union {s}
                    if (refine_block->nonbottom_begin <= source->pos &&
                                         source->pos < notblue_initialised_end)
                    {
                        // The non-bottom state has a transition to a blue
                        // state, so notblue is initialised; however, now it is
                        // revealed to be red anyway.
                        swap(source->pos, --notblue_initialised_end);
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
                                                         red_pred_iter->source;
                    assert(refine_block->nonbottom_begin <= source->pos);
                    assert(source->pos < refine_block->end);
                    // 3.24r: Red := Red union {s_prime}
                    if (// refine_block->nonbottom_begin <= source->pos &&
                                         source->pos < notblue_initialised_end)
                    {
                        // The state has a transition to a blue state, so
                        // notblue is initialised; however, now it is revealed
                        // to be red anyway.
                        swap(source->pos, --notblue_initialised_end);
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
                if (refine_block->marked_bottom_begin==red_visited_begin.block)
                {
                    red_visited_begin.block = refine_block->end;
                }
            }
            END_COROUTINE_DO_WHILE;

            /*  -  -  -  -  -  -  - split off red block -  -  -  -  -  -  -  */

mCRL2log(log::debug, "bisim_dnj") << "The red subblock is smaller.\n";
            // 3.28r: Abort the other coroutine
            ABORT_OTHER_COROUTINE();
            // All non-red states are blue.
            // 3.29r: Move Red to a new block NewB
            // and
            // 3.30r: Destroy all temporary data
            red_block =
                    refine_block->split_off_block(bisim_dnj::new_block_is_red);
            part_tr.adapt_transitions_for_new_block(red_block, refine_block,
                              new_noninert_bunch, bisim_dnj::new_block_is_red);
        END_COROUTINE
    END_COROUTINES_SECTION
    return red_block;
}



/*=============================================================================
=                        Postprocess new bottom states                        =
=============================================================================*/



static struct {
    // variant for std::lower_bound, to compare with a bunch directly
    bool operator()(bisim_dnj::state_info_const_ptr p2,
                                                  const bisim_dnj::bunch_t* p1)
    {
        if (p2->current_out_slice >= p2->succ_inert.begin)  return true;
        return *p1 < *p2->current_out_slice->bunch();
    }

    // variant for std::sort, to compare two state_info_const_ptr values
    bool operator()(bisim_dnj::state_info_const_ptr p2,
                                            bisim_dnj::state_info_const_ptr p1)
    {
        if (p1->current_out_slice >= p1->succ_inert.begin)  return false;
        return operator()(p2, p1->current_out_slice->bunch());
    }
} current_out_slice_greater;


template <class LTS_TYPE>
bisim_dnj::block_t*bisim_partitioner_dnj<LTS_TYPE>::prepare_for_postprocessing(
                            bisim_dnj::block_t* refine_block,
                            bisim_dnj::block_bunch_slice_iter_t last_splitter,
                                                        bool first_preparation)
{
    bisim_dnj::block_t* blue_block;
    assert(refine_block == last_splitter->source_block());
    assert(last_splitter->is_stable());
    assert(part_tr.block_bunch.begin() < part_tr.block_bunch_inert_begin);
    assert(!part_tr.block_bunch_inert_begin[-1].slice.is_null());
    bisim_dnj::block_bunch_slice_iter_t new_noninert_block_bunch =
                                part_tr.block_bunch_inert_begin[-1].slice.iter;
    assert(refine_block == new_noninert_block_bunch->source_block());
    assert(new_noninert_block_bunch->is_stable());

    assert(0 < refine_block->marked_size());
    // First, split into the states that have a new noninert transition and
    // those that haven't.
    if (refine_block->begin < refine_block->marked_bottom_begin)
    {
        refine_block = refine(refine_block, new_noninert_block_bunch,
                    new_noninert_block_bunch->bunch,
                                   bisim_dnj::extend_from_state_markings_only);
        assert(part_st.permutation.begin() < refine_block->begin);
        blue_block = refine_block->begin[-1]->block;
    }
    else
    {
        refine_block->marked_bottom_begin = refine_block->nonbottom_begin;
        refine_block->marked_nonbottom_begin = refine_block->end;
        blue_block = nullptr;
    }
    assert(0 == refine_block->marked_size());
    // if this is the first call to prepare_for_postprocessing in the beginning
    // of postprocess_new_noninert, then unstable_block_bunch_postprocess_end
    // may point to a slice of the red subblock; as we stabilise for
    // all subblocks of it during postprocessing, we pull it before
    // unstable_block_bunch_postprocess_end.
    if (first_preparation && !part_tr.unstable_block_bunch.empty())
    {
        assert(part_tr.unstable_block_bunch.begin() ==
                                 part_tr.unstable_block_bunch_postprocess_end);
        if (part_tr.unstable_block_bunch_postprocess_end->source_block() ==
                                                                  refine_block)
        {
            ++part_tr.unstable_block_bunch_postprocess_end;
        }
        else if (part_tr.unstable_block_bunch.end() !=
                    std::next(part_tr.unstable_block_bunch_postprocess_end) &&
                    std::next(part_tr.unstable_block_bunch_postprocess_end)->
                                                source_block() == refine_block)
        {
            part_tr.unstable_block_bunch.splice(
                part_tr.unstable_block_bunch_postprocess_end,
                    part_tr.unstable_block_bunch,
                      std::next(part_tr.unstable_block_bunch_postprocess_end));
        }
    }
    #ifndef NDEBUG
        // assert that the remainder of the unstable block_bunch slice list
        // does not contain any block_bunch slice whose source_block() is
        // refine_block.
    #endif
    bisim_dnj::permutation_iter_t s_iter = refine_block->begin;
    assert(s_iter < refine_block->nonbottom_begin);
    do
    {
        bisim_dnj::state_info_ptr s = *s_iter;
        s->current_out_slice = s->succ.begin;
    }
    while (++s_iter < refine_block->nonbottom_begin);
    // Sort from last to first bunch
    std::sort(refine_block->begin, refine_block->nonbottom_begin,
                                                    current_out_slice_greater);

    // make all stable block_bunch-slices of refine_block unstable
    // However, the bunch of new noninert transitions and the bunch that was
    // the last splitter do not need to be handled (as long as there are no
    // further new bottom states).
    for (bisim_dnj::block_bunch_slice_iter_t block_bunch_iter =
            refine_block->stable_block_bunch.begin();
                  refine_block->stable_block_bunch.end() != block_bunch_iter; )
    {
        assert(block_bunch_iter->is_stable());
        bisim_dnj::block_bunch_slice_iter_t next_block_bunch_iter =
                                                   std::next(block_bunch_iter);
        if (block_bunch_iter != last_splitter &&
                                  block_bunch_iter != new_noninert_block_bunch)
        {
            block_bunch_iter->make_unstable();
            part_tr.unstable_block_bunch.splice(
                    part_tr.unstable_block_bunch_postprocess_end,
                           refine_block->stable_block_bunch, block_bunch_iter);
        }
        block_bunch_iter = next_block_bunch_iter;
    }
    return blue_block;
}


/// \brief Split a block with new non-inert transitions as needed
/// \details The function splits refine_block by stabilising for all bunches
/// in which it contains transitions.
///
/// When this function starts, it assumes that the states with a new non-inert
/// transition in refine_block are marked.  It is an error if it does not
/// contain any new bottom states.
/// The function implements Algorithm 4 of [GJKW 2017].  It first separates the
/// states with new non-inert transitions from those
/// without;  then it walks through all the bunches
/// that can be reached from the new bottom states to separate them into
/// smaller, stable subblocks.  The return value is the block containing the
/// old bottom states, resulting from the first separation.
/// \param RedB  block containing new bottom states that need to be stabilised
/// \returns the block containing the old bottom states (and every state in
///          refine_block that cannot reach any new non-inert transition)
template <class LTS_TYPE>
bisim_dnj::block_t* bisim_partitioner_dnj<LTS_TYPE>::postprocess_new_noninert(
                             bisim_dnj::block_t* refine_block,
                             bisim_dnj::block_bunch_slice_iter_t last_splitter)
{

    /*---------------- collect reachable block_bunch-slices -----------------*/

    // 4.2: Create an empty list R of refinable bunches
    part_tr.unstable_block_bunch_postprocess_end =
                                          part_tr.unstable_block_bunch.begin();
    bisim_dnj::block_t* const result_block =
                 prepare_for_postprocessing(refine_block, last_splitter, true);

    /*------------ stabilise w. r. t. found action_block-slices -------------*/

    bisim_dnj::block_bunch_slice_iter_t splitter;
    while ((splitter = part_tr.unstable_block_bunch.begin())
                               != part_tr.unstable_block_bunch_postprocess_end)
    {
        refine_block = splitter->source_block();
        assert(0 == refine_block->marked_size());
        assert(refine_block->begin < refine_block->nonbottom_begin);
        // check if we first have to split w. r. t. the splitter suggested
        // by the new bottom states instead:
        // (That should be another unstable block_bunch.)
        if (*splitter->bunch <
            *refine_block->nonbottom_begin[-1]->current_out_slice->bunch() ||
            (// Yes, we have to.  So let's face it:
             splitter = refine_block->nonbottom_begin[-1]->
                               current_out_slice->block_bunch->slice.iter,
             assert(part_tr.unstable_block_bunch_postprocess_end!=splitter),
             // but this splitter might already be stable (if it's equal to
             // last_splitter or to the new noninert transitions)
             !splitter->is_stable() &&
             (// No, it's not.  So we have to mark the bottom states:
              refine_block->marked_bottom_begin = std::lower_bound(
                       refine_block->begin, refine_block->nonbottom_begin,
                                splitter->bunch, current_out_slice_greater),
                                                                        true)))
        {
            // make the splitter stable.
            assert(!splitter->is_stable());
            refine_block->stable_block_bunch.splice(
                            refine_block->stable_block_bunch.end(),
                                       part_tr.unstable_block_bunch, splitter);
                               assert(refine_block->marked_bottom_begin ==
                                               refine_block->nonbottom_begin ||
                            (*refine_block->marked_bottom_begin)->
                                current_out_slice->bunch() == splitter->bunch);
            assert(refine_block->begin == refine_block->marked_bottom_begin ||
                    *splitter->bunch < *refine_block->
                          marked_bottom_begin[-1]->current_out_slice->bunch());

            refine_block = refine(refine_block, splitter, nullptr,
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
        bisim_dnj::permutation_iter_t s_iter = refine_block->begin;
        assert(s_iter < refine_block->nonbottom_begin);
        do
        {
            bisim_dnj::state_info_ptr s = *s_iter;
            assert(s->succ.begin <= s->current_out_slice);
            assert(s->current_out_slice < s->succ_inert.begin);
            assert(!s->current_out_slice->block_bunch->slice.is_null());
            assert(s->current_out_slice->block_bunch->slice.iter == splitter);
            s->current_out_slice =
                              s->current_out_slice->out_slice_before_end() + 1;
        }
        while (++s_iter < refine_block->nonbottom_begin);
        // sort the bottom states of the red subblock according to their
        // new current_out_slice
        std::sort(refine_block->begin, refine_block->nonbottom_begin,
                                                    current_out_slice_greater);
    // 4.25: end for
    }
    // 4.26: Destroy all temporary data
//        We somewhere need to reset:
//            SpSl->begin_from_non_bottom = SpSl->begin;
    return result_block;
}

/*

- == ### Some old comments on the data structure and algorithm details ### == -

Basic choice to be made: either collect the transitions from new bottom states,
so as to mark them just-in-time, or sort the new bottom states according to
their current_out_slice.
The latter would allow to simplify splitting block_bunch-slices a bit, as we
do not have to distinguish between transitions from a new bottom state and
other transitions; it would also allow to save a little memory (very little
memory).  To really save memory, we should make a kind of pool allocation and
deallocation.
Problem of the latter: it requires that the blue bottom states are not moved
around.  So we would have to change the order of red and blue subblock after
refinement, and also the order of marked/unmarked states.  This is a
well-defined problem and can be solved by going through the code; it does not
require additional creativity.

Before we solve that problem, let me think clearly what is needed in that case.

1. Invariant: The stable block_bunch-slices of each block are ordered according
to their bunch.
This invariant is easily maintained for blocks containing bottom states: as
every bottom state has transitions in the stable block_bunch-slices, one just
walks through the out-slices of some bottom state of the new block to order the
(stable) slices of the new block correctly.

Only if the new block does not have any (old) bottom states, it cannot be
maintained.  Then, all block_bunch-slices become unstable anyway, and our only
concern is to find a suitable order in which to stabilise them;  it should make
sure that all bottom states are stabilised up to their current_out_slice.  That
concern can adequately be addressed by sorting the new bottom states in the
order of the bunch of their current_out_slice and then picking the minimum
bunch among these.  If there is no such bunch, all unstable block_bunch-slices
only contain transitions from non-bottom states and would lead to more new
bottom states, without affecting the stability of old bottom states.

Additionally, for every (new) bottom state, the stable block_bunch-slices are
consecutive.  (This will allow to find the predecessor slice of a stable
block_bunch-slice if we know a transition from a new bottom state in the latter
slice or the current_out_slice pointer points to a transition in the latter, in
particular it will enable us to add a just-stabilised block_bunch-slice in the
correct place.)

2. Stable block_bunch-slices are not empty.
As long as a refinement leads to both subblocks having bottom states, it is
clear that all these subblocks have transitions to every stable
block_bunch-slice.  We have to be careful about the slice w. r. t. which we
just stabilised:  the blue subblock will have no transitions in that slice, so
if the red subblock is the new subblock, we shall completely move that slice
from the blue to the red block_bunch-slices as soon as we find it (e. g. upon
the first transition in that slice).

If some subblock comes to have no more bottom states, then some slices of that
block may become empty -- but this is only relevant if the old block comes to
have no more bottom states and all transitions in some of its stable
block_bunch-slices move to the new block.  In that specific case, we are
allowed to walk through the list of stable block_bunch-slices of the old block
immediately afterwards to delete the empty slices; this work can be ascribed to
the stable block_bunch-slices of the new block.  That would not even require
the stable block_bunch-slices to be ordered in a specific way.

3. Unstable block_bunch-slices are not empty too long.
Because unstable block_bunch-slices do not come in a specific order and do not
only belong to a specific block, we may not be able to walk through them and
check all their predecessors.  Even if all unstable block_bunch-slices of the
block that has just been split are at the beginning of the list of unstable
block_bunch-slices, there may not be time to walk through them because the new
subblock may not have all of these -- exactly because they're unstable.  So we
cannot ascribe the work of walking through the unstable block_bunch-slices of
the old subblock to transitions of the new subblock.  Therefore, we cannot
apply the method mentioned under 2 to the list of unstable block_bunch-slices.
However, what we can do is this:  We make sure that an empty block_bunch-slice
is deleted before the transitions split off from it are used to split the new
subblock.  Is this enough?  I think yes if we handle all block_bunch-slices of
the old block before starting on any slice of the new block, because an
unstable block_bunch-slice is only split if the block is being split.  So if we
create a new unstable block_bunch-slice, we should append the new slice to the
very end of (the postprocessing part of) the list.



    Prepare for postprocessing:
        First, split into the states that have a new noninert transition
        and those that haven't.
        Then, for each new bottom state, set its current_out_slice-pointer to
        its first out-slice and sort the new bottom states according to the
        bunch to which the out-slice belongs.
        Then, collect the block_bunch-slices of the red subblock and make them
        all unstable, in the order of the bunches of the out-slices of the new
        bottom states; after that, add all other block_bunch-slices of the red
        subblock to the list.

        Also separate these from_block-slices into the part that starts in
        (new)
    bottom states and the part that doesn't.
    We will need to use begin_from_non_bottom here.

    Convention: begin_from_non_bottom is nullptr for those block_bunch-slices
    that are not in the block_bunch_slice_for_postprocessing-list.  Those that
    are have begin_from_non_bottom in the interval [begin, end].
    (We need begin to initialize begin_from_non_bottom).

    Can we make do without doubly-linked list?  If we accept that (temporarily)
    slices are not in the correct order and some might even be empty...
*/



/*=============================================================================
=                       explicit instantiation requests                       =
=============================================================================*/



template class bisim_partitioner_dnj<lts_lts_t>;
template class bisim_partitioner_dnj<lts_aut_t>;
template class bisim_partitioner_dnj<lts_fsm_t>;

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2
