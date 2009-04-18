// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "MaxMeasureLiftingStrategy.h"
#include <algorithm>
#include <assert.h>

#include <stdio.h>  /* debug */

/* Updated implementation:

    Whenever a vertex is lifted, it is pushed into a priority queue (with its
    own progress measure vector as a priority) and its predecessors are marked
    candidates for lifting.

    Then, to select a next vertex to be lifted, the predecessors of the top node
    in the priority queue are examined, and the first one marked for lifting is
    returned. If there is no such predecessor, the top node is popped, and the
    process is repeated, until the queue is empty.

    (Initially, all vertices are pushed into the queue, and all vertices are
    marked candidates for lifting.)
*/

MaxMeasureLiftingStrategy::MaxMeasureLiftingStrategy(const ParityGame &game)
    : LiftingStrategy(game), queued_(new bool[graph_.V()]),
      pq_pos_(new verti[graph_.V()]), pq_(new verti[graph_.V()])
{
}

MaxMeasureLiftingStrategy::~MaxMeasureLiftingStrategy()
{
    delete[] queued_;
    delete[] pq_pos_;
    delete[] pq_;
}

void MaxMeasureLiftingStrategy::move_up(verti i)
{
    // FIXME: this can be implemented with less swapping if I think harder.
    for (verti  j; i > 0 && cmp(i, j = (i - 1)/2) > 0; i = j) swap(i, j);
}

void MaxMeasureLiftingStrategy::move_down(verti i)
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

void MaxMeasureLiftingStrategy::swap(verti i, verti j)
{
    verti v = pq_[i], w = pq_[j];
    pq_[i] = w;
    pq_[j] = v;
    pq_pos_[w] = i;
    pq_pos_[v] = j;
}

void MaxMeasureLiftingStrategy::push(verti v)
{
    verti i = pq_pos_[v];
    if (i == (verti)-1)
    {
        i = pq_size_++;
        pq_[i] = v;
        pq_pos_[v] = i;
    }
    move_up(i);
}

void MaxMeasureLiftingStrategy::pop()
{
    assert(pq_size_ > 0);
    pq_pos_[pq_[0]] = (verti)-1;
    pq_[0] = pq_[--pq_size_];
    pq_pos_[pq_[0]] = 0;
    move_down(0);
}

int MaxMeasureLiftingStrategy::cmp(verti i, verti j)
{
    return spm_->vector_cmp(pq_[i], pq_[j], spm_->len_);
}

bool MaxMeasureLiftingStrategy::check()
{
    for (verti i = 1; i < pq_size_; ++i)
    {
        if (cmp(i, (i - 1)/2) > 0) return false;
    }

    for (verti i = 0; i < pq_size_; ++i)
    {
        if (pq_pos_[pq_[i]] != i) return false;
    }

    for (verti v = 0; v < graph_.V(); ++v)
    {
        if (pq_pos_[v] != (verti)-1)
        {
            if (pq_[pq_pos_[v]] != v) return false;
        }
    }

    return true;
}

verti MaxMeasureLiftingStrategy::next(verti prev_vertex, bool prev_lifted)
{
    if (prev_vertex == NO_VERTEX)
    {
        // Initialize queue
        pq_size_ = 0;
        for (verti v = 0; v < graph_.V(); ++v)
        {
            queued_[v] = true;
            pq_pos_[v] = (verti)-1;
            push(v);
        }
        /* FIXME: pushing everything takes O(V log V) time; we can sort the 
                  queue array faster than that by using our knowledge that
                  all progress measures are either zero or top. */
    }

    // assert(check());  // debug

    if (prev_lifted)
    {
        // Add to (or move up in) queue
        push(prev_vertex);

        // Mark predecessors as queued
        for (StaticGraph::const_iterator it = graph_.pred_begin(prev_vertex);
             it != graph_.pred_end(prev_vertex); ++it)
        {
            queued_[*it] = true;
        }
    }

    // Find a predecessor to lift
    while (pq_size_ > 0)
    {
        verti w = pq_[0];
        for (StaticGraph::const_iterator it = graph_.pred_begin(w);
             it != graph_.pred_end(w); ++it)
        {
            verti v = *it;
            if (queued_[v])
            {
                queued_[v] = false;
                return v;
            }
        }

        // None of the predecessors of w are queued anymore; remove it.
        pop();
        // assert(check());  // debug
    }

    return NO_VERTEX;
}

size_t MaxMeasureLiftingStrategy::memory_use() const
{
    return graph_.V()*(sizeof(bool) + 2*sizeof(verti));
}
