// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
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

/*! \ingroup LiftingStrategies

    This is an implementation of the "Focus List Approach" described in the
    Multi-Core Solver for Parity Games paper.

    It alternates between two phases. In the first phase, it iterates over the
    vertices linearly and puts all vertices that are lifted on a focus list with
    a starting credit. The implementation is delegated to a
    LinearLiftingStrategy instance.

    In the second phase, each node on the focus list is attempted to be lifted;
    if lifting succeeds, the credit for this node is increased linearly, and if
    fails the credit is decreased exponentially. This phase is repeated until the
    focus list is empty or a maximum number of lifting attempts has been
    reached (whichever occurs first).  Then, the algorithm switches back to
    phase 1 to create a new focus list.

    The focus list has a fixed maximum size.  When this size is reached during
    phase 1, the algorithm switches to phase 2.

    \see LinearLiftingStrategy
*/


class FocusListLiftingStrategy : public LiftingStrategy
{
public:
    /*! Constructor.

        \param alternate    passed to the LinearLiftingStrategy constructor.
        \param max_size     maximum size of the focus list.
        \param max_lifts    maximum number of lifts per second phase.
    */
    FocusListLiftingStrategy( const ParityGame &game, bool alternate,
                              verti max_size, size_t max_lifts );

    void lifted(verti vertex);
    verti next();

    bool max_size() const { return 0 != focus_list_.capacity(); }

protected:
    verti phase1();
    verti phase2();

private:
    typedef std::vector<std::pair<verti, unsigned> > focus_list;

    const verti V_;                     //!< game graph vertex count

    size_t max_lift_attempts_;       //!< maximum lift attempts per list
    int phase_;                         //!< current phase
    size_t num_lift_attempts_;       //!< number of consecutive lift attempts
    bool prev_lifted_;                  //!< whether previous vertex was lifted

    // For phase 1:
    LinearLiftingStrategy lls_;         //!< strategy for phase 1

    // For phase 2:
    focus_list focus_list_;             //!< nodes on the focus list
    focus_list::iterator read_pos_;     //!< current position in the focus list
    focus_list::iterator write_pos_;    //!< current position in the focus list
};

/*! \ingroup LiftingStrategies
    Factory class for FocusListLiftingStrategy instances. */
class FocusListLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    /*! Constructor.

        \param alternate    passed to the LinearLiftingStrategy constructor.
        \param size_ratio   determines the maximum size of the focus list.
            If greater than 1, it is used as the focus list size. If between 0
            and 1 (inclusive) it is interpreted as a fraction of the game size.
            If 0, a default fraction of 10% is used.
        \param lift_ratio   determines the maximum number of lifting attempts
            in the second phase, as a multiple of the focus list size. If 0,
            a default value of 10 is used.

        \see FocusListLiftingStrategy::FocusListLiftingStrategy()
    */
    FocusListLiftingStrategyFactory(
            bool alternate, double size_ratio = 0, double lift_ratio = 0 )
        : alternate_(alternate),
          size_ratio_(size_ratio > 0 ? size_ratio :  0.1),
          lift_ratio_(lift_ratio > 0 ? lift_ratio : 10.0) { };

    //! Return a new FocusListLiftingStrategy instance.
    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm );

private:
    const bool   alternate_;
    const double size_ratio_, lift_ratio_;
};

#endif /* ndef FOCUS_LIST_LIFTING_STRATEGY_H_INCLUDED */
