// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef FOCUS_LIST_LIFTING_STRATEGY_H_INCLUDED
#define FOCUS_LIST_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"
#include "LinearLiftingStrategy.h"
#include <utility>
#include <vector>

/*! This is an implementation of the "Focus List Approach" described in the
    Multi-Core Solver for Parity Games paper.

    It alternates between two passes. In the first pass, it iterates over the
    vertices linearly (forward or backward) and puts all vertices that are
    lifted on a focus list with a starting credit. In the second pass, nodes on
    the focus list are attempted to be lifted; if lifting succeeds, the credit
    for this node is increased linearly, and if lifting fails it is decreased
    exponentially. This pass is repeated until the focus list is empty.

    The focus list has a fixed maximum size. When this size is reached, the
    algorithm switches to pass 2.

    TODO: improve performance by using a fixed size list instead of a std::list
*/

class FocusListLiftingStrategy : public LiftingStrategy
{
public:
    FocusListLiftingStrategy( const ParityGame &game,
        bool backward, bool alternate, verti max_size, long long max_lifts );
    void lifted(verti vertex);
    verti next();
    size_t memory_use() const;
    bool backward() const { return lls_.backward(); }
    bool max_size() const { return focus_list_.capacity() != 0; }

protected:
    verti phase1();
    verti phase2();

private:
    typedef std::vector<std::pair<verti, unsigned> > focus_list;

    compat_uint64_t max_lift_attempts_;       //!< maximum lift attempts per list
    int phase_;                         //!< current phase
    compat_uint64_t num_lift_attempts_;       //!< number of consecutive lift attempts
    bool prev_lifted_;                  //!< whether previous vertex was lifted

    // For phase 1:
    LinearLiftingStrategy lls_;         //!< strategy for pass 1

    // For phase 2:
    focus_list focus_list_;             //!< nodes on the focus list
    focus_list::iterator read_pos_;     //!< current position in the focus list
    focus_list::iterator write_pos_;    //!< current position in the focus list
};


class FocusListLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    FocusListLiftingStrategyFactory(
        bool backward, bool alternate, double size_ratio, double lift_ratio )
        : backward_(backward), alternate_(alternate),
          size_ratio_(size_ratio > 0 ? size_ratio :  0.1),
          lift_ratio_(lift_ratio > 0 ? lift_ratio : 10.0) { };

    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm );

private:
    const bool   backward_, alternate_;
    const double size_ratio_, lift_ratio_;
};

#endif /* ndef FOCUS_LIST_LIFTING_STRATEGY_H_INCLUDED */
