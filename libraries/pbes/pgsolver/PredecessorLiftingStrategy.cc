// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "PredecessorLiftingStrategy.h"
#include "assert.h"

PredecessorLiftingStrategy::PredecessorLiftingStrategy(
    const ParityGame &game, bool backward, bool stack )
    : LiftingStrategy(game), backward_(backward),stack_(stack)
{
    assert(game.graph().edge_dir() & StaticGraph::EDGE_PREDECESSOR);

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

verti PredecessorLiftingStrategy::next(verti prev_vertex, bool prev_lifted)
{
    const StaticGraph &graph = game_.graph();

    if (prev_lifted)
    {
        // prev_vertex was lifted; add its predecessors to the queue
        for ( StaticGraph::const_iterator it = graph.pred_begin(prev_vertex);
              it != graph.pred_end(prev_vertex); ++it )
        {
            if (!queued_[*it])
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
