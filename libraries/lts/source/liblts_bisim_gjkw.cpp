// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file liblts_bisim_gjkw.cpp

#include "mcrl2/lts/detail/liblts_bisim_gjkw.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/transition.h"
#include "mcrl2/lts/lts_utilities.h"

namespace mcrl2
{
namespace lts
{
namespace detail
{
namespace bisim_gjkw
{





/* ************************************************************************* */
/*                                                                           */
/*                   R E F I N A B L E   P A R T I T I O N                   */
/*                                                                           */
/* ************************************************************************* */





block_t* block_t::refinable_first = nullptr;
const char* const block_t::mark_all_states_in_SpB = "mark all states in SpB";
state_type block_t::nr_of_blocks = 0;
constln_t* constln_t::nontrivial_first = nullptr;
state_info_const_ptr state_info_entry::s_i_begin;
permutation_const_iter_t state_info_entry::perm_begin;

/// \details `split_off_blue()` and `split_off_red()` use the same complexity
/// counters because their operations belong together.
static const char* const swap_red_and_blue_states = "swap red and blue states";
static const char* const set_pointer_to_new_block = "set pointer to new block";

/// \brief refine the block (the blue subblock is smaller)
/// \details This function is called after a refinement function has found
/// that the blue subblock is the smaller one.  It creates a new block for
/// the blue states.
/// \param blue_nonbottom_end iterator past the last blue nonbottom state
/// \returns pointer to the new (blue) block
block_t* block_t::split_off_blue(permutation_iter_t blue_nonbottom_end)
{
    assert(nonbottom_end() >= blue_nonbottom_end);
    assert(nonbottom_begin() <= blue_nonbottom_end);
    assert(blue_nonbottom_end - unmarked_nonbottom_begin() +
                                                   unmarked_bottom_size() > 0);
    assert(blue_nonbottom_end - unmarked_nonbottom_begin() +
                                           unmarked_bottom_size() <= size()/2);
    // It is not necessary to reset the nottoblue counters; these counters are
    // anyway only valid for the maybe-blue states.
    state_type swapcount = std::min(unmarked_bottom_size(),
                   (state_type) (marked_nonbottom_end() - blue_nonbottom_end));
    if (0 < swapcount)
    {
        // vector swap the states:
        permutation_iter_t pos1=blue_nonbottom_end, pos2=unmarked_bottom_end();
        state_info_ptr temp = *pos1;
        for (;;)
        {
            check_complexity::count(swap_red_and_blue_states, 1,
                                                    check_complexity::n_log_n);
            *pos1 = *--pos2;
            (*pos1)->pos = pos1;
            ++pos1;
            if (--swapcount == 0)  break;
            *pos2 = *pos1;
            (*pos2)-> pos = pos2;
        }
        *pos2 = temp;
        (*pos2)->pos = pos2;
    }

    // create a new block for the blue states
    permutation_iter_t splitpoint = blue_nonbottom_end+unmarked_bottom_size();
    assert(begin() != splitpoint && splitpoint != end());
    block_t* NewB = new block_t(constln(), begin(), splitpoint);
    if (BLOCK_NO_SEQNR != end()[-1]->block->seqnr())
    {
        NewB->assign_seqnr();
    }
    mCRL2log(log::debug, "bisim_gjkw") << "Created new " << NewB->debug_id()
                << " for " << blue_nonbottom_end - unmarked_nonbottom_begin() +
                                 unmarked_bottom_size() << " blue state(s).\n";
    // NewB->set_begin(begin());
    NewB->set_marked_nonbottom_begin(blue_nonbottom_end);
    NewB->set_bottom_begin(blue_nonbottom_end);
    // NewB->set_marked_bottom_begin(splitpoint);
    // NewB->set_old_bottom_begin(splitpoint);
    // NewB->set_end(splitpoint);
    // NewB->set_inert_begin(?);
    // NewB->set_inert_end(?);
    assert(nullptr == NewB->FromRed);
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        check_complexity::count(set_pointer_to_new_block, 1,
                                                    check_complexity::n_log_n);
        (*s_iter)->block = NewB;
    }

    // adapt the old block: it only keeps the red states
    set_begin(splitpoint);
    set_bottom_begin(marked_bottom_begin());
    set_bottom_begin(marked_bottom_begin());
    set_marked_nonbottom_begin(marked_bottom_begin());
    set_marked_bottom_begin(old_bottom_begin());
    // set_old_bottom_begin(old_bottom_begin());
    // set_end(end());

    constln()->make_nontrivial();

    return NewB;
}

/// \brief refine the block (the red subblock is smaller)
/// \details This function is called after a refinement function has found
/// that the red subblock is the smaller one.  It creates a new block for
/// the red states.
///
/// `split_off_blue()` and `split_off_red()` use the same complexity counters
/// because their operations belong together.
/// \param red_nonbottom_begin iterator to the first red nonbottom state
/// \returns pointer to the new (red) block
block_t* block_t::split_off_red(permutation_iter_t red_nonbottom_begin)
{
    assert(nonbottom_begin() <= red_nonbottom_begin);
    assert(nonbottom_end() >= red_nonbottom_begin);
    assert(marked_nonbottom_begin() == red_nonbottom_begin);
    assert(marked_size() > 0);
    assert(marked_size() <= size() / 2);

    // It is not necessary to reset the nottoblue counters; these counters are
    // anyway only valid for the maybe-blue states.
    state_type swapcount = std::min(unmarked_bottom_size(),
                  (state_type) (marked_nonbottom_end() - red_nonbottom_begin));
    if (0 < swapcount)
    {
        // vector swap the states:
        permutation_iter_t pos1=red_nonbottom_begin,pos2=unmarked_bottom_end();
        state_info_ptr temp = *pos1;
        for (;;)
        {
            check_complexity::count(swap_red_and_blue_states, 1,
                                                    check_complexity::n_log_n);
            *pos1 = *--pos2;
            (*pos1)->pos = pos1;
            ++pos1;
            if (--swapcount == 0)  break;
            *pos2 = *pos1;
            (*pos2)->pos = pos2;
        }
        *pos2 = temp;
        (*pos2)->pos = pos2;
    }
    // create a new block for the red states
    permutation_iter_t splitpoint = red_nonbottom_begin+unmarked_bottom_size();
    assert(begin() != splitpoint && splitpoint != end());
    block_t* NewB = new block_t(constln(), splitpoint, end());
    if (BLOCK_NO_SEQNR != (*begin())->block->seqnr())
    {
        NewB->assign_seqnr();
    }
    mCRL2log(log::debug, "bisim_gjkw") << "Created new " << NewB->debug_id()
                             << " for " << marked_size() << " red state(s).\n";
    // NewB->set_begin(splitpoint);
    NewB->set_bottom_begin(marked_bottom_begin());
    NewB->set_marked_nonbottom_begin(marked_bottom_begin());
    NewB->set_old_bottom_begin(old_bottom_begin());
    NewB->set_marked_bottom_begin(old_bottom_begin());
    // NewB->set_end(end());
    // NewB->set_inert_begin(?);
    // NewB->set_inert_end(?);
    assert(nullptr == NewB->FromRed);
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        check_complexity::count(set_pointer_to_new_block, 1,
                                                    check_complexity::n_log_n);
        (*s_iter)->block = NewB;
    }

    // adapt the old block: it only keeps the blue states
    // set_begin(begin());
    set_marked_nonbottom_begin(red_nonbottom_begin);
    set_bottom_begin(red_nonbottom_begin);
    set_marked_bottom_begin(splitpoint);
    set_old_bottom_begin(splitpoint);
    set_end(splitpoint);

    constln()->make_nontrivial();

