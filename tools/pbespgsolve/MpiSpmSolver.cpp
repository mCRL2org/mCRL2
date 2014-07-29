// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "MpiSpmSolver.h"
#include <sstream>
#include <algorithm>

class InternalLiftingStrategy : public LiftingStrategy
{
public:
    InternalLiftingStrategy(const GamePart &part, LiftingStrategy *other)
        : part_(part), ls_(other)
    {
    }

    ~InternalLiftingStrategy()
    {
        delete ls_;
    }

    void lifted(verti v)
    {
        ls_->lifted(v);
    }

    verti next()
    {
        verti v;
        while ((v = ls_->next()) != NO_VERTEX)
        {
            if (part_.is_internal(v)) break;
            //Logger::debug("Skipping %d", v);
        }
        //if (v != NO_VERTEX) Logger::debug("Lifting %d", part_.global(v));
        return v;
    }

protected:
    const GamePart &part_;
    LiftingStrategy *ls_;
};

/*
class InternalLiftingStrategyFactory : public LiftingStrategyFactory
{
public:
    InternalLiftingStrategyFactory( const GamePart &part,
                                    LiftingStrategyFactory *other )
        : part_(part), lsf_(other)
    {
        lsf_->ref();
    }

    ~InternalLiftingStrategyFactory()
    {
        lsf_->deref();
    }

    LiftingStrategy *create( const ParityGame &game,
                             const SmallProgressMeasures &spm )
    {
        return new InternalLiftingStrategy(part_, lsf_->create(game, spm));
    }

private:
    const GamePart &part_;
    LiftingStrategyFactory *lsf_;
};
*/

MpiSpmSolver::MpiSpmSolver(
        const ParityGame &game, const VertexPartition *vpart,
        LiftingStrategyFactory *lsf, LiftingStatistics *stats,
        const verti *vertex_map, verti vertex_map_size )
    : ParityGameSolver(game), vpart_(vpart), part_(game, *vpart_, mpi_rank),
      lsf_(lsf), stats_(stats), vmap_(vertex_map), vmap_size_(vertex_map_size)
{
    vpart_->ref();
    lsf_->ref();
}

MpiSpmSolver::~MpiSpmSolver()
{
    vpart_->deref();
    lsf_->deref();
}

void MpiSpmSolver::set_vector_space(SmallProgressMeasures &spm)
{
    std::vector<verti> local(spm.len(), 1);
    const ParityGame &game = part_.game();
    for ( GamePart::const_iterator it = part_.begin();
          it != part_.end(); ++it )
    {
        verti v = *it;
        if (!spm.is_top(v))
        {
            int prio = game.priority(v);
            if (prio%2 != spm.player()) ++local[prio/2];
        }
    }
    std::vector<verti> global(spm.len());
    MPI::COMM_WORLD.Allreduce( &local[0], &global[0], spm.len(),
                               MPI_INT, MPI_SUM );
    spm.set_M(&global[0]);
}

void MpiSpmSolver::update( SmallProgressMeasures &spm,
                           LiftingStrategy &ls,
                           verti global_v, const verti vec[] )
{
    verti v = part_.local(global_v);
    //debug("Received vertex %d (top %d)", global_v, spm.is_top(vec));
    if (v == NO_VERTEX)
    {
        // Opponent-controlled non-local vertex lifted to top:
        int prio = game_.priority(global_v);
        assert(spm.is_top(vec) && prio%2 != spm.player());
        spm.decr_M(prio/2);
    }
    else
    {
        // Local external vertex updated:
        bool res = spm.lift_to(v, vec);
        if (res)
        {
            /* Usually, res == true, since we only receive updates on vertices
               when they are actually lifted.  However, due to the vector-space-
               reduction-after-lifting-to-top optimization, it is possible that
               we receive an out-of-bounds value and later an in-bounds value
               that is not actually greater. */

            ls.lifted(v);
        }
    }
}

