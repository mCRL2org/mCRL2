#include "MaxMeasureLiftingStrategy.h"

#include <stdio.h>  /* debug */

/* NOTE: this code abuses the fact that verti is unsigned, making the top vector
         strictly greater than vectors of other measures. */

MaxMeasureLiftingStrategy::MaxMeasureLiftingStrategy(const ParityGame &game)
    : LiftingStrategy(game), queue_pos_(graph_.V(), queue_.end())
{
}

MaxMeasureLiftingStrategy::~MaxMeasureLiftingStrategy()
{
}

verti MaxMeasureLiftingStrategy::next(verti prev_vertex, bool prev_lifted)
{
    if (prev_vertex == NO_VERTEX)
    {
        // Initialize queue
        for (verti v = 0; v < graph_.V(); ++v)
        {
            queue_pos_[v] =
                queue_.insert(std::make_pair(std::vector<verti>(), v)).first;
        }
    }
    else
    {
        if (prev_lifted)
        {
            std::vector<verti> m = vec(prev_vertex);

            // Add predecessors to queue
            for ( StaticGraph::const_iterator it = graph_.pred_begin(prev_vertex);
                  it != graph_.pred_end(prev_vertex); ++it )
            {
                verti u = *it;
                queue_t::iterator it = queue_pos_[u];

                /* Skip this predecessor if it is already queued with an
                   equal or greater weight. */
                if (it != queue_.end() && it->first >= m) continue;

                // If we have an old entry in the queue, remove it first.
                if (it != queue_.end()) queue_.erase(queue_pos_[u]);

                // Add new entry to the queue
                queue_pos_[u] = queue_.insert(std::make_pair(m, u)).first;
            }
        }
    }

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

size_t MaxMeasureLiftingStrategy::memory_use() const
{
    return 0;  // TODO, after the implementation is a bit more sane
}