    return NewB;
}


#ifndef NDEBUG

void part_state_t::print_block(const char* message, const block_t* B,
            permutation_const_iter_t begin, permutation_const_iter_t end) const
{
    if (end - begin != 0)
    {
        mCRL2log(log::debug, "bisim_gjkw") << "\t\t" << message
                                           << (end-begin > 1 ? "s:\n" : ":\n");
        do
        {
            mCRL2log(log::debug, "bisim_gjkw")<<"\t\t\t"<<(*begin)->debug_id();
            if (B != (*begin)->block)
            {
                mCRL2log(log::debug,"bisim_gjkw")<<", inconsistent: points to "
                                                << (*begin)->block->debug_id();
            }
            if (begin != (*begin)->pos)
            {
                mCRL2log(log::debug, "bisim_gjkw")
                               << ", inconsistent pointer to state_info_entry";
            }
            mCRL2log(log::debug, "bisim_gjkw") << "\n";
        }
        while (++begin != end);
    }
}


/// print the partition as a tree (per constellation and block)
void part_state_t::print_part(const part_trans_t& part_tr) const
{
    const constln_t* C = (*permutation.begin())->constln();
    for (;;)
    {
        mCRL2log(log::debug, "bisim_gjkw") << C->debug_id() << ":\n";
        const block_t* B = (*C->begin())->block;
        for (;;)
        {
            mCRL2log(log::debug, "bisim_gjkw") << "\t" << B->debug_id();
            if (C != B->constln())
            {
                mCRL2log(log::debug,"bisim_gjkw")<<", inconsistent: points to "
                                                   << B->constln()->debug_id();
            }
            mCRL2log(log::debug, "bisim_gjkw") << ":\n";
            print_block("Nonbottom state", B, B->nonbottom_begin(),
                                                           B->nonbottom_end());
            print_block("Bottom state", B, B->bottom_begin(), B->bottom_end());
            mCRL2log(log::debug, "bisim_gjkw") << "\t\tThis block has ";
            if (B->inert_end() == part_tr.B_to_C_begin())
            {
                mCRL2log(log::debug, "bisim_gjkw")
                            << "no transitions to its own constellation.\n";
                assert(B->inert_begin() == B->inert_end());
            }
            else
            {
                assert(B->inert_end() == B->inert_end()[-1].B_to_C_slice->end);
                mCRL2log(log::debug, "bisim_gjkw") << B->inert_end() -
                                         B->inert_end()[-1].B_to_C_slice->begin
                    <<" transition(s) to its own constellation,\n\t\tof which "
                                     << B->inert_end() - B->inert_begin()
                                     << (1 == B->inert_end() - B->inert_begin()
                                           ? " is inert.\n" : " are inert.\n");
            }
            // go to next block
            if (C->end() == B->end())  break;
            B = (*B->end())->block;
        }
        // go to next constellation
        if (permutation.end() == C->end())  break;
        C = (*C->end())->constln();
    }
}


void part_state_t::print_trans() const
{
    const fixed_vector<state_info_entry>::const_iterator state_info_end =
                                                          state_info.end() - 1;
    for (fixed_vector<state_info_entry>::const_iterator state_iter =
                state_info.begin(); state_info_end != state_iter; ++state_iter)
    {
        // print transitions out of state
        succ_const_iter_t succ_constln_iter = state_iter->succ_begin();
        if (state_iter->succ_end() != succ_constln_iter)
        {
            mCRL2log(log::debug, "bisim_gjkw") <<state_iter->debug_id() <<":\n";
            do
            {
                // print transitions to a constellation
                mCRL2log(log::debug, "bisim_gjkw") << "\ttransitions to "
                            << succ_constln_iter->target->constln()->debug_id()
                                                                      << ":\n";
                succ_const_iter_t s_iter = succ_constln_iter;
                // set succ_constln_iter to the end of the transitions to this
                // constellation
                succ_constln_iter = succ_constln_iter->constln_slice->end;
                for ( ;s_iter != succ_constln_iter ;++s_iter)
                {
                    mCRL2log(log::debug, "bisim_gjkw") << "\t\tto "
                                         << s_iter->target->debug_id() << "\n";
                    assert(s_iter->B_to_C->pred->succ == s_iter);
                    assert(s_iter->B_to_C->pred->source == &*state_iter);
                }
            }
            while (state_iter->succ_end() != succ_constln_iter);
        }
    }
}

#endif // ifndef NDEBUG





/* ************************************************************************* */
/*                                                                           */
/*                           T R A N S I T I O N S                           */
/*                                                                           */
/* ************************************************************************* */





/* split_inert_to_C splits the B_to_C slice of block B to its own
constellation into two slices: one for the inert and one for the non-inert
transitions.  It is called with SpB just after a constellation is split, as
the transitions from SpB to itself (= the inert transitions) now go to a
different constellation than the other transitions from SpB to its old
constellation.  It does, however, not adapt the other transition arrays to
reflect that noninert and inert transitions from block b would go to
different constellations.
Its time complexity is O(1+min {|out_noninert(b-->C)|, |out_inert(b)|}). */
void part_trans_t::split_inert_to_C(block_t* B)
{
    // if there are no inert transitions
    if (B->inert_begin() == B->inert_end())
    {
        if (B->inert_end() != B_to_C.begin())
        {
            // There are noninert transitions from B to its old constellation:
            // they all go to SpC.
            B->FromRed = B->inert_end()[-1].B_to_C_slice;
            mCRL2log(log::debug, "bisim_gjkw") << "FromRed of " <<B->debug_id()
                        << " set to " << B->FromRed->debug_id() << " (1)\n";
            assert(B->FromRed->from_block() == B);
            // So there are no more transitions from B to its own constellation
            B->set_inert_end(B_to_C.begin());
            B->set_inert_begin(B_to_C.begin());
            mCRL2log(log::debug, "bisim_gjkw") << B->debug_id()
               << " no longer has transitions to its own constellation (1).\n";
        }
        return;
    }
    B_to_C_descriptor* slice = B->inert_begin()->B_to_C_slice;
    // if all transitions are inert
    if (slice->begin == B->inert_begin())
    {
        return;
    }

    // now the slice actually has to be split
    B_to_C_descriptor* new_slice =
                     new B_to_C_descriptor(B->inert_begin(), B->inert_begin());
    // select the lesser number of swaps to decide which part should be the new
    // one:
    if(B->inert_begin() - slice->begin < slice->end - B->inert_begin())
    {
        new_slice->begin = slice->begin;
        slice->begin = B->inert_begin();
        B->FromRed = new_slice;
        mCRL2log(log::debug, "bisim_gjkw") << "FromRed of " << B->debug_id()
                           << " set to " << B->FromRed->debug_id() << " (2)\n";
        assert(B->FromRed->from_block() == B);
    }
    else
    {
        new_slice->end = slice->end;
        slice->end = B->inert_begin();
        B->FromRed = slice;
        mCRL2log(log::debug, "bisim_gjkw") << "FromRed of " << B->debug_id()
                           << " set to " << B->FromRed->debug_id() << " (3)\n";
        assert(B->FromRed->from_block() == B);
    }
    new_slice->prev = slice;
    new_slice->next = slice->next;
    new_slice->next->prev = new_slice;
    new_slice->prev->next = new_slice;
    // set the slice pointers of the smaller part to the new slice:
    for (B_to_C_iter_t iter = new_slice->begin; new_slice->end != iter; ++iter)
    {
        check_complexity::count("split transitions in B_to_C", 1,
                                                    check_complexity::m_log_n);
        assert(B_to_C.end() != iter && iter->pred->succ->B_to_C == iter);
        iter->B_to_C_slice = new_slice;
    }
}


/* part_trans_t::change_to_C has to be called after a transition target has
changed its constellation.  The member function will adapt the transition
data structure.  It assumes that the transition is non-inert and that the
new constellation does not (yet) have _inert_ incoming transitions.  It
returns the boundary between transitions to OldC and transitions to NewC in
the state's outgoing transition array. */
succ_iter_t part_trans_t::change_to_C(pred_iter_t pred_iter, constln_t* OldC,
                               constln_t* NewC, bool first_transition_of_state,
                                                bool first_transition_of_block)
{
    assert(pred_iter!=pred.end() && pred_iter->succ->B_to_C->pred==pred_iter);
    // adapt the B_to_C array:
    // always move the transition to the beginning of the slice (this will make
    // it easier because inert transitions are stored at the end of a slice).
    B_to_C_iter_t old_B_to_C_pos = pred_iter->succ->B_to_C;
    B_to_C_descriptor* old_B_to_C_slice = old_B_to_C_pos->B_to_C_slice;
    B_to_C_iter_t new_B_to_C_pos = old_B_to_C_slice->begin;
    assert(new_B_to_C_pos != B_to_C.end() &&
                         new_B_to_C_pos->pred->succ->B_to_C == new_B_to_C_pos);
    B_to_C_descriptor* new_B_to_C_slice;
    if (first_transition_of_block)
    {
        pred_iter->source->block->FromRed = old_B_to_C_slice;
        mCRL2log(log::debug, "bisim_gjkw") << "FromRed of "
                  << pred_iter->source->block->debug_id() << " set to "
                  << pred_iter->source->block->FromRed->debug_id() << " (4)\n";
        assert(pred_iter->source->block->FromRed->from_block() ==
                                                     pred_iter->source->block);
        new_B_to_C_slice=new B_to_C_descriptor(new_B_to_C_pos, new_B_to_C_pos);
        new_B_to_C_slice->prev = old_B_to_C_slice;
        new_B_to_C_slice->next = old_B_to_C_slice->next;
        new_B_to_C_slice->next->prev = new_B_to_C_slice;
        new_B_to_C_slice->prev->next = new_B_to_C_slice;
    }
    else
    {
        new_B_to_C_slice = new_B_to_C_pos[-1].B_to_C_slice;
    }
    ++new_B_to_C_slice->end;
    ++old_B_to_C_slice->begin;
    assert(new_B_to_C_slice->end == old_B_to_C_slice->begin);
    if (old_B_to_C_slice->begin == old_B_to_C_slice->end)
    {
        mCRL2log(log::debug, "bisim_gjkw") << " lb";
        block_t* B = pred_iter->source->block;
        if (B->inert_end() == old_B_to_C_slice->begin)
        {
            // this was the last transition from B to its own constellation
            assert(B->inert_begin() == B->inert_end());
            B->set_inert_end(B_to_C.begin());
            B->set_inert_begin(B_to_C.begin());
            mCRL2log(log::debug, "bisim_gjkw") << B->debug_id()
               << " no longer has transitions to its own constellation (2).\n";
        }
        assert(B->FromRed == old_B_to_C_slice);
        if (old_B_to_C_slice->prev == old_B_to_C_slice)
        {
            B->FromRed = nullptr;
        }
        else
        {
            B->FromRed =
            old_B_to_C_slice->next->prev = old_B_to_C_slice->prev;
            old_B_to_C_slice->prev->next = old_B_to_C_slice->next;
        }
        delete old_B_to_C_slice;
        mCRL2log(log::debug, "bisim_gjkw") << "FromRed of " << B->debug_id()
                                                           << " cleared (5)\n";
    }
    swap_B_to_C(pred_iter->succ, new_B_to_C_pos->pred->succ);
    new_B_to_C_pos->B_to_C_slice = new_B_to_C_slice;
    // adapt the outgoing transition array:
    // move the transition to the beginning or the end, depending on the order
    // of old/new constellation.
    succ_iter_t old_out_pos = pred_iter->succ, new_out_pos;
    assert(succ.end() != old_out_pos &&
                               old_out_pos->B_to_C->pred->succ == old_out_pos);
    out_descriptor* new_constln_slice;
    if (*OldC < *NewC)
    {
        // move to end. Possibly needs three-way swap.
        --old_out_pos->constln_slice->end;
        new_out_pos = old_out_pos->constln_slice->end;
        if (first_transition_of_state)
        {
            new_constln_slice = new out_descriptor(new_out_pos + 1);
        }
        else
        {
            new_constln_slice = new_out_pos[1].constln_slice;
        }
        --new_constln_slice->begin;
        if (pred_iter->source->constln() == OldC)
        {
            // swap over the inert transitions
            pred_iter->source->set_inert_succ_begin(
                                    pred_iter->source->inert_succ_begin() - 1);
            pred_iter->source->set_inert_succ_end(
                                      pred_iter->source->inert_succ_end() - 1);
            if (pred_iter->source->inert_succ_begin() !=
                                           pred_iter->source->inert_succ_end())
            {
                // there are noninert transitions: 3-way swap needed
                // *old_out_pos -> *new_out_pos -> *inert_pos -> *old_out_pos
                swap3_out(pred_iter->source->inert_succ_begin()->B_to_C->pred,
                                         pred_iter, new_out_pos->B_to_C->pred);
                // the old constln_slice cannot become empty because it
                // contains an inert transition.
                new_out_pos->constln_slice = new_constln_slice;
                return new_out_pos;
            }
        }
        // normal swap
        if (old_out_pos->constln_slice->begin == new_out_pos)
        {
            delete old_out_pos->constln_slice;
        }
        swap_out(pred_iter, new_out_pos->B_to_C->pred);
        new_out_pos->constln_slice = new_constln_slice;
        return new_out_pos;
    }
    else
    {
        // move to beginning
        new_out_pos = old_out_pos->constln_slice->begin;
        if (first_transition_of_state)
        {
            new_constln_slice = new out_descriptor(new_out_pos);
        }
        else
        {
            new_constln_slice = new_out_pos[-1].constln_slice;
        }
        ++new_constln_slice->end;
        ++old_out_pos->constln_slice->begin;
        if (old_out_pos->constln_slice->begin==old_out_pos->constln_slice->end)
        {
            delete old_out_pos->constln_slice;
        }
        swap_out(pred_iter, new_out_pos->B_to_C->pred);
        new_out_pos->constln_slice = new_constln_slice;
        return new_out_pos + 1;
    }
}

/* split_s_inert_out splits the outgoing transitions from s to its own
constellation into two:  the inert transitions become transitions to the
new constellation of which s is now part;  the non-inert transitions remain
transitions to OldC.  It returns the boundary between transitions to
OldC and transitions to NewC in the outgoing transition array of s.
Its time complexity is O(1 + min { |out_\nottau(s)|, |out_\tau(s)| }). */
succ_iter_t part_trans_t::split_s_inert_out(state_info_ptr s, constln_t* OldC)
{
    constln_t* NewC = s->constln();
    succ_iter_t split = s->inert_succ_begin(), to_C_end = s->inert_succ_end();
    assert(succ.end() == split || split->B_to_C->pred->succ == split);
    assert(succ.end() == to_C_end || to_C_end->B_to_C->pred->succ == to_C_end);

    if (split < to_C_end && split->constln_slice->begin < split)
    {
        // s has both inert and non-inert transitions
        succ_iter_t to_C_begin = split->constln_slice->begin;
        assert(succ.end() != to_C_begin &&
                                 to_C_begin->B_to_C->pred->succ == to_C_begin);

        if (*NewC < *OldC)
        {
            // the out-transitions of s also have to be swapped.
            // Actually only B_to_C and the target need to be swapped, as the
            // constln_slices are (still) identical.
            trans_type swapnr = std::min(to_C_end-split, split-to_C_begin);
            assert(0 < swapnr);
            split = to_C_end - split + to_C_begin;
            s->set_inert_succ_begin(to_C_begin);
            s->set_inert_succ_end(split);

            succ_iter_t pos1 = to_C_begin, pos2 = to_C_end;
            state_info_ptr temp_target = pos1->target;
            B_to_C_iter_t temp_B_to_C = pos1->B_to_C;
            for (;;)
            {
                check_complexity::count("swap succ_entries for inert "
                                  "transitions", 1, check_complexity::m_log_n);
                --pos2;
                pos1->target = pos2->target;
                pos1->B_to_C = pos2->B_to_C;
                pos1->B_to_C->pred->succ = pos1;
                ++pos1;
                if (0 >= --swapnr)  break;
                pos2->target = pos1->target;
                pos2->B_to_C = pos1->B_to_C;
                pos2->B_to_C->pred->succ = pos2;
            }
            pos2->target = temp_target;
            pos2->B_to_C = temp_B_to_C;
            pos2->B_to_C->pred->succ = pos2;
        }
        // create a new constln_slice
        out_descriptor* new_constln_slice = new out_descriptor(split);
        // make the smaller part the new slice.
        if (split - to_C_begin < to_C_end - split)
        {
            new_constln_slice->begin = to_C_begin;
            to_C_begin->constln_slice->begin = split;
        }
        else
        {
            new_constln_slice->end = to_C_end;
            to_C_begin->constln_slice->end = split;
        }
        // set the pointer to the slice for the smaller part.
        for (succ_iter_t succ_iter = new_constln_slice->begin;
                              new_constln_slice->end != succ_iter; ++succ_iter)
        {
            check_complexity::count("set pointer to new succ-constellation "
                                        "slice", 1, check_complexity::m_log_n);
            assert(succ.end() != succ_iter &&
                                   succ_iter->B_to_C->pred->succ == succ_iter);
            succ_iter->constln_slice = new_constln_slice;
        }
    }
    else if (*NewC < *OldC)
    {
        if (split < to_C_end)
        {
            // s has no non-inert transitions to OldC. All its transitions to
            // OldC become transitions to NewC.
            return to_C_end;
        }
        else if (s->succ_begin() < to_C_end)
        {
            // s has no inert transitions (but it has noninert ones). It will
            // not have transitions to NewC.
            succ_iter_t to_C_begin = to_C_end[-1].constln_slice->begin;
            assert(succ.end() != to_C_begin &&
                                 to_C_begin->B_to_C->pred->succ == to_C_begin);
            if (to_C_begin->target->constln() == OldC)
            {
                // s has transitions to OldC.
                return to_C_begin;
            }
        }
    }
    return split;
}

/* part_trans_t::new_block_created splits the B_to_C-slices to reflect that
some transitions now start in the new block NewB.  They can no longer be in
the same slice as the transitions that start in the old block.
Its time complexity is O(1 + |out(NewB)|). */
void part_trans_t::new_block_created(block_t* OldB, block_t* NewB)
{
    mCRL2log(log::debug, "bisim_gjkw") << "new_block_created("
                       << OldB->debug_id() << "," << NewB->debug_id() << ")\n";
    assert(OldB->constln() == NewB->constln());
    NewB->set_inert_begin(B_to_C.begin());
    NewB->set_inert_end(B_to_C.begin());
    bool old_fromred_invalid = false;
    // for all outgoing transitions of NewB
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        check_complexity::count("for all states of a new block", 1,
                                                    check_complexity::n_log_n);
        for (succ_iter_t succ_iter = (*s_iter)->succ_begin();
                               (*s_iter)->succ_end() != succ_iter; ++succ_iter)
        {
            check_complexity::count("for all transitions of a new block", 1,
                                                    check_complexity::m_log_n);
            assert(succ.end() != succ_iter &&
                                   succ_iter->B_to_C->pred->succ == succ_iter);
            // Move the transition to a new slice:
            mCRL2log(log::debug, "bisim_gjkw") << "Moving "
                                << succ_iter->B_to_C->pred->debug_id() << ": ";
            B_to_C_iter_t const old_pos = succ_iter->B_to_C;
            B_to_C_descriptor* old_B_to_C_slice = old_pos->B_to_C_slice;
            B_to_C_iter_t const after_new_pos = old_B_to_C_slice->end;
            assert(B_to_C.end() != old_pos &&
                                       old_pos->pred->succ->B_to_C == old_pos);
            assert(B_to_C.end() == after_new_pos ||
                           after_new_pos->pred->succ->B_to_C == after_new_pos);
            B_to_C_descriptor* new_B_to_C_slice;
            if (B_to_C.end() == after_new_pos ||
                    after_new_pos->pred->source->block != NewB ||
                        after_new_pos->pred->succ->target->constln() !=
                                            succ_iter->target->constln())
            {
                mCRL2log(log::debug, "bisim_gjkw") << "new slice, ";
                // create a new B_to_C-slice
                // this can only happen when the first transition from
                // *s_iter to a new constellation is handled (could be
                // simplified if we use nested loops for all constellations
                // reachable from *s_iter).
                new_B_to_C_slice = new B_to_C_descriptor(after_new_pos,
                                                            after_new_pos);
                if (nullptr == NewB->FromRed ||
                     (!old_fromred_invalid && OldB->FromRed==old_B_to_C_slice))
                {
                    mCRL2log(log::debug, "bisim_gjkw") << "set FromRed to "
                        "slice that will contain " <<old_pos->pred->debug_id();
                    NewB->FromRed = new_B_to_C_slice;
                    // new_B_to_C_slice is not yet fully initialised, therefore
                    // the assertion fails:
                    // assert(NewB->FromRed->from_block() == NewB);
                }
                if (OldB->inert_end() == after_new_pos)
                {
                    // this is the first transition from NewB to its own
                    // constellation.  Adapt the pointers accordingly.
                    assert(NewB->inert_begin() == B_to_C.begin());
                    assert(NewB->inert_end() == B_to_C.begin());
                    NewB->set_inert_begin(after_new_pos);
                    NewB->set_inert_end(after_new_pos);
                }
                new_B_to_C_slice->prev = NewB->FromRed;
                new_B_to_C_slice->next = NewB->FromRed->next;
                new_B_to_C_slice->next->prev = new_B_to_C_slice;
                new_B_to_C_slice->prev->next = new_B_to_C_slice;
            }
            else
            {
                // the slice at after_new_pos is already the correct one
                new_B_to_C_slice = after_new_pos->B_to_C_slice;
            }
            --new_B_to_C_slice->begin;
            --old_B_to_C_slice->end;
            assert(new_B_to_C_slice->begin == old_B_to_C_slice->end);
            B_to_C_iter_t new_pos = after_new_pos - 1;
            assert(B_to_C.end() != new_pos &&
                                    new_pos->pred->succ->B_to_C == new_pos);
            if (OldB->inert_end() == after_new_pos)
            {
                // The transition goes from NewB to the constellation of
                // OldB and NewB.
                OldB->set_inert_end(OldB->inert_end() - 1);
                if (OldB->inert_begin() <= old_pos)
                {
                    mCRL2log(log::debug, "bisim_gjkw") << "inert transition\n";
                    // The transition is inert and has to be moved over the
                    // non-inert transitions of NewB.
                    NewB->set_inert_begin(NewB->inert_begin() - 1);
                    new_pos = NewB->inert_begin();
                    // old_pos --> new_pos --> new_B_to_C_slice->begin -->
                    // old_pos
                    swap3_B_to_C(succ_iter, new_pos->pred->succ,
                                    new_B_to_C_slice->begin->pred->succ);
                }
                else
                {
                    mCRL2log(log::debug, "bisim_gjkw")
                            << "noninert transition to own constellation\n";
                    // The transition is non-inert, but it has to be moved
                    // over the inert transitions of OldB.
                    OldB->set_inert_begin(OldB->inert_begin() - 1);
                    // old_pos --> new_pos --> OldB->inert_begin() -> old_pos
                    swap3_B_to_C(succ_iter, new_pos->pred->succ,
                                    OldB->inert_begin()->pred->succ);
                }
                if (old_B_to_C_slice->begin == old_B_to_C_slice->end)
                {
                    // This was the last transition from OldB to its own
                    // constellation.
                    mCRL2log(log::debug, "bisim_gjkw") << OldB->debug_id()
                                    << " no longer has transitions to its own "
                                                        "constellation (3).\n";
                    OldB->set_inert_begin(B_to_C.begin());
                    OldB->set_inert_end(B_to_C.begin());

                    if (old_B_to_C_slice->prev == old_B_to_C_slice)
                    {
                        assert(OldB->FromRed == old_B_to_C_slice);
                        OldB->FromRed = nullptr;
                        mCRL2log(log::debug, "bisim_gjkw") << "FromRed of "
                                    << OldB->debug_id() << " cleared (3)\n";
                    }
                    else
                    {
                        if (OldB->FromRed == old_B_to_C_slice)
                        {
                            OldB->FromRed = old_B_to_C_slice->prev;
                            old_fromred_invalid = true;
                        }
                        old_B_to_C_slice->next->prev = old_B_to_C_slice->prev;
                        old_B_to_C_slice->prev->next = old_B_to_C_slice->next;
                    }
                    delete old_B_to_C_slice;
                }
            }
            else
            {
                mCRL2log(log::debug, "bisim_gjkw")
                            << "noninert transition to other constellation\n";
                // The transition goes from NewB to a constellation that
                // does not contain OldB or NewB.  No special treatment is
                // required.
                swap_B_to_C(succ_iter, new_pos->pred->succ);
                if (old_B_to_C_slice->begin == old_B_to_C_slice->end)
                {
                    if (old_B_to_C_slice->prev == old_B_to_C_slice)
                    {
                        assert(OldB->FromRed == old_B_to_C_slice);
                        OldB->FromRed = nullptr;
                        mCRL2log(log::debug, "bisim_gjkw") << "FromRed of "
                                    << OldB->debug_id() << " cleared (4)\n";
                    }
                    else
                    {
                        if (OldB->FromRed == old_B_to_C_slice)
                        {
                            OldB->FromRed = old_B_to_C_slice->prev;
                        }
                        old_B_to_C_slice->next->prev = old_B_to_C_slice->prev;
                        old_B_to_C_slice->prev->next = old_B_to_C_slice->next;
                    }
                    delete old_B_to_C_slice;
                }
            }
            new_pos->B_to_C_slice = new_B_to_C_slice;
        }
    }
    if (OldB->inert_begin() == OldB->inert_end() &&
            OldB->inert_end() != B_to_C.begin() &&
                (OldB->inert_end()[-1].pred->source->block != OldB ||
                 OldB->inert_end()[-1].pred->succ->target->constln()
                                                        != OldB->constln()))
    {
        // the old block has no transitions to its own constellation, but
        // its inert_begin and inert_end pointers are not set to B_to_C.
        // This should not happen.
        assert(0);
    }
    if (OldB->inert_end() != B_to_C.begin())
    {
        assert(OldB->inert_end()[-1].pred->source->block == OldB);
        assert(OldB->inert_end()[-1].pred->succ->target->constln() ==
                                                              OldB->constln());
    }
    if (NewB->inert_begin() == NewB->inert_end() &&
            NewB->inert_end() != B_to_C.begin() &&
                (NewB->inert_end()[-1].pred->source->block != NewB ||
                 NewB->inert_end()[-1].pred->succ->target->constln()
                                                        != NewB->constln()))
    {
        assert(0);
    }
    if (NewB->inert_end() != B_to_C.begin())
    {
        assert(NewB->inert_end()[-1].pred->source->block == NewB);
        assert(NewB->inert_end()[-1].pred->succ->target->constln() ==
                                                              NewB->constln());
    }
}

bool part_trans_t::is_consistent() const
{
    if (false)
    {
        mCRL2log(log::error) << "error: ";
        return false;
    }
    return true;
}





/* ************************************************************************* */
/*                                                                           */
/*                            A L G O R I T H M S                            */
/*                                                                           */
/* ************************************************************************* */





/*=============================================================================
=                            initialisation helper                            =
=============================================================================*/



/// constructor of the helper class
template<class LTS_TYPE>
bisim_partitioner_gjkw_initialise_helper<LTS_TYPE>::
bisim_partitioner_gjkw_initialise_helper(LTS_TYPE& l, bool branching,
                                                      bool preserve_divergence)
    :aut(l),
     nr_of_states(l.num_states()),
     orig_nr_of_states(l.num_states()),
     nr_of_transitions(l.num_transitions()),
     noninert_out_per_state(l.num_states(), 0),
     inert_out_per_state(l.num_states(), 0),
     noninert_in_per_state(l.num_states(), 0),
     inert_in_per_state(l.num_states(), 0),
     noninert_out_per_block(1, 0),
     inert_out_per_block(1, 0),
     states_per_block(1, l.num_states()),
     nr_of_nonbottom_states(0)
{
    mCRL2log(log::verbose) << "O(m log n) "
                << (preserve_divergence ? "Divergence preserving b" : "B")
                << (branching ? "ranching b" : "")
                << "isimulation partitioner created for " << l.num_states()
                << " states and " << l.num_transitions()
                << " transitions [GJKW 2017]\n";
    // m is not yet initialised fully, so we have to count transitions
    // in a different way:
    check_complexity::init(l.num_states(), l.num_transitions());
    // Iterate over the transitions and collect new states
    for (const transition& t: aut.get_transitions())
    {
        check_complexity::count("visit transitions to find extra Kripke "
                                             "states", 1, check_complexity::m);
        if (!branching || !aut.is_tau(t.label()) ||
                                   (preserve_divergence && t.from() == t.to()))
        {
            // (possibly) create new state
            Key k(t.label(), t.to());
            std::pair<typename std::unordered_map<Key, state_type,
                KeyHasher>::iterator,bool> extra_state = extra_kripke_states.
                                    insert(std::make_pair(k, nr_of_states));
            if (extra_state.second)
            {
                noninert_in_per_state.push_back(0);
                inert_in_per_state.push_back(0);
                noninert_out_per_state.push_back(0);
                inert_out_per_state.push_back(0);

                // (possibly) create new block
                std::pair<std::unordered_map<label_type, state_type>::iterator,
                            bool> action_block =
                              action_block_map.insert(std::make_pair(t.label(),
                                                     states_per_block.size()));
                if (action_block.second)
                {
                    noninert_out_per_block.push_back(0);
                    inert_out_per_block.push_back(0);
                    states_per_block.push_back(0);
                }

                ++noninert_in_per_state[t.to()];
                ++noninert_out_per_state[nr_of_states];
                ++noninert_out_per_block[action_block.first->second];
                ++states_per_block[action_block.first->second];
                ++nr_of_states;
                ++nr_of_transitions;
            }
            ++noninert_in_per_state[extra_state.first->second];
            ++noninert_out_per_state[t.from()];
            ++noninert_out_per_block[0];
        }
        else
        {
            ++inert_in_per_state[t.to()];
            if (1 == ++inert_out_per_state[t.from()])
            {
                // this is the first inert outgoing transition of t.from()
                ++nr_of_nonbottom_states;
            }
            ++inert_out_per_block[0];
        }
    }
    mCRL2log(log::verbose) << "Number of extra states: "
                                         << extra_kripke_states.size() << "\n";
    check_complexity::stats();
    check_complexity::init(nr_of_states, nr_of_transitions);
}


/// initialise the state in part_st and the transitions in part_tr
template<class LTS_TYPE>
inline void bisim_partitioner_gjkw_initialise_helper<LTS_TYPE>::
init_transitions(part_state_t& part_st, part_trans_t& part_tr, bool branching,
                                                      bool preserve_divergence)
{
    assert(part_st.size() == get_nr_of_states());
    assert(part_tr.size() == get_nr_of_transitions());

    // initialise blocks and B_to_C slices
    permutation_iter_t begin = part_st.permutation.begin();
    constln_t* constln = new constln_t(begin, part_st.permutation.end());
    if (states_per_block.size() > 1)
    {
        constln->make_nontrivial();
    }
    std::vector<block_t*> blocks(states_per_block.size());
    B_to_C_iter_t B_to_C_begin = part_tr.B_to_C.begin();
    for (state_type B = 0; B < states_per_block.size(); ++B)
    {
        check_complexity::count("initialise blocks", 1, check_complexity::n);
        permutation_iter_t end = begin + states_per_block[B];
        blocks[B] = new block_t(constln, begin, end);
        blocks[B]->set_inert_begin(B_to_C_begin + noninert_out_per_block[B]);
        blocks[B]->set_inert_end(blocks[B]->inert_begin() +
                                                       inert_out_per_block[B]);
        B_to_C_descriptor* slice =
                    new B_to_C_descriptor(B_to_C_begin,blocks[B]->inert_end());
        slice->next = slice;
        slice->prev = slice;
        assert(B_to_C_begin != slice->end);
        for (; slice->end != B_to_C_begin; ++B_to_C_begin)
        {
            check_complexity::count("initialise B_to_C",1,check_complexity::m);
            B_to_C_begin->B_to_C_slice = slice;
        }
        begin = end;
    }
    // only block 0 has a sequence number and nonbottom states:
    blocks[0]->assign_seqnr();
    blocks[0]->set_marked_nonbottom_begin(blocks[0]->marked_nonbottom_begin() +
                                                       nr_of_nonbottom_states);
    blocks[0]->set_bottom_begin(blocks[0]->marked_nonbottom_begin());
    assert(part_st.permutation.end() == begin);
    assert(part_tr.B_to_C.end() == B_to_C_begin);

    // initialise states and succ slices
    part_st.state_info.begin()->set_pred_begin(part_tr.pred.begin());
    part_st.state_info.begin()->set_succ_begin(part_tr.succ.begin());
    for (state_type s = 0; s < get_nr_of_states(); ++s)
    {
        check_complexity::count("initialise states", 1, check_complexity::n);
        part_st.state_info[s].set_inert_pred_begin(part_st.state_info[s].
                                      pred_begin() + noninert_in_per_state[s]);
        part_st.state_info[s].set_pred_end(part_st.state_info[s].
                                   inert_pred_begin() + inert_in_per_state[s]);
        // part_st.state_info[s+1].set_pred_begin(part_st.state_info[s].
        //                                                         pred_end());

        out_descriptor* s_slice =
                    new out_descriptor(part_st.state_info[s].succ_begin());
        part_st.state_info[s].set_inert_succ_begin(
                                s_slice->begin + noninert_out_per_state[s]);
        s_slice->end = part_st.state_info[s].inert_succ_begin() +
                                                        inert_out_per_state[s];
        part_st.state_info[s].set_succ_end(s_slice->end);
        part_st.state_info[s].set_inert_succ_end(s_slice->end);
        part_st.state_info[s].current_constln = s_slice->end;
        for (succ_iter_t succ_iter = s_slice->begin;
                                        s_slice->end != succ_iter; ++succ_iter)
        {
            check_complexity::count("initialise succ-constellation slices", 1,
                                                          check_complexity::m);
            succ_iter->constln_slice = s_slice;
        }

        if (s < aut.num_states())
        {
            // s is not an extra Kripke state.  It is in block 0.
            part_st.state_info[s].block = blocks[0];
            if (0 < inert_out_per_state[s])
            {
                // nonbottom state:
                assert(0 < nr_of_nonbottom_states--);
                part_st.state_info[s].pos = blocks[0]->begin() +
                                                        nr_of_nonbottom_states;
            }
            else
            {
                // bottom state:
                // The following assertion is incomplete; only the second
                // assertion (after the assignment) makes sure that not too
                // many states become part of this slice.
                assert(0 < states_per_block[0]--);
                part_st.state_info[s].pos = blocks[0]->begin() +
                                                           states_per_block[0];
                assert(part_st.state_info[s].pos >= blocks[0]->bottom_begin());
            }
            *part_st.state_info[s].pos = &part_st.state_info[s];
            // part_st.state_info[s].notblue = 0;
        }
    }

    // initialise transitions (and finalise extra Kripke states)
    for (const transition& t: aut.get_transitions())
    {
        check_complexity::count("initialise transitions", 1,
                                                          check_complexity::m);
        pred_iter_t t_pred;
        succ_iter_t t_succ;
        B_to_C_iter_t t_B_to_C;

        if (!branching || !aut.is_tau(t.label()) ||
                                   (preserve_divergence && t.from() == t.to()))
        {
            // take transition through an extra intermediary state
            Key k(t.label(), t.to());
            state_type extra_state = extra_kripke_states[k];
            if (0 != noninert_out_per_state[extra_state])
            {
                state_type extra_block = action_block_map[t.label()];
                // now initialise extra_state correctly
                part_st.state_info[extra_state].block = blocks[extra_block];
                assert(0 < states_per_block[extra_block]--);
                part_st.state_info[extra_state].pos = blocks[extra_block]->
                                       begin() + states_per_block[extra_block];
                *part_st.state_info[extra_state].pos =
                                              &part_st.state_info[extra_state];
                // part_st.state_info[extra_state].notblue = 0;

                // state extra_state has exactly one outgoing transition,
                // namely a noninert transition to to t.to().  It has to be
                // initialised now.
                assert(0 < noninert_in_per_state[t.to()]--);
                t_pred = part_st.state_info[t.to()].noninert_pred_begin() +
                                                 noninert_in_per_state[t.to()];
                assert(0 == --noninert_out_per_state[extra_state]);
                t_succ = part_st.state_info[extra_state].succ_begin();
                assert(0 < noninert_out_per_block[extra_block]);
                t_B_to_C = blocks[extra_block]->inert_begin() -
                                         noninert_out_per_block[extra_block]--;
                t_pred->source = &part_st.state_info.begin()[extra_state];
                t_pred->succ = t_succ;
                t_succ->target = &part_st.state_info.begin()[t.to()];
                t_succ->B_to_C = t_B_to_C;
                // t_B_to_C->B_to_C_slice = (already initialised);
                t_B_to_C->pred = t_pred;
            }
            // noninert transition from t.from() to extra_state
            assert(0 < noninert_in_per_state[extra_state]--);
            t_pred = part_st.state_info[extra_state].noninert_pred_begin() +
                                            noninert_in_per_state[extra_state];
            assert(0 < noninert_out_per_state[t.from()]--);
            t_succ = part_st.state_info[t.from()].succ_begin() +
                                              noninert_out_per_state[t.from()];
            assert(0 < noninert_out_per_block[0]);
            t_B_to_C = blocks[0]->inert_begin() - noninert_out_per_block[0]--;

            t_pred->source = &part_st.state_info.begin()[t.from()];
            t_pred->succ = t_succ;
            t_succ->target = &part_st.state_info.begin()[extra_state];
            t_succ->B_to_C = t_B_to_C;
            // t_B_to_C->B_to_C_slice = (already initialised);
            t_B_to_C->pred = t_pred;
        }
        else
        {
            // inert transition from t.from() to t.to()
            assert(0 < inert_in_per_state[t.to()]--);
            t_pred = part_st.state_info[t.to()].inert_pred_begin() +
                                                    inert_in_per_state[t.to()];
            assert(0 < inert_out_per_state[t.from()]--);
            t_succ = part_st.state_info[t.from()].inert_succ_begin() +
                                                 inert_out_per_state[t.from()];
            assert(0 < inert_out_per_block[0]--);
            t_B_to_C = blocks[0]->inert_begin() + inert_out_per_block[0];

            t_pred->source = &part_st.state_info.begin()[t.from()];
            t_pred->succ = t_succ;
            t_succ->target = &part_st.state_info.begin()[t.to()];
            t_succ->B_to_C = t_B_to_C;
            // t_B_to_C->B_to_C_slice = (already initialised);
            t_B_to_C->pred = t_pred;
        }
    }
    noninert_out_per_state.clear(); inert_out_per_state.clear();
    noninert_in_per_state.clear();  inert_in_per_state.clear();
    noninert_out_per_block.clear(); inert_out_per_block.clear();
    states_per_block.clear();

    #ifndef NDEBUG
        part_st.print_part(part_tr);
        part_st.print_trans();
    #endif
}

/// \brief Replaces the transition relation of the current LTS by the
/// transitions of the bisimulation-reduced transition system.
/// \details Each transition (s, l, s') is replaced by a transition (t, l, t'),
/// where t and t' are the equivalence classes of s and s', respectively.  If
/// the label l is internal, then the transition is only added if t != t' or
/// preserve_divergence == true.  This effectively removes all inert
/// transitions.  Duplicates are removed from the transitions in the new LTS.
///
/// Note that the number of states nor the initial state are not adapted by
/// this method.  These must be set separately.
///
/// The code is very much inspired by liblts_bisim_gw.h.
///
/// \pre The bisimulation equivalence classes have been computed.
/// \param branching Causes non-internal transitions to be removed.
template <class LTS_TYPE>
void bisim_partitioner_gjkw_initialise_helper<LTS_TYPE>::
               replace_transitions(const part_state_t& part_st, bool branching,
                                                      bool preserve_divergence)
{
    std::unordered_map <state_type, Key> to_lts_map;
    // obtain a map from state to <action, state> pair from extra_kripke_states
    for (typename std::unordered_map<Key, state_type, KeyHasher>::iterator it =
            extra_kripke_states.begin(); it != extra_kripke_states.end(); ++it)
    {
        to_lts_map.insert(std::make_pair(it->second, it->first));
    }
    extra_kripke_states.clear();
      
    // Put all the non-inert transitions in a set.  A set is used to remove
    // double occurrences of transitions.
    std::set<transition> resulting_transitions;
    const label_type tau_label = determine_tau_label(aut);

    // traverse the outgoing transitions of the original LTS states
    for (fixed_vector<state_info_entry>::const_iterator s_iter =
                                part_st.state_info.begin() + orig_nr_of_states;
                                      part_st.state_info.begin() != s_iter--; )
    {
        state_type s_eq = s_iter->block->seqnr();
        mCRL2log(log::debug, "bisim_gjkw") << "Handling transitions from "
                        << s_iter->debug_id() << " in block " << s_eq << ":\n";
        assert(BLOCK_NO_SEQNR != s_eq);
        for (succ_const_iter_t succ_iter = s_iter->succ_begin();
                                             s_iter->succ_end() != succ_iter; )
        {
            mCRL2log(log::debug, "bisim_gjkw") << "\ttransition to "
                                              << succ_iter->target->debug_id();
            state_type tgt_id = succ_iter->target-&*part_st.state_info.begin();
            state_type t_eq = succ_iter->target->block->seqnr();
            if (BLOCK_NO_SEQNR != t_eq)
            {
                assert(branching);
                // We have a transition that originally was inert.
                mCRL2log(log::debug,"bisim_gjkw") <<", i. e. to block " <<t_eq;
                if (s_eq == t_eq)
                {
                    // The transition is still inert.
                    if (!preserve_divergence || &*s_iter != succ_iter->target)
                    {
                        // but it was not a self-loop to start with.  So we do
                        // not add it.  (Even self-loops are not added if we
                        // don't preserve divergence.)
                        ++succ_iter;
                        mCRL2log(log::debug, "bisim_gjkw") << " (not added)\n";
                        continue;
                    }
                }
                assert((label_type) -1 != tau_label);
                resulting_transitions.insert(transition(s_eq,tau_label,t_eq));
                mCRL2log(log::debug, "bisim_gjkw")
                                           << " (added as inert transition)\n";
            }
            else
            {
                // We have a non-inert transition to an intermediary state.
                // Look up the label and where the transition from the
                // intermediary state goes.
                Key k = to_lts_map.find(tgt_id)->second;
                t_eq = part_st.state_info[k.second].block->seqnr();
                mCRL2log(log::debug, "bisim_gjkw")
                             << ", i. e. indirectly to block " << t_eq << "\n";
                assert(BLOCK_NO_SEQNR != t_eq);
                resulting_transitions.insert(transition(s_eq, k.first, t_eq));
                // The target state could also be found through the pointer
                // structure (but we also need the labels, which are not stored
                // in the refinable partition):
                assert(&part_st.state_info.begin()[k.second] ==
                                succ_iter->target->succ_begin()->target);
                assert(succ_iter->target->succ_end() -
                                    succ_iter->target->succ_begin() == 1);
            }
            // Skip over other transitions from the same state to the same
            // constellation -- they would be mapped to the same resulting
            // transition.
            succ_iter = succ_iter->constln_slice->end;
        }
    }

    // Copy the transitions from the set into the transition system.
    aut.clear_transitions();
    for (std::set<transition>::const_iterator i=resulting_transitions.begin();
                                       resulting_transitions.end() != i; ++i)
    {
        aut.add_transition(*i);
    }
}

} // end namespace bisim_gjkw



