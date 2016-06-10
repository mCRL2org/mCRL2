// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASYNC_MPI_ATTRACTOR_ALGORITHM_H_INCLUDED
#define ASYNC_MPI_ATTRACTOR_ALGORITHM_H_INCLUDED

#include "MpiAttractorAlgorithm.h"
#include "mcrl2/utilities/logger.h"

/*! Implementation of the asynchronous MPI-based attractor set computer.

    All worker processes send and receive vertices to be added to the set, while
    independently running a breadth-first search over their local vertex set.
    This should reduce latency.

    @see AsyncMpiAttractorAlgorithm
*/
class AsyncMpiAttractorImpl : public virtual Logger,
                              public MpiTermination
{
public:
    AsyncMpiAttractorImpl( const VertexPartition &vpart,
                           const GamePart &part, ParityGame::Player player,
                           DenseSet<verti> &attr, std::deque<verti> &queue,
                           ParityGame::Strategy &strategy );

    ~AsyncMpiAttractorImpl();

private:
    friend class AsyncMpiAttractorAlgorithm;

    void solve(bool quick_start);

    /*! Helper function for make_attractor_set() that transmits `v` to relevant
        other processes, and then receives any pending vertices from other
        processes, which are then added to `queue` and `attr`. When messages
        are sent or received, `num_send` and `num_recv` are incremented. */
    void notify_others(verti i);

private:
    /* `vpart` and 'part' describe the vertex partition and corresponding game
        partition for the local process. */
    const VertexPartition     &vpart_;            //!< current vertex partition
    const GamePart            &part;          //!< corresponding game partition

    /* The attractor set is computed for the given target player, starting from
       a given set of vertices in `queue` (and also set in `attr`). After,
       completion `attr` will contain the vertices in the attractor set
       intersected with the local vertex set, and `strategy` will be updated
       with a valid strategy for the vertices added (note that this is a
       strategy for the current partition only). */
    const ParityGame::Player  player;                        //!< target player
    DenseSet<verti>           &attr;       //!< initial and final attractor set
    std::deque<verti>         &queue;            //!< queue of initial vertices
    ParityGame::Strategy      &strategy_;               //!< resulting strategy

    /* When an external vertex is added to the attractor set, its index is
       received in `vertex_val`: */
    verti vertex_val;        //!< temporary buffer to receive external vertices
};

/*! Asynchronous MPI-based attractor set computer.

    @see AsyncMpiAttractorImpl
*/
class AsyncMpiAttractorAlgorithm : public MpiAttractorAlgorithm
{
    void make_attractor_set( const VertexPartition &vpart,
        const GamePart &part, ParityGame::Player player,
        DenseSet<verti> &attr, std::deque<verti> &queue,
        bool quick_start, ParityGame::Strategy &strategy )
    {
        mCRL2log(mcrl2::log::debug2) << "enter make_attractor_set("
                                     << part.debug_str(attr) << ", "
                                     << player << ", "
                                     << std::boolalpha << quick_start << " in "
                                     << part.debug_str() << std::endl;
        AsyncMpiAttractorImpl impl(vpart, part, player, attr, queue, strategy);
        impl.solve(quick_start);
        mCRL2log(mcrl2::log::debug2) << "return -> " << part.debug_str(attr) << std::endl;
    }
};

#endif // ndef ASYNC_MPI_ATTRACTOR_ALGORITHM_H_INCLUDED