void MpiSpmSolver::solve_all(SmallProgressMeasures &spm)
{
    assert(sizeof(int) == sizeof(verti));
    assert(&spm.game() == &part_.game());

    const StaticGraph &global_graph = game_.graph();
    std::vector<verti> data_in(1 + spm.len());
    std::vector<verti> data_out(1 + spm.len());

    MpiTermination term((int)data_in.size(), MPI_INT, &data_in[0]);
    std::auto_ptr<LiftingStrategy> ls(
        new InternalLiftingStrategy(part_, lsf_->create(spm.game(), spm)));

    for (;;)
    {
        // Lift a vertex, if possible:
        std::pair<verti, bool> lift_result = spm.solve_one(*ls);
        verti v = lift_result.first;

        if (v == NO_VERTEX)  // no work remains locally
        {
            // Go into idle mode:
            //debug("Idle");
            term.idle();
            if (!term.recv()) break;
            update(spm, *ls, (verti)data_in[0], (const verti*)&data_in[1]);
            term.start();
            continue;
        }

#ifdef DEBUG
        if (lift_result.second)
        {
            //std::ostringstream oss;
            //if (spm.is_top(spm.vec(v))) oss << " T"; else
            //for (int i = 0; i < spm.len(v); ++i) oss << ' ' << spm.vec(v)[i];
            //debug("Vertex %d lifted to%s", part_.global(v), oss.str().c_str());
        }
        else
        {
            //debug("Vertex %d not lifted", part_.global(v));
        }
#endif

        if (lift_result.second)  // lifting succeeded
        {
            // Notify interested other processes
            std::vector<int> procs;
            verti global_v = part_.global(v);
            if (spm.is_top(v) && part_.game().priority(v)%2 != spm.player())
            {
                /* If vertex was lifted to top and controlled by the opponent,
                   we should send it to everyone, so the vector space can be
                   decreased in all worker processes. */
                procs.reserve(mpi_size - 1);
                for (int i = 0; i < mpi_size; ++i)
                {
                    if (i != mpi_rank) procs.push_back(i);
                }
            }
            else
            {
                /* Send to processes that manage a predecessor vertex: */
                for ( StaticGraph::const_iterator it =
                        global_graph.pred_begin(global_v);
                    it != global_graph.pred_end(global_v); ++it )
                {
                    int proc = (*vpart_)(*it);
                    if (proc != mpi_rank) procs.push_back(proc);
                }
            }
            if (!procs.empty())
            {
                // Build data packet
                const verti *vec = spm.vec(v);
                data_out[0] = global_v;
                std::copy(vec, vec + spm.len(), &data_out[1]);
                //debug("Sending vertex %d (top %d)", global_v, spm.is_top(vec));

                // Remove duplicates
                std::sort(procs.begin(), procs.end());
                procs.erase( std::unique(procs.begin(), procs.end()),
                            procs.end() );

                // Send to all relevant processes
                for ( std::vector<int>::const_iterator it = procs.begin();
                        it != procs.end(); ++it )
                {
                    term.send(&data_out[0], (int)data_out.size(), *it);
                }
            }
        }

        // Receive all available updates:
        while (term.test())
        {
            update(spm, *ls, (verti)data_in[0], (const verti*)&data_in[1]);
            term.start();
        }
    }
}

void MpiSpmSolver::propagate_solved( SmallProgressMeasures &src,
                                     SmallProgressMeasures &dst )
{
    assert(src.player() != dst.player());
    for (verti v = 0; v < part_.total_size(); ++v)
    {
        if (!src.is_top(v)) dst.lift_to_top(v);
    }
}

ParityGame::Strategy MpiSpmSolver::combine_strategies(
    ParityGame::Strategy &local_strategy )
{
    ParityGame::Strategy result;
    const verti V = game_.graph().V();
    if (mpi_rank == 0)
    {
        info("Merging strategy...");
        result.resize(V, NO_VERTEX - 1);
        for (verti v = 0; v < V; ++v)
        {
            int p = (*vpart_)(v);
            if (p == 0)
            {
                verti w = part_.local(v);
                assert(w != NO_VERTEX);
                verti res = local_strategy[w];
                if (res != NO_VERTEX) res = part_.global(res);
                result[v] = res;
            }
            else
            {
                MPI::COMM_WORLD.Recv(&result[v], 1, MPI_INT, p, 0);
            }
        }
    }
    else
    {
        for (verti v = 0; v < V; ++v)
        {
            if ((*vpart_)(v) == mpi_rank)
            {
                verti w = part_.local(v);
                if (w != NO_VERTEX)
                {
                    verti out = local_strategy[w];
                    if (out != NO_VERTEX) out = part_.global(out);
                    MPI::COMM_WORLD.Send(&out, 1, MPI_INT, 0, 0);
                }
            }
        }
    }
    return result;
}

ParityGame::Strategy MpiSpmSolver::solve()
{
    assert(sizeof(verti) == sizeof(int));

    info( "Game part size: %d internal + %d external = %d local",
           (int)part_.internal_size(),
           (int)part_.external_size(),
           (int)part_.total_size() );

    // Create a local statistics object, but only if required globally:
    std::auto_ptr<LiftingStatistics> stats;
    if (stats_) stats.reset(new LiftingStatistics(part_.game()));

    // Create two SPM instances (one for each player):
    std::auto_ptr<SmallProgressMeasures> spm[2];
    {
        /* NOTE: DenseSPM initializes vertices with just a beneficial loop to
           Top, so the initial game in each process must be preprocessed in the
           same way, and the loops on external vertices must be kept, or the
           programs will be out of sync!

           This is really ugly, and could be fixed by removing the preprocessing
           code from DenseSPM and instead always use DeloopSolver/DecycleSolver
           to remove loops from the game before solving.
        */
        spm[0].reset( new DenseSPM( part_.game(), ParityGame::PLAYER_EVEN,
                                    stats.get(), NULL, 0 ) );
        spm[1].reset( new DenseSPM( part_.game(), ParityGame::PLAYER_ODD,
                                    stats.get(), NULL, 0 ) );
    }

    // Solve the two games, one after the other:
    info("Initializing vector space...");
    set_vector_space(*spm[0]);
    info("Solving game for Even...");
    solve_all(*spm[0]);
#ifdef DEBUG
    debug_print(*spm[0]);
#endif
    info("Propagating winning set to dual game...");
    propagate_solved(*spm[0], *spm[1]);
    info("Initializing vector space...");
    set_vector_space(*spm[1]);
    info("Solving dual game for Odd...");
    solve_all(*spm[1]);
#ifdef DEBUG
    debug_print(*spm[1]);
#endif
    info("Extracting local strategy...");
    const ParityGame &game = part_.game();
    ParityGame::Strategy strategy(game.graph().V(), NO_VERTEX);
    for ( GamePart::const_iterator it = part_.begin();
          it != part_.end(); ++it )
    {
        strategy[*it] = spm[game.player(*it)]->get_strategy(*it);
    }

    // Combine lifting statistics
    if (stats_)
    {
        const verti V = game_.graph().V();  // N.B. GLOBAL graph size!
        if (mpi_rank == 0)
        {
            info("Merging lifting statistics...");
            // Receive total lift statistics from other workers:
            long long attempted = stats->lifts_attempted();
            long long succeeded = stats->lifts_succeeded();
            long long as[2];
            for (int i = 1; i < mpi_size; ++i)
            {
                MPI::COMM_WORLD.Recv(as, 2, MPI_LONG_LONG, i, 0);
                attempted += as[0];
                succeeded += as[1];
            }
            stats_->add_lifts_attempted(attempted);
            stats_->add_lifts_succeeded(succeeded);

            // Receive per-vertex lifting statistics:
            for (verti v = 0; v < V; ++v)
            {
                int p = (*vpart_)(v);
                if (p == 0)
                {
                    verti w = part_.local(v);
                    assert(w != NO_VERTEX);
                    as[0] = stats->lifts_attempted(w);
                    as[1] = stats->lifts_succeeded(w);
                }
                else
                {
                    MPI::COMM_WORLD.Recv(as, 2, MPI_LONG_LONG, p, 0);
                }

                // Assign statistics, with respect to vertec mapping:
                verti u = v;
                if (vmap_ && v < vmap_size_) u = vmap_[u];
                stats_->add_lifts_attempted(u, as[0]);
                stats_->add_lifts_succeeded(u, as[1]);
            }
        }
        else
        {
            // Send total lift statistics to root:
            long long as[2] = { stats->lifts_attempted(),
                                stats->lifts_succeeded() };
            MPI::COMM_WORLD.Send(as, 2, MPI_LONG_LONG, 0, 0);

            // Send per-vertex lifting statistics:
            for (verti v = 0; v < V; ++v)
            {
                if ((*vpart_)(v) == mpi_rank)
                {
                    verti w = part_.local(v);
                    assert(w != NO_VERTEX);
                    as[0] = stats->lifts_attempted(w);
                    as[1] = stats->lifts_attempted(w);
                    MPI::COMM_WORLD.Send(as, 2, MPI_LONG_LONG, 0, 0);
                }
            }
        }
    }

    return combine_strategies(strategy);
}

void MpiSpmSolver::debug_print(const SmallProgressMeasures &spm) const
{
    for (verti v = 0; v < part_.total_size(); ++v)
    {
        std::ostringstream oss;
        if (spm.is_top(v)) oss << " T"; else
        for (int i = 0; i < spm.len(v); ++i) oss << ' ' << spm.vec(v)[i];
        debug("%d:%s", (int)part_.global(v), oss.str().c_str());
    }
}

MpiSpmSolverFactory::MpiSpmSolverFactory( LiftingStrategyFactory *lsf,
        const VertexPartition *vpart, LiftingStatistics *stats )
    : lsf_(lsf), vpart_(vpart), stats_(stats)
{
    vpart_->ref();
    lsf_->ref();
}

MpiSpmSolverFactory::~MpiSpmSolverFactory()
{
    lsf_->deref();
    vpart_->deref();
}

ParityGameSolver *MpiSpmSolverFactory::create( const ParityGame &game,
    const verti *vertex_map, verti vertex_map_size )
{
    return new MpiSpmSolver( game, vpart_, lsf_, stats_,
                             vertex_map, vertex_map_size );
}
