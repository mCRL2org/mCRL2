// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "PredecessorLiftingStrategy.h"
#include "assert.h"

PredecessorLiftingStrategy::PredecessorLiftingStrategy(
    const ParityGame &game, const SmallProgressMeasures &spm,
    bool backward, bool stack )
    : LiftingStrategy(game), spm_(spm), backward_(backward), stack_(stack)
{
    assert(graph_.edge_dir() & StaticGraph::EDGE_PREDECESSOR);

    // Initialize data
    verti V = game.graph().V();
    queued_ = new bool[V];
    std::fill(queued_, queued_ + V, true);
    queue_ = new verti[V];
    queue_size_ = queue_capacity_ = V;
    queue_begin_ = queue_end_ = 0;
    for (verti v = 0; v < V; ++v) queue_[v] = backward ? V - 1 - v : v;
}

PredecessorLiftingStrategy::~PredecessorLiftingStrategy()
{
    delete[] queued_;
    delete[] queue_;
}

void PredecessorLiftingStrategy::lifted(verti v)
{
    for ( StaticGraph::const_iterator it = graph_.pred_begin(v);
          it != graph_.pred_end(v); ++it )
    {
        if (!queued_[*it] && !spm_.is_top(*it))
        {
            // Add predecessor to the queue
            queued_[*it] = true;
            queue_[queue_end_++] = *it;
            if (queue_end_ == queue_capacity_) queue_end_ = 0;
            ++queue_size_;
            assert(queue_size_ <= queue_capacity_);
        }
    }
}

verti PredecessorLiftingStrategy::next()
{
    if (queue_size_ == 0) return NO_VERTEX;

    // Remove an element from the queue
    verti res;
    if (stack_)
    {
        // Remove from the back of the queue
        if (queue_end_ == 0) queue_end_ = queue_capacity_;
        res = queue_[--queue_end_];
    }
    else
    {
        // Remove from the front of the queue
        res = queue_[queue_begin_++];
        if (queue_begin_ == queue_capacity_) queue_begin_ = 0;
    }
    --queue_size_;
    queued_[res] = false;
    return res;
}

size_t PredecessorLiftingStrategy::memory_use() const
{
    return queue_capacity_*(sizeof(verti) + sizeof(char));
}

LiftingStrategy *PredecessorLiftingStrategyFactory::create(
    const ParityGame &game, const SmallProgressMeasures &spm )
{
    return new PredecessorLiftingStrategy(game, spm, backward_, stack_);
}
