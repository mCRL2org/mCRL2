// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MPI_ATTRACTOR_ALGORITHM_H_INCLUDED
#define MPI_ATTRACTOR_ALGORITHM_H_INCLUDED

#include "DenseSet.h"
#include "GamePart.h"
#include "MpiUtils.h"
#include "ParityGame.h"
#include "VertexPartition.h"
#include <deque>
#include <vector>

/*! Base class for MPI-based algorithms to compute attractor sets.

    @see MpiRecursiveSolver
*/
class MpiAttractorAlgorithm
{
public:
    virtual ~MpiAttractorAlgorithm() { }

    /*! Extends the vertices marked in `attr` to the attractor set for `player`
        in the game partition `part`. Initially, `queue` must contain precisely
        those vertices marked in `attr`.

        If `quick_start` is set to true, then `attr` should include all external
        vertices that are in the `attr` in other worker processes. Otherwise,
        `attr` initially only contains internal vertices.

        After returning, the set is extended to the attractor set for `player`
        and includes both internal and external vertices in the attractor set.
    */
    virtual void make_attractor_set( const VertexPartition &vpart,
        const GamePart &part, ParityGame::Player player,
        DenseSet<verti> &attr, std::deque<verti> &queue,
        bool quick_start, ParityGame::Strategy &strategy ) = 0;
};

#endif // ndef MPI_ATTRACTOR_ALGORITHM_H_INCLUDED
