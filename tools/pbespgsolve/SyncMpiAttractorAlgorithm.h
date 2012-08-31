// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SYNC_MPI_ATTRACTOR_ALGORITHM_H_INCLUDED
#define SYNC_MPI_ATTRACTOR_ALGORITHM_H_INCLUDED

#include "MpiAttractorAlgorithm.h"
#include "mcrl2/utilities/logger.h"

/*! Attractor set computation implementation that runs synchronously; i.e. all
    worker processes compute the attractor set in lockstep, synchronizing after
    adding a layer of vertices to the attractor set.

    In every step the vertices that lie one step further from the initial set
    are computed. The downside of this is that a lot synchronization is done
    when there are vertices in the attractor set that lie far away from the
    closest initial vertex, but the advantage is that termination is easy to
    detect, which is why this algorithm was initially implemented. */
class SyncMpiAttractorImpl : public virtual Logger
{
public:
    SyncMpiAttractorImpl( const VertexPartition &vpart,
                          const GamePartition &part, ParityGame::Player player,
                          DenseSet<verti> &attr, std::deque<verti> &queue,
                          ParityGame::Strategy &strategy );

private:
    friend class SyncMpiAttractorAlgorithm;

    void solve(bool quick_start);

    /*! Exchanges attractor set queues between MPI worker processes.

        For each worker, `queue' contains internal vertices added to `attr'
        last iteration, which are sent to other workers controlling the
        predecessors of these vertices. The vertices received this way are added
        to `next_queue' and set in `attr'. Consequently, only external vertices
        are added to `next_queue'.
    */
    void exchange_queues(std::deque<verti> &next_queue);

private:
    // TODO: document these + add trailing newlines for consistency
    const VertexPartition       &vpart_;
    const GamePartition         &part;
    const ParityGame::Player    player;
    DenseSet<verti>             &attr;
    std::deque<verti>           &queue;
    ParityGame::Strategy        &strategy_;
};

class SyncMpiAttractorAlgorithm : public MpiAttractorAlgorithm
{
    void make_attractor_set( const VertexPartition &vpart,
        const GamePartition &part, ParityGame::Player player,
        DenseSet<verti> &attr, std::deque<verti> &queue,
        bool quick_start, ParityGame::Strategy &strategy )
    {
        /* Logger::debug( "enter make_attractor_set(%s, %d, %d) in %s",
            part.debug_str(attr).c_str(), (int)player, (int)quick_start,
            part.debug_str().c_str() ); */
        SyncMpiAttractorImpl impl(vpart, part, player, attr, queue, strategy);
        impl.solve(quick_start);
        // Logger::debug("return -> %s", part.debug_str(attr).c_str());
    }
};

#endif // ndef SYNC_MPI_ATTRACTOR_ALGORITHM_H_INCLUDED
