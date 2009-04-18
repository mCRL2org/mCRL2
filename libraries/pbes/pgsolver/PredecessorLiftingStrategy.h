// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED
#define PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"
#include <deque>
#include <vector>

/*! A simple lifting strategy that puts all nodes in a queue, then takes them
    out one at a time; whenever a node is successfully lifted, its predecessors
    are put back in the queue as they may need to be lifted too.

    This strategy requires predecessor edges to be stored in the game graph.

    The queue can operate as a true queue or as a stack; the latter may result
    in better locality of reference and/or fewer unsuccessful lifting attempts.
    (This has not been tested.)

    (The Multi-Core Solver for Parity Games paper contains a description of
     a "work list approach" that is similar.)
*/

class PredecessorLiftingStrategy : public LiftingStrategy
{
public:
    /*! Construct a new predecessor lifting strategy instance.

        If `stack` is set to true, vertices are removed in last-in-first-out
        order (instead of the default first-in-first-out order).

        If `backward` is set to true, initial nodes are pushed in the queue
        backward (for a stack, this actually causes the nodes to be extracted
        in forward order instead of in reverse).
    */
    PredecessorLiftingStrategy( const ParityGame &game,
                                bool backward, bool stack );
    ~PredecessorLiftingStrategy();
    verti next(verti prev_vertex, bool prev_lifted);
    size_t memory_use() const;

    bool backward() const { return backward_; }
    bool stack() const { return stack_; }

private:
    const bool backward_;
    const bool stack_;
    bool *queued_;
    verti *queue_;
    size_t queue_size_, queue_capacity_, queue_begin_, queue_end_;
};

#endif /* ndef PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED */
