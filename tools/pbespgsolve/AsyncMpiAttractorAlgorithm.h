// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASYNC_MPI_ATTRACTOR_ALGORITHM_H_INCLUDED
#define ASYNC_MPI_ATTRACTOR_ALGORITHM_H_INCLUDED

#include "MpiAttractorAlgorithm.h"
#include "mcrl2/utilities/logger.h"

/*! Attractor set computation implementation that runs asynchronously; i.e. all
   worker processes send and receive vertices to be added to the set while they
   are running independent breadth-first search over their local vertex set.
   This is should reduce latency. */
class AsyncMpiAttractorImpl : public virtual Logger,
                              public MpiTermination
{
public:
    AsyncMpiAttractorImpl( const VertexPartition &vpart,
                           const GamePartition &part, ParityGame::Player player,
                           DenseSet<verti> &attr, std::deque<verti> &queue,
                           ParityGame::Strategy &strategy );

    ~AsyncMpiAttractorImpl();

private:
    friend class AsyncMpiAttractorAlgorithm;

    void solve(bool quick_start);

    /*! Helper function for make_attractor_set() that transmits `v' to relevant
        other processes, and then receives any pending vertices from other
        processes, which are then added to `queue' and `attr'. When messages
        are sent or received, `num_send' and `num_recv' are incremented. */
    void notify_others(verti v);

private:
    /* `vpart' and 'part' describe the vertex partition and corresponding game
        partition for the local process. */    
    const VertexPartition     &vpart_;            //!< current vertex partition
    const GamePartition       &part;          //!< corresponding game partition

    /* The attractor set is computed for the given target player, starting from
       a given set of vertices in `queue' (and also set in `attr'). After,
       completion `attr' will contain the vertices in the attractor set
       intersected with the local vertex set, and `strategy' will be updated
       with a valid strategy for the vertices added (note that this is a
       strategy for the current partition only). */
    const ParityGame::Player  player;                        //!< target player
    DenseSet<verti>           &attr;       //!< initial and final attractor set
    std::deque<verti>         &queue;            //!< queue of initial vertices
    ParityGame::Strategy      &strategy_;               //!< resulting strategy

    /* When an external vertex is added to the attractor set, its index is
       received in `vertex_val': */
    verti vertex_val;        //!< temporary buffer to receive external vertices
};

class AsyncMpiAttractorAlgorithm : public MpiAttractorAlgorithm
{
    void make_attractor_set( const VertexPartition &vpart,
        const GamePartition &part, ParityGame::Player player,
        DenseSet<verti> &attr, std::deque<verti> &queue,
        bool quick_start, ParityGame::Strategy &strategy )
    {
        /* Logger::debug( "enter make_attractor_set(%s, %d, %d) in %s",
            part.debug_str(attr).c_str(), (int)player, (int)quick_start,
            part.debug_str().c_str() ); */
        AsyncMpiAttractorImpl impl(vpart, part, player, attr, queue, strategy);
        impl.solve(quick_start);
        // Logger::debug("return -> %s", part.debug_str(attr).c_str());
    }
};

#endif // ndef ASYNC_MPI_ATTRACTOR_ALGORITHM_H_INCLUDED
