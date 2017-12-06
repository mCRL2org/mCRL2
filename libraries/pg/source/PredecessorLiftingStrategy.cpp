// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/pg/PredecessorLiftingStrategy.h"

#include <cassert>

PredecessorLiftingStrategy::PredecessorLiftingStrategy(
    const ParityGame &game, const SmallProgressMeasures &spm,
    bool stack, int version )
    : LiftingStrategy(), LiftingStrategy2(), spm_(spm), stack_(stack)
{
    assert(game.graph().edge_dir() & StaticGraph::EDGE_PREDECESSOR);

    // Initialize data
    const verti V = game.graph().V();
    queue_ = new verti[V];
    queue_capacity_ = V;
    queue_begin_ = queue_end_ = queue_size_ = 0;

    if (version == 1)
    {
        // v1 API requires explicit tracking of queued vertices.
        queued_ = new bool[V]();
        for (verti v = 0; v < V; ++v)
        {
            if (!spm_.is_top(v))
            {
                queued_[v] = true;
                push(v);
            }
        }
    }
    else  // version != 1
    {
        assert(version == 2);
        queued_ = NULL;
    }
}

PredecessorLiftingStrategy::~PredecessorLiftingStrategy()
{
    delete[] queue_;
    delete[] queued_;
}

void PredecessorLiftingStrategy::push(verti v)
{
    mCRL2log(mcrl2::log::debug) << "push(" << v << ")" << std::endl;
    queue_[queue_end_++] = v;
    if (queue_end_ == queue_capacity_) queue_end_ = 0;
    ++queue_size_;
    assert(queue_size_ <= queue_capacity_);
}

verti PredecessorLiftingStrategy::pop()
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
    mCRL2log(mcrl2::log::debug) << "pop() -> " << res << std::endl;
    return res;
}

void PredecessorLiftingStrategy::lifted(verti v)
{
    const StaticGraph &graph = spm_.game().graph();
    for ( StaticGraph::const_iterator it = graph.pred_begin(v);
          it != graph.pred_end(v); ++it )
    {
        verti u = *it;
        if (!queued_[u] && !spm_.is_top(u))
        {
            queued_[u] = true;
            push(u);
        }
    }
}

verti PredecessorLiftingStrategy::next()
{
    verti res = pop();
    if (res != NO_VERTEX) queued_[res] = false;
    return res;
}

bool PredecessorLiftingStrategyFactory::supports_version(int version)
{
    return version == 1 || version == 2;
}

LiftingStrategy *PredecessorLiftingStrategyFactory::create(
    const ParityGame &game, const SmallProgressMeasures &spm )
{
    return new PredecessorLiftingStrategy(game, spm, stack_, 1);
}

LiftingStrategy2 *PredecessorLiftingStrategyFactory::create2(
    const ParityGame &game, const SmallProgressMeasures &spm )
{
    return new PredecessorLiftingStrategy(game, spm, stack_, 2);
}
