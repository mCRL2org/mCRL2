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
state_type block_t::nr_of_blocks = 0;
constln_t* constln_t::nontrivial_first = nullptr;
#ifndef NDEBUG
    // These variables are only accessed in debug mode.  In release mode,
    // accessing them would lead to a linker error.
    const char block_t::mark_all_states_in_SpB[] = "mark all states in SpB";
#endif
    state_info_const_ptr state_info_entry::s_i_begin;
    permutation_const_iter_t state_info_entry::perm_begin;
#ifndef NDEBUG
    const char part_state_t::delete_constellations[] = "delete constellations";
    const char part_state_t::delete_blocks[] = "delete blocks";
    const char part_trans_t::delete_out_descriptors[]="delete out_descriptors";
#endif

/// \details `split_off_blue()` and `split_off_red()` use the same complexity
/// counters because their operations belong together.
static const char swap_red_and_blue_states[] = "swap red and blue states";
static const char set_pointer_to_new_block[] = "set pointer to new block";

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
    assert(begin() < splitpoint && splitpoint < end());
    block_t* NewB = new block_t(constln(), begin(), splitpoint);
    if (BLOCK_NO_SEQNR != seqnr())
    {
        NewB->assign_seqnr();
    }
    mCRL2log(log::debug, "bisim_gjkw") << "Created new " << NewB->debug_id()
                << " for " << blue_nonbottom_end - unmarked_nonbottom_begin() +
                                 unmarked_bottom_size() << " blue state(s).\n";
    // NewB->set_begin(begin());
    NewB->set_bottom_begin(blue_nonbottom_end);
    NewB->set_marked_nonbottom_begin(blue_nonbottom_end);
    // NewB->set_marked_bottom_begin(splitpoint);
    // NewB->set_end(splitpoint);
    // NewB->set_inert_begin(?);
    // NewB->set_inert_end(?);
    assert(NewB->to_constln.empty());
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        check_complexity::count(set_pointer_to_new_block, 1,
                                                    check_complexity::n_log_n);
        (*s_iter)->block = NewB;
    }

    // adapt the old block: it only keeps the red states
    // set_end(end());
    set_bottom_begin(marked_bottom_begin());
    // set_marked_bottom_begin(marked_bottom_begin());
    set_marked_nonbottom_begin(marked_bottom_begin());
    set_begin(splitpoint);

    constln()->make_nontrivial();

    return NewB;
}

/// \brief refine the block (the red subblock is smaller)
/// \details This function is called after a refinement function has found
/// that the red subblock is the smaller one.  It creates a new block for
/// the red states.
///
/// Both `split_off_blue()` and `split_off_red()` unmark all states in the blue
/// subblock and mark all bottom states in the red subblock.  (This will help
/// the caller to distinguish old bottom states from new bottom states found
/// after `split_off_blue()` or `split_off_red()`, respectively.)  The two
/// functions use the same complexity counters because their operations belong
/// together.
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
    assert(begin() < splitpoint && splitpoint < end());
    block_t* NewB = new block_t(constln(), splitpoint, end());
    if (BLOCK_NO_SEQNR != seqnr())
    {
        NewB->assign_seqnr();
    }
    mCRL2log(log::debug, "bisim_gjkw") << "Created new " << NewB->debug_id()
                             << " for " << marked_size() << " red state(s).\n";
    // NewB->set_end(end());
    NewB->set_marked_bottom_begin(marked_bottom_begin());
    NewB->set_bottom_begin(marked_bottom_begin());
    NewB->set_marked_nonbottom_begin(marked_bottom_begin());
    // NewB->set_begin(splitpoint);
    // NewB->set_inert_begin(?);
    // NewB->set_inert_end(?);
    assert(NewB->to_constln.empty());
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
                assert(B->inert_end()[-1].B_to_C_slice->from_block() == B);
                assert(B->inert_end()[-1].B_to_C_slice->to_constln() == C);
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
            mCRL2log(log::debug, "bisim_gjkw")<<state_iter->debug_id()<<":\n";
            assert(state_iter->succ_begin() <= state_iter->inert_succ_begin());
            assert(state_iter->inert_succ_begin() <=
                                                 state_iter->inert_succ_end());
            assert(state_iter->succ_begin()==state_iter->inert_succ_begin() ||
                       *state_iter->inert_succ_begin()[-1].target->constln() <=
                                                       *state_iter->constln());
            assert(state_iter->succ_begin()==state_iter->inert_succ_begin() ||
                            state_iter->inert_succ_begin()[-1].target->block !=
                                                            state_iter->block);
            assert(state_iter->inert_succ_begin() ==
                                                state_iter->inert_succ_end() ||
                      (state_iter->inert_succ_begin()->target->block ==
                                                           state_iter->block &&
                       state_iter->inert_succ_end()[-1].target->block ==
                                                           state_iter->block));
            assert(state_iter->succ_end() == state_iter->inert_succ_end() ||
                             *state_iter->constln() <
                             *state_iter->inert_succ_end()->target->constln());
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
                                                 << s_iter->target->debug_id();
                    if (state_iter->inert_succ_begin() <= s_iter &&
                                         s_iter < state_iter->inert_succ_end())
                    {
                        mCRL2log(log::debug, "bisim_gjkw") << " (inert)";
                    }
                    mCRL2log(log::debug, "bisim_gjkw") << "\n";
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





/* split_inert_to_C splits the B_to_C slice of block SpB to its own
constellation into two slices: one for the inert and one for the non-inert
transitions.  It is called with SpB just after a constellation is split, as
the transitions from SpB to itself (= the inert transitions) now go to a
different constellation than the other transitions from SpB to its old
constellation.  It does, however, not adapt the other transition arrays to
reflect that noninert and inert transitions from block SpB would go to
different constellations.
Its time complexity is O(1+min {|out_noninert(SpB-->C)|, |out_inert(SpB)|}). */
void part_trans_t::split_inert_to_C(block_t* SpB)
{
    // if there are no inert transitions
    if (SpB->inert_begin() == SpB->inert_end())
    {
        if (SpB->inert_end() != B_to_C.begin())
        {
            // There are noninert transitions from SpB to its old
            // constellation:  they all go to SpC.
            SpB->SetFromRed(SpB->inert_end()[-1].B_to_C_slice);
            // There are no more transitions from SpB to its own constellation
            SpB->set_inert_begin(B_to_C.begin());
            SpB->set_inert_end(B_to_C.begin());
            mCRL2log(log::debug, "bisim_gjkw") << SpB->debug_id()
               << " no longer has transitions to its own constellation (1).\n";
        }
        return;
    }
    B_to_C_desc_iter_t slice = SpB->inert_begin()->B_to_C_slice;
    // if all transitions are inert
    if (slice->begin == SpB->inert_begin())
    {
        return;
    }

    // now the slice actually has to be split
    B_to_C_desc_iter_t new_slice;
    // select the smaller number of swaps to decide which part should be the
    // new one:
    if(SpB->inert_begin() - slice->begin < slice->end - SpB->inert_begin())
    {
        // fewer noninert transitions
        SpB->to_constln.emplace_front(slice->begin, SpB->inert_begin());
        new_slice = SpB->to_constln.begin();
        // SpB->SetFromRed(new_slice);
        mCRL2log(log::debug, "bisim_gjkw") << "FromRed of " << SpB->debug_id()
                            << " set to " << new_slice->debug_id() << " (2)\n";
        assert(new_slice->from_block() == SpB);
        slice->begin = SpB->inert_begin();
    }
    else
    {
        // fewer inert transitions
        SpB->to_constln.emplace_back(SpB->inert_begin(), slice->end);
        new_slice = std::prev(SpB->to_constln.end());
        slice->end = SpB->inert_begin();
        SpB->SetFromRed(slice);
    }
    // new_bottom_end is set to some invalid value:
    new_slice->new_bottom_end = SpB->end() + 1;
    // set the slice pointers of the smaller part to the new slice:
    for (B_to_C_iter_t iter = new_slice->begin; new_slice->end != iter; ++iter)
    {
        check_complexity::count("split transitions in B_to_C", 1,
                                                    check_complexity::m_log_n);
        assert(B_to_C.end() > iter && iter->pred->succ->B_to_C == iter);
        iter->B_to_C_slice = new_slice;
    }
}


/* part_trans_t::change_to_C has to be called after a transition target has
changed its constellation.  The member function will adapt the transition
data structure.  It assumes that the transition is non-inert and that the
new constellation does not (yet) have _inert_ incoming transitions.  It
returns the boundary between transitions to SpC and transitions to NewC in
the state's outgoing transition array. */
succ_iter_t part_trans_t::change_to_C(pred_iter_t pred_iter, constln_t* SpC,
                               constln_t* NewC, bool first_transition_of_state,
                                                bool first_transition_of_block)
{
    assert(pred_iter<pred.end() && pred_iter->succ->B_to_C->pred == pred_iter);
    // adapt the B_to_C array:
    // always move the transition to the beginning of the slice (this will make
    // it easier because inert transitions are stored at the end of a slice).
    B_to_C_iter_t old_B_to_C_pos = pred_iter->succ->B_to_C;
    B_to_C_desc_iter_t old_B_to_C_slice = old_B_to_C_pos->B_to_C_slice;
    B_to_C_iter_t new_B_to_C_pos = old_B_to_C_slice->begin;
    assert(new_B_to_C_pos < B_to_C.end() &&
                         new_B_to_C_pos->pred->succ->B_to_C == new_B_to_C_pos);
    B_to_C_desc_iter_t new_B_to_C_slice;
    if (first_transition_of_block)
    {
        // create a new slice in B_to_C for the transitions from RefB to NewC
        block_t* RefB = pred_iter->source->block;
        RefB->to_constln.emplace_back(new_B_to_C_pos, new_B_to_C_pos);
        new_B_to_C_slice = std::prev(RefB->to_constln.end());
        RefB->SetFromRed(old_B_to_C_slice);
        // set new_bottom_end to some invalid value:
        new_B_to_C_slice->new_bottom_end = RefB->end() + 1;
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
        // this was the last transition from RefB to SpC
        mCRL2log(log::debug, "bisim_gjkw") << " lb";
        block_t* RefB = pred_iter->source->block;
        if (RefB->inert_end() == old_B_to_C_slice->begin)
        {
            // this was the last transition from RefB to its own constellation
            assert(RefB->inert_begin() == RefB->inert_end());
            RefB->set_inert_begin(B_to_C.begin());
            RefB->set_inert_end(B_to_C.begin());
            mCRL2log(log::debug, "bisim_gjkw") << RefB->debug_id()
               << " no longer has transitions to its own constellation (2).\n";
        }
        assert(RefB->to_constln.begin() == old_B_to_C_slice);
        RefB->to_constln.erase(old_B_to_C_slice);
        mCRL2log(log::debug, "bisim_gjkw") << "FromRed of " << RefB->debug_id()
                                                           << " cleared (5)\n";
    }
    swap_B_to_C(pred_iter->succ, new_B_to_C_pos->pred->succ);
    new_B_to_C_pos->B_to_C_slice = new_B_to_C_slice;
    // adapt the outgoing transition array:
    // move the transition to the beginning or the end, depending on the order
    // of old/new constellation.
    succ_iter_t old_out_pos = pred_iter->succ, new_out_pos;
    assert(succ.end() > old_out_pos &&
                               old_out_pos->B_to_C->pred->succ == old_out_pos);
    out_descriptor* new_constln_slice;
    if (*SpC < *NewC)
    {
        mCRL2log(log::debug, "bisim_gjkw") << "*SpC<*NewC";
        // move to end. Possibly needs three-way swap.
        --old_out_pos->constln_slice->end;
        assert(old_out_pos->constln_slice->begin <=
                                              old_out_pos->constln_slice->end);
        assert(old_out_pos->B_to_C->pred->source->succ_begin() <=
                                              old_out_pos->constln_slice->end);
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
        assert(old_out_pos->constln_slice->end == new_constln_slice->begin);
        const state_info_ptr s = pred_iter->source;
        if (s->constln() == SpC)
        {
            // swap over the inert transitions
            s->set_inert_succ_begin_and_end(s->inert_succ_begin() - 1,
                                                      s->inert_succ_end() - 1);
            if (s->inert_succ_begin() != s->inert_succ_end())
            {
                // there are noninert transitions: 3-way swap needed
                // *old_out_pos -> *new_out_pos -> *inert_pos -> *old_out_pos
                swap3_out(pred_iter, new_out_pos->B_to_C->pred,
                                          s->inert_succ_begin()->B_to_C->pred);
                assert(s->inert_succ_begin()->target->block == s->block);
                assert(s->inert_succ_end()[-1].target->block == s->block);
                assert(s->succ_end() > s->inert_succ_end() &&
                               *SpC < *s->inert_succ_end()->target->constln());
                // the old constln_slice cannot become empty because it
                // contains an inert transition.
                assert(0 < old_out_pos->constln_slice->size());
                new_out_pos->constln_slice = new_constln_slice;
                return new_out_pos;
            }
        }
        // normal swap
        if (0 == old_out_pos->constln_slice->size())
        {
            delete old_out_pos->constln_slice;
        }
        swap_out(pred_iter, new_out_pos->B_to_C->pred);
        new_out_pos->constln_slice = new_constln_slice;
        return new_out_pos;
    }
    else
    {
        mCRL2log(log::debug, "bisim_gjkw") << "*NewC<*SpC";
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
        assert(new_constln_slice->end == old_out_pos->constln_slice->begin);
        assert(old_out_pos->constln_slice->begin <=
                                              old_out_pos->constln_slice->end);
        assert(old_out_pos->constln_slice->begin <=
                                old_out_pos->B_to_C->pred->source->succ_end());
        if (0 == old_out_pos->constln_slice->size())
        {
            delete old_out_pos->constln_slice;
        }
        swap_out(pred_iter, new_out_pos->B_to_C->pred);
        new_out_pos->constln_slice = new_constln_slice;
        return new_out_pos + 1;
    }
    // unreachable
    assert(0);
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
    succ_iter_t to_C_begin = s->succ_begin() == to_C_end ? s->succ_begin()
                                           : to_C_end[-1].constln_slice->begin;
    assert(to_C_begin <= split);
    assert(split <= to_C_end);
    assert(succ.end() == split || split->B_to_C->pred->succ == split);
    assert(succ.end() == to_C_end || to_C_end->B_to_C->pred->succ == to_C_end);
    assert(succ.end() == to_C_begin ||
                                 to_C_begin->B_to_C->pred->succ == to_C_begin);

    if (split < to_C_end && to_C_begin < split)
    {
        // s has both inert and non-inert transitions
        if (*NewC < *OldC)
        {
            // the out-transitions of s also have to be swapped.
            // Actually only B_to_C and the target need to be swapped, as the
            // constln_slices are (still) identical.
            trans_type swapnr = std::min(to_C_end-split, split-to_C_begin);
            split = to_C_end - split + to_C_begin;
            assert(0 < swapnr);

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

            s->set_inert_succ_begin_and_end(to_C_begin, split);
            assert(s->succ_begin() == s->inert_succ_begin() ||
                         *s->inert_succ_begin()[-1].target->constln() < *NewC);
            assert(s->inert_succ_begin()->target->block == s->block);
            assert(s->inert_succ_end()[-1].target->block == s->block);
            assert(s->inert_succ_end() < s->succ_end());
            assert(OldC == s->inert_succ_end()->target->constln());
        }
        // create a new constln_slice
        out_descriptor* new_constln_slice = new out_descriptor(split);
        // make the smaller part the new slice.
        if (split - to_C_begin < to_C_end - split)
        {
            new_constln_slice->begin = to_C_begin;
            to_C_begin->constln_slice->begin = split;
            assert(to_C_begin->constln_slice->begin == new_constln_slice->end);
        }
        else
        {
            new_constln_slice->end = to_C_end;
            to_C_begin->constln_slice->end = split;
            assert(new_constln_slice->begin == to_C_begin->constln_slice->end);
        }
        // set the pointer to the slice for the smaller part.
        for (succ_iter_t succ_iter = new_constln_slice->begin;
                              new_constln_slice->end != succ_iter; ++succ_iter)
        {
            check_complexity::count("set pointer to new succ-constellation "
                                        "slice", 1, check_complexity::m_log_n);
            assert(succ.end() > succ_iter &&
                                   succ_iter->B_to_C->pred->succ == succ_iter);
            succ_iter->constln_slice = new_constln_slice;
        }
    }
    else if (*NewC < *OldC)
    {
        if (split < to_C_end)
        {
            // s has inert but no non-inert transitions to OldC. All its
            // transitions to OldC become transitions to NewC.
            split = to_C_end;
        }
        else if (s->succ_begin() < to_C_end)
        {
            // s has no inert transitions (but it has noninert ones). It will
            // not have transitions to NewC.
            succ_iter_t to_C_begin = to_C_end[-1].constln_slice->begin;
            assert(succ.end() > to_C_begin &&
                                 to_C_begin->B_to_C->pred->succ == to_C_begin);
            if (to_C_begin->target->constln() == OldC)
            {
                // s has (noninert) transitions to OldC.
                s->set_inert_succ_begin_and_end(to_C_begin, to_C_begin);
                assert(s->succ_begin() == s->inert_succ_begin() ||
                         *s->inert_succ_begin()[-1].target->constln() < *NewC);
                assert(s->succ_end() > s->inert_succ_end() &&
                               OldC == s->inert_succ_end()->target->constln());
                split = to_C_begin;
            }
            else
            {
                assert(*to_C_begin->target->constln() < *NewC);
            }
        }
    }
#ifndef NDEBUG
    if (s->succ_begin() != s->succ_end())
    {
        assert(s->succ_begin()->constln_slice->begin == s->succ_begin());
        for (succ_iter_t succ_iter=s->succ_begin(); ; ++succ_iter)
        {
            assert(succ_iter->constln_slice->begin <= succ_iter);
            assert(succ_iter->constln_slice->end > succ_iter);
            assert(succ_iter->B_to_C->pred->source == s);
            if (s->succ_end() == succ_iter+1)
            {
                break;
            }
            if (succ_iter->constln_slice == succ_iter[1].constln_slice)
            {
                assert(succ_iter->target->constln() ==
                                               succ_iter[1].target->constln());
            }
            else
            {
                assert(succ_iter->constln_slice->end == succ_iter+1);
                assert(succ_iter[1].constln_slice->begin == succ_iter+1);
                assert(*succ_iter->target->constln() <
                                              *succ_iter[1].target->constln());
            }
        }
        assert(s->succ_end()[-1].constln_slice->end == s->succ_end());
    }
#endif
    return split;
}


/* part_trans_t::new_blue_block_created splits the B_to_C-slices to reflect
that some transitions now start in the new block NewB.  They can no longer be
in the same slice as the transitions that start in the old block.
Its time complexity is O(1 + |out(NewB)|). */
static const char for_all_states_of_a_new_block[] =
                                               "for all states of a new block";
static const char for_all_transitions_of_a_new_block[] =
                                          "for all transitions of a new block";
