// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LINPRED_LIFTING_STRATEGY_H_INCLUDED
#define LINPRED_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"
#include <algorithm>
#include <vector>

/*! A lifting strategy that combines the linear and predecessor lifting
    strategies: vertices are lifted in sequential order (like with the linear
    lifting strategy) but after a single pass over all vertices, only those
    vertices which had a successor lifted are (like with the predecessor lifting
    strategy).

    Intended for debugging, as it shouldn't really offer any benefits over
    predecessor lifting using a queue. Its main advantage is that the order in
    which vertices are lifted is more predictable. */
class LinPredLiftingStrategy : public LiftingStrategy
{
public:
    LinPredLiftingStrategy( const ParityGame &game,
                            const SmallProgressMeasures &spm )
        : LiftingStrategy(game)
    {
        (void)spm;  // unused
        cur_queue.reserve(graph_.V());
        for (verti v = 0; v < graph_.V(); ++v) cur_queue.push_back(v);
        pos = cur_queue.begin();
    }

    void lifted(verti v)
    {
        for ( StaticGraph::const_iterator it = graph_.pred_begin(v);
              it != graph_.pred_end(v); ++it ) next_queue.push_back(*it);
    }

    verti next()
    {
        if (pos == cur_queue.end())
        {
            std::sort(next_queue.begin(), next_queue.end());
            next_queue.erase( std::unique(next_queue.begin(), next_queue.end()),
                              next_queue.end() );
            cur_queue.clear();
            cur_queue.swap(next_queue);
            pos = cur_queue.begin();
        }
        if (pos == cur_queue.end()) return NO_VERTEX;
        return *pos++;
    }

    size_t memory_use() const
    {
        return sizeof(*this) + sizeof(cur_queue[0])*cur_queue.capacity() +
                               sizeof(next_queue[0])*next_queue.capacity();
    }

private:
    std::vector<verti> cur_queue, next_queue;
    std::vector<verti>::const_iterator pos;
};

class LinPredLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm )
    {
        return new LinPredLiftingStrategy(game, spm);
    }
};

#endif /* ndef PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED */
