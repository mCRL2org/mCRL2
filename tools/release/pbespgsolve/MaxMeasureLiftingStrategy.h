// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
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

/*! \ingroup LiftingStrategies

    A lifting strategy that propagate maximum measures first.

    Conceptually this is a specialization of the predecessor lifting strategy.
    However, of all feasible vertices to select for the next lifting attempt,
    one is chosen specifically that has the largest maximum progress measure
    vector.

    The implementation uses a custom heap structure to act as a priority queue.
    Special care must be taken to maintain the heap property, because lifting
    vertices changes the associated progress measure!
*/
class MaxMeasureLiftingStrategy2 : public LiftingStrategy2
{
public:
    enum Order { QUEUE = 0, STACK = 1, HEAP = 2 }; 
    enum Metric { MAX_VALUE = 0, MAX_STEP = 1, MIN_VALUE = 2 };

    MaxMeasureLiftingStrategy2( const ParityGame &game,
                                const SmallProgressMeasures &spm,
                                Order order, Metric metric );
    ~MaxMeasureLiftingStrategy2();

    void push(verti v);
    void bump(verti v);
    verti pop();

protected:

    /*! Moves the element at index i up the heap until the heap property
        is restored. */
    void move_up(verti i);

    /*! Moves the element at index i down the heap until the heap property
        is restored. */
    void move_down(verti i);

    /*! Swaps the elements at indices i and j in the heap. */
    void swap(verti i, verti j);

    /*! Removes the vertex from the queue, if it is present. */
    //void remove(verti v);

    /*! Compares the vertices referred through indices i and j in the heap. */
    int cmp(verti i, verti j);

    /*! Checks if the queue satisfies the heap property (used for debugging) */
    bool check();

private:
    MaxMeasureLiftingStrategy2(const MaxMeasureLiftingStrategy2 &);
    MaxMeasureLiftingStrategy2 &operator=(const MaxMeasureLiftingStrategy2 &);

private:
    const SmallProgressMeasures &spm_;  //!< SPM instance being solved
    const Order order_;                 //!< vertex extraction order
    const Metric metric_;               //!< comparison metric

    uint64_t next_id_;        //!< number of insertions
    uint64_t * insert_id_;    //!< for each vertex: last insertion time

    verti * const pq_pos_;      //!< for each vertex: position in the p.q. or -1
    verti * const pq_;          //!< priority queue of lifted vertices
    verti pq_size_;             //!< priority queue size

    std::vector<verti> bumped_;
};

/*! \ingroup LiftingStrategies
    A factory class for MaxMeasureLiftingStrategy instances. */
class MaxMeasureLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    MaxMeasureLiftingStrategyFactory(
            MaxMeasureLiftingStrategy2::Order order =
                MaxMeasureLiftingStrategy2::HEAP,
            MaxMeasureLiftingStrategy2::Metric metric =
                MaxMeasureLiftingStrategy2::MAX_VALUE )
        : order_(order), metric_(metric) { };

    bool supports_version(int version);

    LiftingStrategy *create(const ParityGame&, const SmallProgressMeasures &);

    LiftingStrategy2 *create2( const ParityGame &game,
                               const SmallProgressMeasures &spm );

private:
    const MaxMeasureLiftingStrategy2::Order  order_;
    const MaxMeasureLiftingStrategy2::Metric metric_;
};

#endif /* ndef PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED */
