// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "AsyncMpiAttractorAlgorithm.h"
#include "attractor.h"  // for is_subset_of()

AsyncMpiAttractorImpl::AsyncMpiAttractorImpl( const VertexPartition &vpart,
        const GamePart &part, ParityGame::Player player,
        DenseSet<verti> &attr, std::deque<verti> &queue,
        ParityGame::Strategy &strategy )
    : MpiTermination(1, MPI_INT, &vertex_val), vpart_(vpart), part(part),
      player(player), attr(attr), queue(queue), strategy_(strategy)
{
}

AsyncMpiAttractorImpl::~AsyncMpiAttractorImpl()
{
}

void AsyncMpiAttractorImpl::solve(bool quick_start)
{
    const StaticGraph &graph = part.game().graph();

    // When quick starting, processes are aware of each other's initial vertex
    // sets. If not, then we must first transmit the contents of the initial
    // set to the relevant other processes.
    if (!quick_start)
    {
        // N.B. queue.size() cannot be calculated inside the loop, because
        //      notify_others may push new, non-local vertices in the queue!
        size_t n = queue.size();
        for (size_t i = 0; i < n; ++i) notify_others(queue[i]);
    }

    for (;;)
    {
        // Active: process queued vertices
        //debug("active");
        while (!queue.empty())
        {
            const verti w = queue.front();
            queue.pop_front();
            for ( StaticGraph::const_iterator it = graph.pred_begin(w);
                  it != graph.pred_end(w); ++it )
            {
                const verti v = *it;

                // Skip vertices already in the attractor set:
                if (attr.count(v)) continue;

                // Skip vertices not assigned to this worker process:
                // FIXME: maybe use part.is_internal() instead?
                if (vpart_(part.global(v)) != mpi_rank) continue;

                if (part.game().player(v) == player)
                {
                    // Store strategy for player-controlled vertex:
                    strategy_[part.global(v)] = part.global(w);
                }
                else  // opponent-controlled vertex
                if (is_subset_of(graph.succ_begin(v), graph.succ_end(v), attr))
                {
                    // Store strategy for opponent-controlled vertex:
                    strategy_[part.global(v)] = NO_VERTEX;
                }
                else
                {
                    // Not in the attractor set yet!
                    continue;
                }
                // Add vertex v to the attractor set:
                attr.insert(v);
                queue.push_back(v);
                //debug("added %d to attractor set", part.global(v));
                notify_others(v);
            }
        }

        idle();
        if (recv())
        {
            //debug("received %d (a)", vertex_val);
            verti i = part.local(vertex_val);
            start();
            assert(!attr.count(i));
            attr.insert(i);
            queue.push_back(i);
        }
        else
        {
            // Global termination detected!
            break;
        }
    }
}

void AsyncMpiAttractorImpl::notify_others(verti i)
{
    const StaticGraph &graph = part.game().graph();
    verti v = part.global(i);
    std::vector<bool> recipients(mpi_size);
    for ( StaticGraph::const_iterator it = graph.pred_begin(i);
            it != graph.pred_end(i); ++it )
    {
        recipients[vpart_(part.global(*it))] = true;
    }
    for ( StaticGraph::const_iterator it = graph.succ_begin(i);
            it != graph.succ_end(i); ++it )
    {
        recipients[vpart_(part.global(*it))] = true;
    }
    for (int dest = 0; dest < mpi_size; ++dest)
    {
        if (recipients[dest] && dest != mpi_rank)
        {
            //debug("sending %d to %d", v, dest);
            send(&v, 1, dest);
        }
    }

    // Receive pending vertex updates:
    while (test())
    {
        //debug("received %d (b)", vertex_val);
        i = part.local(vertex_val);
        assert(!attr.count(i));
        attr.insert(i);
        queue.push_back(i);
    }
}
