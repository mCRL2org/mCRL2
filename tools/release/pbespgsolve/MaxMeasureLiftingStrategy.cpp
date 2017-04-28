// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "MaxMeasureLiftingStrategy.h"
#include <algorithm>
#include <cassert>

#include <cstdio>  /* debug */

/* TODO: write short description of how this works! */

MaxMeasureLiftingStrategy2::MaxMeasureLiftingStrategy2(
    const ParityGame &game, const SmallProgressMeasures &spm,
    Order order, Metric metric )
        : LiftingStrategy2(), spm_(spm), order_(order), metric_(metric),
          next_id_(0),
          insert_id_(order < HEAP ? new uint64_t[game.graph().V()] : 0),
          pq_pos_(new verti[game.graph().V()]),
          pq_(new verti[game.graph().V()]), pq_size_(0)
{
    std::fill(&pq_pos_[0], &pq_pos_[game.graph().V()], NO_VERTEX);
}

MaxMeasureLiftingStrategy2::~MaxMeasureLiftingStrategy2()
{
    delete[] insert_id_;
    delete[] pq_pos_;
    delete[] pq_;
}

void MaxMeasureLiftingStrategy2::move_up(verti i)
{
    // FIXME: this can be implemented with less swapping if I think harder.
    for (verti j; i > 0 && cmp(i, j = (i - 1)/2) > 0; i = j) swap(i, j);
}

void MaxMeasureLiftingStrategy2::move_down(verti i)
{
    // FIXME: this can be implemented with less swapping if I think harder.
    for (;;)
    {
        verti j = 2*i + 1;
        verti k = 2*i + 2;
        int d = j < pq_size_ ? cmp(i, j) : 1;
        int e = k < pq_size_ ? cmp(i, k) : 1;

        if (d < 0 && e < 0)
        {
            // both children are larger than current node
            if (cmp(j, k) >= 0)
            {
                // left child is largest
                swap(i, j);
                i = j;
            }
            else
            {
                // right child is largest;
                swap(i, k);
                i = k;
            }
        }
        else
        if (d < 0)
        {
            // left child is larger
            swap(i, j);
            i = j;
        }
        else
        if (e < 0)
        {
            // right child is larger
            swap(i, k);
            i = k;
        }
        else
        {
            // both children are smaller; we're done
            break;
        }
    }
}

void MaxMeasureLiftingStrategy2::swap(verti i, verti j)
{
    verti v = pq_[i], w = pq_[j];
    pq_[i] = w;
    pq_[j] = v;
    pq_pos_[w] = i;
    pq_pos_[v] = j;
}

void MaxMeasureLiftingStrategy2::push(verti v)
{
    mCRL2log(mcrl2::log::debug) <<"push(" << v << ")" << std::endl;
    assert(pq_pos_[v] == NO_VERTEX);
    pq_[pq_size_] = v;
    pq_pos_[v] = pq_size_;
    ++pq_size_;
    if (insert_id_) insert_id_[v] = next_id_++;
    bumped_.push_back(pq_pos_[v]);
}

void MaxMeasureLiftingStrategy2::bump(verti v)
{
    mCRL2log(mcrl2::log::debug) << "bump(" << v << ")" << std::endl;
    bumped_.push_back(pq_pos_[v]);
}

verti MaxMeasureLiftingStrategy2::pop()
{
#ifdef DEBUG
    static long long ops;
    ops += bumped_.size() + 1;
#endif

    if (!bumped_.empty())
    {
        // Move bumped vertices up the heap.
        std::sort(bumped_.begin(), bumped_.end());
        bumped_.erase( std::unique(bumped_.begin(), bumped_.end()),
                       bumped_.end() );
        for (std::vector<verti>::iterator it = bumped_.begin();
             it != bumped_.end(); ++it) move_up(*it);

        // CHECKME: why is this necessary for MAX_STEP too?
        //          shouldn't this just be for MIN_STEP?
        if (metric_ != MAX_VALUE)
        {
            /* Note: minimization is a bit trickier than maximization, since
               we need to move bumped vertices down the heap (rather than up
               when maximizing) but pushed vertices still need to move up.

               Unfortunately, we can't easily distinguish between bumped or
               pushed or pushed-and-then-bumped vertices, so the easiest safe
               way to handle the situation is to move up first, and then down.

               FIXME: optimize this?
            */

            // Move bumped vertices down the heap.
            for (std::vector<verti>::reverse_iterator it = bumped_.rbegin();
                 it != bumped_.rend(); ++it) move_down(*it);
        }
        bumped_.clear();
    }

    if (pq_size_ == 0) return NO_VERTEX;

#ifdef DEBUG
    if (ops >= pq_size_)
    {
        Logger::debug("checking heap integrity");
        assert(check());
        ops -= pq_size_;
    }
#endif

    // Extract top element from the heap.
    verti v = pq_[0];
    mCRL2log(mcrl2::log::debug) << "pop() -> " << v << std::endl;
    pq_pos_[v] = NO_VERTEX;
    if (--pq_size_ > 0)
    {
        pq_[0] = pq_[pq_size_];
        pq_pos_[pq_[0]] = 0;
        move_down(0);
    }
    return v;
}

static int cmp_ids(uint64_t x, uint64_t y)
{
    return (x > y) - (x < y);
}

/* This returns +1 if lifting v1 to v2 would increase the progress measure
   by a larger difference than lifting w1 to w2.

   Assumes v1 >= v2 and w1 >= w2 (for the first v_len and w_len elements
   respectively) and that all vectors are within bounds.  (If they are not,
   the results are still somewhat sensible, but it may be possible that two
   vectors compare unequal even though their steps are equally large.)
*/
static int cmp_step( const verti *v1, const verti *v2, int v_len, bool v_carry,
                     const verti *w1, const verti *w2, int w_len, bool w_carry )
{
    for (int i = 0; i < v_len || i < w_len; ++i)
    {
        int a = i < v_len ? v2[i] - v1[i] : 0;
        int b = i < w_len ? w2[i] - w1[i] : 0;
        if (a != b) return (a > b) - (a < b);
    }
    if (v_carry || w_carry)
    {
        if (!w_carry) return +1;
        if (!v_carry) return -1;
        if (v_len < w_len) return +1;
        if (v_len > w_len) return -1;
    }
    return 0;
}

int MaxMeasureLiftingStrategy2::cmp(verti i, verti j)
{
    verti v = pq_[i], w = pq_[j];
    int d = 0;

    switch (metric_)
    {
    case MAX_VALUE:
        d = spm_.vector_cmp( spm_.get_successor(v),
                             spm_.get_successor(w), spm_.len_ );
        break;

    case MIN_VALUE:
        d = -spm_.vector_cmp( spm_.get_successor(v),
                              spm_.get_successor(w), spm_.len_ );
        break;

    case MAX_STEP:
#ifdef DEBUG
        // We assume vertices are only queued when they can be lifted;
        // i.e. their value is less than (or equal to) their successor:
        assert(spm_.vector_cmp(v, spm_.get_successor(v), spm_.len(v))
                    < spm_.compare_strict(v));
        assert(spm_.vector_cmp(w, spm_.get_successor(w), spm_.len(w))
                    < spm_.compare_strict(w));
#endif
        d = cmp_step( spm_.vec(v), spm_.vec(spm_.get_successor(v)),
                      spm_.len(v), spm_.compare_strict(v),
                      spm_.vec(w), spm_.vec(spm_.get_successor(w)),
                      spm_.len(w), spm_.compare_strict(w) );
        break;
    }

    if (d == 0)
    {
        // Tie-break on insertion order: smallest insert-id first in queue
        // mode, or largest insert-id first in stack mode.
        switch (order_)
        {
        case QUEUE: d = cmp_ids(insert_id_[w], insert_id_[v]); break;
        case STACK: d = cmp_ids(insert_id_[v], insert_id_[w]); break;
        default:    break;
        }
    }

    return d;
}

bool MaxMeasureLiftingStrategy2::check()
{
    for (verti i = 1; i < pq_size_; ++i)
    {
        if (cmp(i, (i - 1)/2) > 0) return false;
    }

    for (verti i = 0; i < pq_size_; ++i)
    {
        if (pq_pos_[pq_[i]] != i) return false;
    }

    const verti V = spm_.game().graph().V();
    for (verti v = 0; v < V; ++v)
    {
        if (pq_pos_[v] != NO_VERTEX)
        {
            if (pq_[pq_pos_[v]] != v) return false;
        }
    }

    return true;
}

bool MaxMeasureLiftingStrategyFactory::supports_version(int version)
{
    return version == 2;
}

LiftingStrategy *MaxMeasureLiftingStrategyFactory::create(
    const ParityGame& /*game*/, const SmallProgressMeasures &/*spm*/ )
{
    return nullptr;
}

LiftingStrategy2 *MaxMeasureLiftingStrategyFactory::create2(
    const ParityGame &game, const SmallProgressMeasures &spm )
{
    return new MaxMeasureLiftingStrategy2(game, spm, order_, metric_);
}
