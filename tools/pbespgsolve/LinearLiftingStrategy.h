// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LINEAR_LIFTING_STRATEGY_H_INCLUDED
#define LINEAR_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"

/*! A simple lifting strategy that attempts to lift vertices in order (moving
    either forward or backward) and stops when no more vertices can be lifted.

    (The Multi-Core Solver for Parity Games paper calls this "Swiping")
*/

class LinearLiftingStrategy : public LiftingStrategy
{
public:
    LinearLiftingStrategy(const ParityGame &game, bool backward);
    verti next(verti prev_vertex, bool prev_lifted);

    bool backward() const { return backward_; }

private:
    const bool backward_;       //!< indicates the direction to move
    verti failed_lifts_;        //!< number of consecutive failed lift attempts
};


class LinearLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    LinearLiftingStrategyFactory(bool backward = false)
        : backward_(backward) { };

    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm );

private:
    const bool backward_;
};

#endif /* ndef LINEAR_LIFTING_STRATEGY_H_INCLUDED */
