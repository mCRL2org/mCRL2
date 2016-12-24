// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "ParityGameSolver.h"
#include <assert.h>

void merge_strategies( std::vector<verti> &strategy,
                       const std::vector<verti> &substrat,
                       const std::vector<verti> &vertex_map )
{
    assert(substrat.size() == vertex_map.size());
    for (verti v = 0; v < (verti)vertex_map.size(); ++v)
    {
        strategy[vertex_map[v]] =
            (substrat[v] == NO_VERTEX) ? NO_VERTEX : vertex_map[substrat[v]];
    }
}
