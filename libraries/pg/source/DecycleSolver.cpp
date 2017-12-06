// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/pg/DecycleSolver.h"
#include "mcrl2/pg/attractor.h"
#include <memory>
#include <cassert>

/*! This helper class searches for cycles of a fixed priority in subgames
    controlled entirely by the corresponding player. */
struct CycleFinder
{
    /*! Construct an instance for the subgame of `game` induced by `mapping`,
        looking for cycles of dominant priority `prio`. */
    CycleFinder( const ParityGame &game, std::size_t prio,
                 const std::vector<verti> &mapping );

    /*! Search for minimum-priority cycles and vertices in their attractor sets,
        and update `strategy`, `done_set` and `done_queue` accordingly.
        Takes O(E) time. */
    void run( ParityGame::Strategy &strategy,
              DenseSet<verti> &done_set, std::deque<verti> &done_queue );

    // SCC callback
    int operator()(const verti *scc, std::size_t scc_size);

private:
    priority_t                  prio_;          //!< selected priority
    const std::vector<verti>    &mapping_;      //!< priority induced vertex set
    ParityGame                  subgame_;       //!< priority induced subgame
    DenseSet<verti>             winning_set_;   //!< winning set of the subgame
    std::deque<verti>           winning_queue_; //!< queue of winning vertices
    ParityGame::Strategy        substrat_;      //!< current winning strategy
};

CycleFinder::CycleFinder( const ParityGame &game,
                          std::size_t prio, const std::vector<verti> &mapping )
    : prio_(prio), mapping_(mapping), winning_set_(0, (verti)mapping.size()),
      winning_queue_(), substrat_(mapping.size(), NO_VERTEX)
{
    subgame_.make_subgame(game, mapping.begin(), mapping.end(), false);
}

void CycleFinder::run( ParityGame::Strategy &strategy,
    DenseSet<verti> &done_set, std::deque<verti> &done_queue )
{
    // Identify key vertices which are part of the winning set:
    decompose_graph(subgame_.graph(), *this);

    if (!winning_queue_.empty())
    {
        // Extend to attractor in subgame. This guarantees the strategy indeed
        // leads to cycles of priority prio_:
        make_attractor_set( subgame_, (ParityGame::Player)(prio_%2),
                            winning_set_, winning_queue_, substrat_ );

        // Map computed winning set and strategy back to global game:
        for ( DenseSet<verti>::const_iterator it = winning_set_.begin();
              it != winning_set_.end(); ++it )
        {
            verti v = mapping_[*it];
            verti w = substrat_[*it];
            if (w != NO_VERTEX) w = mapping_[w];
            strategy[v] = w;
            assert(!done_set.count(v));
            done_set.insert(v);
            done_queue.push_back(v);
        }
    }
}

int CycleFinder::operator()(const verti *scc, std::size_t scc_size)
{
    // Search for a vertex with minimum priority, with a successor in the SCC:
    for (std::size_t i = 0; i < scc_size; ++i)
    {
        verti v = scc[i];
        if (subgame_.priority(v) == prio_)
        {
            // Search for an edge inside the component:
            // FIXME: complexity analysis? has_succ is not constant time!
            for (std::size_t j = 0; j < scc_size; ++j)
            {
                verti w = scc[j];
                if (subgame_.graph().has_succ(v, w))
                {
                    if (subgame_.player(v) == static_cast<int>(prio_%2))
                    {
                        substrat_[v] = w;
                    }
                    winning_set_.insert(v);
                    winning_queue_.push_back(v);
                    return 0;  // continue enumerating SCCs
                }
            }
            assert(scc_size == 1);
        }
    }
    return 0;  // continue enumerating SCCs
}

DecycleSolver::DecycleSolver(
    const ParityGame &game, ParityGameSolverFactory &pgsf,
    const verti *vmap, verti vmap_size )
    : ParityGameSolver(game), pgsf_(pgsf), vmap_(vmap), vmap_size_(vmap_size)
{
    pgsf_.ref();
}

DecycleSolver::~DecycleSolver()
{
    pgsf_.deref();
}

ParityGame::Strategy DecycleSolver::solve()
{
    mCRL2log(mcrl2::log::verbose, "DecycleSolver") << "Searching for winner-controlled cycles..." << std::endl;

    const verti V = game_.graph().V();
    ParityGame::Strategy strategy(V, NO_VERTEX);
    DenseSet<verti> solved_set(0, V);

    // Find owner-controlled cycles for every priority value:
    for (priority_t prio = 0; prio < game_.d(); ++prio)
    {
        verti old_size = solved_set.size();

        // Find set of unsolved vertices with priority >= prio
        std::vector<verti> mapping;
        for (verti v = 0; v < V; ++v)
        {
            if ( solved_set.count(v) == 0 &&
                 game_.priority(v) >= prio &&
                 ( game_.player(v) == static_cast<int>(prio%2) ||
                   game_.graph().outdegree(v) == 1 ) )
            {
                mapping.push_back(v);
            }
        }

        // Find (attractor set of) winning cycles in subgame:
        std::deque<verti> solved_queue;
        CycleFinder cf(game_, prio, mapping);
        cf.run(strategy, solved_set, solved_queue);

        // Extend to attractor set in the global game:
        make_attractor_set( game_, (ParityGame::Player)(prio%2),
                            solved_set, solved_queue, strategy );

        verti new_size = solved_set.size();
        if (old_size < new_size)
        {
            mCRL2log(mcrl2::log::verbose, "DecycleSolver") << "Identified " << new_size - old_size
                                                           << " vertices in " << prio << "-dominated cycles" << std::endl;
        }

        // Early out: if all vertices are solved, it is pointless to continue.
        if (new_size == V) return strategy;
    }

    if (solved_set.empty())
    {
        // Don't construct a subgame if it is identical to the input game:
        mCRL2log(mcrl2::log::verbose, "DecycleSolver") << "No suitable cycles found! Solving..." << std::endl;
        std::unique_ptr<ParityGameSolver> subsolver(
            pgsf_.create(game_, vmap_, vmap_size_) );
        subsolver->solve().swap(strategy);
        return strategy;
    }

    const verti num_unsolved = V - (verti)solved_set.size();
    mCRL2log(mcrl2::log::verbose, "DecycleSolver") << "Creating subgame with " << num_unsolved
                                                   << " vertices remaining..." << std::endl;

    // Gather remaining unsolved vertices:
    std::vector<verti> unsolved;
    unsolved.reserve(num_unsolved);
    for (verti v = 0; v < V; ++v)
    {
        if (!solved_set.count(v)) unsolved.push_back(v);
    }
    assert(!unsolved.empty() && unsolved.size() == num_unsolved);

    // Construct subgame for the unsolved part:
    ParityGame subgame;
    subgame.make_subgame(game_, unsolved.begin(), unsolved.end(), true);

    // Construct solver:
    std::vector<verti> submap;  // declared here so it survives subsolver
    std::unique_ptr<ParityGameSolver> subsolver;
    if (vmap_size_ > 0)
    {
        // Need to create merged vertex map:
        submap = unsolved;
        merge_vertex_maps(submap.begin(), submap.end(), vmap_, vmap_size_);
        subsolver.reset(
            pgsf_.create(subgame, &submap[0], submap.size()) );
    }
    else
    {
        subsolver.reset(
            pgsf_.create(subgame, &unsolved[0], unsolved.size()) );
    }

    mCRL2log(mcrl2::log::verbose, "DecycleSolver") << "Solving..." << std::endl;
    ParityGame::Strategy substrat = subsolver->solve();
    if (!substrat.empty())
    {
        mCRL2log(mcrl2::log::verbose, "DecycleSolver") << "Merging strategies..." << std::endl;
        merge_strategies(strategy, substrat, unsolved);
    }

    return strategy;
}

ParityGameSolver *DecycleSolverFactory::create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size )
{
    return new DecycleSolver(game, pgsf_, vertex_map, vertex_map_size);
}
