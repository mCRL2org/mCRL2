// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "FocusListLiftingStrategy.h"
#include <assert.h>

/*! Credit for a vertex when it is put on the focus list. */
static const unsigned initial_credit  = 2;

/*! Credit increase when a vertex on the focus list is successfully lifted. */
static const unsigned credit_increase = 2;


FocusListLiftingStrategy::FocusListLiftingStrategy( const ParityGame &game,
    bool backward, bool alternate, verti max_size, long long max_lifts )
    : LiftingStrategy(game), max_lift_attempts_(max_lifts),
      phase_(1), num_lift_attempts_(0), lls_(game, backward, alternate)
{
    focus_list_.reserve(max_size);
}

void FocusListLiftingStrategy::lifted(verti vertex)
{
    if (phase_ == 1)
    {
        lls_.lifted(vertex);
        if (focus_list_.size() < focus_list_.capacity())
        {
            focus_list_.push_back(std::make_pair(vertex, initial_credit));
        }
    }
    else /* phase_ == 2 */
    {
        if (vertex == read_pos_->first) prev_lifted_ = true;
    }
}

verti FocusListLiftingStrategy::next()
{
    verti res =  phase_ == 1 ? phase1() : phase2();
    ++num_lift_attempts_;
    return res;
}

verti FocusListLiftingStrategy::phase1()
{
    if (focus_list_.size() == focus_list_.capacity() ||
        num_lift_attempts_ >= graph_.V())
    {
        if (focus_list_.empty())
        {
            /* This can only happen if lls_.num_failed >= graph_.V() too */
            assert(lls_.next() == NO_VERTEX);
            return NO_VERTEX;
        }

        /* Switch to phase 2: */
        phase_ = 2;
        num_lift_attempts_ = 0;
        read_pos_ = write_pos_ = focus_list_.begin();
        mCRL2log(mcrl2::log::verbose) << "Switching to focus list of size " << focus_list_.size() << std::endl;
        return phase2();
    }

    return lls_.next();
}

verti FocusListLiftingStrategy::phase2()
{
    if (num_lift_attempts_ > 0)
    {
        // Adjust previous vertex credit and move to next position
        focus_list::value_type prev = *read_pos_++;
        if (prev_lifted_)
        {
            prev.second += credit_increase;
            *write_pos_++ = prev;
        }
        else
        if (prev.second > 0)
        {
            prev.second /= 2;
            *write_pos_++ = prev;
        }
        // else, drop from list.
    }

    // Check if we've reached the end of the focus list; if so, restart:
    if (read_pos_ == focus_list_.end())
    {
        focus_list_.erase(write_pos_, focus_list_.end());
        read_pos_ = write_pos_ = focus_list_.begin();
    }

    if (focus_list_.empty() || num_lift_attempts_ >= max_lift_attempts_)
    {
        if (focus_list_.empty())
        {
            mCRL2log(mcrl2::log::verbose) << "Focus list exhausted." << std::endl;
        }
        else
        {
            mCRL2log(mcrl2::log::verbose) << "Maximum lift attempts (" << max_lift_attempts_ << ") on focus list reached." << std::endl;
            focus_list_.clear();
        }

        /* Switch to phase 1 */
        phase_ = 1;
        num_lift_attempts_ = 0;
        return phase1();
    }

    // Return current item on the focus list
    prev_lifted_ = false;
    return read_pos_->first;
}

size_t FocusListLiftingStrategy::memory_use() const
{
    return sizeof(*this) + sizeof(focus_list_[0])*focus_list_.capacity();
}

LiftingStrategy *FocusListLiftingStrategyFactory::create(
    const ParityGame &game, const SmallProgressMeasures &spm )
{
    (void)spm;  // unused

    /* Ratio is absolute value if >1, or a fraction of the size of the game's
       vertex set if <= 1. */
    verti V = game.graph().V();
    verti max_size  = (size_ratio_ > 1) ? static_cast<verti>(size_ratio_) : static_cast<verti>(size_ratio_*V); // XXX Ugly casting here
    if (max_size == 0) max_size = 1;
    if (max_size >  V) max_size = V;
    verti max_lifts = (verti)(lift_ratio_ * max_size);
    return new FocusListLiftingStrategy(
        game, backward_, alternate_, max_size, max_lifts );
}
