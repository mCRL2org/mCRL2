// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
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

class OldMaxMeasureLiftingStrategy : public LiftingStrategy
{
public:
    OldMaxMeasureLiftingStrategy( const ParityGame &game,
                                  const SmallProgressMeasures &spm );
    ~OldMaxMeasureLiftingStrategy();

    void lifted(verti v);
    verti next();
    size_t memory_use() const;

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


class OldMaxMeasureLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm );
};

#endif /* ndef OLD_PREDECESSOR_LIFTING_STRATEGY_H_INCLUDED */
