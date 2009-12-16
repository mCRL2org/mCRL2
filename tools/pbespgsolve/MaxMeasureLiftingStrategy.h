// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MAX_MEASURE_LIFTING_STRATEGY_H_INCLUDED
#define MAX_MEASURE_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"
#include <vector>
#include <set>
#include <utility>

class MaxMeasureLiftingStrategy : public LiftingStrategy
{
public:
    MaxMeasureLiftingStrategy( const ParityGame &game,
                               const SmallProgressMeasures &spm );
    ~MaxMeasureLiftingStrategy();

    verti next(verti prev_vertex, bool prev_lifted);
    size_t memory_use() const;

protected:

    /*! Moves the element at index i up the heap until the heap property
        is restored. */
    void move_up(verti i);

    /*! Moves the element at index i down the heap until the heap property
        is restored. */
    void move_down(verti i);

    /*! Swaps the elements at indices i and j in the heap. */
    void swap(verti i, verti j);

    /*! Pushes the vertex into the queue. */
    void push(verti v);

    /*! Pops the top element from the heap and restores the heap property */
    void pop();

    /*! Compares the vertices referred through indices i and j in the heap. */
    int cmp(verti i, verti j);

    /*! Checks if the queue satisfies the heap property (used for debugging) */
    bool check();

private:
    //! the SPM instance using this strategy
    const SmallProgressMeasures spm_;

    bool * const queued_;       //!< for each vertex: is it queued?

    verti * const pq_pos_;      //!< for each vertex: position in the p.q. or -1
    verti * const pq_;          //!< priority queue of lifted vertices
    verti pq_size_;             //!< priority queue size
};


class MaxMeasureLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm );
};

#endif /* ndef PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED */
