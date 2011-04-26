// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
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
    LinearLiftingStrategy(const ParityGame &game, bool backward, bool alternate);
    void lifted(verti v);
    verti next();

    bool backward() const { return backward_; }
    bool alternate() const { return alternate_; }

private:
    const bool backward_;       //!< initial direction of iteration
    const bool alternate_;      //!< alternate direction after each pass
    bool dir_;                  //!< current direction of iteration
    verti vertex_;              //!< next vertex to lift
    verti failed_lifts_;        //!< number of consecutive failed lift attempts
    verti max_failed_;          //!< max. failures possible in unsolved game
};


class LinearLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    LinearLiftingStrategyFactory(bool backward = false, bool alternate = false)
        : backward_(backward), alternate_(alternate) { };

    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm );

private:
    const bool backward_, alternate_;
};

#endif /* ndef LINEAR_LIFTING_STRATEGY_H_INCLUDED */