void part_trans_t::new_blue_block_created(block_t* RefB, block_t* NewB,
                                                                  bool primary)
{
    mCRL2log(log::debug, "bisim_gjkw") << "new_blue_block_created("
                       << RefB->debug_id() << "," << NewB->debug_id() << ")\n";
    assert(RefB->constln() == NewB->constln());
    assert(NewB->end() == RefB->begin());
    NewB->set_inert_begin_and_end(B_to_C.begin(), B_to_C.begin());
    bool old_fromred_invalid = false;
    // for all outgoing transitions of NewB
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        check_complexity::count(for_all_states_of_a_new_block, 1,
                                                    check_complexity::n_log_n);
        for (succ_iter_t succ_iter = (*s_iter)->succ_begin();
                               (*s_iter)->succ_end() != succ_iter; ++succ_iter)
        {
            check_complexity::count(for_all_transitions_of_a_new_block, 1,
                                                    check_complexity::m_log_n);
            assert(succ.end() > succ_iter &&
                                   succ_iter->B_to_C->pred->succ == succ_iter);
            // Move the transition to a new slice:
            mCRL2log(log::debug, "bisim_gjkw") << "Moving "
                                << succ_iter->B_to_C->pred->debug_id() << ": ";
            B_to_C_iter_t const old_pos = succ_iter->B_to_C;
            B_to_C_desc_iter_t old_B_to_C_slice = old_pos->B_to_C_slice;
            B_to_C_iter_t const after_new_pos = old_B_to_C_slice->end;
            assert(B_to_C.end() > old_pos &&
                                       old_pos->pred->succ->B_to_C == old_pos);
            assert(B_to_C.end() == after_new_pos ||
                           after_new_pos->pred->succ->B_to_C == after_new_pos);
            B_to_C_desc_iter_t new_B_to_C_slice;
            if (B_to_C.end() == after_new_pos ||
                    after_new_pos->pred->source->block != NewB ||
                        after_new_pos->pred->succ->target->constln() !=
                                            succ_iter->target->constln())
            {
                mCRL2log(log::debug, "bisim_gjkw") << "new slice, ";
                // create a new B_to_C-slice
                // this can only happen when the first transition from
                // *s_iter to a new constellation is handled.
                if (primary ? !old_fromred_invalid &&
                                  RefB->to_constln.begin() == old_B_to_C_slice
                            : !old_B_to_C_slice->needs_postprocessing())
                {
                    // during primary_split:
                    // the old B_to_C_slice is in the FromRed-position, i. e.
                    // it contains the transitions to SpC.  So the new slice
                    // also contains the transitions to SpC.
                    // during secondary_split:
                    // the old_B_to_C_slice does not need postprocessing, so
                    // its corresponding new slice should be in a similar
                    // position near the beginning of the list of slices.
                    if (primary)
                    {
                        mCRL2log(log::debug, "bisim_gjkw") << "set FromRed to "
                                                "slice that will contain "
                                                  << old_pos->pred->debug_id();
                    }
                    NewB->to_constln.emplace_front(after_new_pos,
                                                                after_new_pos);
                    new_B_to_C_slice = NewB->to_constln.begin();
                    // new_B_to_C_slice is not yet fully initialised, therefore
                    // the assertion fails:
                    // assert(new_B_to_C_slice->from_block() == NewB);
                }
                else
                {
                    // during primary_split:
                    // the old_B_to_C_slice is not in the FromRed-position. The
                    // corresponding slice of NewB should be moved into a
                    // position that does not change a potential FromRed-slice
                    // there.
                    // during secondary_split:
                    // the old_B_to_C_slice needs postprocessing, so also the
                    // new_B_to_C_slice will need postprocessing.
                    NewB->to_constln.emplace_back(after_new_pos,after_new_pos);
                    new_B_to_C_slice = std::prev(NewB->to_constln.end());
                }
                if (primary || !old_B_to_C_slice->needs_postprocessing())
                {
                    assert(RefB->end() < old_B_to_C_slice->new_bottom_end);
                    // set new_bottom_end to some invalid value
                    new_B_to_C_slice->new_bottom_end = NewB->end() + 1;
                }
                else
                {
                    new_B_to_C_slice->new_bottom_end = NewB->bottom_end();
                    assert(RefB->bottom_begin() <=
                                             old_B_to_C_slice->new_bottom_end);
                    assert(old_B_to_C_slice->new_bottom_end <=
                                                           RefB->bottom_end());
                }
                if (RefB->inert_end() == after_new_pos)
                {
                    // this is the first transition from NewB to its own
                    // constellation.  Adapt the pointers accordingly.
                    assert(NewB->inert_begin() == B_to_C.begin());
                    assert(NewB->inert_end() == B_to_C.begin());
                    NewB->set_inert_end(after_new_pos);
                    NewB->set_inert_begin(after_new_pos);
                }
            }
            else
            {
                // the slice at after_new_pos is already the correct one
                new_B_to_C_slice = after_new_pos->B_to_C_slice;
            }
            --new_B_to_C_slice->begin;
            --old_B_to_C_slice->end;
            assert(new_B_to_C_slice->begin == old_B_to_C_slice->end);
            B_to_C_iter_t new_pos = std::prev(after_new_pos);
            assert(B_to_C.end() > new_pos &&
                                       new_pos->pred->succ->B_to_C == new_pos);
            if (RefB->inert_end() == after_new_pos)
            {
                // The transition goes from NewB to the constellation of
                // RefB and NewB.
                if (RefB->inert_begin() <= old_pos)
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
                    // over the inert transitions of RefB.
                    RefB->set_inert_begin(RefB->inert_begin() - 1);
                    // old_pos --> new_pos --> RefB->inert_begin() -> old_pos
                    swap3_B_to_C(succ_iter, new_pos->pred->succ,
                                    RefB->inert_begin()->pred->succ);
                }
                RefB->set_inert_end(RefB->inert_end() - 1);
                if (old_B_to_C_slice->begin == old_B_to_C_slice->end)
                {
                    // This was the last transition from RefB to its own
                    // constellation.
                    mCRL2log(log::debug, "bisim_gjkw") << RefB->debug_id()
                                    << " no longer has transitions to its own "
                                                        "constellation (3).\n";
                    RefB->set_inert_begin(B_to_C.begin());
                    RefB->set_inert_end(B_to_C.begin());

                    if (!old_fromred_invalid &&
                                  RefB->to_constln.begin() == old_B_to_C_slice)
                    {
                        old_fromred_invalid = true;
                    }
                    RefB->to_constln.erase(old_B_to_C_slice);
                }
            }
            else
            {
                mCRL2log(log::debug, "bisim_gjkw")
                            << "noninert transition to other constellation\n";
                // The transition goes from NewB to a constellation that
                // does not contain RefB or NewB.  No special treatment is
                // required.
                swap_B_to_C(succ_iter, new_pos->pred->succ);
                if (old_B_to_C_slice->begin == old_B_to_C_slice->end)
                {
                    RefB->to_constln.erase(old_B_to_C_slice);
                }
            }
            new_pos->B_to_C_slice = new_B_to_C_slice;
        }
    }
    if (RefB->inert_begin() == RefB->inert_end() &&
            RefB->inert_end() != B_to_C.begin() &&
                (RefB->inert_end()[-1].pred->source->block != RefB ||
                 RefB->inert_end()[-1].pred->succ->target->constln()
                                                        != RefB->constln()))
    {
        // the old block has no transitions to its own constellation, but
        // its inert_begin and inert_end pointers are not set to B_to_C.
        // This should not happen.
        assert(0);
    }
    if (RefB->inert_end() != B_to_C.begin())
    {
        assert(RefB->inert_end()[-1].pred->source->block == RefB);
        assert(RefB->inert_end()[-1].pred->succ->target->constln() ==
                                                              RefB->constln());
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


/* part_trans_t::new_red_block_created splits the B_to_C-slices to reflect
that some transitions now start in the new block NewB.  They can no longer be
in the same slice as the transitions that start in the old block.
Its time complexity is O(1 + |out(NewB)|). */
void part_trans_t::new_red_block_created(block_t* RefB, block_t* NewB,
                                                                  bool primary)
{
    mCRL2log(log::debug, "bisim_gjkw") << "new_red_block_created("
                       << RefB->debug_id() << "," << NewB->debug_id() << ")\n";
    assert(RefB->constln() == NewB->constln());
    assert(NewB->begin() == RefB->end());
    NewB->set_inert_begin_and_end(B_to_C.begin(), B_to_C.begin());
    bool old_fromred_invalid = false;
    // for all outgoing transitions of NewB
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        check_complexity::count(for_all_states_of_a_new_block, 1,
                                                    check_complexity::n_log_n);
        for (succ_iter_t succ_iter = (*s_iter)->succ_begin();
                               (*s_iter)->succ_end() != succ_iter; ++succ_iter)
        {
            check_complexity::count(for_all_transitions_of_a_new_block, 1,
                                                    check_complexity::m_log_n);
            assert(succ.end() > succ_iter &&
                                   succ_iter->B_to_C->pred->succ == succ_iter);
            // Move the transition to a new slice:
            mCRL2log(log::debug, "bisim_gjkw") << "Moving "
                                << succ_iter->B_to_C->pred->debug_id() << ": ";
            B_to_C_iter_t const old_pos = succ_iter->B_to_C;
            B_to_C_desc_iter_t old_B_to_C_slice = old_pos->B_to_C_slice;
            B_to_C_iter_t new_pos = old_B_to_C_slice->begin;
            assert(B_to_C.end() > old_pos &&
                                       old_pos->pred->succ->B_to_C == old_pos);
            assert(B_to_C.end() > new_pos &&
                                       new_pos->pred->succ->B_to_C == new_pos);
            B_to_C_desc_iter_t new_B_to_C_slice;
            if (B_to_C.begin() == new_pos ||
                    new_pos[-1].pred->source->block != NewB ||
                        new_pos[-1].pred->succ->target->constln() !=
                                            succ_iter->target->constln())
            {
                mCRL2log(log::debug, "bisim_gjkw") << "new slice, ";
                // create a new B_to_C-slice
                // this can only happen when the first transition from
                // *s_iter to a new constellation is handled.
                if (primary ? !old_fromred_invalid &&
                                  RefB->to_constln.begin() == old_B_to_C_slice
                            : !old_B_to_C_slice->needs_postprocessing())
                {
                    // during primary_split:
                    // the old B_to_C_slice is in the FromRed-position, i. e.
                    // it contains the transitions to SpC.  So the new slice
                    // also contains the transitions to SpC.
                    // during secondary_split:
                    // the old_B_to_C_slice does not need postprocessing, so
                    // its corresponding new slice should be in a similar
                    // position near the beginning of the list of slices.
                    if (primary)
                    {
                        mCRL2log(log::debug, "bisim_gjkw") << "set FromRed to "
                                                "slice that will contain "
                                                  << old_pos->pred->debug_id();
                    }
                    NewB->to_constln.emplace_front(new_pos, new_pos);
                    new_B_to_C_slice = NewB->to_constln.begin();
                    // new_B_to_C_slice is not yet fully initialised, therefore
                    // the assertion fails:
                    // assert(new_B_to_C_slice->from_block() == NewB);
                }
                else
                {
                    // during primary_split:
                    // the old_B_to_C_slice is not in the FromRed-position. The
                    // corresponding slice of NewB should be moved into a
                    // position that does not change a potential FromRed-slice
                    // there.
                    // during secondary_split:
                    // the old_B_to_C_slice needs postprocessing, so also the
                    // new_B_to_C_slice will need postprocessing.
                    NewB->to_constln.emplace_back(new_pos, new_pos);
                    new_B_to_C_slice = std::prev(NewB->to_constln.end());
                }
                if (primary || !old_B_to_C_slice->needs_postprocessing())
                {
                    assert(RefB->end() < old_B_to_C_slice->new_bottom_end);
                    // set new_bottom_end to some invalid value
                    new_B_to_C_slice->new_bottom_end = NewB->end() + 1;
                }
                else
                {
                    new_B_to_C_slice->new_bottom_end =
                                              old_B_to_C_slice->new_bottom_end;
                    assert(NewB->bottom_begin() <=
                                             new_B_to_C_slice->new_bottom_end);
                    assert(new_B_to_C_slice->new_bottom_end <=
                                                           NewB->bottom_end());
                    old_B_to_C_slice->new_bottom_end = RefB->bottom_end();
                }
                if (RefB->inert_end() == old_B_to_C_slice->end)
                {
                    // this is the first transition from NewB to its own
                    // constellation.  Adapt the pointers accordingly.
                    assert(NewB->inert_begin() == B_to_C.begin());
                    assert(NewB->inert_end() == B_to_C.begin());
                    NewB->set_inert_end(new_pos);
                    NewB->set_inert_begin(new_pos);
                }
            }
            else
            {
                // the slice before new_pos is already the correct one
                new_B_to_C_slice = new_pos[-1].B_to_C_slice;
            }
            ++new_B_to_C_slice->end;
            ++old_B_to_C_slice->begin;
            assert(new_B_to_C_slice->end == old_B_to_C_slice->begin);
            if (RefB->inert_end() == old_B_to_C_slice->end)
            {
                // The transition goes from NewB to the constellation of
                // RefB and NewB.
                NewB->set_inert_end(NewB->inert_end() + 1);
                if (RefB->inert_begin() <= old_pos)
                {
                    mCRL2log(log::debug, "bisim_gjkw") << "inert transition\n";
                    // The transition is inert and has to be moved over the
                    // non-inert transitions of RefB.
                    // old_pos --> new_pos --> RefB->inert_begin() --> old_pos
                    swap3_B_to_C(succ_iter, new_pos->pred->succ,
                                              RefB->inert_begin()->pred->succ);
                    RefB->set_inert_begin(RefB->inert_begin() + 1);
                }
                else
                {
                    mCRL2log(log::debug, "bisim_gjkw")
                            << "noninert transition to own constellation\n";
                    // The transition is non-inert, but it has to be moved
                    // over the inert transitions of NewB.
                    new_pos = NewB->inert_begin();
                    NewB->set_inert_begin(NewB->inert_begin() + 1);
                    // old_pos --> new_pos --> NewB->inert_end() - 1 -> old_pos
                    swap3_B_to_C(succ_iter, new_pos->pred->succ,
                                             NewB->inert_end()[-1].pred->succ);
                }
                if (old_B_to_C_slice->begin == old_B_to_C_slice->end)
                {
                    // This was the last transition from RefB to its own
                    // constellation.
                    mCRL2log(log::debug, "bisim_gjkw") << RefB->debug_id()
                                    << " no longer has transitions to its own "
                                                        "constellation (3).\n";
                    RefB->set_inert_begin(B_to_C.begin());
                    RefB->set_inert_end(B_to_C.begin());

                    if (!old_fromred_invalid &&
                                  RefB->to_constln.begin() == old_B_to_C_slice)
                    {
                        old_fromred_invalid = true;
                    }
                    RefB->to_constln.erase(old_B_to_C_slice);
                }
            }
            else
            {
                mCRL2log(log::debug, "bisim_gjkw")
                            << "noninert transition to other constellation\n";
                // The transition goes from NewB to a constellation that
                // does not contain RefB or NewB.  No special treatment is
                // required.
                swap_B_to_C(succ_iter, new_pos->pred->succ);
                if (old_B_to_C_slice->begin == old_B_to_C_slice->end)
                {
                    RefB->to_constln.erase(old_B_to_C_slice);
                }
            }
            new_pos->B_to_C_slice = new_B_to_C_slice;
        }
    }
    if (RefB->inert_begin() == RefB->inert_end() &&
            RefB->inert_end() != B_to_C.begin() &&
                (RefB->inert_end()[-1].pred->source->block != RefB ||
                 RefB->inert_end()[-1].pred->succ->target->constln()
                                                        != RefB->constln()))
    {
        // the old block has no transitions to its own constellation, but
        // its inert_begin and inert_end pointers are not set to B_to_C.
        // This should not happen.
        assert(0);
    }
    if (RefB->inert_end() != B_to_C.begin())
    {
        assert(RefB->inert_end()[-1].pred->source->block == RefB);
        assert(RefB->inert_end()[-1].pred->succ->target->constln() ==
                                                              RefB->constln());
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
  : aut(l),
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
//    log::mcrl2_logger::set_reporting_level(log::debug);

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
    constln_t* constln = new constln_t(begin, part_st.permutation.end(),
                                                         part_tr.B_to_C_end());
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
        blocks[B]->set_inert_begin_and_end(B_to_C_begin +
                                                     noninert_out_per_block[B],
            B_to_C_begin + noninert_out_per_block[B] + inert_out_per_block[B]);
        blocks[B]->to_constln.emplace_back(B_to_C_begin,
                                                       blocks[B]->inert_end());
        B_to_C_desc_iter_t slice = std::prev(blocks[B]->to_constln.end());
        // set new_bottom_end to some invalid value:
        slice->new_bottom_end = end + 1;
        assert(B_to_C_begin < slice->end);
        for (; slice->end != B_to_C_begin; ++B_to_C_begin)
        {
            check_complexity::count("initialise B_to_C",1,check_complexity::m);
            B_to_C_begin->B_to_C_slice = slice;
        }
        begin = end;
    }
    assert(part_st.permutation.end() == begin);
    assert(part_tr.B_to_C.end() == B_to_C_begin);
    // only block 0 has a sequence number and nonbottom states:
    blocks[0]->assign_seqnr();
    blocks[0]->set_bottom_begin(blocks[0]->begin() + nr_of_nonbottom_states);
    blocks[0]->set_marked_nonbottom_begin(blocks[0]->bottom_begin());

    // initialise states and succ slices
    part_st.state_info.begin()->set_pred_begin(part_tr.pred.begin());
    part_st.state_info.begin()->set_succ_begin(part_tr.succ.begin());
    for (state_type s = 0; s < get_nr_of_states(); ++s)
    {
        check_complexity::count("initialise states", 1, check_complexity::n);
        part_st.state_info[s].set_pred_end(part_st.state_info[s].pred_begin() +
                             noninert_in_per_state[s] + inert_in_per_state[s]);
        part_st.state_info[s].set_inert_pred_begin(part_st.state_info[s].
                                      pred_begin() + noninert_in_per_state[s]);
        // part_st.state_info[s+1].set_pred_begin(part_st.state_info[s].
        //                                                         pred_end());

        out_descriptor* s_slice =
                    new out_descriptor(part_st.state_info[s].succ_begin());
        s_slice->end = s_slice->begin + noninert_out_per_state[s] +
                                                        inert_out_per_state[s];
        part_st.state_info[s].set_succ_end(s_slice->end);
        part_st.state_info[s].set_inert_succ_begin_and_end(s_slice->begin +
                                      noninert_out_per_state[s], s_slice->end);
        for (succ_iter_t succ_iter = s_slice->begin;
                                        s_slice->end != succ_iter; ++succ_iter)
        {
            check_complexity::count("initialise succ-constellation slices", 1,
                                                          check_complexity::m);
            succ_iter->constln_slice = s_slice;
        }
        part_st.state_info[s].set_current_constln(s_slice->end);

        if (s < aut.num_states())
        {
            // s is not an extra Kripke state.  It is in block 0.
            part_st.state_info[s].block = blocks[0];
            if (0 < inert_out_per_state[s])
            {
                // nonbottom state:
                assert(0 < nr_of_nonbottom_states);
                --nr_of_nonbottom_states;
                part_st.state_info[s].pos = blocks[0]->begin() +
                                                        nr_of_nonbottom_states;
            }
            else
            {
                // bottom state:
                // The following assertion is incomplete; only the second
                // assertion (after the assignment) makes sure that not too
                // many states become part of this slice.
                assert(0 < states_per_block[0]);
                --states_per_block[0];
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
                assert(0 < states_per_block[extra_block]);
                --states_per_block[extra_block];
                part_st.state_info[extra_state].pos = blocks[extra_block]->
                                       begin() + states_per_block[extra_block];
                *part_st.state_info[extra_state].pos =
                                              &part_st.state_info[extra_state];
                // part_st.state_info[extra_state].notblue = 0;

                // state extra_state has exactly one outgoing transition,
                // namely a noninert transition to to t.to().  It has to be
                // initialised now.
                assert(0 < noninert_in_per_state[t.to()]);
                --noninert_in_per_state[t.to()];
                t_pred = part_st.state_info[t.to()].noninert_pred_begin() +
                                                 noninert_in_per_state[t.to()];
                --noninert_out_per_state[extra_state];
                assert(0 == noninert_out_per_state[extra_state]);
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
            assert(0 < noninert_in_per_state[extra_state]);
            --noninert_in_per_state[extra_state];
            t_pred = part_st.state_info[extra_state].noninert_pred_begin() +
                                            noninert_in_per_state[extra_state];
            assert(0 < noninert_out_per_state[t.from()]);
            --noninert_out_per_state[t.from()];
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
            assert(0 < inert_in_per_state[t.to()]);
            --inert_in_per_state[t.to()];
            t_pred = part_st.state_info[t.to()].inert_pred_begin() +
                                                    inert_in_per_state[t.to()];
            assert(0 < inert_out_per_state[t.from()]);
            --inert_out_per_state[t.from()];
            t_succ = part_st.state_info[t.from()].inert_succ_begin() +
                                                 inert_out_per_state[t.from()];
            assert(0 < inert_out_per_block[0]);
            --inert_out_per_block[0];
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

    aut.clear_transitions();

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
        bisim_gjkw::check_complexity::count("obtain a map from extra Kripke "
                                 "states", 1, bisim_gjkw::check_complexity::n);
        to_lts_map.insert(std::make_pair(it->second, it->first));
    }
    extra_kripke_states.clear();

    const label_type tau_label = determine_tau_label(aut);
    // In the following loop, we visit a bottom state of each block and take
    // its transitions.  As the partition is (assumed to be) stable, in this
    // way we visit each transition of a lumped state exactly once.
    for (permutation_const_iter_t s_iter = part_st.permutation.begin();
                                 part_st.permutation.end() != s_iter; ++s_iter)
    {
        check_complexity::count("traverse states for quotient construction", 1,
                                                          check_complexity::n);
        // forward to last state of block, i. e. to a bottom state:
        s_iter = (*s_iter)->block->end() - 1;
        assert((*s_iter)->block->bottom_end() > s_iter);
        assert((*s_iter)->block->bottom_begin() <= s_iter);
        assert((*s_iter)->block->end() == (*s_iter)->constln()->end());
        state_type s_eq = (*s_iter)->block->seqnr();
        if (BLOCK_NO_SEQNR == s_eq)
        {
            mCRL2log(log::debug, "bisim_gjkw") << "Skipping "
                                                << (*s_iter)->block->debug_id()
                                                    << " and further blocks\n";
            break;
        }
        mCRL2log(log::debug, "bisim_gjkw") << "Handling transitions from "
                                    << (*s_iter)->debug_id() << " in "
                                      << (*s_iter)->block->debug_id() << ":\n";

        for (succ_const_iter_t succ_iter = (*s_iter)->succ_begin();
                                          (*s_iter)->succ_end() != succ_iter; )
        {
            bisim_gjkw::check_complexity::count(
                           "traverse transitions for quotient construction", 1,
                                              bisim_gjkw::check_complexity::m);
            mCRL2log(log::debug, "bisim_gjkw") << "\t"
                               << (const void*) &*succ_iter << "transition to "
                                              << succ_iter->target->debug_id();
            assert(succ_iter->constln_slice->begin == succ_iter);
            assert(succ_iter < succ_iter->constln_slice->end);
            state_type t_eq = succ_iter->target->block->seqnr();
            if (BLOCK_NO_SEQNR != t_eq)
            {
                assert(branching);
                // We have a transition that originally was inert.
                mCRL2log(log::debug,"bisim_gjkw") <<", i. e. to block " <<t_eq;
                if (s_eq == t_eq)
                {
                    // The transition is still inert.
                    if (!preserve_divergence)
                    {
                        // As we do not preserve divergence, we do not add it.
                        // Nor will we add other transitions to the same
                        // constellation.
                        mCRL2log(log::debug, "bisim_gjkw")<<" (not added 1)\n";
                        succ_iter = succ_iter->constln_slice->end;
                        continue;
                    }
                    if (*s_iter != succ_iter->target)
                    {
                        // The transition was not a self-loop to start with.
                        // So we do not add it either.
                        ++succ_iter;
                        mCRL2log(log::debug, "bisim_gjkw") << " (not added)\n";
                        continue;
                    }
                }
                assert((label_type) -1 != tau_label);
                aut.add_transition(transition(s_eq, tau_label, t_eq));
                mCRL2log(log::debug, "bisim_gjkw")
                                           << " (added as inert transition)\n";
            }
            else
            {
                state_type tgt_id = succ_iter->target -
                                                  &*part_st.state_info.begin();
                // We have a non-inert transition to an intermediary state.
                // Look up the label and where the transition from the
                // intermediary state goes.
                Key k = to_lts_map.find(tgt_id)->second;
                t_eq = part_st.state_info[k.second].block->seqnr();
                mCRL2log(log::debug, "bisim_gjkw")
                             << ", i. e. indirectly to block " << t_eq << "\n";
                assert(BLOCK_NO_SEQNR != t_eq);
                aut.add_transition(transition(s_eq, k.first, t_eq));
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
            mCRL2log(log::debug, "bisim_gjkw") << "succ_iter is now " << &*succ_iter << '\n';
        }
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
        // 2.8: Mark block SpB as refinable
        {
            bool SpB_was_not_refinable = SpB->make_refinable();
            (void) SpB_was_not_refinable; //< avoid warnig on unused variable
            assert(SpB_was_not_refinable);
        }
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
                assert(part_tr.pred_end() > pred_iter &&
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
                s_prime->set_current_constln(part_tr.change_to_C(pred_iter,
                              SpC, SpB->constln(), first_transition_of_state,
                                                   first_transition_of_block));
            // 2.16: end for
            }
            // 2.17: Register that the transitions from s to out_inert(s) go to
            //       NewC (instead of SpC)
                // (In the loop, we only adapt the ``succ'' transition array.)
            // and
            // 2.18: Store whether s still has some transition to SpC\SpB
            s->set_current_constln(part_tr.split_s_inert_out(s, SpC));
            assert(s->succ_begin() == s->current_constln() ||
                          *s->current_constln()[-1].target->constln() <= *SpC);
            assert(s->succ_end() == s->current_constln() ||
                             *SpC <= *s->current_constln()->target->constln());
        // 2.19: end for
        }
        mCRL2log(log::debug, "bisim_gjkw") << "\n";
#ifndef NDEBUG
        // The following tests cannot be executed during the above loops
        // because a state s_prime may have multiple transitions to SpB.
        for (bisim_gjkw::permutation_iter_t s_iter = SpB->begin();
                                                SpB->end() != s_iter; ++s_iter)
        {
            const bisim_gjkw::state_info_ptr s = *s_iter;
            for (bisim_gjkw::pred_iter_t pred_iter = s->noninert_pred_begin();
                            s->noninert_pred_end() != pred_iter; ++pred_iter)
            {
                const bisim_gjkw::state_info_ptr s_prime = pred_iter->source;
                // check consistency of s_prime->current_constln()
                assert(s_prime->succ_begin() == s_prime->current_constln() ||
                      *s_prime->current_constln()[-1].target->constln()<=*SpC);
                assert(s_prime->succ_end() == s_prime->current_constln() ||
                       *SpC <= *s_prime->current_constln()->target->constln());
                // check consistency of s_prime->inert_succ_begin() and
                // s_prime->inert_succ_end()
                assert(s_prime->succ_begin() == s_prime->inert_succ_begin() ||
                          *s_prime->inert_succ_begin()[-1].target->constln() <=
                                                          *s_prime->constln());
                assert(s_prime->succ_begin() == s_prime->inert_succ_begin() ||
                               s_prime->inert_succ_begin()[-1].target->block !=
                                                               s_prime->block);
                assert(s_prime->inert_succ_begin()==s_prime->inert_succ_end()||
                          (s_prime->inert_succ_begin()->target->block ==
                                                            s_prime->block &&
                           s_prime->inert_succ_end()[-1].target->block ==
                                                              s_prime->block));
                assert(s_prime->succ_end() == s_prime->inert_succ_end() ||
                          *s_prime->constln() <
                                *s_prime->inert_succ_end()->target->constln());
            }
            // check consistency of s->inert_succ_begin() and
            // s->inert_succ_end()
            assert(s->succ_begin() == s->inert_succ_begin() ||
                  *s->inert_succ_begin()[-1].target->constln()<=*s->constln());
            assert(s->succ_begin() == s->inert_succ_begin() ||
                          s->inert_succ_begin()[-1].target->block != s->block);
            assert(s->inert_succ_begin() == s->inert_succ_end() ||
                          (s->inert_succ_begin()->target->block == s->block &&
                           s->inert_succ_end()[-1].target->block == s->block));
            assert(s->succ_end() == s->inert_succ_end() ||
                      *s->constln() < *s->inert_succ_end()->target->constln());
        }
#endif

        /*------------------ stabilise the partition again ------------------*/

        // 2.20: for all refinable blocks RefB do
        while (nullptr != bisim_gjkw::block_t::get_some_refinable())
        {
            // There are at most m log n blocks that contain predecessors of
            // splitters, but there may be an additional n splitters that need
            // to be refined.
            bisim_gjkw::check_complexity::count("for all refinable blocks", 1,
                                        bisim_gjkw::check_complexity::m_log_n +
                                              bisim_gjkw::check_complexity::n);
            bisim_gjkw::block_t* RefB =
                                    bisim_gjkw::block_t::get_some_refinable();
            mCRL2log(log::debug, "bisim_gjkw") << "Refining "
                                                << RefB->debug_id() << ".\n";
            // 2.21: Mark RefB as non-refinable
            RefB->make_nonrefinable();
            // 2.22: RedB := PrimaryRefine(RefB, SpB, marked states in RefB,
            //                             unmarked bottom states in RefB)
            bisim_gjkw::block_t* RedB = primary_refine(RefB, SpB->constln());
            // 2.23: Unmark all states of RefB
                // (for the blue block and the red nonbottom states, is already
                // done in primary_refine)
            RedB->set_marked_bottom_begin(RedB->end());

            if (RedB->size() > 1 && nullptr != RedB->FromRed(SpC))
            {
                // If the block has 1 element, or it has no transitions to SpC,
                // the refinement would be trivial anyway.

                // 2.24: SecondaryRefine(RedB, SpC\SpB, states in RedB with a
                //                       transition to SpC\SpB, bottom states
                //                       in RedB without transition to SpC\SpB)
                RedB = secondary_refine(RedB, SpC, RedB->FromRed(SpC));
                if (nullptr != RedB)
                {
                    RedB->set_marked_bottom_begin(RedB->end());
                }
            }

            /*---------------- postprocess new bottom states ----------------*/

            // 2.27: Postprocess new bottom states
            postprocess_new_bottom();
        // 2.25: end for
        }
        #ifndef NDEBUG
            part_st.print_part(part_tr);
            part_st.print_trans();
        #endif
    // 2.29: end while
    }
    // 2.30: return C
        // (this happens implicitly, through the bisim_partitioner_gjkw object
        // data)
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
   primary_refine(bisim_gjkw::block_t* RefB, const bisim_gjkw::constln_t* NewC)
{
    const bisim_gjkw::permutation_iter_t red_end = RefB->end();
    bisim_gjkw::block_t* RedB;

    mCRL2log(log::debug, "bisim_gjkw") << "primary_refine(" << RefB->debug_id()
                                                                      << ")\n";
    // 3.3: Spend the same amount of work on either coroutine:
    // and
    // 3.29: RedB := RefB  or  RedB := NewB , respectively
    RUN_COROUTINES(primary_blue, (RefB, NewC), RedB = RefB,
                   primary_red,  (RefB, NewC), RedB = red_end[-1]->block,
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
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB,
                                const bisim_gjkw::constln_t*, NewC),
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
        RefB->set_marked_nonbottom_begin(RefB->nonbottom_end());
        // RefB->set_unmarked_bottom_end(RefB->unmarked_bottom_begin());
        mCRL2log(log::debug, "bisim_gjkw") << "primary_blue: empty\n";
        TERMINATE_COROUTINE_SUCCESSFULLY();
    }
    blue_nonbottom_end = RefB->unmarked_nonbottom_begin();
    COROUTINE_DO_WHILE(PRIMARY_BLUE_STATE_HANDLED,
                                             blue_nonbottom_end != visited_end)
    {
        // The time complexity to find the smaller block is the number of its
        // incoming (inert) edges, but at least 1 per state (m log n+n log n).
        // Additionally, the other procedure may take one superfluous step per
        // refinement (m log n+n).
        bisim_gjkw::check_complexity::count(primary_search, 1,
                                        bisim_gjkw::check_complexity::m_log_n +
                                        bisim_gjkw::check_complexity::m_log_n +
                                        bisim_gjkw::check_complexity::n_log_n +
                                              bisim_gjkw::check_complexity::n);
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
                                        bisim_gjkw::check_complexity::m_log_n +
                                        bisim_gjkw::check_complexity::m_log_n +
                                        bisim_gjkw::check_complexity::n_log_n +
                                              bisim_gjkw::check_complexity::n);
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
    part_tr.new_blue_block_created(RefB, NewB, true);
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
            assert(part_tr.pred_end() > pred_iter &&
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
                RefB->set_marked_nonbottom_begin(RefB->bottom_begin() - 1);
                RefB->set_bottom_begin(RefB->marked_nonbottom_begin());
                swap_permutation(s_prime->pos, RefB->bottom_begin());
                assert((s_prime->current_constln() < s_prime->succ_end() &&
                    s_prime->current_constln()->target->constln() == NewC) ||
                   (s_prime->current_constln() > s_prime->succ_begin() &&
                    s_prime->current_constln()[-1].target->constln() == NewC));
                (void) NewC; //< avoid warning about unused variable
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
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB,
                                const bisim_gjkw::constln_t*, NewC),
    /* local variables:     */ (bisim_gjkw::permutation_iter_t, visited_begin,
                                bisim_gjkw::state_info_ptr, s,
                                bisim_gjkw::pred_iter_t, pred_iter),
    /* shared data:         */ bisim_gjkw::permutation_iter_t,
                                                    notblue_initialised_end,
    /* interrupt locations: */ (PRIMARY_RED_PREDECESSOR_HANDLED,
                                PRIMARY_RED_STATE_HANDLED))
{
    assert(0 < RefB->marked_size());
    // 3.4r: whenever |Red| > |RefB| / 2 do  Abort this coroutine
    if (RefB->marked_size() > RefB->size() / 2)  ABORT_THIS_COROUTINE();

    /*  -  -  -  -  -  -  -  - visit red states -  -  -  -  -  -  -  -  */

    // 3.5r: while Red contains unvisited states do
    visited_begin = RefB->marked_bottom_end();
    if (RefB->marked_bottom_begin() == visited_begin)
        visited_begin = RefB->marked_nonbottom_end();
    COROUTINE_DO_WHILE(PRIMARY_RED_STATE_HANDLED,
                               RefB->marked_nonbottom_begin() != visited_begin)
    {
        bisim_gjkw::check_complexity::count(primary_search, 1,
                                        bisim_gjkw::check_complexity::m_log_n +
                                        bisim_gjkw::check_complexity::m_log_n +
                                        bisim_gjkw::check_complexity::n_log_n +
                                              bisim_gjkw::check_complexity::n);
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
                                        bisim_gjkw::check_complexity::m_log_n +
                                        bisim_gjkw::check_complexity::m_log_n +
                                        bisim_gjkw::check_complexity::n_log_n +
                                              bisim_gjkw::check_complexity::n);
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
    part_tr.new_red_block_created(RefB, NewB, true);
    // 3.21r: for all nonbottom s in NewB do
        // we have to run through the states backwards because otherwise, we
        // might miss out some states.
    for (bisim_gjkw::permutation_iter_t s_iter = NewB->nonbottom_end();
                                           NewB->nonbottom_begin() != s_iter; )
    {
        bisim_gjkw::check_complexity::count(for_all_states_in_the_new_block, 1,
                                        bisim_gjkw::check_complexity::n_log_n);
        --s_iter;
        const bisim_gjkw::state_info_ptr s = *s_iter;
        // 3.22r: for all s_prime in inert_out(s) \ New do
        for (bisim_gjkw::succ_iter_t succ_iter = s->inert_succ_begin();
                                 s->inert_succ_end() != succ_iter; ++succ_iter)
        {
            bisim_gjkw::check_complexity::count(
                              for_all_incoming_transitions_of_the_new_block, 1,
                                        bisim_gjkw::check_complexity::m_log_n);
            assert(part_tr.succ_end() > succ_iter &&
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
            NewB->set_marked_nonbottom_begin(NewB->bottom_begin() - 1);
            NewB->set_bottom_begin(NewB->marked_nonbottom_begin());
            swap_permutation(s->pos, NewB->bottom_begin());
            assert((s->current_constln() < s->succ_end() &&
                    s->current_constln()->target->constln() == NewC) ||
                   (s->current_constln() > s->succ_begin() &&
                    s->current_constln()[-1].target->constln() == NewC));
            (void) NewC; /* avoid warning about unused variable */
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
that distribute all bottom and some nonbottom states over red and blue.  There
may be some bottom states that are already known to be red -- these states will
be marked.  For the rest, the function has to colour both bottom and nonbottom
states.  When calling, the only structure is:

|            nonbottom             |           unmarked           |marked |
|                                  |            bottom            |bottom |

All unmarked bottom and some nonbottom states have their current constellation
pointer set in a way that it is easy to check whether the state has a
transition to SpC\SpB.

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
                                  const bisim_gjkw::B_to_C_descriptor* FromRed,
                                  bool split_unreachable_new_bottom/*=false*/)
{
    const bisim_gjkw::permutation_iter_t red_end = RefB->end();
    bisim_gjkw::block_t* RedB;

    assert(RefB->marked_nonbottom_begin() == RefB->marked_nonbottom_end());
    assert(nullptr == FromRed || FromRed->from_block() == RefB);
    assert(nullptr == FromRed || FromRed->to_constln() == SpC);
    mCRL2log(log::debug, "bisim_gjkw") << "secondary_refine("
        << RefB->debug_id() << "," << SpC->debug_id() << ","
        <<(nullptr!=FromRed ?FromRed->debug_id() :std::string("NULL")) <<")\n";
    // 4.3: Spend the same amount of work on either coroutine:
    // and
    // 3.29: RedB := RefB  or  RedB := NewB , respectively
    RUN_COROUTINES(secondary_blue, (RefB, SpC, split_unreachable_new_bottom),
                                                         RedB = RefB,
                   secondary_red,  (RefB, SpC, FromRed), RedB =
                                                (red_end[-1]->block == RefB
                                                 ?nullptr :red_end[-1]->block),
               /* shared data: */ struct bisim_gjkw::secondary_refine_shared,
                                        ({ RefB->nonbottom_begin(), false }));
    // 4.33: return RedB
    return RedB;
}

/*--------------------------- handle blue states ----------------------------*/

template <class LTS_TYPE>
DEFINE_COROUTINE(bisim_partitioner_gjkw<LTS_TYPE>::, secondary_blue,
    /* formal parameters:   */ (bisim_gjkw::block_t*, RefB,
                                const bisim_gjkw::constln_t*, SpC,
                                bool, split_unreachable_new_bottom),
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
        // A consequence of the changed order of states is that the blue bottom
        // states are counted twice:  once here and once when we look for their
        // predecessors.
        //
        // If the blue subblock is smaller, we spend the following time:
        // * to find the blue bottom states:
        //   + if called from the main loop: the number of marked bottom states
        //     m_noninert log n (predecessors) + n_bottom log n (splitters)
        //   + if called from postprocess_new_bottom: the number of blue bottom
        //     states: n_bottom log n, plus all outgoing non-inert edges of new
        //     bottom states: m_noninert
        //   The maximum of these two is m_noninert log n + n_bottom log n.
        //   (The second case only happens if n >= 2, and then log n >= 1.)
        // * to find predecessors of blue states: all states and all incoming
        //   (inert) edges of this subblock: n log n + m_inert log n
        // * for the (slow) test whether some state is really blue:
        //   + all outgoing edges of this subblock: m log n
        //   + all outgoing edges of new bottom states: m
        // * the red coroutine may spend the same amount of time, plus one
        //   extra step per refinement: m_noninert log n + n
        // The total is, therefore, 5m log n + 4n log n + 2m + n.
        // If the red subblock is smaller, we spend the following time:
        // * to find the FromRed states:
        //   + all outgoing edges of this subblock: m_noninert log n
        // * all states and all incoming (inert) edges of this subblock:
        //   n log n + m_inert log n
        // * the blue coroutine may spend the same amount of time, plus one
        //   extra step per refinement: m_noninert log n + n
        // The total is, therefore, 3m log n + 2n log n + n.
        // Obviously, the first of the two totals is larger.
        bisim_gjkw::check_complexity::count(secondary_search, 1,
                                    bisim_gjkw::check_complexity::m_log_n * 5 +
                                    bisim_gjkw::check_complexity::n_log_n * 4 +
                                          bisim_gjkw::check_complexity::m * 2 +
                                              bisim_gjkw::check_complexity::n);
        // 4.7l: Choose an unvisited s in MaybeBlue.
        s = *visited_end;
        // 4.8l (order of lines changed): Mark s as visited.
        ++visited_end;
        // 4.10l: if not isBlueTest(s) then
        if (s->surely_has_transition_to(SpC))
        {
            assert(!split_unreachable_new_bottom);
            (void) split_unreachable_new_bottom; //< avoid warning about unused
                                                 //  variable
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
        assert(split_unreachable_new_bottom ||
                                          s->surely_has_no_transition_to(SpC));
        // 4.13l: Blue := Blue union {s}
            // already done by incrementing visited_end
            mCRL2log(log::debug, "bisim_gjkw") <<s->debug_id() <<" is blue.\n";
        // 4.5l: whenever |Blue| > |RefB|/2 do  Abort this coroutine
        if ((state_type) (visited_end - RefB->unmarked_bottom_begin()) >
                                                              RefB->size() / 2)
        {
            ABORT_THIS_COROUTINE();
        }
    // 4.27l: end while
    }
    END_COROUTINE_WHILE;

    if (0 == RefB->unmarked_bottom_size())
    {
        RefB->set_marked_nonbottom_begin(RefB->marked_nonbottom_end());
        // RefB->set_marked_bottom_begin(RefB->bottom_begin());
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
                                    bisim_gjkw::check_complexity::m_log_n * 5 +
                                    bisim_gjkw::check_complexity::n_log_n * 4 +
                                          bisim_gjkw::check_complexity::m * 2 +
                                              bisim_gjkw::check_complexity::n);
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
                                    bisim_gjkw::check_complexity::m_log_n * 5 +
                                    bisim_gjkw::check_complexity::n_log_n * 4 +
                                          bisim_gjkw::check_complexity::m * 2 +
                                              bisim_gjkw::check_complexity::n);
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
                                    bisim_gjkw::check_complexity::m_log_n * 5 +
                                    bisim_gjkw::check_complexity::n_log_n * 4 +
                                          bisim_gjkw::check_complexity::m * 2 +
                                              bisim_gjkw::check_complexity::n);
                    // binary search for transitions from
                    // s_prime to constellation SpC.
                    bisim_gjkw::succ_const_iter_t mid = begin + (end-begin)/2;
                    if (*SpC <= *mid->target->constln())
                    {
                        end = mid->constln_slice->begin;
                    }
                    if (*mid->target->constln() <= *SpC)
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
    part_tr.new_blue_block_created(RefB, NewB, false);

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
            assert(part_tr.pred_end() > pred_iter &&
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
                RefB->set_marked_nonbottom_begin(RefB->bottom_begin() - 1);
                RefB->set_bottom_begin(RefB->marked_nonbottom_begin());
                swap_permutation(s_prime->pos, RefB->bottom_begin());
                assert((s_prime->current_constln() < s_prime->succ_end() &&
                    s_prime->current_constln()->target->constln() == SpC) ||
                   (s_prime->current_constln() > s_prime->succ_begin() &&
                    s_prime->current_constln()[-1].target->constln() == SpC));
                // the following statement is only needed to let the assertion
                // ``assert(s->surely_has_no_transition_to(SpC));'' (just above
                // Line 4.13) go through.
                s_prime->set_current_constln(s_prime->succ_begin());
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
                                const bisim_gjkw::constln_t*, SpC,
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
    if (RefB->marked_size() > RefB->size() / 2)
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
        assert(FromRed->from_block() == RefB);
        assert(FromRed->to_constln() == SpC);
        (void) SpC; //< avoid warning about unused variable
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
                                    bisim_gjkw::check_complexity::m_log_n * 5 +
                                    bisim_gjkw::check_complexity::n_log_n * 4 +
                                          bisim_gjkw::check_complexity::m * 2 +
                                              bisim_gjkw::check_complexity::n);
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
        // RefB->unmark_all_states(); // not needed: there are no marked states
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
                                    bisim_gjkw::check_complexity::m_log_n * 5 +
                                    bisim_gjkw::check_complexity::n_log_n * 4 +
                                          bisim_gjkw::check_complexity::m * 2 +
                                              bisim_gjkw::check_complexity::n);
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
                                    bisim_gjkw::check_complexity::m_log_n * 5 +
                                    bisim_gjkw::check_complexity::n_log_n * 4 +
                                          bisim_gjkw::check_complexity::m * 2 +
                                              bisim_gjkw::check_complexity::n);
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
    part_tr.new_red_block_created(RefB, NewB, false);

    // 4.31: Find new non-inert transitions and bottom states
    // (as Lines 3.21-3.28)

    // 3.21r: for all nonbottom s in NewB do
        // we have to run through the states backwards because otherwise, we
        // might miss out some states.
    for (bisim_gjkw::permutation_iter_t s_iter = NewB->nonbottom_end();
                                           NewB->nonbottom_begin() != s_iter; )
    {
        bisim_gjkw::check_complexity::count(for_all_states_in_the_new_block, 1,
                                        bisim_gjkw::check_complexity::n_log_n);
        --s_iter;
        const bisim_gjkw::state_info_ptr s = *s_iter;
        // 3.22r: for all s_prime in inert_out(s) \ New do
        for (bisim_gjkw::succ_iter_t succ_iter = s->inert_succ_begin();
                                s->inert_succ_end()!=succ_iter; ++succ_iter)
        {
            bisim_gjkw::check_complexity::count(
                            for_all_incoming_transitions_of_the_new_block, 1,
                                        bisim_gjkw::check_complexity::m_log_n);
            assert(part_tr.succ_end() > succ_iter &&
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
            NewB->set_marked_nonbottom_begin(NewB->bottom_begin() - 1);
            NewB->set_bottom_begin(NewB->marked_nonbottom_begin());
            swap_permutation(s->pos, NewB->bottom_begin());
            assert((s->current_constln() < s->succ_end() &&
                    s->current_constln()->target->constln() == SpC) ||
                   (s->current_constln() > s->succ_begin() &&
                    s->current_constln()[-1].target->constln() == SpC));
            // the following statement is only needed to let the assertion
            // ``assert(s->surely_has_no_transition_to(SpC));'' (just above
            // Line 4.13) go through.
            s->set_current_constln(s->succ_begin());
        // 3.27r: end if
        }
    // 3.28r: end for
    }
}
END_COROUTINE



/*=============================================================================
=              PostprocessNewBottom -- Algorithm 5 in [GJKW 2017]             =
=============================================================================*/


/// \brief function object to compare two constln_t pointers based on their
/// contents
class constln_ptr_less
{
  public:
    bool operator() (const bisim_gjkw::constln_t* a,
                                          const bisim_gjkw::constln_t* b) const
    {
        return *a < *b;
    }
};


typedef std::set<bisim_gjkw::constln_t*, constln_ptr_less> R_map_t;

/// \brief Split a block with new bottom states as needed
/// \details The function splits SplitB by checking whether all new bottom
/// states can reach the constellations that SplitB can reach.
///
/// When this function starts, it assumes that the general new_bottom_states
/// list already has been processed and the old bottom states of SplitB are
/// marked.
template <class LTS_TYPE>
void bisim_partitioner_gjkw<LTS_TYPE>::postprocess_block(
                                                   bisim_gjkw::block_t* SplitB)
{
    mCRL2log(log::debug, "bisim_gjkw") << "Postprocessing "
                                                 << SplitB->debug_id() << "\n";

    /*------- collect constellations reachable from new bottom states -------*/

    // 5.3: Create an empty search tree R
    R_map_t R;
    // 5.4: repeat
Line_5_4:
    bisim_gjkw::permutation_iter_t orig_new_bottom_end =
                                                 SplitB->unmarked_bottom_end();
    assert(SplitB->unmarked_bottom_begin() < SplitB->unmarked_bottom_end());
    do
    {
        bisim_gjkw::check_complexity::count("repeat until no more new bottom "
                       "states are found", 1, bisim_gjkw::check_complexity::n);
        // 5.5: for all new bottom states b in SplitB (except those handled
        //              earlier) do
        for (bisim_gjkw::permutation_iter_t
                        b_iter = SplitB->unmarked_bottom_begin();
                             SplitB->unmarked_bottom_end() != b_iter; ++b_iter)
        {
            bisim_gjkw::check_complexity::count("for all new bottom states in "
                                 "SplitB", 1, bisim_gjkw::check_complexity::n);
            bisim_gjkw::state_info_ptr b = *b_iter;
            // 5.6: for all constellations C reachable from b do
            for (bisim_gjkw::succ_iter_t C_iter = b->succ_begin();
                  b->succ_end() != C_iter; C_iter = C_iter->constln_slice->end)
            {
                bisim_gjkw::check_complexity::count("for all constellations C "
                       "reachable from b", 1, bisim_gjkw::check_complexity::m);
                assert(part_tr.succ_end() > C_iter &&
                                         C_iter->B_to_C->pred->succ == C_iter);
                bisim_gjkw::constln_t* C = C_iter->target->constln();
                bisim_gjkw::B_to_C_desc_iter_t new_slice =
                                                  C_iter->B_to_C->B_to_C_slice;
                // 5.7: Add C to R
                // and
                // 5.8: Register that the transitions from SplitB to C need
                //      postprocessing
                if (R.insert(C).second) //< complexity log(n)
                {
                    mCRL2log(log::debug, "bisim_gjkw") << "\tcan reach "
                                                      << C->debug_id() << "\n";
                    // the constellation was not yet in R: no transitions to C
                    // are registered as needing postprocessing.
                    assert(C->postprocess_begin == C->postprocess_end);
                    C->postprocess_end = new_slice->end;
                }
                else if (C->postprocess_begin <= new_slice->begin)
                {
                    // the constellation already was in R and the transitions
                    // from SplitB to C are already registered.  (Strict <
                    // instead of == is possible after backtracking.)
                    assert(new_slice->end <= C->postprocess_end);
                    assert(new_slice->new_bottom_end >= orig_new_bottom_end);
                    continue;
                }
                else
                {
                    // the constellation already was in R, but the transitions
                    // still need to be registered.  This only happens after
                    // backtracking (see Line 5.25 below).
                    assert(C->postprocess_begin == new_slice->end);
                }
                C->postprocess_begin = new_slice->begin;
                assert(C->postprocess_begin < C->postprocess_end);
                new_slice->new_bottom_end = orig_new_bottom_end;
                assert(new_slice->from_block() == SplitB);
                SplitB->to_constln.splice(SplitB->to_constln.end(),
                                                SplitB->to_constln, new_slice);
            // 5.9: end for
            }
            // 5.10: Set the current constellation pointer of b to the first
            //       constellation it can reach
            b->set_current_constln(b->succ_begin());
        // 5.11: end for
        }
        // 5.12: if SplitB can reach some constellation C not in R then
        assert(SplitB->to_constln.begin() != SplitB->to_constln.end());
        if (!SplitB->to_constln.begin()->needs_postprocessing())
        {
            bisim_gjkw::constln_t*C = SplitB->to_constln.begin()->to_constln();
            mCRL2log(log::debug, "bisim_gjkw") << "\tcanNOT reach "
                                                      << C->debug_id() << "\n";
            // 5.13: SplitB := SecondaryRefine(SplitB, C, states in SplitB with
            //                 a transition to C, new bottom states in SplitB)
            SplitB = secondary_refine(SplitB, C, SplitB->FromRed(C), true);
            new_bottom_states.clear();
            assert(SplitB->marked_bottom_begin() <= orig_new_bottom_end &&
                           orig_new_bottom_end <= SplitB->marked_bottom_end());
        // 5.14: end if
        }
    // 5.15: until Line 5.13 was skipped ...
        else
        {
            break;
        }
    // 5.15 (continued): ... or Line 5.13 did not find more new bottom states
    }
    while (SplitB->unmarked_bottom_begin() != SplitB->unmarked_bottom_end());

    /*---------------- stabilise w.r.t. found constellations ----------------*/

    // 5.16: for all constellations SpC in R (in order) do
    while (R.begin() != R.end())
    {
        bisim_gjkw::check_complexity::count(
                                   "for all constellations in R (in order)", 1,
                                              bisim_gjkw::check_complexity::m);
        bisim_gjkw::constln_t* SpC = *R.begin();
        // 5.17: for all blocks B with transitions to SpC that need
        //               postprocessing do
        while (SpC->postprocess_begin != SpC->postprocess_end)
        {
            bisim_gjkw::check_complexity::count(
                                "for all blocks B with transitions to C", 1,
                                        bisim_gjkw::check_complexity::m_log_n);
                                                        //< or perhaps m * 2?
            bisim_gjkw::B_to_C_iter_t B_iter = SpC->postprocess_begin;
            assert(part_tr.B_to_C_end() > B_iter &&
                                         B_iter->pred->succ->B_to_C == B_iter);
            bisim_gjkw::block_t* B = B_iter->pred->source->block;
            mCRL2log(log::debug, "bisim_gjkw") << "Now postprocessing "
                                                      << B->debug_id() << "\n";
            const bisim_gjkw::B_to_C_desc_iter_t FromRed=B_iter->B_to_C_slice;
            const bisim_gjkw::permutation_iter_t new_bottom_end =
                                                       FromRed->new_bottom_end;
            assert(FromRed->begin == B_iter);
            // 5.18: Delete the transitions from B to SpC from those that need
            //       postprocessing
            SpC->postprocess_begin = FromRed->end;
            assert(SpC->postprocess_begin <= SpC->postprocess_end);
            assert(B->to_constln.begin() != B->to_constln.end());
            if (B->to_constln.begin() != FromRed)
            {
                assert(FromRed->from_block() == B);
                B->to_constln.splice(B->to_constln.begin(), B->to_constln,
                                                                      FromRed);
            }
            FromRed->new_bottom_end = B->end() + 1;
            // 5.19: RedB := SecondaryRefine(B, SpC, states in B with a
            //               transition to SpC, new bottom states in B without
            //               transition to SpC)
            B->set_marked_bottom_begin(new_bottom_end);
            bisim_gjkw::block_t* RedB = secondary_refine(B, SpC, &*FromRed);
            new_bottom_states.clear();
            // 5.20: for all new bottom states s in RedB (except those just
            //                                      found in Line 5.19) do
            for (bisim_gjkw::permutation_iter_t
                                s_iter = RedB->marked_bottom_begin();
                                            s_iter != new_bottom_end; ++s_iter)
            {
                bisim_gjkw::check_complexity::count(
                                            "for all red new bottom states", 1,
                                              bisim_gjkw::check_complexity::m);
                bisim_gjkw::state_info_ptr s = *s_iter;
                // 5.21: Advance the current constellation pointer of s to the
                //       next constellation it can reach
                s->set_current_constln(s->current_constln()->constln_slice->
                                                                          end);
                assert(s->succ_begin() < s->current_constln() &&
                            s->current_constln()[-1].target->constln() == SpC);
                assert(s->succ_end() == s->current_constln() ||
                              *SpC < *s->current_constln()->target->constln());
            // 5.22: end for
            }
            // 5.23: if Line 5.19 has found more new bottom states then
            if (RedB->unmarked_bottom_begin() != RedB->unmarked_bottom_end())
            {
                // 5.24: SplitB := RedB
                SplitB = RedB;
                // 5.25: Restart the procedure (but keep R),
                //       i. e. go to Line 5.4
                goto Line_5_4;
            // 5.26: end if
            }
            // 5.30: Destroy all temporary data
                // As part of this line, we register that no more states are to
                // be regarded as old
            RedB->set_marked_bottom_begin(RedB->end());
        // 5.27: end for
        }
        // 5.28: Delete SpC from R
        R.erase(R.begin());
    // 5.29: end for
    }
    // 5.30: Destroy all temporary data
    assert(R.empty());

    mCRL2log(log::debug, "bisim_gjkw") << "Finished postprocessing\n";
}


template <class LTS_TYPE>
void bisim_partitioner_gjkw<LTS_TYPE>::postprocess_new_bottom()
{
    bisim_gjkw::block_t* B1 = nullptr, * B2 = nullptr;

    /*---------------- collect blocks with new bottom states ----------------*/

    // for all new bottom states b do
        // (we iterate over the new bottom states backwards because we expect
        // to do fewer swaps then)
    for (std::vector<bisim_gjkw::state_info_ptr>::iterator b_iter =
                new_bottom_states.end(); new_bottom_states.begin() != b_iter; )
    {
        bisim_gjkw::check_complexity::count(
                                    "collect blocks with new bottom states", 1,
                                              bisim_gjkw::check_complexity::n);
        --b_iter;
        const bisim_gjkw::state_info_ptr b = *b_iter;

        if (b->block != B1)
        {
            // if the block of b was not yet found then
            if (b->block != B2)
            {
                assert(nullptr == B2);
                // Mark the bottom states of this block as old
                b->block->set_marked_bottom_begin(b->block->bottom_begin());
            }
            B2 = B1;
            B1 = b->block;
        }
        // Move b to the non-old bottom states
            // (this swap is trivial if b was added to the new bottom states as
            // a result of the most recent refinement)
        bisim_gjkw::swap_permutation(b->pos, B1->marked_bottom_begin());
        B1->set_marked_bottom_begin(B1->marked_bottom_begin() + 1);
    // end for
    }
    // empty the set of new bottom states
    new_bottom_states.clear();

    /*----------------------- handle collected blocks -----------------------*/

    // postprocess the one or two found blocks
    if (nullptr != B1)
    {
        postprocess_block(B1);
        if (nullptr != B2)
        {
            postprocess_block(B2);
        }
    }
    else
    {
        assert(nullptr == B2);
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
