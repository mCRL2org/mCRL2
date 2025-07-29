// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_PG_PREDECESSOR_LIFTING_STRATEGY_H
#define MCRL2_PG_PREDECESSOR_LIFTING_STRATEGY_H

#include "mcrl2/pg/SmallProgressMeasures.h"

#include <memory>

/*! \ingroup LiftingStrategies
 
    A simple lifting strategy that puts all nodes in a queue, then takes them
    out one at a time; whenever a node is successfully lifted, its predecessors
    are put back in the queue as they may need to be lifted too.

    This strategy requires predecessor edges to be stored in the game graph.

    The queue can operate as a true queue or as a stack; the latter may result
    in better locality of reference and/or fewer unsuccessful lifting attempts.
    (This has not been tested.)

    (The Multi-Core Solver for Parity Games paper contains a description of
     a "work list approach" that is similar.)
*/

class PredecessorLiftingStrategy : virtual public LiftingStrategy,
                                   virtual public LiftingStrategy2
{
public:
    /*! Construct a new predecessor lifting strategy instance.

        If `stack` is set to true, vertices are removed in last-in-first-out
        order (instead of the default first-in-first-out order).
    */
    PredecessorLiftingStrategy(
        const ParityGame &game, const SmallProgressMeasures &spm, bool stack,
        int version );

    bool stack() const { return stack_; }

    // v1 API
    void lifted(verti v) override;
    verti next() override;

    // v2 API
    void push(verti v) override;
    verti pop() override;
    void bump(verti /*vertex*/) override {}

  private:
    const SmallProgressMeasures &spm_;
    const bool stack_;
    std::unique_ptr<bool[]> queued_;
    std::unique_ptr<verti[]> queue_;
    std::size_t queue_size_, queue_capacity_, queue_begin_, queue_end_;
};

/*! \ingroup LiftingStrategies
    Factory class for PredecessorLiftingStrategy instances. */
class PredecessorLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    PredecessorLiftingStrategyFactory(bool stack = false) : stack_(stack) { };

    bool supports_version(int version) override;
    LiftingStrategy* create(const ParityGame& game, const SmallProgressMeasures& spm) override;
    LiftingStrategy2* create2(const ParityGame& game, const SmallProgressMeasures& spm) override;

  private:
    const bool stack_;
};

#endif /* ndef MCRL2_PG_PREDECESSOR_LIFTING_STRATEGY_H */
