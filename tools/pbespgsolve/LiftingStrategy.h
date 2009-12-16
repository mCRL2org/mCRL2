// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LIFTING_STRATEGY_H_INCLUDED
#define LIFTING_STRATEGY_H_INCLUDED

#include "ParityGame.h"
#include <string>

class SmallProgressMeasures;

/*! Instances of this class encapsulate vertex lifting strategies to be used
    with the small progress measures parity game solver. */
class LiftingStrategy
{
public:

    /*! Construct a strategy for the given parity game. */
    LiftingStrategy(const ParityGame &game)
        : graph_(game.graph()), game_(game) { };

    /*! Destroy the strategy */
    virtual ~LiftingStrategy() { };

    /*! Select the next vertex to lift.

        This method is called repeatedly by the SPM solver; the return value
        indicates which vertex to attempt to lift next. If lifting succeeds,
        the vertex will have a greater progress measure vector assigned to it.
        When no more vertices can be lifted, NO_VERTEX should be returned.

        \param prev_vertex Index of the vertex returned by the previous call
                           (or NO_VERTEX for the first call).
        \param prev_lifted Indicates wheter the vertex could be lifted.
    */
    virtual verti next(verti prev_vertex, bool prev_lifted) = 0;

    /*! Returns an estimation of the peak memory use for this strategy. */
    virtual size_t memory_use() const { return 0; }

protected:
    const StaticGraph &graph_;          //!< the game graph to work on
    const ParityGame &game_;            //!< the parity game to work on
};

/*! Abstract base class for lifting strategy factories. */
class LiftingStrategyFactory
{
public:
    virtual ~LiftingStrategyFactory();

    /*! Creates a lifting strategy factory from a string description. Returns
        NULL if the description could not be interpreted.

        String descriptions are as follows:

            linear:backward
                Use a linear lifting strategy (swiping).
                If backward is non-zero, scan vertices backward.
                Default: linear:0

            predecessor:backward:stack
                Use a predecessor lifting strategy (worklist).
                If backward is non-zero, scan vertices backward.
                If stack is non-zero, use a stack instead of a queue.
                Default: predecessor:0:0

            focuslist:backward:max_size
                Use a lifting strategy with a focus list.
                If backward is non-zero, scan vertices backward.
                max_size specificies the maximum size of the focus list, either
                as an absolute size greater than 1, or as a ratio between zero
                and 1, relative to the total number of vertices in the graph.
                Default: focuslist:0:0.1

            maxmeasure
                Variant on the predecessor lifting strategy that prefers to
                lift vertices with higher current progress measures, thereby
                (hopefully) increasing the average step size when lifting
                succeeeds, and decreasing the number of lifting atttempts
                required to arrive at a fixpoint.

            oldmaxmeasure
                Older implementation of max. measure lifting strategy.
    */
    static LiftingStrategyFactory *create(const std::string &description);

    /*! Create a lifting strategy for the given game, to be used by the given
        Small Progress Measures solver. */
    virtual LiftingStrategy *create( const ParityGame &game,
                                     const SmallProgressMeasures &spm ) = 0;
};


#endif /* ndef LIFTING_STRATEGY_H_INCLUDED */
