// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "LinearLiftingStrategy.h"

LinearLiftingStrategy::LinearLiftingStrategy(
    const ParityGame &game, bool backward )
    : LiftingStrategy(game), backward_(backward), failed_lifts_(0)
{
}

verti LinearLiftingStrategy::next(verti prev_vertex, bool prev_lifted)
{
    const verti last_vertex = graph_.V() - 1;

    if (prev_vertex == NO_VERTEX)
    {
        /* First vertex; pick either first or last depending on direction. */
        return backward_ ? last_vertex : 0;
    }

    if (prev_lifted)
    {
        failed_lifts_ = 0;
    }
    else
    {
        failed_lifts_ += 1;
        if (failed_lifts_ == graph_.V()) return NO_VERTEX;
    }

    if (backward_)
    {
        return prev_vertex == 0 ? last_vertex : prev_vertex - 1;
    }
    else
    {
        return prev_vertex == last_vertex ? 0 : prev_vertex + 1;
    }
}

LiftingStrategy *LinearLiftingStrategyFactory::create(
    const ParityGame &game, const SmallProgressMeasures &spm )
{
    (void)spm;  // unused
    return new LinearLiftingStrategy(game, backward_);
}