/*=============================================================================
=            dbStutteringEquivalence -- Algorithm 2 in [GJKW 2017]            =
=============================================================================*/



template <class LTS_TYPE>
void bisim_partitioner_gjkw<LTS_TYPE>::
        create_initial_partition_gjkw(bool branching, bool preserve_divergence)
{
    // 2.2: P := P_0, i. e. the initial, cycle-free partition; C = {S}
    // and
    // 2.3: Initialise all temporary data
    init_helper.init_transitions(part_st, part_tr, branching,
                                                        preserve_divergence);
}

template <class LTS_TYPE>
void bisim_partitioner_gjkw<LTS_TYPE>::
                                refine_partition_until_it_becomes_stable_gjkw()
{
    // 2.4: while there is a nontrivial constellation SpC do
    while (nullptr != bisim_gjkw::constln_t::get_some_nontrivial())
    {
        bisim_gjkw::check_complexity::count("for each nontrivial "
                        "constellation", 1, bisim_gjkw::check_complexity::n);
        bisim_gjkw::constln_t* const SpC =
                                bisim_gjkw::constln_t::get_some_nontrivial();
        // 2.5: Choose a small splitter block SpB subset of SpC from P,
        //      i.e. |SpB| <= 1/2*|SpC|
        // and
        // 2.6: Create a new constellation NewC and move SpB from SpC to NewC
        // and
        // 2.7: C := partition C where SpB is removed from SpC and NewC is
        //      added
        bisim_gjkw::block_t* const SpB = SpC->split_off_small_block();
        mCRL2log(log::debug, "bisim_gjkw") << "Splitting off "
                                                << SpB->debug_id() << ".\n";

        /*-------------------- find predecessors of SpB ---------------------*/

        assert(nullptr == bisim_gjkw::block_t::get_some_refinable());
        assert(new_bottom_states.empty());
        // 2.8: Mark block SpB as refinable
        assert(SpB->make_refinable());
        // 2.9: Mark all states of SpB as predecessors
        SpB->mark_all_states();
        // 2.17: Register that the transitions from s to out_inert(s) go to
        //       NewC (instead of SpC)
            // (Before we enter the loop, we already adapt the ``B_to_C''
            // transition array.)
        part_tr.split_inert_to_C(SpB);
        // 2.10: for all s in SpB do
        for (bisim_gjkw::permutation_iter_t s_iter = SpB->begin();
                                                SpB->end() != s_iter; ++s_iter)
        {
            bisim_gjkw::check_complexity::count("for all states in SpB", 1,
                                        bisim_gjkw::check_complexity::n_log_n);
            const bisim_gjkw::state_info_ptr s = *s_iter;
            // 2.11: for all s_prime in noninert_in(s) do
            for (bisim_gjkw::pred_iter_t pred_iter = s->noninert_pred_begin();
                            s->noninert_pred_end() != pred_iter; ++pred_iter)
            {
                bisim_gjkw::check_complexity::count(
                                "for all incoming non-inert transitions", 1,
                                        bisim_gjkw::check_complexity::m_log_n);
                assert( // pred.end() != pred_iter &&
                                pred_iter->succ->B_to_C->pred == pred_iter);
                const bisim_gjkw::state_info_ptr s_prime = pred_iter->source;
                mCRL2log(log::debug, "bisim_gjkw") << pred_iter->debug_id();
                // 2.12: Mark the block of s_prime as refinable
                bool first_transition_of_block =
                                            s_prime->block->make_refinable();
                if (first_transition_of_block)
                {
                    mCRL2log(log::debug, "bisim_gjkw") << " fb";
                }
                // 2.13: Mark s_prime as predecessor of SpB
                bool first_transition_of_state=s_prime->block->mark(s_prime);
                if (first_transition_of_state)
                {
                    mCRL2log(log::debug, "bisim_gjkw") << " fs";
                }
                // 2.14: Register that s_prime->s goes to NewC (instead of SpC)
                // and
                // 2.15: Store whether s' still has some transition to SpC\SpB
                s_prime->current_constln = part_tr.change_to_C(pred_iter,
                                SpC, SpB->constln(), first_transition_of_state,
                                                first_transition_of_block);
            // 2.16: end for
            }
            // 2.17: Register that the transitions from s to out_inert(s) go to
            //       NewC (instead of SpC)
                // (In the loop, we only adapt the ``succ'' transition array.)
            // and
            // 2.18: Store whether s still has some transition to SpC\SpB
            s->current_constln = part_tr.split_s_inert_out(s, SpC);
        // 2.19: end for
        }
        mCRL2log(log::debug, "bisim_gjkw") << "\n";

        /*------------------ stabilise the partition again ------------------*/

        // 2.20: for all refinable blocks RefB do
        while (nullptr != bisim_gjkw::block_t::get_some_refinable())
        {
            bisim_gjkw::check_complexity::count("for all refinable blocks", 1,
                                        bisim_gjkw::check_complexity::m_log_n);
            bisim_gjkw::block_t* RefB =
                                    bisim_gjkw::block_t::get_some_refinable();
            mCRL2log(log::debug, "bisim_gjkw") << "Refining "
                                                << RefB->debug_id() << ".\n";
            // 2.21: Mark RefB as non-refinable
            RefB->make_nonrefinable();
            // 2.22: RedB := PrimaryRefine(RefB, SpB, marked states in RefB,
            //                             unmarked bottom states in RefB)
            bisim_gjkw::block_t* RedB = primary_refine(RefB);
            // 2.23: Unmark all states of RefB
                // (this is already done in primary_refine)
            if (RedB->size() <= 1 || RedB->FromRed->to_constln() != SpC)
            {
                // If the block has 1 element, or it has no transitions to SpC,
                // the refinement would be trivial anyway.
                continue;
            }
            // 2.24: SecondaryRefine(RedB, SpC\SpB, states in RedB with a
            //                       transition to SpC\SpB, bottom states in
            //                       RedB without transition to SpC\SpB)
            secondary_refine(RedB, SpC, RedB->FromRed);
        // 2.25: end for
        }

        /*------------------ postprocess new bottom states ------------------*/

        // 2.26: while there are new bottom states do
        while (!new_bottom_states.empty())
        {
            bisim_gjkw::check_complexity::count("while there are new bottom "
                                "states", 1, bisim_gjkw::check_complexity::n);
            // 2.27: PostprocessNewBottom()
            postprocess_new_bottom();
        // 2.28: end while
        }
    // 2.29: end while
    #ifndef NDEBUG
part_st.print_part(part_tr);
part_st.print_trans();
    #endif
    }
    // 2.30: return C
        // (this happens implicitly, through the bisim_partitioner_gjkw object
        // data)
    bisim_gjkw::check_complexity::stats();
    #ifndef NDEBUG
        part_st.print_part(part_tr);
        part_st.print_trans();
    #endif
}



/*=============================================================================
=                 PrimaryRefine -- Algorithm 3 in [GJKW 2017]                 =
=============================================================================*/



/* When calling primary_refine(), the block RefB is sliced into these parts:

|                 unmarked                  |  marked   | unmarked |  marked  |
|                 nonbottom                 | nonbottom |  bottom  |  bottom  |

The block does not contain old bottom states.

From the beginning, all marked states are regarded as red, and the unmarked
bottom states are regarded as blue.  The unmarked nonbottom states are further
sliced as follows:

                 shared variable
             notblue_initialised_end
                        v
|   blue    | notblue>0 |  notblue  |        red        |   blue   |   red    |
| nonbottom |           | undefined |     nonbottom     |  bottom  |  bottom  |
            ^                       ^
     primary_blue()               block.
     local variable      unmarked_nonbottom_end()
   blue_nonbottom_end

Note that the red nonbottom states may actually become larger than the marked
nonbottom states have been at the start of primary_refine().

Then, as soon as one of the two coroutines finishes finding red or blue states,
respectively, it refines the block.  The states with notblue>0 or undefined
turn into the colour that has not yet finished. */

template <class LTS_TYPE>
bisim_gjkw::block_t* bisim_partitioner_gjkw<LTS_TYPE>::
                                    primary_refine(bisim_gjkw::block_t* RefB)
{
    const bisim_gjkw::permutation_iter_t red_end = RefB->end();
    bisim_gjkw::block_t* RedB;

    mCRL2log(log::debug, "bisim_gjkw") << "primary_refine(" << RefB->debug_id()
                                                                      << ")\n";
    // 3.3: Spend the same amount of work on either coroutine:
    // and
    // 3.29: RedB := RefB  or  RedB := NewB , respectively
    RUN_COROUTINES(primary_blue, (RefB), RedB = RefB,
                   primary_red,  (RefB), RedB = red_end[-1]->block,
   /* shared data: */ bisim_gjkw::permutation_iter_t,
                                        (RefB->unmarked_nonbottom_begin()));
    // 3.31: return RedB
    return RedB;
}

