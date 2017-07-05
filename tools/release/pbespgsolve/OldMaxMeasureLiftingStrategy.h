// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef OLD_MAX_MEASURE_LIFTING_STRATEGY_H_INCLUDED
#define OLD_MAX_MEASURE_LIFTING_STRATEGY_H_INCLUDED

#include "SmallProgressMeasures.h"
#include <vector>
#include <set>
#include <utility>

/*! \ingroup LiftingStrategies

    Old implementation of MaxMeasureLiftingStrategy.

    This is strategy is rather inefficient (in time and space) because it stores
    both vertex indices and a copy of the highest successor's progress measure
    in a std::set.

    This class is basically obsolete, but retained in order to be able to test
    for regressions.
*/
class OldMaxMeasureLiftingStrategy : public LiftingStrategy
{
public:
    OldMaxMeasureLiftingStrategy( const ParityGame &game,
                                  const SmallProgressMeasures &spm );
    ~OldMaxMeasureLiftingStrategy();

    void lifted(verti v);
    verti next();

protected:
    std::vector<verti> vec(verti v) {
        return std::vector<verti>(spm_.vec(v), spm_.vec(v) + spm_.len(v));
    }

private:
    OldMaxMeasureLiftingStrategy(const OldMaxMeasureLiftingStrategy &);
    OldMaxMeasureLiftingStrategy operator=(const OldMaxMeasureLiftingStrategy &);

private:
    const SmallProgressMeasures &spm_;
    typedef std::set<std::pair<std::vector<verti>, verti> > queue_t;
    queue_t queue_;
    std::vector<queue_t::iterator> queue_pos_;
};

/*! \ingroup LiftingStrategies
    A factory class for OldMaxMeasureLiftingStrategy instances. */
class OldMaxMeasureLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm );
};

#endif /* ndef OLD_PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED */
