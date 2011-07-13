// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "OldMaxMeasureLiftingStrategy.h"

#include <stdio.h>  /* debug */

/* NOTE: this code abuses the fact that verti is unsigned, making the top vector
         strictly greater than vectors of other measures. */

OldMaxMeasureLiftingStrategy::OldMaxMeasureLiftingStrategy(
    const ParityGame &game, const SmallProgressMeasures &spm )
        : LiftingStrategy(game), spm_(spm), queue_pos_(graph_.V(), queue_.end())
{
    // Initialize queue
    for (verti v = 0; v < graph_.V(); ++v)
    {
        queue_pos_[v] =
            queue_.insert(std::make_pair(std::vector<verti>(), v)).first;
    }
}

OldMaxMeasureLiftingStrategy::~OldMaxMeasureLiftingStrategy()
{
}

void OldMaxMeasureLiftingStrategy::lifted(verti v)
{
    std::vector<verti> m = vec(v);

    // Add predecessors to queue
    for ( StaticGraph::const_iterator it = graph_.pred_begin(v);
          it != graph_.pred_end(v); ++it )
    {
        verti u = *it;
        queue_t::iterator it1 = queue_pos_[u];

        /* Skip this predecessor if it is already queued with an
            equal or greater weight. */
        if (it1 != queue_.end() && it1->first >= m) continue;

        // If we have an old entry in the queue, remove it first.
        if (it1 != queue_.end()) queue_.erase(queue_pos_[u]);

        // Add new entry to the queue
        queue_pos_[u] = queue_.insert(std::make_pair(m, u)).first;
    }
}

verti OldMaxMeasureLiftingStrategy::next()
{
    // Take a predecessor from the queue
    verti v = NO_VERTEX;
    queue_t::iterator it = queue_.end();
    if (it != queue_.begin())
    {
        --it;
        v = it->second;
        queue_.erase(it);
        queue_pos_[v] = queue_.end();
    }
    return v;
}

size_t OldMaxMeasureLiftingStrategy::memory_use() const
{
    return 0;  // TODO, after the implementation is a bit more sane
}

LiftingStrategy *OldMaxMeasureLiftingStrategyFactory::create(
    const ParityGame &game, const SmallProgressMeasures &spm )
{
    return new OldMaxMeasureLiftingStrategy(game, spm);
}