/// \details PrimaryRefine and SecondaryRefine, each in the blue and the red
/// coroutine, use the same complexity counters because their operations belong
/// together.
static const char* const for_all_states_in_the_new_block =
                                            "for all states in the new block";
static const char* const for_all_incoming_transitions_of_the_new_block =
                            "for all incoming transitions of the new block";
static const char* const primary_search =
                                "primary search for states and transitions";
static const char* const secondary_search =
                                "secondary search for states and transitions";

/*--------------------------- handle blue states ----------------------------*/

template <class LTS_TYPE>
DEFINE_COROUTINE(bisim_partitioner_gjkw<LTS_TYPE>::, primary_blue,
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB),
    /* local variables:     */ (bisim_gjkw::permutation_iter_t, visited_end,
                                bisim_gjkw::state_info_ptr, s,
                                bisim_gjkw::pred_iter_t, pred_iter,
                                bisim_gjkw::permutation_iter_t,
                                                        blue_nonbottom_end),
    /* shared data:         */ bisim_gjkw::permutation_iter_t,
                                                    notblue_initialised_end,
    /* interrupt locations: */ (PRIMARY_BLUE_PREDECESSOR_HANDLED,
                                PRIMARY_BLUE_STATE_HANDLED))
{
    assert(RefB->unmarked_nonbottom_begin() == notblue_initialised_end);
    // 3.4l: whenever |Blue| > |RefB| / 2 do  Abort this coroutine
    if (RefB->unmarked_bottom_size() > RefB->size()/2)  ABORT_THIS_COROUTINE();

    /*  -  -  -  -  -  -  -  - visit blue states -  -  -  -  -  -  -  -  */

    // 3.5l: while Blue contains unvisited states do
    visited_end = RefB->unmarked_bottom_begin();
    if (RefB->unmarked_bottom_end() == visited_end)
    {
        RefB->unmark_all_states();
        mCRL2log(log::debug, "bisim_gjkw") << "primary_blue: empty\n";
        TERMINATE_COROUTINE_SUCCESSFULLY();
    }
    blue_nonbottom_end = RefB->unmarked_nonbottom_begin();
    COROUTINE_DO_WHILE(PRIMARY_BLUE_STATE_HANDLED,
                                             blue_nonbottom_end != visited_end)
    {
        bisim_gjkw::check_complexity::count(primary_search, 1,
                                bisim_gjkw::check_complexity::primary_m_log_n);
        // 3.6l: Choose an unvisited s in Blue
        s = *visited_end;
        // 3.7l: Mark s as visited
        ++visited_end;
        // 3.8l: for all s_prime in inert_in(s) \ Red do
        COROUTINE_FOR (PRIMARY_BLUE_PREDECESSOR_HANDLED,
                                pred_iter = s->inert_pred_begin(),
                                s->inert_pred_end() != pred_iter, ++pred_iter)
        {
            bisim_gjkw::check_complexity::count(primary_search, 1,
                                bisim_gjkw::check_complexity::primary_m_log_n);
            const bisim_gjkw::state_info_ptr s_prime = pred_iter->source;
            if (s_prime->pos >= RefB->marked_nonbottom_begin())  continue;
            // 3.9l: if notblue(s_prime) undefined then
            if (s_prime->pos >= notblue_initialised_end)
            {
                // 3.10l notblue(s_prime) := |out_inert(s_prime)|
                s_prime->notblue = s_prime->inert_succ_end() -
                                                s_prime->inert_succ_begin();
                bisim_gjkw::swap_permutation(s_prime->pos,
                                                    notblue_initialised_end);
                ++notblue_initialised_end;
            // 3.11l: endif
            }
            // 3.12l: notblue(s_prime) := notblue(s_prime) - 1
            --s_prime->notblue;
            // 3.13l: if notblue(s_prime) == 0 then
            if (0 == s_prime->notblue)
            {
                // 3.14l: Blue := Blue union {s_prime}
                bisim_gjkw::swap_permutation(s_prime->pos,
                                                        blue_nonbottom_end);
                ++blue_nonbottom_end;
                // 3.4l: whenever |Blue|>|RefB|/2 do  Abort this coroutine
                if (blue_nonbottom_end - RefB->unmarked_nonbottom_begin() +
                                RefB->unmarked_bottom_size() > RefB->size()/2)
                {
                    ABORT_THIS_COROUTINE();
                }
            // 3.15l: endif
            }
        // 3.16l: end for
        }
        END_COROUTINE_FOR;
    // 3.17l: end while
        if (RefB->unmarked_bottom_end() == visited_end)
            visited_end = RefB->unmarked_nonbottom_begin();
    }
    END_COROUTINE_DO_WHILE;

    /*  -  -  -  -  -  -  -  - split off blue block -  -  -  -  -  -  -  -  */

    // 3.18l: Abort the other coroutine
    ABORT_OTHER_COROUTINE();
    // From now on, we do no longer need the reentrant ``local'' variables.
    // All non-blue states are red.
    // 3.19l: Move Blue to a new block NewB
    // and
    // 3.20l: Destroy all temporary data
    bisim_gjkw::block_t* const NewB = RefB->split_off_blue(blue_nonbottom_end);
    part_tr.new_block_created(RefB, NewB);
    // 3.21l: for all s in NewB do
    for (bisim_gjkw::permutation_iter_t s_iter = NewB->begin();
                                            NewB->end() != s_iter; ++s_iter)
    {
        bisim_gjkw::check_complexity::count(for_all_states_in_the_new_block, 1,
                                        bisim_gjkw::check_complexity::n_log_n);
        const bisim_gjkw::state_info_ptr s = *s_iter;
        // 3.22l: for all s_prime in inert_in(s) \ New do
        for (bisim_gjkw::pred_iter_t pred_iter = s->inert_pred_begin();
                                s->inert_pred_end()!=pred_iter; ++pred_iter)
        {
            bisim_gjkw::check_complexity::count(
                            for_all_incoming_transitions_of_the_new_block, 1,
                                        bisim_gjkw::check_complexity::m_log_n);
            assert( // pred.end() != pred_iter &&
                                pred_iter->succ->B_to_C->pred == pred_iter);
            const bisim_gjkw::state_info_ptr s_prime = pred_iter->source;
            if (s_prime->block == NewB)  continue;
            assert(s_prime->block == RefB);
            // 3.23l: s_prime --> s is no longer inert
            part_tr.make_noninert(pred_iter->succ);
            // 3.24l: if |inert_out(s_prime)| == 0 then
            if (s_prime->inert_succ_begin() == s_prime->inert_succ_end())
            {
                // 3.25l: s_prime is a new bottom state
                new_bottom_states.push_back(s_prime);
                mCRL2log(log::debug, "bisim_gjkw") << "found new bottom "
                                                << s_prime->debug_id() << "\n";
                // moved here from line 5.8: move s_prime to bottom states
                RefB->set_bottom_begin(RefB->bottom_begin() - 1);
                swap_permutation(s_prime->pos, RefB->bottom_begin());
            // 3.26l: end if
            }
        // 3.27l: end for
        }
    // 3.28l: end for
    }
}
END_COROUTINE

/*---------------------------- handle red states ----------------------------*/

template <class LTS_TYPE>
DEFINE_COROUTINE(bisim_partitioner_gjkw<LTS_TYPE>::, primary_red,
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB),
    /* local variables:     */ (bisim_gjkw::permutation_iter_t, visited_begin,
                                bisim_gjkw::state_info_ptr, s,
                                bisim_gjkw::pred_iter_t, pred_iter),
    /* shared data:         */ bisim_gjkw::permutation_iter_t,
                                                    notblue_initialised_end,
    /* interrupt locations: */ (PRIMARY_RED_PREDECESSOR_HANDLED,
                                PRIMARY_RED_STATE_HANDLED))
{
    assert(RefB->old_bottom_begin() == RefB->old_bottom_end());
    assert(0 != RefB->marked_bottom_size());
            //< this assertion does not always hold!  Error as mentioned in my
            // email to Jan Friso Groote, Anton Wijs and Jeroen Keiren on
            // 13 June 2016.
    // 3.4r: whenever |Red| > |RefB| / 2 do  Abort this coroutine
    if (RefB->marked_bottom_size() > RefB->size() / 2)  ABORT_THIS_COROUTINE();

    /*  -  -  -  -  -  -  -  - visit red states -  -  -  -  -  -  -  -  */

    // 3.5r: while Red contains unvisited states do
    visited_begin = RefB->marked_bottom_end();
    COROUTINE_DO_WHILE(PRIMARY_RED_STATE_HANDLED,
                               RefB->marked_nonbottom_begin() != visited_begin)
    {
        bisim_gjkw::check_complexity::count(primary_search, 1,
                                bisim_gjkw::check_complexity::primary_m_log_n);
        // 3.7r (order of lines changed): Mark s as visited
        --visited_begin;
        // 3.6r: Choose an unvisited s in Red
        s = *visited_begin;
        // 3.8r: for all s_prime in inert_in(s) do
        COROUTINE_FOR (PRIMARY_RED_PREDECESSOR_HANDLED,
            pred_iter = s->inert_pred_begin(),
                                s->inert_pred_end() != pred_iter, ++pred_iter)
        {
            bisim_gjkw::check_complexity::count(primary_search, 1,
                                bisim_gjkw::check_complexity::primary_m_log_n);
            const bisim_gjkw::state_info_ptr s_prime = pred_iter->source;
            // 3.14r: Red := Red union {s_prime}
            if (s_prime->pos < notblue_initialised_end)
            {
                // three-way swap would be slightly more efficient, but the
                // data is put into the cache after the first swap anyway.
                bisim_gjkw::swap_permutation(s_prime->pos,
                                                    --notblue_initialised_end);
            }
            if (RefB->mark_nonbottom(s_prime) &&
            // 3.4r: whenever |Red| > |RefB|/2 do  Abort this coroutine
                                        RefB->marked_size() > RefB->size() / 2)
            {
                ABORT_THIS_COROUTINE();
            }
        // 3.16r: end for
        }
        END_COROUTINE_FOR;
    // 3.17r: end while
        if (RefB->marked_bottom_begin() == visited_begin)
            visited_begin = RefB->marked_nonbottom_end();
    }
    END_COROUTINE_DO_WHILE;

    /*  -  -  -  -  -  -  -  - split off red block -  -  -  -  -  -  -  -  */

    // 3.18r: Abort the other coroutine
    ABORT_OTHER_COROUTINE();
    // From now on, we do no longer need the reentrant ``local'' variables.
    // All non-red states are blue.
    // 3.19r: Move Red to a new block NewB
    // and
    // 3.20r: Destroy all temporary data
    bisim_gjkw::block_t* const NewB =
                        RefB->split_off_red(RefB->marked_nonbottom_begin());
    part_tr.new_block_created(RefB, NewB);
    // 3.21r: for all nonbottom s in NewB do
    for (bisim_gjkw::permutation_iter_t s_iter = NewB->nonbottom_begin();
                                     NewB->nonbottom_end() != s_iter; ++s_iter)
    {
        bisim_gjkw::check_complexity::count(for_all_states_in_the_new_block, 1,
                                        bisim_gjkw::check_complexity::n_log_n);
        const bisim_gjkw::state_info_ptr s = *s_iter;
        // 3.22r: for all s_prime in inert_out(s) \ New do
        for (bisim_gjkw::succ_iter_t succ_iter = s->inert_succ_begin();
                                 s->inert_succ_end() != succ_iter; ++succ_iter)
        {
            bisim_gjkw::check_complexity::count(
                              for_all_incoming_transitions_of_the_new_block, 1,
                                        bisim_gjkw::check_complexity::m_log_n);
            assert( // succ.end() != succ_iter &&
                                succ_iter->B_to_C->pred->succ == succ_iter);
            const bisim_gjkw::state_info_ptr s_prime = succ_iter->target;
            if (s_prime->block == NewB)  continue;
            assert(s_prime->block == RefB);
            // 3.23r: s --> s_prime is no longer inert
            part_tr.make_noninert(succ_iter);
        // 3.24r: end for
        }
        // 3.25r: if |inert_out(s)| == 0 then
        if (s->inert_succ_begin() == s->inert_succ_end())
        {
            // 3.26r: s is a new bottom state
            new_bottom_states.push_back(s);
            mCRL2log(log::debug, "bisim_gjkw") << "found new bottom "
                                                      << s->debug_id() << "\n";
            // moved here from line 5.8: move s to bottom states
            NewB->set_bottom_begin(NewB->bottom_begin() - 1);
            swap_permutation(s->pos, NewB->bottom_begin());
        // 3.27r: end if
        }
    // 3.28r: end for
    }
}
END_COROUTINE



/*=============================================================================
=                SecondaryRefine -- Algorithm 4 in [GJKW 2017]                =
=============================================================================*/



/* SecondaryRefine has to work harder than PrimaryRefine: there are no markings
that distribute all bottom and some nonbottom states over red and blue.  The
function has to colour both bottom and nonbottom states.  When calling, the
only structure is:

|            nonbottom             |            bottom            |  old  |
|                                  |                              |bottom |

All bottom and some nonbottom states have their current constellation pointer
set in a way that it is easy to check whether the state has a transition to
SpC\SpB.  The old bottom states are often empty, but if there are old bottom
states, they are guaranteed to be red.

SecondaryRefine slices these states further as follows:

                                  secondary_blue()  secondary_red()
          shared variable          local variable   local variable
      notblue_initialised_end        visited_end     visited_begin
                 v                        v                v
|  blue  |notblue| notblue |  red  | blue |unknown|not blue| red  |red old|
| nonbott|  > 0  | undef'd |nonbott|bottom|bottom | bottom |bottom|bottom |
         ^                 ^                      ^
 secondary_blue()   block.unmarked_        block.unmarked_
  local variable    nonbottom_end()          bottom_end()
blue_nonbottom_end

As soon as one of the two coroutines finishes finding states, it refines the
block into the two parts. */


struct bisim_gjkw::secondary_refine_shared {
    bisim_gjkw::permutation_iter_t notblue_initialised_end;
    bool fromred_finished;
};


template <class LTS_TYPE>
bisim_gjkw::block_t* bisim_partitioner_gjkw<LTS_TYPE>::
                                secondary_refine(bisim_gjkw::block_t* RefB,
                                const bisim_gjkw::constln_t* SpC,
                                const bisim_gjkw::B_to_C_descriptor* FromRed)
{
    const bisim_gjkw::permutation_iter_t red_end = RefB->end();
    bisim_gjkw::block_t* RedB;

    assert(RefB->non_old_marked_bottom_begin() ==
                                            RefB->non_old_marked_bottom_end());
    assert(RefB->marked_nonbottom_begin() == RefB->marked_nonbottom_end());
    assert(nullptr == FromRed || FromRed->from_block() == RefB);
    assert(nullptr == FromRed || FromRed->to_constln() == SpC);
    mCRL2log(log::debug, "bisim_gjkw") << "secondary_refine("
        << RefB->debug_id() << "," << SpC->debug_id() << ","
        <<(nullptr!=FromRed ?FromRed->debug_id() :std::string("NULL")) <<")\n";
    // 4.3: Spend the same amount of work on either coroutine:
    // and
    // 3.29: RedB := RefB  or  RedB := NewB , respectively
    RUN_COROUTINES(secondary_blue,(RefB,SpC),   RedB = RefB,
                   secondary_red,(RefB,FromRed),(RedB=red_end[-1]->block)==RefB
                                                ? (RedB = nullptr) : nullptr,
               /* shared data: */ struct bisim_gjkw::secondary_refine_shared,
                                        ({ RefB->nonbottom_begin(), false }));
    // 4.33: return RedB
    return RedB;
}

/*--------------------------- handle blue states ----------------------------*/

