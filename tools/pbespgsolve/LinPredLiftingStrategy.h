// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LINPRED_LIFTING_STRATEGY_H_INCLUDED
#define LINPRED_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"
#include <algorithm>
#include <vector>

/*! \ingroup LiftingStrategies

    A lifting strategy that combines the linear and predecessor lifting
    strategies: vertices are lifted in sequential order (like with the linear
    lifting strategy) but after a single pass over all vertices, only those
    vertices which had a successor lifted are (like with the predecessor lifting
    strategy).

    Intended for debugging, as it shouldn't really offer any benefits over
    predecessor lifting using a queue. Its main advantage is that the order in
    which vertices are lifted is more predictable.
*/
class LinPredLiftingStrategy : public LiftingStrategy
{
public:
    LinPredLiftingStrategy( const ParityGame &game,
                            const SmallProgressMeasures &spm )
        : LiftingStrategy(), graph_(game.graph())
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

private:
    //! Graph for the game being solved.
    const StaticGraph &graph_;

    //! List of vertices to be lifted in the current pass.
    std::vector<verti> cur_queue;

    /*! List of vertices to be lifted in the next next pass.
        These are the predecessors of the vertices that have been successfully
        lifted during the current pass. */
    std::vector<verti> next_queue;

    /*! Iterator over `cur_queue` that points to the next vertex to be lifted
        in the current pass. */     
    std::vector<verti>::const_iterator pos; 
};

/*! \ingroup LiftingStrategies
    A factory class for LinPredLiftingStrategy instances. */
class LinPredLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    //! Return a new LinPredLiftingStrategy instance.
    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm )
    {
        return new LinPredLiftingStrategy(game, spm);
    }
};

#endif /* ndef PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED */
