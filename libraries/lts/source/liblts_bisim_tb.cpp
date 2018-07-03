// Author(s): David N. Jansen, Radboud Universiteit, Nijmegen, The Netherlands
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file liblts_bisim_tb.cpp
///
/// \brief O(m log n)-time branching bisimulation algorithm
///
/// \details This file implements an efficient partition refinement algorithm
/// for labelled transition systems inspired by Groote / Jansen / Keiren / Wijs
/// and Valmari (2009) to calculate the branching bisimulation classes of a
/// labelled transition system.
///
/// \author David N. Jansen, Institute of Software, Chinese Academy of
/// Sciences, Beijing, China

#include <iterator>

#include "mcrl2/lts/detail/liblts_bisim_tb.h"
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
namespace bisim_tb
{



/// \brief forward iterator to visit all transitions in a bunch
/// \details Normally one visits the transitions in a bunch starting somewhere
/// in the middle, just from one of the basic sets of transitions. The iterator
/// first extends to one side and then to the other side to find all
/// transitions.
/// Internally, it stores two pointers: the current entry and the base.  If the
/// base is not nullptr, we are exploring forward until we no longer find
/// transitions in the same bunch; then we restart from the base and explore
/// backward.  When exploring backwards, the base is nullptr.  If both base and
/// the current entry are nullptr, we have reached the end.
/*
class bunch_iterator_t
{
  private:
    B_a_B_ptr_t entry;
    B_a_B_desc_iter_t base; //< is nonnull <==> forward

    // create the begin iterator
    bunch_iterator_t(B_a_B_desc_iter_t FromRed)
      : entry(//nullptr==FromRed || FromRed->begin()==FromRed->end() ?nullptr :
                                                             FromRed->begin()),
        base(//nullptr==FromRed || FromRed->begin()==FromRed->end() ? nullptr :
                                                                       FromRed)
    {  };
    friend class ...;
  public:
    bunch_iterator_t& operator++()
    {
        B_a_B_desc_iter_t slice = entry->slice;
        ++entry;
        if (slice->end() == entry)
        {
            B_a_B_desc_iter_t new_slice;
            if (nullptr != base)
            {
                new_slice = std::next(slice);
                if (nullptr==new_slice || new_slice->bunch() != slice->bunch())
                {
                    new_slice = std::prev(base);
                    base = nullptr;
                    if (nullptr==new_slice||new_slice->bunch()!=slice->bunch())
                    {
                        // end reached
                        entry = nullptr;
                        // base = nullptr;
                        return *this;
                    }
                }
            }
            else
            {
                new_slice = std::prev(slice);
                if (nullptr==new_slice || new_slice->bunch() != slice->bunch())
                {
                    // end reached
                    entry = nullptr;
                    // base = nullptr;
                    return *this;
                }
            }
            entry = new_slice->begin();
        }
        return *this;
    }
    B_a_B_entry_t& operator*()  {  return *entry;  }
    B_a_B_ptr_t operator->()  {  return entry;  }

    // to simplify comparison with the end() ``iterator'' (which is actually
    // nullptr).
    operator B_a_B_ptr_t()  {  return entry;  }
};
*/
#ifndef NDEBUG
    state_info_const_ptr state_info_entry::s_i_begin;
    state_info_const_ptr state_info_entry::s_i_end;
    permutation_const_iter_t block_t::perm_begin;
#endif
state_type block_t::nr_of_blocks;
bunch_t* bunch_t::first_nontrivial;
block_t* block_t::postprocessing_this_bunch_first;





/* ************************************************************************* */
/*                                                                           */
/*                   R E F I N A B L E   P A R T I T I O N                   */
/*                                                                           */
/* ************************************************************************* */





/// \brief refine the block (the blue subblock is smaller)
/// \details This function is called after a refinement function has found
/// that the blue subblock is the smaller one.  It creates a new block for
/// the blue states.
/// \param blue_nonbottom_end iterator past the last blue non-bottom state
/// \returns pointer to the new (blue) block
block_t* block_t::split_off_blue(permutation_iter_t blue_nonbottom_end)
{
    assert(unmarked_nonbottom_end() >= blue_nonbottom_end);
    assert(unmarked_nonbottom_begin() <= blue_nonbottom_end);
    assert(0 != unmarked_bottom_size());
    permutation_iter_t const splitpoint = blue_nonbottom_end +
                                                        unmarked_bottom_size();
    assert(splitpoint < end() && begin() < splitpoint);
    assert((state_type) (splitpoint - begin()) <= size()/2);
    // It is not necessary to reset the nottoblue counters; these counters are
    // anyway only valid for the maybe-blue states.
    state_type swapcount = std::min(unmarked_bottom_size(),
                   (state_type) (marked_nonbottom_end() - blue_nonbottom_end));
    if (0 != swapcount)
    {
        // vector swap the states:
        permutation_iter_t pos1=blue_nonbottom_end, pos2=unmarked_bottom_end();
        state_info_ptr const temp = *pos1;
        for (;;)
        {
            --pos2;
            *pos1 = *pos2;
            (*pos1)->pos = pos1;
            ++pos1;
            if (0 == --swapcount)  break;
            *pos2 = *pos1;
            (*pos2)-> pos = pos2;
        }
        *pos2 = temp;
        (*pos2)->pos = pos2;
    }

    // create a new block for the blue states
    block_t* const NewB = new block_t(begin(), splitpoint);
    //mCRL2log(log::debug, "bisim_gjkw") << "Created new " << NewB->debug_id()
    //          << " for " << blue_nonbottom_end - unmarked_nonbottom_begin() +
    //                           unmarked_bottom_size() << " blue state(s).\n";
    // NewB->set_begin(begin());
    NewB->set_bottom_begin(blue_nonbottom_end);
    NewB->set_marked_nonbottom_begin(blue_nonbottom_end);
    // NewB->set_marked_bottom_begin(splitpoint);
    // NewB->set_end(splitpoint);
    // NewB->set_inert_begin(?);
    // NewB->set_inert_end(?);
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        (*s_iter)->block = NewB;
    }

    // adapt the old block: it only keeps the red states
    // set_end(end());
    set_bottom_begin(marked_bottom_begin());
    // set_marked_bottom_begin(marked_bottom_begin());
    set_marked_nonbottom_begin(marked_bottom_begin());
    set_begin(splitpoint);

    return NewB;
}


/// \brief refine the block (the red subblock is smaller)
/// \details This function is called after a refinement function has found
/// that the red subblock is the smaller one.  It creates a new block for
/// the red states.
/// \param red_nonbottom_begin iterator to the first red non-bottom state
/// \returns pointer to the new (red) block
block_t* block_t::split_off_red(permutation_iter_t red_nonbottom_begin)
{
    assert(marked_nonbottom_begin() == red_nonbottom_begin);
    assert(marked_nonbottom_end() >= red_nonbottom_begin);
    assert(0 != marked_size());

    permutation_iter_t const splitpoint = red_nonbottom_begin +
                                                        unmarked_bottom_size();
    assert(begin() < splitpoint && splitpoint < end());
    assert((state_type) (end() - splitpoint) <= size() / 2);

    // It is not necessary to reset the nottoblue counters; these counters are
    // anyway only valid for the maybe-blue states.
    state_type swapcount = std::min(unmarked_bottom_size(),
                  (state_type) (marked_nonbottom_end() - red_nonbottom_begin));
    if (0 != swapcount)
    {
        // vector swap the states:
        permutation_iter_t pos1=red_nonbottom_begin,pos2=unmarked_bottom_end();
        state_info_ptr const temp = *pos1;
        for (;;)
        {
            *pos1 = *--pos2;
            (*pos1)->pos = pos1;
            ++pos1;
            if (0 == --swapcount)  break;
            *pos2 = *pos1;
            (*pos2)->pos = pos2;
        }
        *pos2 = temp;
        (*pos2)->pos = pos2;
    }
    // create a new block for the red states
    block_t* const NewB = new block_t(splitpoint, end());

    //mCRL2log(log::debug, "bisim_gjkw") << "Created new " << NewB->debug_id()
    //                       << " for " << marked_size() << " red state(s).\n";
    // NewB->set_end(end());
mCRL2log(log::debug, "bisim_tb") << "a\n";
    NewB->set_marked_bottom_begin(marked_bottom_begin());
    NewB->set_bottom_begin(marked_bottom_begin());
    NewB->set_marked_nonbottom_begin(marked_bottom_begin());
mCRL2log(log::debug, "bisim_tb") << "b\n";
    // NewB->set_begin(splitpoint);
    // NewB->set_inert_begin(?);
    // NewB->set_inert_end(?);
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        (*s_iter)->block = NewB;
    }

    // adapt the old block: it only keeps the blue states
    // set_begin(begin());
mCRL2log(log::debug, "bisim_tb") << "c\n";
    set_marked_nonbottom_begin(red_nonbottom_begin);
    set_bottom_begin(red_nonbottom_begin);
    set_marked_bottom_begin(splitpoint);
    set_end(splitpoint);
mCRL2log(log::debug, "bisim_tb") << "d\n";

    return NewB;
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

    mCRL2log(log::debug, "bisim_tb") << '\t' << message
                                         << (1 < end - begin ? "s:\n" : ":\n");
    do
    {
        mCRL2log(log::debug, "bisim_tb") << "\t\t" << (*begin)->debug_id();
        if (B != (*begin)->block)
        {
            mCRL2log(log::debug,"bisim_tb") << ", inconsistent: points to "
                                                << (*begin)->block->debug_id();
        }
        if (begin != (*begin)->pos)
        {
            mCRL2log(log::debug, "bisim_tb")
                               << ", inconsistent pointer to state_info_entry";
        }
        mCRL2log(log::debug, "bisim_tb") << '\n';
    }
    while (++begin != end);
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
        mCRL2log(log::debug, "bisim_tb") << B->debug_id() << ":\n";
        print_block("Non-bottom state", B, B->nonbottom_begin(),
                                                           B->nonbottom_end());
        print_block("Bottom state", B, B->bottom_begin(), B->bottom_end());
/*
        mCRL2log(log::debug, "bisim_tb") << "\tThis block has ";
        if (B->inert_end() == part_tr.B_to_C_begin())
        {
            mCRL2log(log::debug, "bisim_tb")
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
*/
        // go to next block
        if (permutation.end() == B->end())  break;
        B = (*B->end())->block;
    }
}

#endif // ifndef NDEBUG






/* ************************************************************************* */
/*                                                                           */
/*                           T R A N S I T I O N S                           */
/*                                                                           */
/* ************************************************************************* */





/// \brief transition is moved to a new bunch
/// \details part_trans_t::move_to_new_bunch has to be called after a
/// transition has changed its bunch.  The member function will adapt the
/// transition data structure.  It assumes that the transition is non-inert.
/// It returns the boundary between transitions in OldBu and transitions in
/// NewBu in the state's outgoing transition array.
/// \param s_iter  transition that has to be changed
/// \param OldBu   splitter bunch
/// \param NewBu   new bunch, where the transition is now in
/// \param first_transition_of_state  This is the first transition of the
///                                   state, so a new bunch_slice has to be
///                                   allocated.
succ_iter_t part_trans_t::move_to_new_bunch(B_a_B_iter_t const s_iter,
                bunch_t* OldBu, bunch_t* NewBu, bool first_transition_of_state)
{
    // adapt the outgoing transition array:
    // move the transition to the beginning (requires that the new bunch is
    // ``smaller'' than / ordered before the old bunch!)
    succ_iter_t const old_out_pos = s_iter->pred->succ;
    // move to beginning
    succ_iter_t const new_out_pos = old_out_pos->bunch_slice->begin;
    out_descriptor* new_bunch_slice;
    if (first_transition_of_state)
    {
        if (1 == old_out_pos->bunch_slice->size())
        {
            // There was only one transition from pred_iter->source to SpC.
            // This transition now points to NewC; the out_descriptors do
            // not have to be changed.
            return new_out_pos + 1;
        }
        new_bunch_slice = new out_descriptor(new_out_pos);
    }
    else
    {
        new_bunch_slice = new_out_pos[-1].bunch_slice;
    }
    assert(old_out_pos->bunch_slice->begin == new_bunch_slice->end);
    assert(new_bunch_slice->end == new_out_pos);
    new_bunch_slice->end = new_out_pos + 1;
    old_out_pos->bunch_slice->begin = new_out_pos + 1;
    if (0 == old_out_pos->bunch_slice->size())
    {
        delete old_out_pos->bunch_slice;
    }
    swap_out(old_out_pos, new_out_pos);
    new_out_pos->bunch_slice = new_bunch_slice;
    return new_out_pos + 1;
}


/// \brief create a new slice for transitions from NewB in the correct bunch
/// \details When one creates a new slice for transitions from NewB, one has
/// to ensure that the order of slices in the list `NewB->from_block` is
/// correct:  slices that belong to the same bunch should be placed together.
/// This function creates a new slice and inserts it in the correct place in
/// that list.
inline B_a_B_desc_iter_t part_trans_t::create_new_slice_for_NewB(
        B_a_B_desc_iter_t const old_B_a_B_slice, block_t* NewB,
              B_a_B_iter_t const new_pos, const bunch_t* const futureFromRedBu)
{
    // As we have to create a slice for transitions
    // starting in NewB, we have to use the same mechanism
    // as above to create that slice.
    bunch_t* const Bu = old_B_a_B_slice->bunch;
    // (`new_B_a_B_slice` is now the position of the new
    // slice in the list `NewB->from_block`.)
    B_a_B_desc_iter_t new_B_a_B_slice;
    if (Bu->new_slice_block != NewB)
    {
        // new_B_a_B_slice is an invalid position, so
        // insert at the end:
        Bu->new_slice_block = NewB;
        new_B_a_B_slice = NewB->from_block.end();
    }
    else
    {
        new_B_a_B_slice = Bu->new_slice_pos;
        assert(NewB->from_block.end() != new_B_a_B_slice);
        assert(new_B_a_B_slice->bunch == Bu);
    }
    new_B_a_B_slice = NewB->from_block.emplace(new_B_a_B_slice, new_pos,
                                          new_pos, old_B_a_B_slice->label, Bu);
    Bu->new_slice_pos = new_B_a_B_slice;
    if (Bu->is_trivial())
    {
        Bu->make_nontrivial();
    }
    if (Bu == futureFromRedBu)
    {
        futureFromRed = new_B_a_B_slice;
    }

    return new_B_a_B_slice;
}


/// \brief handle one transition after a block has been split
/// \details The main task of this method is to move the transition to the
/// correct place in the B_a_B slice.  In addition, it also checks whether the
/// transition has become non-inert and if necessary adapts pred and succ, and
/// it makes the source state a new bottom state.
///
/// Transitions from OldB to OldB are a bit problematic:  They fall apart into
/// four slices.  (The only case that cannot happen is that there is a tau
/// transition from the blue block to the red block, but visible transitions
/// may still happen.)  We put the slice NewB ==> NewB after OldB ==> OldB and
/// the other two slices before in the following way:  The caller should ensure
/// that first, all outgoing transitions of NewB are handled (including
/// transitions from NewB to both OldB and NewB), and then all incoming
/// transitions of NewB are handled (except the transitions within NewB).  This
/// way, the transitions NewB ==> OldB will all move to one slice and the
/// transitions OldB ==> NewB will all move to another slice.
///
/// It also maintains futureFromRed correct.  If the new block is the red
/// block and the transition starts in the red block, `futureFromRedBu` should
/// be set to the bunch of FromRed;  otherwise, `futureFromRedBu` should be
/// `nullptr`.
ONLY_IF_DEBUG( template<class LTS_TYPE> )
void part_trans_t::handle_transition(B_a_B_iter_t const old_pos,
                                      block_t* const OldB, block_t* const NewB,
                                        const bunch_t* const futureFromRedBu
                                        ONLY_IF_DEBUG( , const LTS_TYPE& aut) )
{
mCRL2log(log::debug,"bisim_tb") << "Handling " << old_pos->pred->debug_id(aut)
                                                                       << '\n';
    // move the transition to the new slice in B_a_B
    state_info_ptr const source = old_pos->pred->source;
    state_info_ptr const target = old_pos->pred->succ->target;
    assert(NewB == source->block ||
                        (nullptr == futureFromRedBu && NewB == target->block));
    B_a_B_desc_iter_t const old_B_a_B_slice = old_pos->B_a_B_slice;
    if (NewB == source->block && NewB == target->block)
    {
        // Both the source and the target have moved to the new block.  We move
        // the transition to a new slice near the end of the old slice.
        // Transitions that were inert and remain inert are handled here.  If
        // such a transition is moved, the inert_slice() of the new block is
        // filled!
        B_a_B_iter_t const after_new_pos = old_B_a_B_slice->end;
        B_a_B_desc_iter_t new_B_a_B_slice;
        if (B_a_B.end() == after_new_pos ||
                       (new_B_a_B_slice = after_new_pos->B_a_B_slice,
                        new_B_a_B_slice->from_block() != NewB ||
                        new_B_a_B_slice->to_block() != NewB ||
                        new_B_a_B_slice->label != old_B_a_B_slice->label))
        {
            // there is not yet a slice for these transitions
            if (1 == old_B_a_B_slice->end - old_B_a_B_slice->begin)
            {
                // the slice contains only one transition.  Later we will
                // implement the following optimisation:  No need to create a
                // new slice and then immediately delete the old one.
            }
            new_B_a_B_slice = create_new_slice_for_NewB(old_B_a_B_slice, NewB,
                                               after_new_pos, futureFromRedBu);

            if (OldB->inert_slice() == old_B_a_B_slice)
            {
                // The transition was inert (and remains inert).
                assert(NewB->inert_slice() == NewB->from_block.end());
                NewB->set_inert_slice(new_B_a_B_slice);
            }
        }
        assert(new_B_a_B_slice->bunch == old_B_a_B_slice->bunch);
        assert(OldB->inert_slice() != old_B_a_B_slice ||
                                       NewB->inert_slice() == new_B_a_B_slice);
        assert(old_B_a_B_slice->begin==old_B_a_B_slice->begin_from_non_bottom);
        old_pos->B_a_B_slice = new_B_a_B_slice;
        swap_B_a_B(old_pos, std::prev(after_new_pos));
        --old_B_a_B_slice->end;
        --new_B_a_B_slice->begin;
        assert(old_B_a_B_slice->end == new_B_a_B_slice->begin);
        --new_B_a_B_slice->begin_from_non_bottom;
        assert(new_B_a_B_slice->begin==new_B_a_B_slice->begin_from_non_bottom);
    }
    else
    {
mCRL2log(log::debug, "bisim_tb") << "2\n";
        // One of source and target are in NewB, the other is not.
        if (OldB->inert_slice() == old_B_a_B_slice)
        {
            // The transition was inert but is no longer so.
            // It now goes from NewB to OldB or from OldB to NewB.
            assert(old_B_a_B_slice->begin ==
                                       old_B_a_B_slice->begin_from_non_bottom);
            assert(source->block == OldB || target->block == OldB);
            assert(source->pos < source->block->nonbottom_end());
            // Make it non-inert.
            make_noninert_pred_succ(old_pos);
mCRL2log(log::debug, "bisim_tb") << "3\n";

            if (source->inert_succ_begin() == source->inert_succ_end())
            {
mCRL2log(log::debug, "bisim_tb") << "4\n";
                // the source state is now a new bottom state.
                source->block->set_marked_nonbottom_begin(
                                    source->block->marked_nonbottom_end() - 1);
                source->block->set_bottom_begin(
                                      source->block->marked_nonbottom_begin());
                swap_permutation(source->pos, source->block->bottom_begin());
                assert(source->pos < source->block->unmarked_bottom_end());
            }
        }
mCRL2log(log::debug, "bisim_tb") << "5\n";

        B_a_B_iter_t const new_pos = old_B_a_B_slice->begin;
        B_a_B_desc_iter_t new_B_a_B_slice;
if (B_a_B.begin() != new_pos)  {  mCRL2log(log::debug, "bisim_tb")
<< "new_B_a_B_slice == " << new_pos[-1].B_a_B_slice->debug_id_short() << '\n';  }
        if (B_a_B.begin() == new_pos ||
                (new_B_a_B_slice = new_pos[-1].B_a_B_slice,
                 new_B_a_B_slice->from_block() != source->block ||
                 new_B_a_B_slice->to_block() != target->block ||
                 new_B_a_B_slice->bunch != old_B_a_B_slice->bunch ||
                 new_B_a_B_slice->label != old_B_a_B_slice->label))
        {
mCRL2log(log::debug, "bisim_tb") << "6\n";
            // there is not yet a slice for these transitions
            if (1 == old_B_a_B_slice->end - old_B_a_B_slice->begin)
            {
                // the slice contains only one transition.  Later we will
                // implement the following optimisation:  No need to create a
                // new slice and then immediately delete the old one.
            }
            if (NewB == source->block)
            {
mCRL2log(log::debug, "bisim_tb") << "6a\n";
                assert(NewB != target->block);
                new_B_a_B_slice = create_new_slice_for_NewB(old_B_a_B_slice,
                                               NewB, new_pos, futureFromRedBu);
            }
            else
            {
mCRL2log(log::debug, "bisim_tb") << "6b\n";
                assert(NewB == target->block);
                // futureFromRedBu = nullptr;
                new_B_a_B_slice = source->block->from_block.emplace(
                               old_B_a_B_slice, new_pos, new_pos,
                               old_B_a_B_slice->label, old_B_a_B_slice->bunch);
                if (old_B_a_B_slice->bunch->is_trivial())
                {
                    old_B_a_B_slice->bunch->make_nontrivial();
                }
            }
        }
mCRL2log(log::debug, "bisim_tb") << "7\n";
        old_pos->B_a_B_slice = new_B_a_B_slice;
        if (new_pos == old_B_a_B_slice->begin_from_non_bottom)
        {
            // the slice does not contain transitions from (new) bottom
            // states
            swap_B_a_B(old_pos, new_pos);
            ++old_B_a_B_slice->begin_from_non_bottom;
        }
        else if (old_pos < old_B_a_B_slice->begin_from_non_bottom)
        {
            // the transition starts in a bottom state. move to
            // transitions from bottom states in the new slice.
            swap3_B_a_B(old_pos, new_B_a_B_slice->begin_from_non_bottom,
                                                                      new_pos);
            ++new_B_a_B_slice->begin_from_non_bottom;
        }
        else
        {
            // the transition starts in a non-bottom state. Move over the
            // transitions from bottom states.
            swap3_B_a_B(old_pos, new_pos,
                                       old_B_a_B_slice->begin_from_non_bottom);
            ++old_B_a_B_slice->begin_from_non_bottom;
        }
        ++old_B_a_B_slice->begin;
        ++new_B_a_B_slice->end;
        assert(old_B_a_B_slice->begin == new_B_a_B_slice->end);
    }

    if (old_B_a_B_slice->begin == old_B_a_B_slice->end)
    {
mCRL2log(log::debug, "bisim_tb") << "8\n";
        // the old slice has become empty, delete it. But first make
        // sure other pointers to it are changed.
        if (OldB->inert_slice() == old_B_a_B_slice)
        {
            OldB->set_inert_slice(OldB->from_block.end());
        }
        if (futureFromRed == old_B_a_B_slice)
        {
            assert(nullptr == futureFromRedBu);
            // `futureFromRed` is set to an adjacent slice in the same
            // bunch.
            futureFromRed = std::next(old_B_a_B_slice);
            if (OldB->from_block.end() == futureFromRed ||
                        futureFromRed->bunch != old_B_a_B_slice->bunch)
            {
                if (OldB->from_block.begin() == old_B_a_B_slice ||
                              (futureFromRed = std::prev(old_B_a_B_slice),
                               futureFromRed->bunch != old_B_a_B_slice->bunch))
                {
                    futureFromRed = OldB->from_block.end();
                }
            }
        }
        assert(OldB->inert_slice() != old_B_a_B_slice);
        assert(futureFromRed != old_B_a_B_slice);
        if (old_B_a_B_slice->bunch->begin->slice ==
                                         old_B_a_B_slice->bunch->end[-1].slice)
        {
            // The bunch has become trivial, so we can remove it from the list
            // of nontrivial bunches.  In practice, this is not always
            // possible, so it may be necessary to skip it later.
            if (bunch_t::get_some_nontrivial() == old_B_a_B_slice->bunch)
            {
                old_B_a_B_slice->bunch->make_trivial();
            }
        }
        OldB->from_block.erase(old_B_a_B_slice);
    }
}


/// \brief handle B_a_B slices after a new block has been created
/// \details part_trans_t::new_block_created splits the B_a_B-slices to
/// reflect that some transitions now start or end in the new block NewB.  They
/// can no longer be in the same slice as the transitions that start or end in
/// the old block, respectively.  It also marks the transitions that have
/// become non-inert as such and finds new bottom states.
///
/// The function also ensures the properties:
/// - part_trans_t::futureFromRed needs to point to a sensible slice, namely
///   a slice containing transitions from RedB in the correct bunch.
/// - During postprocessing, transitions starting in (new) bottom states need
///   to be separated from those starting in non-bottom states.
/// Its time complexity is O(1 + |in(NewB)| + |out(NewB)|).
ONLY_IF_DEBUG( template<class LTS_TYPE> )
void part_trans_t::new_block_created(block_t* const OldB, block_t* const NewB,
                    bool NewB_is_blue  ONLY_IF_DEBUG( , const LTS_TYPE& aut ) )
{
    const bunch_t* const futureFromRedBu =
               NewB_is_blue || NewB->from_block.end() == futureFromRed
                                              ? nullptr : futureFromRed->bunch;
    if (!NewB_is_blue)  futureFromRed = NewB->from_block.end();

    // for all outgoing transitions of NewB
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        state_info_ptr const s = *s_iter;
        for (succ_iter_t succ_iter = s->succ_begin();
                                       s->succ_end() != succ_iter; ++succ_iter)
        {
            handle_transition(succ_iter->B_a_B, OldB, NewB, futureFromRedBu
                        // perhaps here futureFromRedBu needs to be replaced
                        // by nullptr sometimes...
                                                      ONLY_IF_DEBUG( , aut ) );
    // end for
        }
    }

    // We cannot join the loop above with the one below because transitions
    // from OldB ==> NewB and NewB ==> OldB may be interspersed but need to go
    // to different new slices.

    // for all incoming transitions of NewB (except transitions from NewB to
    // NewB)
    for(permutation_iter_t s_iter=NewB->begin(); NewB->end()!=s_iter; ++s_iter)
    {
        state_info_ptr const s = *s_iter;
        for (pred_iter_t pred_iter = s->pred_begin();
                              s->noninert_pred_end() != pred_iter; ++pred_iter)
        {
            if (NewB != pred_iter->source->block) {
                handle_transition(pred_iter->succ->B_a_B, OldB, NewB,
                                             nullptr  ONLY_IF_DEBUG( , aut ) );
            }
    // end for
        }
    }
}

#ifndef NDEBUG

/// \brief print all transitions
/// \details Transitions are printed organised into bunches.
template <class LTS_TYPE>
void part_trans_t::print_trans(const LTS_TYPE& aut) const
{
    if (B_a_B.begin() == B_a_B.end())
    {
        mCRL2log(log::debug, "bisim_tb") << "No transitions.\n";
        return;
    }
    // for all bunches
    const bunch_t* Bu = B_a_B.begin()->B_a_B_slice->bunch;
    for (;;)
    {
        mCRL2log(log::debug, "bisim_tb") << Bu->debug_id_short() << ":\n";
        assert(Bu->begin < Bu->end);
        // for all transition sets in Bu
        B_a_B_desc_const_iter_t Tr = Bu->begin->B_a_B_slice;
        for (;;)
        {
            mCRL2log(log::debug,"bisim_tb")<<'\t'<<Tr->debug_id_short()<<":\n";
            // for all transitions in Tr
            for (B_a_B_const_iter_t tr_iter = Tr->begin; Tr->end != tr_iter;
                                                                     ++tr_iter)
            {
                mCRL2log(log::debug, "bisim_tb") << "\t\t"
                                       << tr_iter->pred->debug_id(aut) << '\n';
            }
            // go to next transition set
            if (Bu->end == Tr->end)  break;
            Tr = Tr->end->B_a_B_slice;
        }
        // go to next bunch
        if (B_a_B.end() == Bu->end)  break;
        Bu = Bu->end->B_a_B_slice->bunch;
    }
    return;
}


// \brief assert that the data structure is consistent and stable
/// \details The data structure is tested against a large number of assertions
/// to ensure that everything is consistent, e. g. pointers that should point
/// to successors of state s actually point to a transition that starts in s.
///
/// Additionally, it is asserted that the partition is stable. i. e. every
/// bottom state in every block can reach exactly the constellations in the
/// list of constellations that should be reachable from it, and every
/// nonbottom state can reach a subset of them.
template <class LTS_TYPE>
void part_trans_t::assert_stability(const bool branching,
                         const bool preserve_divergence,
                         const part_state_t&part_st, const LTS_TYPE& aut) const
{
    mCRL2log(log::debug, "bisim_tb") << "Checking stability of partition...\n";

    // for all blocks do
    const block_t* B = (*part_st.permutation.begin())->block;
    assert(B->begin() == part_st.permutation.begin());
    for (;;)
    {
        // some assertions on the block
        assert(B->begin() <= B->marked_nonbottom_begin());
        assert(B->marked_nonbottom_begin() <= B->bottom_begin());
        assert(B->bottom_begin() <= B->marked_bottom_begin());
        assert(B->marked_bottom_begin() <= B->end());
        assert(B->bottom_begin() < B->bottom_end());
        assert(B->seqnr < block_t::nr_of_blocks);

        // for all states in the block
        for (permutation_const_iter_t s_iter = B->begin(); B->end() != s_iter;
                                                                      ++s_iter)
        {
            state_info_ptr s = *s_iter;
            // check some conditions of the state
            assert(s->pos == s_iter);
            assert(s->block == B);
            assert(s->pred_begin() <= s->inert_pred_begin());
            assert(s->inert_pred_begin() <= s->inert_pred_end());
            assert(s->inert_pred_end() == s->pred_end());
            assert(s->succ_begin() <= s->inert_succ_begin());
            assert(s->inert_succ_begin() <= s->inert_succ_end());
            assert(s->inert_succ_end() <= s->succ_end());
        }
        // for all bunches in the list of transitions do
        //     for all transition sets in the list of transitions do
        B_a_B_desc_const_iter_t tr_iter = B->from_block.begin();
        if (B->from_block.end() != tr_iter)
        {
            bool contains_noninert_transition = false;
            std::vector<bool> bottom_states_with_transition =
                                    std::vector<bool>(B->bottom_size(), false);
            std::set<const bunch_t*> bunches_in_tr_list;
            for (;;)
            {
                if (tr_iter->from_block()->inert_slice() == tr_iter)
                {
                    assert(branching);
                    assert(aut.is_tau(aut.apply_hidden_label_map(
                                                             tr_iter->label)));
                    for (B_a_B_iter_t t = tr_iter->begin; t!=tr_iter->end; ++t)
                    {
                        state_info_const_ptr const source = t->pred->source;
                        state_info_const_ptr const target =
                                                         t->pred->succ->target;
                        // assert some properties of this inert transition
                        assert(source->block == target->block);
                        assert(source != target);
                        assert(target->inert_pred_begin() <= t->pred);
                        assert(t->pred < target->inert_pred_end());
                        assert(source->inert_succ_begin() <= t->pred->succ);
                        assert(t->pred->succ < source->inert_succ_end());
                        assert(source->block->bottom_begin() > source->pos);
                    }
                }
                else
                {
                    contains_noninert_transition = true;
                    for (B_a_B_iter_t t = tr_iter->begin; t!=tr_iter->end; ++t)
                    {
                        state_info_const_ptr const source = t->pred->source;
                        state_info_const_ptr const target =
                                                         t->pred->succ->target;
                        contains_noninert_transition = true;
                        assert(!branching ||
                                (preserve_divergence && source == target) ||
                                source->block != target->block ||
                                !aut.is_tau(aut.apply_hidden_label_map(
                                                             tr_iter->label)));
                        assert(target->noninert_pred_begin() <= t->pred);
                        assert(t->pred < target->noninert_pred_end());
                        assert(source->noninert_succ_begin() <= t->pred->succ);
                        assert(t->pred->succ < source->noninert_succ_end());
                        if (source->block->bottom_begin() <= source->pos)
                        {
                            // It is a bottom state
                            // check that every bottom state has a transition in
                            // the bunch
                            bottom_states_with_transition[source->pos -
                                         source->block->bottom_begin()] = true;
                        }
                    }
                }

            // go to next transition set
                const bunch_t* const oldBu = tr_iter->bunch;
                ++tr_iter;
                if (B->from_block.end() == tr_iter || oldBu != tr_iter->bunch)
                {
        // go to next bunch:

                    // Now every bottom state needs to be in vector<bool>
                    if (contains_noninert_transition)
                    {
                        assert(std::find(bottom_states_with_transition.begin(),
                                 bottom_states_with_transition.end(), false) ==
                                          bottom_states_with_transition.end());
                    }
                    // check that the old bunch hasn't yet appeared in
                    // from_block, and insert it in the set.
                    assert(bunches_in_tr_list.insert(oldBu).second);
                    if (B->from_block.end() == tr_iter)  break;

                    // then reset all transitions to false
                    bottom_states_with_transition.assign(B->bottom_size(),
                                                                        false);
                }
            }
        }
        // go to next block
        if (B->end() == part_st.permutation.end())  break;
        assert(B->end() < part_st.permutation.end());
        assert(B->end() == (*B->end())->block->begin());
        B = (*B->end())->block;
    }

    if (B_a_B.begin() == B_a_B.end())
    {
        assert(nullptr == bunch_t::get_some_nontrivial());
        return;
    }

    // count the nontrivial bunches (to check later whether every nontrivial
    // bunch is reachable from the first nontrivial bunch)
    state_type nr_of_nontrivial_bunches = 0;
    const bunch_t* Bu = bunch_t::get_some_nontrivial();
    if (nullptr != Bu)
    {
        for (;;)
        {
            ++nr_of_nontrivial_bunches;
            if (Bu->get_next_nontrivial() == Bu)  break;
            Bu = Bu->get_next_nontrivial();
            assert(nullptr != Bu);
        }
    }

    // for all bunches Bu do
    Bu = B_a_B.begin()->B_a_B_slice->bunch;
    assert(Bu->begin == B_a_B.begin());
    for (;;)
    {
        // assert some properties of the bunch Bu
        assert(Bu->begin < Bu->end);
        // for all transition sets Tr in Bu do
        B_a_B_desc_const_iter_t Tr = Bu->begin->B_a_B_slice;
        assert(Tr->begin == Bu->begin);
        if (Bu->is_trivial())
        {
            // a trivial bunch only contains one transition set
            assert(Tr->end == Bu->end);
        }
        else
        {
            // a nontrivial bunch contains at least two transition sets
            // however, it may happen that some bunches are marked as
            // nontrivial but have become trivial in the meantime.
            assert(Tr->end <= Bu->end);
            --nr_of_nontrivial_bunches;
        }
        for (;;)
        {
            // assert some properties of the transition set Tr
            assert(Tr->begin < Tr->end);
            assert(Tr->begin_from_non_bottom == Tr->begin);
            assert(Tr->bunch == Bu);

            B_a_B_const_iter_t t = Tr->begin;
            const block_t* const start_block = t->pred->source->block;
            const block_t* const target_block = t->pred->succ->target->block;
            for (;;)
            {
                // assert some properties of the transition t
                assert(t == t->pred->succ->B_a_B);
                assert(t->B_a_B_slice == Tr);
                assert(t->pred->source->succ_begin() <= t->pred->succ);
                assert(t->pred->succ < t->pred->source->succ_end());
                assert(t->pred->succ->target->pred_begin() <= t->pred);
                assert(t->pred < t->pred->succ->target->pred_end());

                ++t;
                if (Tr->end == t)  break;
                // Do all transitions start and end in the correct block?
                assert(start_block == t->pred->source->block);
                assert(target_block == t->pred->succ->target->block);
            }

            // go to next transition set
            if (Tr->end == Bu->end)  break;
            assert(Tr->end < Bu->end);
            assert(Tr->end == Tr->end->B_a_B_slice->begin);
            Tr = Tr->end->B_a_B_slice;
        }
        // go to next bunch
        if (B_a_B.end() == Bu->end)  break;
        assert(Bu->end < B_a_B.end());
        assert(Bu->end == Bu->end->B_a_B_slice->bunch->begin);
        Bu = Bu->end->B_a_B_slice->bunch;
    }
    assert(0 == nr_of_nontrivial_bunches);
    return;
}

#endif

} // end namespace bisim_tb





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
/// has a non-inert transition in a bunch Bu, then every bottom state in the
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
void bisim_partitioner_tb<LTS_TYPE>::
          create_initial_partition_tb(bool branching, bool preserve_divergence)
{
    log::mcrl2_logger::set_reporting_level(log::debug, "bisim_tb");

    mCRL2log(log::verbose, "bisim_tb") << "Strictly O(m log n) "
                   << (branching ? (preserve_divergence
                                           ? "divergence-preserving branching "
                                           : "branching ")
                                 : "")
                 << "bisimulation partitioner created for " << aut.num_states()
                 << " states and " << aut.num_transitions()
                 << " transitions\n";

    // create a single block

    bisim_tb::block_t* B = new bisim_tb::block_t(part_st.permutation.begin(),
                                                    part_st.permutation.end());

    // Iterate over the transitions to count how to order them in part_trans_t

    // counters for the non-inert outgoing and incoming transitions per state
    // are provided in part_st.state_info.  These counters have been
    // initialised to zero in the constructor of part_state_t.

    // counters for the non-inert transition per label:
    std::vector<trans_type> trans_per_label =
              std::vector<trans_type>(aut.num_action_labels(), (trans_type) 0);

    // counter for the total number of inert transitions:
    trans_type inert_transitions = 0;

    for (const transition& t: aut.get_transitions())
    {
        part_st.state_info[t.from()].state_out.count++;
        part_st.state_info[t.to()].state_in.count++;
        if (branching && aut.is_tau(aut.apply_hidden_label_map(t.label()))
               && (t.from() != t.to() || (assert(preserve_divergence), false)))
        {
            // The transition is inert.
            ++inert_transitions;
            // The source state should become non-bottom:
            if (B->marked_bottom_begin() <= part_st.state_info[t.from()].pos)
            {
                // marked bottom state becomes a marked non-bottom state.
                swap3_permutation(part_st.state_info[t.from()].pos,
                                  B->bottom_begin(), B->marked_bottom_begin());
                B->set_marked_bottom_begin(B->marked_bottom_begin() + 1);
                B->set_bottom_begin(B->bottom_begin() + 1);
            }
            else if (B->bottom_begin() <= part_st.state_info[t.from()].pos)
            {
                // unmarked bottom state becomes an unmarked non-bottom state.
                swap3_permutation(part_st.state_info[t.from()].pos,
                               B->marked_nonbottom_begin(), B->bottom_begin());
                B->set_bottom_begin(B->bottom_begin() + 1);
                B->set_marked_nonbottom_begin(B->marked_nonbottom_begin() + 1);
            }
        }
        else
        {
            // The transition is non-inert.  (It may be a self-loop).

            assert(0 <= aut.apply_hidden_label_map(t.label()));
            assert(aut.apply_hidden_label_map(t.label()) <
                                                       trans_per_label.size());
            ++trans_per_label[aut.apply_hidden_label_map(t.label())];

            // Mark state t.from() as a state with a non-inert transition
            B->mark(&part_st.state_info[t.from()]);
        }
    }

    // set the pointers to transition slices in the state info entries

    bisim_gjkw::fixed_vector<bisim_tb::state_info_entry>::iterator state_iter =
                                                    part_st.state_info.begin();
    if (std::prev(part_st.state_info.end()) != state_iter)
    {
        state_iter->set_pred_begin(part_tr.pred.begin());
        state_iter->set_succ_begin(part_tr.succ.begin());
        do
        {
            state_iter->set_inert_pred_begin(state_iter->pred_begin() +
                                                   state_iter->state_in.count);
            state_iter->set_pred_end(state_iter->inert_pred_begin());
            state_iter->set_inert_succ_begin(state_iter->succ_begin() +
                                                  state_iter->state_out.count);
            state_iter->set_succ_end(state_iter->inert_succ_begin());
            state_iter->current_bunch = state_iter->inert_succ_begin();
            state_iter->block = B;
            // state_iter->notblue = 0;

            // create slice descriptors in part_tr.succ for each state with
            // outgoing transitions.
            if (state_iter->succ_begin() != state_iter->succ_end())
            {
                bisim_tb::out_descriptor* new_slice =
                        new bisim_tb::out_descriptor(state_iter->succ_begin());
                new_slice->end = state_iter->succ_end();
                for (bisim_tb::succ_iter_t succ_iter = new_slice->begin;
                                      new_slice->end != succ_iter; ++succ_iter)
                {
                    succ_iter->bunch_slice = new_slice;
                }
            }

            ++state_iter;
        }
        while (std::prev(part_st.state_info.end()) != state_iter);
    }

    // create a single bunch containing all transitions

    bisim_tb::bunch_t* bunch =
             new bisim_tb::bunch_t(part_tr.B_a_B.begin(), part_tr.B_a_B.end());

    // create slice descriptors in part_tr.B_a_B for each label

    trans_type prev = 0;

    for (label_type i = 0; i < trans_per_label.size(); ++i)
    {
        if (0 == trans_per_label[i])  continue;
        if (0 != prev)
        {
            bunch->make_nontrivial_during_init();
            trans_per_label[i] += prev;
        }
        // create a B_a_B slice for the transitions with label lbl_iter->first
        B->from_block.emplace_front(part_tr.B_a_B.begin() + prev,
                         part_tr.B_a_B.begin() + trans_per_label[i], i, bunch);
        bisim_tb::B_a_B_desc_iter_t const new_slice = B->from_block.begin();

        // set the B_a_B_slice pointer of all transitions in this slice
        for (bisim_tb::B_a_B_iter_t B_a_B_iter = new_slice->begin;
                                    new_slice->end != B_a_B_iter; ++B_a_B_iter)
        {
            B_a_B_iter->B_a_B_slice = new_slice;
        }
        prev = trans_per_label[i];
    }

    // create slice of inert transitions:
    if (0 != inert_transitions)
    {
        if (0 != prev)
        {
            bunch->make_nontrivial_during_init();
            inert_transitions += prev;
        }
        B->from_block.emplace_front(part_tr.B_a_B.begin() + prev,
                                    part_tr.B_a_B.begin() + inert_transitions,
                                                 aut.tau_label_index(), bunch);
        bisim_tb::B_a_B_desc_iter_t const new_slice = B->from_block.begin();

        B->set_inert_slice(new_slice);
        // set the B_a_B_slice pointer of all transitions in this slice
        for (bisim_tb::B_a_B_iter_t B_a_B_iter = new_slice->begin;
                                    new_slice->end != B_a_B_iter; ++B_a_B_iter)
        {
            B_a_B_iter->B_a_B_slice = new_slice;
        }
        assert(aut.num_transitions() == inert_transitions);
    }
    else
    {
        B->set_inert_slice(B->from_block.end());
        assert(aut.num_transitions() == prev);
    }

    // distribute the transitions over the data structures

    // The following could perhaps be replaced by reusing data fields in the
    // state_info_entry data structure, to save further memory.  For example,
    // one could use the fields ``notblue'' and ``current_bunch''.
    bisim_gjkw::fixed_vector<trans_type> used_in_per_state(aut.num_states(),
                                                               (trans_type) 0);
    bisim_gjkw::fixed_vector<trans_type> used_out_per_state(aut.num_states(),
                                                               (trans_type) 0);

    for (const transition& t: aut.get_transitions())
    {
        assert(part_st.state_info[t.from()].block == B);

        bisim_tb::B_a_B_iter_t B_a_B_pos;
        bisim_tb::pred_iter_t pred_pos;
        bisim_tb::succ_iter_t succ_pos;

        if (branching && aut.is_tau(aut.apply_hidden_label_map(t.label()))
               && (t.from() != t.to() || (assert(preserve_divergence), false)))
        {
            // It is a (normal) inert transition: place near the end of the
            // respective pred/succ slices, just before the other inert
            // transitions.
            assert(part_st.state_info[t.from()].pos < B->nonbottom_end());

            part_st.state_info[t.to()].set_inert_pred_begin(
                            part_st.state_info[t.to()].inert_pred_begin() - 1);
            pred_pos = part_st.state_info[t.to()].inert_pred_begin();
            part_st.state_info[t.from()].set_inert_succ_begin(
                          part_st.state_info[t.from()].inert_succ_begin() - 1);
            succ_pos = part_st.state_info[t.from()].inert_succ_begin();
            B_a_B_pos = part_tr.B_a_B.begin() + --inert_transitions;

            assert(B->inert_slice()->begin <= B_a_B_pos);
            assert(B_a_B_pos < B->inert_slice()->end);
        }
        else
        {
            // It is a non-inert transition (possibly a self-loop): place at
            // the beginning of the respective pred/succ slices
            pred_pos = part_st.state_info[t.to()].pred_begin() +
                                                   used_in_per_state[t.to()]++;
            succ_pos = part_st.state_info[t.from()].succ_begin() +
                                                used_out_per_state[t.from()]++;
            B_a_B_pos = part_tr.B_a_B.begin() +
                      --trans_per_label[aut.apply_hidden_label_map(t.label())];

            assert(pred_pos < part_st.state_info[t.to()].inert_pred_begin());
            assert(succ_pos < part_st.state_info[t.from()].inert_succ_begin());
            assert(B->inert_slice() == B->from_block.end() ||
                                          B_a_B_pos < B->inert_slice()->begin);
        }
        assert(aut.apply_hidden_label_map(t.label()) ==
                                                B_a_B_pos->B_a_B_slice->label);
        assert(part_st.state_info[t.to()].pred_begin() <= pred_pos);
        assert(pred_pos < part_st.state_info[t.to()].pred_end());
        assert(part_st.state_info[t.from()].succ_begin() <= succ_pos);
        assert(succ_pos < part_st.state_info[t.from()].succ_end());

        B_a_B_pos->pred = pred_pos;
        pred_pos->succ = succ_pos;
        pred_pos->source = &part_st.state_info[t.from()];
        succ_pos->B_a_B = B_a_B_pos;
        succ_pos->target = &part_st.state_info[t.to()];
    }
    trans_per_label.clear();
    used_in_per_state.clear();
    used_out_per_state.clear();

    // and also now split all blocks with marked states into the part with
    // non-inert transitions and the part without.
    if (B->marked_size() != 0)
    {
        B = refine(B, B->from_block.end());
        B->set_marked_bottom_begin(B->marked_bottom_end());
    }

    // delete transitions already -- they are no longer needed.  We will add
    // new transitions at the end.
    aut.clear_transitions();
}


// replace_transition_system() replaces the transitions of the LTS stored
// here by those of its bisimulation quotient.  However, it does not change
// anything else; in particular, it does not change the number of states of
// the LTS.
template <class LTS_TYPE>
void bisim_partitioner_tb<LTS_TYPE>::replace_transition_system(bool branching,
                                                      bool preserve_divergence)
{
    for (bisim_tb::permutation_const_iter_t s_iter=part_st.permutation.begin();
                                         part_st.permutation.end() != s_iter; )
    {
        const bisim_tb::block_t* B = (*s_iter)->block;
        for(bisim_tb::B_a_B_desc_const_iter_t trans_iter=B->from_block.begin();
                               B->from_block.end() != trans_iter; ++trans_iter)
        {
            if (B->inert_slice() == trans_iter)  continue;
            // add a transition from the source block to the goal block with
            // the indicated label.
            aut.add_transition(transition(B->seqnr, trans_iter->label,
                                               trans_iter->to_block()->seqnr));
        }
    // go to next block
        s_iter = B->end();
    }

    // Merge the states, by setting the state labels of each state to the concatenation of the state labels of its
    // equivalence class.

    if (aut.has_state_info())   /* If there are no state labels this step can be ignored */
    {
      /* Create a vector for the new labels */
      bisim_gjkw::fixed_vector<typename LTS_TYPE::state_label_t> new_labels(
                                              bisim_tb::block_t::nr_of_blocks);

      for(std::size_t i=aut.num_states(); i>0; )
      {
        --i;
        const std::size_t new_index=part_st.block(i)->seqnr;  /* get_eq_class(i) */
        new_labels[new_index]=aut.state_label(i)+new_labels[new_index];
      }

      aut.set_num_states(bisim_tb::block_t::nr_of_blocks);
      for(std::size_t i=0; i < bisim_tb::block_t::nr_of_blocks; ++i)
      {
        aut.set_state_label(i,new_labels[i]);
      }
    }
    else
    {
      aut.set_num_states(bisim_tb::block_t::nr_of_blocks);
    }

    aut.set_initial_state(part_st.block(aut.initial_state())->seqnr);
}



/*=============================================================================
=                               main algorithm                                =
=============================================================================*/



template <class LTS_TYPE>
void bisim_partitioner_tb<LTS_TYPE>::
              refine_partition_until_it_becomes_stable_tb(const bool branching,
                                                const bool preserve_divergence)
{
    // while there is a nontrivial bunch SpBu
    while (nullptr != bisim_tb::bunch_t::get_some_nontrivial())
    {
        #ifndef NDEBUG
            if (mCRL2logEnabled(log::debug, "bisim_tb"))
            {
                part_st.print_part();
                part_tr.print_trans(aut);
            }

            part_tr.assert_stability(branching, preserve_divergence, part_st,
                                                                          aut);
        #endif

        /*------------------------- find a splitter -------------------------*/

        bisim_tb::bunch_t* const SpBu=bisim_tb::bunch_t::get_some_nontrivial();
        mCRL2log(log::debug, "bisim_tb") <<"Refining "<<SpBu->debug_id()<<'\n';
        if (SpBu->begin->B_a_B_slice == SpBu->end[-1].B_a_B_slice)
        {
            mCRL2log(log::debug, "bisim_tb") << "Actually the bunch has "
                                                   "become trivial already.\n";
            SpBu->make_trivial();
            continue;
        }
        // select a small B_a_B-slice SpSl in SpBu
        bisim_tb::B_a_B_desc_iter_t const SpSl = SpBu->split_off_small_B_a_B();
        mCRL2log(log::debug, "bisim_tb") << "Splitting off "
                                                   << SpSl->debug_id() << '\n';
        bisim_tb::block_t* const B = SpSl->from_block();
        // select a B_a_B-slice that remains in SpBu as futureFromRed
        if (SpSl == B->from_block.begin() ||
                                      (part_tr.futureFromRed = std::prev(SpSl),
                                       part_tr.futureFromRed->bunch != SpBu))
        {
            // the slice just before SpSl is not in SpBu.  Now try the slice
            // just after SpSl.
            part_tr.futureFromRed = std::next(SpSl);
            if (B->from_block.end() != part_tr.futureFromRed &&
                                          part_tr.futureFromRed->bunch != SpBu)
            {
                // the slice just after is not in SpBu either.  (This can
                // happen if SpBu contains transitions from other blocks.)
                part_tr.futureFromRed = B->from_block.end();
                // // Possible simplification:  In this case, both splits
                // // should be trivial.
                // continue;
            }
        }
        else if (std::next(SpSl) != B->from_block.end() &&
                                                std::next(SpSl)->bunch == SpBu)
        {
            // SpSl is somewhere in the middle of the list of B_a_B slices that
            // emanate from B, and both its predecessor and successor in this
            // list belong to that same bunch. Therefore we move SpSl to the
            // beginning of the list so this list satisfies the constraint
            // ``B_a_B slices in the same bunch immediately follow each other''
            // again.
            B->from_block.splice(B->from_block.begin(), B->from_block, SpSl);
            assert(B->from_block.begin() == SpSl);
        }

        if (1 == B->size())  continue;

        /*----------- special treatment for the inert transitions -----------*/

        if (B->inert_slice() == SpSl)
        {
            // The split is trivial, but we have to adapt the data structures
            // for the outgoing transitions because there may be states that
            // have both inert and non-inert transitions that have been in
            // SpBu.

            // for all transitions s --> t in SpSl
            for(bisim_tb::B_a_B_iter_t s_iter = SpSl->begin;
                                                 SpSl->end != s_iter; ++s_iter)
            {
                bisim_tb::state_info_entry* s = s_iter->pred->source;
                assert(s->block == B);
                assert(s->inert_succ_begin() != s->inert_succ_end());
                if (s->inert_succ_begin()->bunch_slice->begin !=
                                                         s->inert_succ_begin())
                {
                    assert(s->succ_begin() != s->inert_succ_begin());
                    // There are other transitions emanating from s that were
                    // in the same bunch. So let's split the descriptor.
                    s->inert_succ_begin()->bunch_slice->end =
                                                         s->inert_succ_begin();
                    bisim_tb::out_descriptor* new_inert_slice =
                           new bisim_tb::out_descriptor(s->inert_succ_begin());
                    new_inert_slice->end = s->inert_succ_end();
                    for (bisim_tb::succ_iter_t s_iter = new_inert_slice->begin;
                                new_inert_slice->end != s_iter; ++s_iter)
                    {
                        s_iter->bunch_slice = new_inert_slice;
                    }
                }
            // end for
            }

            continue;
        }

        /*-------------------- find predecessors of SpSl --------------------*/

        // for all transitions s --> t in SpSl
        for(bisim_tb::B_a_B_iter_t s_iter = SpSl->begin;
                                                 SpSl->end != s_iter; ++s_iter)
        {
            bisim_tb::state_info_entry* s = s_iter->pred->source;
            assert(s->block == B);
            // mark state s
            bool const first_transition_of_state = B->mark(s);
            // register whether state s still has transitions to SpBu
            s->current_bunch = part_tr.move_to_new_bunch(s_iter, SpBu,
                                       SpSl->bunch, first_transition_of_state);
        // end for
        }

        /*------------------- stabilise the partition again -----------------*/

        // stabilise this block under SpSl (using the marked states)
        assert(0 != B->marked_size());
        bisim_tb::block_t* RedB = refine(B, B->from_block.end());
        // handle new bottom states resulting from the first refinement
        if (RedB->unmarked_bottom_begin() != RedB->unmarked_bottom_end())
        {
            RedB = postprocess_new_bottom(RedB);
            if (nullptr == RedB)  continue;
        }
        else
        {
            RedB->set_marked_bottom_begin(RedB->marked_bottom_end());
        }

        // stabilise this block under SpBu \ SpSl (using FromRed)
        assert(0 == RedB->marked_size());
        if (RedB->size() == 1)  continue;
        bisim_tb::B_a_B_desc_iter_t const FromRed = part_tr.futureFromRed;
        if (RedB->from_block.end() == FromRed)  continue;
        part_tr.futureFromRed = RedB->from_block.end();
        RedB = refine(RedB, FromRed, SpSl->bunch, false);
        // handle new bottom states resulting from the second refinement
        if (RedB->unmarked_bottom_begin() != RedB->unmarked_bottom_end())
        {
            RedB = postprocess_new_bottom(RedB);
        }
        else
        {
            RedB->set_marked_bottom_begin(RedB->marked_bottom_end());
        }
mCRL2log(log::debug, "bisim_tb") << "9\n";

        assert(nullptr == RedB || 0 == RedB->marked_size());
    // end while
    }
mCRL2log(log::debug, "bisim_tb") << "10\n";
    #ifndef NDEBUG
        if (mCRL2logEnabled(log::debug, "bisim_tb"))
        {
            part_st.print_part();
            part_tr.print_trans(aut);
        }

        part_tr.assert_stability(branching, preserve_divergence, part_st, aut);
    #endif
    // return P
        // (this happens implicitly, through the bisim_partitioner_tb object
        // data)
}



/*=============================================================================
=                                   Refine                                    =
=============================================================================*/



namespace bisim_tb
{

/// \brief structure containing information shared between the coroutines
struct refine_shared_t {
    permutation_iter_t notblue_initialised_end;
    bool FromRed_is_handled;
};

} // end namespace bisim_tb

/// \param RfnB           the block that is being refined
/// \param FromRed        if all states with a transition to the splitter are
///                       marked, this variable should be nullptr.  But if not
///                       all such states are marked (and the red coroutine
///                       needs to go through FromRed to find these states)
///                       this parameter is a B_a_B_desc_iter_t.
///                       containing some transition in the splitter bunch.
/// \param NewBu          If refine_blue finds a blue state before FromRed has
///                       been handled completely, a slow check may be avoided
///                       if the current constellation pointer is pointing to
///                       the right place.  However, to find out whether it's
///                       pointing to the right place, we need NewBu, the bunch
///                       adjacent to the bunch of FromRed.
/// \param unmarked_bottom_states_are_blue  if FromRed != nullptr, sometimes it
///                       still may be the case that all bottom states have
///                       already been marked.  (In that case, the blue
///                       coroutine does not need to find blue bottom states
///                       first.)
template <class LTS_TYPE>
bisim_tb::block_t* bisim_partitioner_tb<LTS_TYPE>::refine(
                           bisim_tb::block_t* const RfnB,
                           bisim_tb::B_a_B_desc_iter_t const FromRed,
                           const bisim_tb::bunch_t* const NewBu,
                           const bool unmarked_bottom_states_are_blue)
{
    mCRL2log(log::debug, "bisim_tb") << "refine(" << RfnB->debug_id() << ','
    << (RfnB->from_block.end()==FromRed ? std::string("null")
                                        : FromRed->debug_id())
    << ',' << NewBu->debug_id_short()
    << (unmarked_bottom_states_are_blue ? ",true)\n" : ",false)\n");

    bisim_tb::block_t* RedB;
    assert(RfnB->from_block.end()!=FromRed || unmarked_bottom_states_are_blue);

    RUN_COROUTINES(refine_blue, (RfnB)(FromRed)
                                    (NewBu)(unmarked_bottom_states_are_blue),
                                (RedB = RfnB),
                   refine_red,  (RfnB)(FromRed)
                                (unmarked_bottom_states_are_blue),
                                (RedB = (*RfnB->end())->block),
             /* shared data: */ struct bisim_tb::refine_shared_t,
                                (RfnB->nonbottom_begin())
                                (RfnB->from_block.end() == FromRed)
    );
    return RedB;
}


template <class LTS_TYPE>
DEFINE_COROUTINE(bisim_partitioner_tb<LTS_TYPE>::, refine_blue,
    /* formal parameters:*/ ((bisim_tb::block_t* const, RfnB))
                            ((bisim_tb::B_a_B_desc_const_iter_t const,FromRed))
                            ((const bisim_tb::bunch_t* const, NewBu))
                            ((const bool, unmarked_bottom_states_are_blue)),
    /* local variables:  */ ((bisim_tb::permutation_iter_t, visited_end))
                            ((bisim_tb::state_info_ptr, s))
                            ((bisim_tb::pred_iter_t, pred_iter))
                            ((bisim_tb::state_info_ptr, s_prime))
                            ((bisim_tb::permutation_iter_t,blue_nonbottom_end))
                            ((bisim_tb::succ_const_iter_t, succ_iter)),
    /* shared data:      */ struct bisim_tb::refine_shared_t, shared_data,
    /* interrupt locatns:*/ (REFINE_BLUE_PREDECESSOR_HANDLED)
                            (REFINE_BLUE_TESTING)
                            (REFINE_BLUE_STATE_HANDLED)
                            (REFINE_BLUE_COLLECT_BOTTOM))
{
mCRL2log(log::debug, "bisim_tb") << "refine_blue started\n";
    if (!unmarked_bottom_states_are_blue)
    {
        // we have to decide which unmarked bottom states are blue.  So we walk
        // over all of them and check whether they have a transition to the
        // bunch of FromRed or not.
        // 3.3: ..., Blue := {}
        visited_end = RfnB->unmarked_bottom_begin();
        // 3.5l: whenever |Blue| > |RfnB|/2 do  Abort this coroutine
            // nothing needs to be done now, as |Blue| = 0 here.

        /*-  -  -  -  -  -  - collect blue bottom states -  -  -  -  -  -  -*/

        // 3.6l: while Test is not empty and FromRed is not empty do
            // We use the variable visited_end in this loop to indicate the
            // boundary between blue states (namely those in the interval
            // [RfnB->unmarked_bottom_begin(), visited_end) ) and Test states
            // (namely those in [visited_end, RfnB->unmarked_bottom_end()) ).
        COROUTINE_WHILE (REFINE_BLUE_COLLECT_BOTTOM,
                    RfnB->unmarked_bottom_end() > visited_end &&
                                               !shared_data.FromRed_is_handled)
        {
            // 3.7l: Choose s in Test
            s = *visited_end;
mCRL2log(log::debug, "bisim_tb") << s->debug_id();
            // 3.8l: if s --> SpBu then
            if (s->surely_has_transition_to(FromRed->bunch))
            {
mCRL2log(log::debug, "bisim_tb") << " is not blue\n";
                // 3.9l: Move s from Test to Red
                // The state s is not blue.  Move it to the slice of non-blue
                // bottom states.
                RfnB->set_marked_bottom_begin(RfnB->marked_bottom_begin() - 1);
                bisim_tb::swap_permutation(visited_end,
                                                  RfnB->marked_bottom_begin());
                // 3.5r: whenever |Red| > |RfnB|/2 do  Abort the red coroutine
                if (RfnB->marked_size() > RfnB->size() / 2)
                {
                    ABORT_OTHER_COROUTINE();
                }
            // 3.10l: else
            }
            else
            {
mCRL2log(log::debug, "bisim_tb") << " is blue\n";
                assert(s->surely_has_no_transition_to(FromRed->bunch, NewBu));
                // 3.11l: Move s from Test to Blue
                ++visited_end;
                // 3.5l: whenever |Blue| > |RfnB|/2 do  Abort this coroutine
                if ((state_type) (visited_end-RfnB->unmarked_bottom_begin()) >
                                                              RfnB->size() / 2)
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
        // blue, i. e. the whole interval [RfnB->unmarked_bottom_begin(),
        // RfnB->unmarked_bottom_end()).
    // 3.5l: whenever |Blue| > |RfnB|/2 do  Abort this coroutine
    if (RfnB->unmarked_bottom_size() > RfnB->size() / 2)
    {
        ABORT_THIS_COROUTINE();
    }
    if (0 == RfnB->unmarked_bottom_size())
    {
        // all bottom states are red, so there cannot be any blue states.
        RfnB->set_marked_nonbottom_begin(RfnB->marked_nonbottom_end());
        // RfnB->set_marked_bottom_begin(RfnB->bottom_begin());
        TERMINATE_COROUTINE_SUCCESSFULLY();
    }

     /*  -  -  -  -  -  -  -  - visit blue states -  -  -  -  -  -  -  -  */

    // 3.15l: while Blue contains unvisited states do
    visited_end = RfnB->unmarked_bottom_begin();
    blue_nonbottom_end = RfnB->unmarked_nonbottom_begin();
    COROUTINE_DO_WHILE (REFINE_BLUE_STATE_HANDLED,
                                             visited_end != blue_nonbottom_end)
    {
        // 3.16l: Choose an unvisited s in Blue
        s = *visited_end;
        // 3.17l: Mark s as visited
        ++visited_end;
        // 3.18l: for all s_prime in inert_in(s) \ Red do
        COROUTINE_FOR (REFINE_BLUE_PREDECESSOR_HANDLED,
                            pred_iter = s->inert_pred_begin(),
                                 s->inert_pred_end() != pred_iter, ++pred_iter)
        {
            s_prime = pred_iter->source;
            if (s_prime->pos >= RfnB->marked_nonbottom_begin())
            {
                continue;
            }
            // 3.19l: if notblue(s_prime) undefined then
            if (s_prime->pos >= shared_data.notblue_initialised_end)
            {
                // 3.20l: notblue(s_prime) := |inert_out(s_prime)|
                s_prime->notblue = s_prime->inert_succ_end() -
                                                   s_prime->inert_succ_begin();
                bisim_tb::swap_permutation(s_prime->pos,
                                          shared_data.notblue_initialised_end);
                ++shared_data.notblue_initialised_end;
            // 3.21l: end if
            }
            // 3.22l: notblue(s_prime) := notblue(s_prime) - 1
            --s_prime->notblue;
            // 3.23l: if notblue(s_prime) == 0 && ...
            if (0 != s_prime->notblue)  continue;
            // 3.23l: ... && (FromRed == {} ||
            //                out_noninert(s_prime) intersect SpBu == {}) then
            if (!shared_data.FromRed_is_handled)
            {
                if (s_prime->surely_has_transition_to(FromRed->bunch))
                {
                    continue;
                }
                if(!s_prime->surely_has_no_transition_to(FromRed->bunch,NewBu))
                {
                    // It is not yet known whether s_prime has a transition to
                    // SpC or not.  Execute the slow test now.
                    // This is a linear search through the bunches (instead of
                    // the binary search in the previous version).
                    COROUTINE_FOR (REFINE_BLUE_TESTING,
                            succ_iter = s_prime->succ_begin(),
                                    s_prime->succ_end() != succ_iter &&
                                        succ_iter->B_a_B->B_a_B_slice->bunch !=
                                                                FromRed->bunch,
                                    succ_iter = succ_iter->bunch_slice->end)
                    {  }
                    END_COROUTINE_FOR;
                    if (s_prime->succ_end() != succ_iter)  continue;
                }
            }
            // 3.24l: Blue := Blue union {s_prime}
            bisim_tb::swap_permutation(s_prime->pos, blue_nonbottom_end);
            ++blue_nonbottom_end;
            // 3.5l: whenever |Blue| > |RfnB| / 2 do  Abort this coroutine
            if (blue_nonbottom_end - RfnB->unmarked_nonbottom_begin() +
                               RfnB->unmarked_bottom_size() > RfnB->size() / 2)
            {
                ABORT_THIS_COROUTINE();
            }
            // 3.25l: end if
                // this is implicit in the `continue` statements above.
        // 3.26l: end for
        }
        END_COROUTINE_FOR;
    // 3.27l: end while
        if (RfnB->unmarked_bottom_end() == visited_end)
        {
            visited_end = RfnB->unmarked_nonbottom_begin();
        }
    }
    END_COROUTINE_DO_WHILE;

    /*  -  -  -  -  -  -  -  - split off blue block -  -  -  -  -  -  -  -  */

mCRL2log(log::debug, "bisim_tb") << "The blue subblock is smaller.\n";
    // 3.28l: Abort the other coroutine
    ABORT_OTHER_COROUTINE();
    // From now on, we do no longer need the reentrant ``local'' variables.
    // All non-blue states are red.
    // 3.29l: Move Blue to a new block NewB
    // and
    // 3.30l: Destroy all temporary data
    bisim_tb::block_t* const NewB = RfnB->split_off_blue(blue_nonbottom_end);
    part_tr.new_block_created(RfnB, NewB, true, aut);
}
END_COROUTINE


template <class LTS_TYPE>
DEFINE_COROUTINE(bisim_partitioner_tb<LTS_TYPE>::, refine_red,
    /* formal parameters:*/ ((bisim_tb::block_t* const, RfnB))
                            ((bisim_tb::B_a_B_desc_iter_t const, FromRed))
                            ((const bool, unmarked_bottom_states_are_blue)),
    /* local variables:  */ ((bisim_tb::B_a_B_iter_t, fromred_visited_ptr))
                            ((bool, forward))
                            ((bisim_tb::B_a_B_desc_iter_t, slice))
                            ((bisim_tb::permutation_iter_t, visited_begin))
                            ((bisim_tb::state_info_ptr, s))
                            ((bisim_tb::pred_iter_t, pred_iter)),
    /* shared data:      */ struct bisim_tb::refine_shared_t, shared_data,
    /* interrupt locatns:*/ (REFINE_RED_COLLECT_FROMRED)
                            (REFINE_RED_PREDECESSOR_HANDLED)
                            (REFINE_RED_STATE_HANDLED)
                            (REFINE_RED_COLLECT_FROMRED_SLICE))
{
mCRL2log(log::debug, "bisim_tb") << "refine_red started\n";
    // 3.5r: whenever |Red| > |RfnB|/2 then  Abort this coroutine
    if (RfnB->marked_size() > RfnB->size() / 2)  ABORT_THIS_COROUTINE();

    /*  -  -  -  -  -  -  - collect states from FromRed -  -  -  -  -  -  -  */

    // 3.6r: while FromRed != {} do
    if (!shared_data.FromRed_is_handled)
    {
        // for all transitions in the bunch containing FromRed
        forward = true;
        slice = FromRed;
        assert(RfnB->from_block.end() != slice);
        COROUTINE_WHILE(REFINE_RED_COLLECT_FROMRED_SLICE, true)
        {
            if (RfnB->inert_slice() != slice &&
                       (fromred_visited_ptr = slice->begin,
                        !unmarked_bottom_states_are_blue ||
                        (fromred_visited_ptr = slice->begin_from_non_bottom,
                                           fromred_visited_ptr != slice->end)))
            {
                COROUTINE_DO_WHILE(REFINE_RED_COLLECT_FROMRED,
                                             fromred_visited_ptr != slice->end)
                {
                    // 3.7r: Choose s --> t in FromRed
                    bisim_tb::state_info_ptr const s =
                                             fromred_visited_ptr->pred->source;
mCRL2log(log::debug, "bisim_tb") << s->debug_id() << " is red\n";
                    assert(s->block == RfnB);
                    // 3.8r: Test := Test \ {s}
                    // and
                    // 3.9r: Red := Red union {s}
                    if (s->pos < shared_data.notblue_initialised_end)
                    {
                        // The non-bottom state has a transition to a blue
                        // state, so notblue is initialised; however, now it is
                        // revealed to be red anyway.
                        bisim_tb::swap_permutation(s->pos,
                                        --shared_data.notblue_initialised_end);
                    }
                    if (RfnB->mark(s) &&
                    // 3.5r: whenever |Red| > |RfnB|/2 do  Abort this coroutine
                                        RfnB->marked_size() > RfnB->size() / 2)
                    {
                        ABORT_THIS_COROUTINE();
                    }
                    // 3.10r: FromRed := FromRed \ {s --> t}
                    ++fromred_visited_ptr;
    // 3.13r: end while
                }
                END_COROUTINE_DO_WHILE;
            }
            if (forward)
            {
                ++slice;
                if (RfnB->from_block.end() != slice &&
                                                slice->bunch == FromRed->bunch)
                {
                    continue;
                }
                forward = false;
                slice = FromRed;
            }
            if (RfnB->from_block.begin() == slice ||
                                            (--slice)->bunch != FromRed->bunch)
            {
                break;
            }
        }
        END_COROUTINE_WHILE;

        // The shared variable FromRed_is_handled is set to true as soon as
        // FromRed should be considered empty.  (From that moment on, no slow
        // tests are needed any more.)
        shared_data.FromRed_is_handled = true;
    }

    /*  -  -  -  -  -  -  -  - visit red states -  -  -  -  -  -  -  -  */

    assert(0 != RfnB->marked_size());
    visited_begin = RfnB->marked_bottom_end();
    if (RfnB->marked_bottom_begin() == visited_begin)
    {
        // It may happen that all found states are non-bottom states.  (In that
        // case, some of these states will become new bottom states.)
        visited_begin = RfnB->marked_nonbottom_end();
    }
    // 3.15r: while Red contains unvisited states do
    COROUTINE_DO_WHILE(REFINE_RED_STATE_HANDLED,
                               RfnB->marked_nonbottom_begin() != visited_begin)
    {
        // 3.17r (order of lines changed): Mark s as visited
        --visited_begin;
        // 3.16r: Choose an unvisited s in Red
        s = *visited_begin;
        // 3.18r: for all s_prime in inert_in(s) do
        COROUTINE_FOR (REFINE_RED_PREDECESSOR_HANDLED,
                    pred_iter = s->inert_pred_begin(),
                                 s->inert_pred_end() != pred_iter, ++pred_iter)
        {
            bisim_tb::state_info_ptr const s_prime = pred_iter->source;
            // 3.24r: Red := Red union {s_prime}
            if (s_prime->pos < shared_data.notblue_initialised_end)
            {
                // The state has a transition to a blue state, so notblue is
                // initialised; however, now it is revealed to be red anyway.
                bisim_tb::swap_permutation(s_prime->pos,
                                        --shared_data.notblue_initialised_end);
            }
            if (RfnB->mark_nonbottom(s_prime) &&
            // 3.5r: whenever |Red| > |RfnB|/2 do  Abort this coroutine
                                        RfnB->marked_size() > RfnB->size() / 2)
            {
                ABORT_THIS_COROUTINE();
            }
        // 3.26r: end for
        }
        END_COROUTINE_FOR;
    // 3.27r: end while
        if (RfnB->marked_bottom_begin() == visited_begin)
        {
            visited_begin = RfnB->marked_nonbottom_end();
        }
    }
    END_COROUTINE_DO_WHILE;

    /*  -  -  -  -  -  -  -  - split off red block -  -  -  -  -  -  -  -  */

mCRL2log(log::debug, "bisim_tb") << "The red subblock is smaller.\n";
    // 3.28r: Abort the other coroutine
    ABORT_OTHER_COROUTINE();
    // From now on, we do no longer need the reentrant ``local'' variables.
    // All non-red states are blue.
    // 3.29r: Move Red to a new block NewB
    // and
    // 3.30r: Destroy all temporary data
    bisim_tb::block_t* const NewB =
                           RfnB->split_off_red(RfnB->marked_nonbottom_begin());
    part_tr.new_block_created(RfnB, NewB, false ONLY_IF_DEBUG( , aut ) );
}
END_COROUTINE



/*=============================================================================
=                        Postprocess new bottom states                        =
=============================================================================*/


template <class LTS_TYPE>
bisim_tb::block_t* bisim_partitioner_tb<LTS_TYPE>::prepare_for_postprocessing(
                                                      bisim_tb::block_t** RfnB)
{
    bisim_tb::block_t* ResultB;

    if (0 != (*RfnB)->marked_bottom_size())
    {
        ResultB = refine(*RfnB, (*RfnB)->from_block.end());
        // 4.26: Destroy all temporary data
            // As part of this line, we unmark all states.
        ResultB->set_marked_bottom_begin(ResultB->marked_bottom_end());
        *RfnB = ResultB->begin()[-1]->block;
    }
    else
    {
        ResultB = nullptr;
    }

    // do not refine a trivial block
    if (1 != (*RfnB)->size())
    {
        // Now we do not create a search tree of bunches because that may take
        // too long. However, we do move all bunches to the list of unstable
        // bunches and handle them in the order that they happen to appear in
        // the list.

        // 4.8: for all bottom states s in RfnB do
        bisim_tb::permutation_iter_t s_iter = (*RfnB)->unmarked_bottom_begin();
        do
        {
            bisim_tb::state_info_ptr const s = *s_iter;
            // for all transitions s --> t do
            for (bisim_tb::succ_iter_t succ_iter = s->succ_begin();
                                       succ_iter != s->succ_end(); ++succ_iter)
            {
                bisim_tb::B_a_B_desc_iter_t slice =
                                                 succ_iter->B_a_B->B_a_B_slice;
                if (slice->begin_from_non_bottom == slice->begin)
                {
                    // this is the first transition in the slice for this run
                    // of postprocessing.
                    // insert the B_a_B_slice of s --> t into R
                    if (nullptr == slice->bunch->postprocess.null)
                    {
                        slice->bunch->postprocess.list_entry =
                                                      part_tr.R.before_begin();
                    }
                    slice->bunch->postprocess.list_entry = part_tr.R.
                      insert_after(slice->bunch->postprocess.list_entry,slice);
                    assert(nullptr != slice->bunch->postprocess.null);
                }
                // insert s --> t into the set of transitions from bottom
                // states
                part_tr.swap_B_a_B(slice->begin_from_non_bottom++,
                                                             succ_iter->B_a_B);
            // end for
            }
        // 4.10: end for
            ++s_iter;
        }
        while ((*RfnB)->unmarked_bottom_end() != s_iter);
    }
    return ResultB;
}


/// \brief Split a block with new bottom states as needed
/// \details The function splits RedB by checking whether all new bottom
/// states can reach the constellations that RedB can reach.
///
/// When this function starts, it assumes that the old bottom states of RedB
/// are marked and the new ones are not.  It is an error if RedB does not
/// contain any new bottom states.
/// The function implements Algorithm 4 of [GJKW 2017].  It first separates the
/// old from the new bottom states; then it walks through all constellations
/// that can be reached from the new bottom states to separate them into
/// smaller, stable subblocks.  The return value is the block containing the
/// old bottom states, resulting from the first separation.
/// \param RedB  block containing new bottom states that need to be stabilised
/// \returns the block containing the old bottom states (and every state in
///          RedB that can reach some old bottom state through inert
///          transitions)
template <class LTS_TYPE>
bisim_tb::block_t* bisim_partitioner_tb<LTS_TYPE>::postprocess_new_bottom(
                                 bisim_tb::block_t* RfnB
                                 /* , bisim_tb::block_t* BlueB */)
{

    /*------- collect constellations reachable from new bottom states -------*/

    // 4.2: Create an empty list R of refinable bunches
        // note that this list is unsorted
        // actually it is not a list of bunches, but of B_a_B sets.  For each
        // bunch, we include the B_a_B sets that contain new bottom states.
        // The sets that belong to the same bunch need to be placed immediately
        // after each other.
        // I think it can be a forward list... Let's see what operations are
        // needed.
    assert(part_tr.R.empty());
        // a list of pointers to B_a_B slices, containing every slice with
        // transitions starting in a bottom state.
        //   Problem: if backtracking is needed because new bottom states are
        // found afterwards, the transitions starting in these new bottom
        // states should be added to R. Note that these transitions need to be
        // added only once; some may already be in R (because splitting a block
        // should entail that both parts are added to R?).
        //
        // R is a list of pointers to B_a_B slices, one per bunch.
        //   Problem: If RfnB is split, both parts need to be refined w. r. t.
        // the transition in the bunch to which the B_a_B slice belongs.  I
        // think that thich is possible to do in the time required.
    // 4.3: <ResultB, RfnB> := Refine(RfnB, cosplit(RfnB, BlueB), {old bottom
    //                                                     states in RfnB}, {})
    bisim_tb::block_t* ResultB;

    ResultB = prepare_for_postprocessing(&RfnB);

    /*--------------- stabilise w. r. t. found constellations ---------------*/

    assert(nullptr == bisim_tb::block_t::postprocessing_this_bunch_first);
    // for all refinable bunches SpBu do
    while (!part_tr.R.empty())
    {
        bisim_tb::B_a_B_desc_iter_t SpSl = part_tr.R.front();
        bisim_tb::bunch_t* const SpBu = SpSl->bunch;
        // for all slices SpSl in R belonging to SpBu do
        // make the bunch non-refinable and mark the red bottom states
        do
        {
            // Remove slice SpSl from R
            part_tr.R.pop_front();
            // Add the source block of transitions in SpSl to the set of blocks
            // that need postprocessing
            RfnB = SpSl->from_block();
            if (1 < RfnB->size())
            {
                // (While we didn't add blocks of 1 state to R earlier, it may
                // happen that RfnB has been split in the meantime so only 1
                // state remains in it.)
                RfnB->make_postprocessing_this_bunch();
                // for all transitions from new bottom states in SpSl do
                for (bisim_tb::B_a_B_iter_t tr_iter = SpSl->begin;
                             SpSl->begin_from_non_bottom != tr_iter; ++tr_iter)
                {
                    // Mark the source state
                    RfnB->mark(tr_iter->pred->source);
                }
            }
            else
            {
                SpSl->begin_from_non_bottom = SpSl->begin;
            }
            if (part_tr.R.empty())  break;
            SpSl = part_tr.R.front();
        }
        while (SpSl->bunch == SpBu);
        SpBu->postprocess.null = nullptr;

        // for all blocks RfnB in the set of those that need postprocessing do
        while(nullptr!=bisim_tb::block_t::get_some_postprocessing_this_bunch())
        {
            RfnB = bisim_tb::block_t::get_some_postprocessing_this_bunch();
            assert(1 < RfnB->size());

            // RedB := refine(RfnB)
            // and insert new slices into R as needed -- this is triggered
            // because RfnB is a block that needs_postprocessing_this_bunch().
            bisim_tb::block_t* RedB = refine(RfnB, SpSl, nullptr/* NewBu cannot
                                 be given, so we'll only use the slow test */);

            // if RedB has new bottom states
            if (RedB->unmarked_bottom_size() != 0)
            {
                // Prepare the slices of outgoing transitions of RedB similar
                // to Lines 4.4-4...
                prepare_for_postprocessing(&RedB);
            }
            // remove the block from the set of blocks that need postprocessing
            RfnB->make_non_postprocessing_this_bunch();
        }
        // empty the set of blocks that need postprocessing
        assert(nullptr == bisim_tb::block_t::postprocessing_this_bunch_first);

    // 4.25: end for
    }
    // 4.26: Destroy all temporary data
//        We somewhere need to reset:
//            SpSl->begin_from_non_bottom = SpSl->begin;

    return ResultB;
}



/*=============================================================================
=                       explicit instantiation requests                       =
=============================================================================*/



template class bisim_partitioner_tb<lts_lts_t>;
template class bisim_partitioner_tb<lts_aut_t>;
template class bisim_partitioner_tb<lts_fsm_t>;

} // end namespace detail
} // end namespace lts
} // end namespace mcrl2
