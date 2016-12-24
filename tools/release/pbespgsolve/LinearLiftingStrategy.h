// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LINEAR_LIFTING_STRATEGY_H_INCLUDED
#define LINEAR_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"

/*! \ingroup LiftingStrategies

    A simple lifting strategy that attempts to lift vertices in order and stops
    when no more vertices can be lifted.

    The Multi-Core Solver for Parity Games paper calls this "Swiping", but
    only examines forward, non-reversing order.
*/

class LinearLiftingStrategy : public LiftingStrategy
{
public:
    LinearLiftingStrategy(const ParityGame &game, bool alternate);
    void lifted(verti v);
    verti next();

    bool alternate() const { return alternate_; }

private:
    const bool alternate_;      //!< alternate direction after each pass
    const verti last_vertex_;   //!< last vertex index
    bool dir_;                  //!< current direction of iteration
    verti vertex_;              //!< next vertex to lift
    verti failed_lifts_;        //!< number of consecutive failed lift attempts
};

/*! \ingroup LiftingStrategies
    A factory class for LinearLiftingStrategy instances. */
class LinearLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    LinearLiftingStrategyFactory(bool alternate = false)
        : alternate_(alternate) { };

    //! Return a new LinearLiftingStrategy instance.
    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm );

private:
    const bool alternate_;
};

#endif /* ndef LINEAR_LIFTING_STRATEGY_H_INCLUDED */