template <class LTS_TYPE>
DEFINE_COROUTINE(bisim_partitioner_gjkw<LTS_TYPE>::, secondary_blue,
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB,
                                const bisim_gjkw::constln_t*, SpC),
    /* local variables:     */ (bisim_gjkw::permutation_iter_t, visited_end,
                                bisim_gjkw::state_info_ptr, s,
                                bisim_gjkw::pred_iter_t, pred_iter,
                                bisim_gjkw::state_info_ptr, s_prime,
                                bisim_gjkw::permutation_iter_t,
                                                            blue_nonbottom_end,
                                bisim_gjkw::succ_const_iter_t, begin,
                                bisim_gjkw::succ_const_iter_t, end),
    /* shared data:         */ struct bisim_gjkw::secondary_refine_shared,
                                                                shared_data,
    /* interrupt locations: */ (SECONDARY_BLUE_PREDECESSOR_HANDLED,
                                SECONDARY_BLUE_TESTING,
                                SECONDARY_BLUE_STATE_HANDLED,
                                SECONDARY_BLUE_COLLECT_BOTTOM))
{
    // 4.4l: Blue := {}
    blue_nonbottom_end = RefB->unmarked_nonbottom_begin();
    assert(RefB->nonbottom_begin() == shared_data.notblue_initialised_end);
    visited_end = RefB->unmarked_bottom_begin();
    // 4.5l: whenever |Blue| > |RefB|/2 do  Abort this coroutine
        // nothing needs to be done now, as |Blue| = 0 here.

    /*  -  -  -  -  -  -  - collect blue bottom states -  -  -  -  -  -  -  */

    // We implement secondary_blue() using a changed order of visiting states:
    // we first only collect blue bottom states and only after that look for
    // predecessors of states.

    // 4.6l: while MaybeBlue contains unvisited states do
    COROUTINE_WHILE (SECONDARY_BLUE_COLLECT_BOTTOM,
                                    RefB->unmarked_bottom_end() != visited_end)
    {
        bisim_gjkw::check_complexity::count(secondary_search, 1,
                            bisim_gjkw::check_complexity::secondary_m_log_n);
        // 4.7l: Choose an unvisited s in MaybeBlue.
        s = *visited_end;
        // 4.8l (order of lines changed): Mark s as visited.
        ++visited_end;
        // 4.10l: if not isBlueTest(s) then
        if (s->surely_has_transition_to(SpC))
        {
            // The state s is not blue.  Move it to the slice of non-blue
            // bottom states.
            --visited_end;
            RefB->set_marked_bottom_begin(RefB->marked_bottom_begin() - 1);
            bisim_gjkw::swap_permutation(visited_end,
                                                RefB->marked_bottom_begin());
            // 4.11l: continue to Line 4.6l
            continue;
        // 4.12l: end if
        }
        assert(s->surely_has_no_transition_to(SpC));
        // 4.13l: Blue := Blue union {s}
            // already done by incrementing visited_end
            mCRL2log(log::debug, "bisim_gjkw") <<s->debug_id() <<" is blue.\n";
        // 4.5l: whenever |Blue| > |RefB|/2 do  Abort this coroutine
        if (visited_end - RefB->unmarked_bottom_begin() > RefB->size() / 2)
        {
            ABORT_THIS_COROUTINE();
        }
    // 4.27l: end while
    }
    END_COROUTINE_WHILE;

    if (0 == RefB->unmarked_bottom_size())
    {
        RefB->unmark_all_states();
        mCRL2log(log::debug, "bisim_gjkw") << "secondary_blue: empty\n";
        TERMINATE_COROUTINE_SUCCESSFULLY();
    }

    /*  -  -  -  -  -  -  -  - visit blue states -  -  -  -  -  -  -  -  */

    // 4.6l: while Blue contains unvisited states do
    visited_end = RefB->unmarked_bottom_begin();
    COROUTINE_DO_WHILE (SECONDARY_BLUE_STATE_HANDLED,
                                             visited_end != blue_nonbottom_end)
    {
        bisim_gjkw::check_complexity::count(secondary_search, 1,
                            bisim_gjkw::check_complexity::secondary_m_log_n);
        // 4.7l: Choose an unvisited s in Blue
        s = *visited_end;
        assert(visited_end < blue_nonbottom_end ||
                               (RefB->unmarked_bottom_begin() <= visited_end &&
                                   RefB->unmarked_bottom_end() > visited_end));
        assert(RefB->unmarked_nonbottom_begin() <= blue_nonbottom_end);
        assert(blue_nonbottom_end <= shared_data.notblue_initialised_end);
        assert(RefB->unmarked_nonbottom_end() >=
                                          shared_data.notblue_initialised_end);
        // 4.8l (order of lines changed): Mark s as visited
        ++visited_end;
        // 4.16l: for all s_prime in inert_in(s) \ Red do
        COROUTINE_FOR (SECONDARY_BLUE_PREDECESSOR_HANDLED,
                            pred_iter = s->inert_pred_begin(),
                                 s->inert_pred_end() != pred_iter, ++pred_iter)
        {
            bisim_gjkw::check_complexity::count(secondary_search, 1,
                            bisim_gjkw::check_complexity::secondary_m_log_n);
            s_prime = pred_iter->source;
            if (s_prime->pos >= RefB->marked_nonbottom_begin())
            {
                mCRL2log(log::debug, "bisim_gjkw") << s_prime->debug_id()
                                                    << " is already red.\n";
                continue;
            }
            // 4.17l: if notblue(s_prime) undefined then
            if (s_prime->pos >= shared_data.notblue_initialised_end)
            {
                // additional optimisation: If we can quickly determine that
                // s_prime has a transition to SpC, then it is not blue.
                if (s_prime->surely_has_transition_to(SpC))
                {
                    mCRL2log(log::debug, "bisim_gjkw") << s_prime->debug_id()
                                            << " has a transition to SpC.\n";
                    continue;
                }
                // 4.18l: notblue(s_prime):=|inert_out(s_prime)|
                s_prime->notblue = s_prime->inert_succ_end() -
                                                s_prime->inert_succ_begin();
                mCRL2log(log::debug, "bisim_gjkw") << s_prime->debug_id()
                                        << " has " << s_prime->notblue
                                        << " outgoing inert transition(s).\n";
                bisim_gjkw::swap_permutation(s_prime->pos,
                                        shared_data.notblue_initialised_end);
                ++shared_data.notblue_initialised_end;
            // 4.19l: end if
            }
            // 4.20l: notblue(s_prime) := notblue(s_prime) - 1
            --s_prime->notblue;
            // 4.21l: if notblue(s_prime) == 0 then
            if (0 != s_prime->notblue)  continue;
            // 4.22l: if out_noninert(s_prime) intersect SpC == {} then
            if (!shared_data.fromred_finished &&
                                !s_prime->surely_has_no_transition_to(SpC))
            {
                // It is not yet known whether s_prime has a transition to
                // SpC or not.  Execute the slow test now.
                COROUTINE_FOR (SECONDARY_BLUE_TESTING,
                    (begin = s_prime->succ_begin(), end = s_prime->succ_end()),
                                                        begin < end, (void) 0)
                {
                    bisim_gjkw::check_complexity::count(secondary_search, 1,
                            bisim_gjkw::check_complexity::secondary_m_log_n);
                    // binary search for transitions from
                    // s_prime to constellation SpC.
                    bisim_gjkw::succ_const_iter_t mid = begin + (end-begin)/2;
                    if (mid->target->constln() >= SpC)
                    {
                        end = mid->constln_slice->begin;
                    }
                    if (mid->target->constln() <= SpC)
                    {
                        begin = mid->constln_slice->end;
                    }
                }
                END_COROUTINE_FOR;
                if (begin != end)  continue;
            }
            mCRL2log(log::debug, "bisim_gjkw") << "Nonbottom "
                                       << s_prime->debug_id() << " is blue.\n";
            // 4.23l: Blue := Blue union {s_prime}
            bisim_gjkw::swap_permutation(s_prime->pos, blue_nonbottom_end);
            ++blue_nonbottom_end;
            // 4.5l: whenever |Blue| > |RefB| / 2 do  Abort this coroutine
            if (blue_nonbottom_end - RefB->unmarked_nonbottom_begin() +
                            RefB->unmarked_bottom_size() > RefB->size() / 2)
            {
                ABORT_THIS_COROUTINE();
            }
        // 4.26l: end for
        }
        END_COROUTINE_FOR;
    // 4.27l: end while
        if (RefB->unmarked_bottom_end() == visited_end)
        {
            mCRL2log(log::debug, "bisim_gjkw")
                                           << "Finished with bottom states.\n";
            visited_end = RefB->unmarked_nonbottom_begin();
        }
    }
    END_COROUTINE_DO_WHILE;

    /*  -  -  -  -  -  -  -  - split off blue block -  -  -  -  -  -  -  -  */

    // 4.28l: Abort the other coroutine
    ABORT_OTHER_COROUTINE();
    // From now on, we do no longer need the reentrant ``local'' variables.
    // All non-blue states are red.
    // 4.29l: Move Blue to a new block NewB
    // and
    // 4.30l: Destroy all temporary data
    bisim_gjkw::block_t* const NewB = RefB->split_off_blue(blue_nonbottom_end);
    part_tr.new_block_created(RefB, NewB);

    // 4.31: Find new non-inert transitions and bottom states
    // (as Lines 3.21-3.28)

    // 3.21l: for all s in NewB do
    for (bisim_gjkw::permutation_iter_t s_iter = NewB->begin();
                                            NewB->end() != s_iter; ++s_iter)
    {
        bisim_gjkw::check_complexity::count(for_all_states_in_the_new_block, 1,
                                        bisim_gjkw::check_complexity::n_log_n);
        const bisim_gjkw::state_info_ptr s = *s_iter;
        // 3.22l: for all s_prime in inert_in(s) \ New do
        for (bisim_gjkw::pred_iter_t pred_iter = s->inert_pred_begin();
                                s->inert_pred_end()!=pred_iter; ++pred_iter)
        {
            bisim_gjkw::check_complexity::count(
                            for_all_incoming_transitions_of_the_new_block, 1,
                                        bisim_gjkw::check_complexity::m_log_n);
            assert( // pred.end() != pred_iter &&
                                pred_iter->succ->B_to_C->pred == pred_iter);
            const bisim_gjkw::state_info_ptr s_prime = pred_iter->source;
            if (s_prime->block == NewB)  continue;
            assert(s_prime->block == RefB);
            // 3.23l: s_prime --> s is no longer inert
            part_tr.make_noninert(pred_iter->succ);
            // 3.24l: if |inert_out(s_prime)| == 0 then
            if (s_prime->inert_succ_begin() == s_prime->inert_succ_end())
            {
                // 3.25l: s_prime is a new bottom state
                new_bottom_states.push_back(s_prime);
                mCRL2log(log::debug, "bisim_gjkw") << "found new bottom "
                                                << s_prime->debug_id() << "\n";
                // moved here from line 5.8: move s_prime to bottom states
                RefB->set_bottom_begin(RefB->bottom_begin() - 1);
                swap_permutation(s_prime->pos, RefB->bottom_begin());
            // 3.26l: end if
            }
        // 3.27l: end for
        }
    // 3.28l: end for
    }
}
END_COROUTINE

/*---------------------------- handle red states ----------------------------*/

template <class LTS_TYPE>
DEFINE_COROUTINE(bisim_partitioner_gjkw<LTS_TYPE>::, secondary_red,
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB,
                                const bisim_gjkw::B_to_C_descriptor*, FromRed),
    /* local variables:     */ (bisim_gjkw::B_to_C_iter_t,
                                                        fromred_visited_begin,
                                bisim_gjkw::permutation_iter_t, visited_begin,
                                bisim_gjkw::state_info_ptr, s,
                                bisim_gjkw::pred_iter_t, pred_iter),
    /* shared data:         */ struct bisim_gjkw::secondary_refine_shared,
                                                                shared_data,
    /* interrupt locations: */ (SECONDARY_RED_COLLECT_FROMRED,
                                SECONDARY_RED_PREDECESSOR_HANDLED,
                                SECONDARY_RED_STATE_HANDLED))
{
    // 4.5r: whenever |Red| > |RefB|/2 then  Abort this coroutine
    if (RefB->marked_bottom_size() > RefB->size() / 2)
    {
        // This can happen if there are very many old bottom states.
        ABORT_THIS_COROUTINE();
    }

    // We implement secondary_red() using a changed order of visiting states:
    // we first only collect states that have a transition in FromRed and only
    // after that look for predecessors of states.

    /*  -  -  -  -  -  -  - collect states from FromRed -  -  -  -  -  -  -  */

    if (nullptr != FromRed)
    {
        fromred_visited_begin = FromRed->end;
        if (RefB->inert_end() == fromred_visited_begin)
        {
            // skip inert transitions in FromRed.
            fromred_visited_begin = RefB->inert_begin();
        }

        // 4.6r: while FromRed contains unvisited elements do
        COROUTINE_WHILE(SECONDARY_RED_COLLECT_FROMRED,
                FromRed->begin != fromred_visited_begin)
        {
            bisim_gjkw::check_complexity::count(secondary_search, 1,
                            bisim_gjkw::check_complexity::secondary_m_log_n);
            // 4.9r (order of lines changed): Mark s --> s'' as visited
            --fromred_visited_begin;
            // 4.7r: Choose an unvisited s --> s'' in FromRed
            s = fromred_visited_begin->pred->source;
            assert(RefB->begin() <= s->pos && s->pos < RefB->end());
            // 4.13r: Red := Red union {s}
            if (s->pos < shared_data.notblue_initialised_end)
            {
                // three-way swap would be slightly more efficient,
                // but the data is put into the cache after the
                // first swap anyway.
                bisim_gjkw::swap_permutation(s->pos,
                        --shared_data.notblue_initialised_end);
            }
            mCRL2log(log::debug, "bisim_gjkw") << s->debug_id()
                          << " is red (FromRed: "
                          << fromred_visited_begin->pred->debug_id() << ").\n";
            if (RefB->mark(s) &&
            // 3.4r: whenever |Red| > |RefB|/2 do  Abort this coroutine
                                        RefB->marked_size() > RefB->size() / 2)
            {
                ABORT_THIS_COROUTINE();
            }
        }
        END_COROUTINE_WHILE;
    }
    if (0 == RefB->marked_size())
    {
        RefB->unmark_all_states();
        mCRL2log(log::debug, "bisim_gjkw") << "secondary_red: empty\n";
        TERMINATE_COROUTINE_SUCCESSFULLY();
    }

    // Now we can set a boolean variable to convey to secondary_blue() the
    // information that all transitions in FromRed have been handled and no
    // slow tests are needed any more.
    shared_data.fromred_finished = true;

    // The following code is (almost) identical to the code in primary_red().

    /*  -  -  -  -  -  -  -  - visit red states -  -  -  -  -  -  -  -  */

    // 4.4r: Red := {old bottom states}
    visited_begin = RefB->marked_bottom_end();
    if (RefB->marked_bottom_begin() == visited_begin)
    {
        // It may happen that all found states are nonbottom states.  (In that
        // case, some of these states will become new bottom states.)
        visited_begin = RefB->marked_nonbottom_end();
    }
    // 4.6r: while Red contains unvisited elements do
    COROUTINE_DO_WHILE(SECONDARY_RED_STATE_HANDLED,
                               RefB->marked_nonbottom_begin() != visited_begin)
    {
        bisim_gjkw::check_complexity::count(secondary_search, 1,
                            bisim_gjkw::check_complexity::secondary_m_log_n);
        // 4.15r (order of lines changed): Mark s as visited
        --visited_begin;
        assert(RefB->marked_bottom_begin() <= visited_begin ||
                            (RefB->marked_nonbottom_begin() <= visited_begin &&
                                RefB->marked_nonbottom_end() > visited_begin));
        // 4.7r: Choose an unvisited s in Red
        s = *visited_begin;
        // 4.16r: for all s_prime in inert_in(s) do
        COROUTINE_FOR (SECONDARY_RED_PREDECESSOR_HANDLED,
            pred_iter = s->inert_pred_begin(),s->inert_pred_end() != pred_iter,
                                                                ++pred_iter)
        {
            bisim_gjkw::check_complexity::count(secondary_search, 1,
                            bisim_gjkw::check_complexity::secondary_m_log_n);
            const bisim_gjkw::state_info_ptr s_prime = pred_iter->source;
            // 4.23r: Red := Red union {s_prime}
            if (s_prime->pos < shared_data.notblue_initialised_end)
            {
                // three-way swap would be slightly more efficient,
                // but the data is put into the cache after the
                // first swap anyway.
                bisim_gjkw::swap_permutation(s_prime->pos,
                                        --shared_data.notblue_initialised_end);
            }
            mCRL2log(log::debug, "bisim_gjkw") << "nonbottom "
                                          << s_prime->debug_id() << " is red ("
                                            << pred_iter->debug_id() << ").\n";
            if (RefB->mark_nonbottom(s_prime) &&
            // 4.5r: whenever |Red| > |RefB|/2 do  Abort this coroutine
                                        RefB->marked_size() > RefB->size() / 2)
            {
                ABORT_THIS_COROUTINE();
            }
        // 4.26r: end for
        }
        END_COROUTINE_FOR;
    // 4.27r: end while
        if (RefB->marked_bottom_begin() == visited_begin)
        {
            visited_begin = RefB->marked_nonbottom_end();
        }
    }
    END_COROUTINE_DO_WHILE;

    /*  -  -  -  -  -  -  -  - split off red block -  -  -  -  -  -  -  -  */

    // 4.28r: Abort the other coroutine
    ABORT_OTHER_COROUTINE();
    // From now on, we do no longer need the reentrant ``local'' variables.
    // All non-red states are blue.
    // 4.29r: Move Red to a new block NewB
    // and
    // 4.30r: Destroy all temporary data
    bisim_gjkw::block_t* const NewB =
                        RefB->split_off_red(RefB->marked_nonbottom_begin());
    part_tr.new_block_created(RefB, NewB);

    // 4.31: Find new non-inert transitions and bottom states
    // (as Lines 3.21-3.28)

    // 3.21r: for all nonbottom s in NewB do
    for (bisim_gjkw::permutation_iter_t s_iter = NewB->nonbottom_begin();
                                    NewB->nonbottom_end() != s_iter; ++s_iter)
    {
        bisim_gjkw::check_complexity::count(for_all_states_in_the_new_block, 1,
                                        bisim_gjkw::check_complexity::n_log_n);
        const bisim_gjkw::state_info_ptr s = *s_iter;
        // 3.22r: for all s_prime in inert_out(s) \ New do
        for (bisim_gjkw::succ_iter_t succ_iter = s->inert_succ_begin();
                                s->inert_succ_end()!=succ_iter; ++succ_iter)
        {
            bisim_gjkw::check_complexity::count(
                            for_all_incoming_transitions_of_the_new_block, 1,
                                        bisim_gjkw::check_complexity::m_log_n);
            assert( // succ.end() != succ_iter &&
                                succ_iter->B_to_C->pred->succ == succ_iter);
            bisim_gjkw::state_info_ptr s_prime = succ_iter->target;
            if (s_prime->block == NewB)  continue;
            assert(s_prime->block == RefB);
            // 3.23r: s --> s_prime is no longer inert
            part_tr.make_noninert(succ_iter);
        // 3.24r: end for
        }
        // 3.25r: if |inert_out(s)| == 0 then
        if (s->inert_succ_begin() == s->inert_succ_end())
        {
            // 3.26r: s is a new bottom state
            new_bottom_states.push_back(s);
            mCRL2log(log::debug, "bisim_gjkw") << "found new bottom "
                                                      << s->debug_id() << "\n";
            // moved here from line 5.8: move s to bottom states
            NewB->set_bottom_begin(NewB->bottom_begin() - 1);
            swap_permutation(s->pos, NewB->bottom_begin());
        // 3.27r: end if
        }
    // 3.28r: end for
    }
}
END_COROUTINE



/*=============================================================================
=              PostprocessNewBottom -- Algorithm 5 in [GJKW 2017]             =
=============================================================================*/



/* bisim_partitioner_gjkw<LTS_TYPE>::postprocess_new_bottom splits RefB by
checking whether all new bottom states can reach the constellations that RefB
can reach. */
template <class LTS_TYPE>
void bisim_partitioner_gjkw<LTS_TYPE>::postprocess_new_bottom()
{

    /*---------------- collect blocks with new bottom states ----------------*/

    // 5.3: for all new bottom states b do
    for (std::vector<bisim_gjkw::state_info_entry*>::iterator b_iter =
        new_bottom_states.begin(); new_bottom_states.end() != b_iter; ++b_iter)
    {
        const bisim_gjkw::state_info_ptr b = *b_iter;
        // 5.6: Mark this block as refinable
        // and
        // 5.4: if the block of b is not refinable then
        if (b->block->make_refinable())
        {
            // 5.5: Mark the bottom states of this block as old
            b->block->set_old_bottom_begin(b->block->bottom_begin());
            b->block->set_marked_bottom_begin(b->block->bottom_begin());
        // 5.7: end if
        }
        // 5.8: Make b a bottom state
            // needs to be done as soon as the new bottom state is found. See
            // lines 3.25l and 3.26r
            // However, we here have to move b to the non-old bottom states.
        bisim_gjkw::swap_permutation(b->pos, b->block->old_bottom_begin());
        b->block->set_old_bottom_begin(b->block->old_bottom_begin() + 1);
        b->block->set_marked_bottom_begin(b->block->old_bottom_begin());
        // 5.9: Set the current constellation pointer of b to the first
        //      constellation it can reach
        b->current_constln = b->succ_begin();
    // 5.10: end for
    }
    // 5.11: empty the set of new bottom states
    new_bottom_states.clear();

    /*----------------------- handle collected blocks -----------------------*/

    // 5.12: for all refinable blocks SplitB do
    while (nullptr != bisim_gjkw::block_t::get_some_refinable())
    {
        bisim_gjkw::check_complexity::count("for all blocks with new bottom "
                                "states", 1, bisim_gjkw::check_complexity::n);
        bisim_gjkw::block_t* SplitB=bisim_gjkw::block_t::get_some_refinable();
        mCRL2log(log::debug, "bisim_gjkw") <<"processing new bottom states in "
                                                 << SplitB->debug_id() << "\n";
        // 5.13: Mark SplitB as non-refinable
        SplitB->make_nonrefinable();
        // 5.14: for all constellations C reachable from SplitB do
            // The following assertion fails if there is a block whose bottom
            // states are all new! This is a problem described in my email to
            // Jan Friso Groote, Jeroen Keiren and Anton Wijs on 13 June 2016.
        assert(SplitB->old_bottom_begin() < SplitB->old_bottom_end());
        const bisim_gjkw::state_info_ptr oldbottom=*SplitB->old_bottom_begin();
        for (bisim_gjkw::succ_iter_t C_iter = oldbottom->succ_begin();
                                        oldbottom->succ_end() != C_iter;
                                        C_iter = C_iter->constln_slice->end)
        {
            bisim_gjkw::check_complexity::count(
                        "for all constellations C reachable from SplitB", 1,
                                            bisim_gjkw::check_complexity::m);
                                            //< I am unsure about this number
            assert( // succ.end() != C_iter &&
                                        C_iter->B_to_C->pred->succ == C_iter);
            bisim_gjkw::constln_t* C = C_iter->target->constln();
            // 5.15: Register that the transitions from SplitB to C need
            //       postprocessing
            C->postprocess_begin = C_iter->B_to_C->B_to_C_slice->begin;
            C->postprocess_end = C_iter->B_to_C->B_to_C_slice->end;
        // 5.16: end for
        }
        // 5.17: for all constellations C with incoming transitions that need
        //       postprocessing do
        for (bisim_gjkw::succ_iter_t C_iter = oldbottom->succ_begin();
                                            oldbottom->succ_end() != C_iter; )
        {
            assert( // succ.end() != C_iter &&
                                        C_iter->B_to_C->pred->succ == C_iter);
            C_iter = C_iter->constln_slice->end;
            bisim_gjkw::constln_t* C = C_iter->target->constln();
            mCRL2log(log::debug, "bisim_gjkw") << "postprocessing "
                                                      << C->debug_id() << "\n";
            // 5.18: for all blocks B with outgoing transitions to C that need
            //       postprocessing do
            for (bisim_gjkw::B_to_C_iter_t B_iter = C->postprocess_begin;
                                                B_iter != C->postprocess_end; )
            {
                bisim_gjkw::check_complexity::count(
                                "for all blocks B with transitions to C", 1,
                                        bisim_gjkw::check_complexity::m_log_n);
                                                        //< or perhaps m * 2?
                assert( // B_to_C.end() != B_iter &&
                                        B_iter->pred->succ->B_to_C == B_iter);
                bisim_gjkw::block_t* B = B_iter->pred->source->block;
                mCRL2log(log::debug, "bisim_gjkw") << "postprocessing "
                                                      << B->debug_id() << "\n";
                const bisim_gjkw::B_to_C_descriptor* FromRed =
                                                          B_iter->B_to_C_slice;
                B_iter = FromRed->end;  //< set B_iter to new value before
                                        // splitting the block
                // 5.19: RedB := SecondaryRefine(B, C, states in B with a
                //                               transition to C and old bottom
                //                               states, new bottom states in B
                //                               without transition to C)
                bisim_gjkw::block_t* RedB = secondary_refine(B, C, FromRed);
                assert (nullptr != RedB);
                // 5.20: for all new bottom states s in RedB do
                for (bisim_gjkw::permutation_iter_t s_iter =
                                                RedB->non_old_bottom_begin();
                                RedB->non_old_bottom_end() != s_iter; ++s_iter)
                {
                    bisim_gjkw::check_complexity::count(
                                    "advance current constellation pointer", 1,
                                            bisim_gjkw::check_complexity::m);
                    const bisim_gjkw::state_info_ptr s = *s_iter;
                    // 5.21: Advance the current constellation pointer of s to
                    //       the next constellation it can reach
                    s->current_constln=s->current_constln->constln_slice->end;
                // 5.22: end for
                }
            // 5.24: end for
            }
            // 5.23: Delete the transitions from B to C from those that
            //       need postprocessing
                // nothing needs to be done; we could perhaps clean up with:
                // C->postprocess_begin = C->postprocess_end;
        // 5.25: end for
        }
        // 5.26: Destroy all temporary data
            // the only thing that needs to be done here is to reset the
            // old_bottom_begin pointer. As all old bottom states are in the
            // same block, we only need to change the block that contains the
            // (known) old bottom state.
        oldbottom->block->set_marked_bottom_begin(oldbottom->block->end());
        oldbottom->block->set_old_bottom_begin(oldbottom->block->end());
    // 5.27: end for
    }
// 5.28: return
}



/*=============================================================================
=                       explicit instantiation requests                       =
=============================================================================*/



namespace bisim_gjkw
{

template class bisim_partitioner_gjkw_initialise_helper<lts_lts_t>;
template class bisim_partitioner_gjkw_initialise_helper<lts_aut_t>;
template class bisim_partitioner_gjkw_initialise_helper<lts_fsm_t>;

} // end namespace bisim_gjkw

template class bisim_partitioner_gjkw<lts_lts_t>;
template class bisim_partitioner_gjkw<lts_aut_t>;
template class bisim_partitioner_gjkw<lts_fsm_t>;

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2
