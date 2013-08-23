// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "DeloopSolver.h"
#include "attractor.h"
#include <memory>
#include <assert.h>

DeloopSolver::DeloopSolver(
    const ParityGame &game, ParityGameSolverFactory &pgsf,
    const verti *vmap, verti vmap_size )
    : ParityGameSolver(game), pgsf_(pgsf), vmap_(vmap), vmap_size_(vmap_size)
{
    pgsf_.ref();
}

DeloopSolver::~DeloopSolver()
{
    pgsf_.deref();
}

ParityGame::Strategy DeloopSolver::solve()
{
    const verti V = game_.graph().V();
    ParityGame::Strategy strategy(V, NO_VERTEX);

    mCRL2log(mcrl2::log::verbose, "DeloopSolver") << "Searching for winning loops..." << std::endl;
    DenseSet<verti> solved(0, V);
    for (int player = 0; player < 2; ++player)
    {
        verti old_solved = (verti)solved.size();
        std::deque<verti> winning;
        for (verti v = 0; v < V; ++v)
        {
            if ( static_cast<int>(game_.priority(v)%2) == player &&
                 game_.graph().outdegree(v) == 1 &&
                 *game_.graph().succ_begin(v) == v )
            {
                assert(solved.count(v) == 0);
                strategy[v] = game_.player(v) == player ? v : NO_VERTEX;
                winning.push_back(v);
                solved.insert(v);
            }
        }

        // Compute attractor set and associated strategy:
        for ( std::deque<verti>::const_iterator it = winning.begin();
                it != winning.end(); ++it ) solved.insert(*it);
        make_attractor_set( game_, (ParityGame::Player)player,
                            solved, winning, strategy );

        verti num_solved = (verti)solved.size() - old_solved;
        mCRL2log(mcrl2::log::verbose, "DeloopSolver") << "Found " << num_solved << " vertices won by " << (player == 0 ? "Even" : "Odd" ) << std::endl;

        update_memory_use( sizeof(strategy[0])*strategy.capacity() +
            solved.memory_use() + sizeof(winning[0])*winning.size()*2  );
    }

    std::vector<verti> unsolved;
    ParityGame subgame;
    std::vector<verti> submap;  // must survive subsolver!
    std::auto_ptr<ParityGameSolver> subsolver;
    ParityGame::Strategy substrat;

    if (solved.empty())
    {
        // Don't construct a subgame if it is identical to the input game:
        mCRL2log(mcrl2::log::verbose, "DeloopSolver") << "Solving game." << std::endl;
        subsolver.reset(pgsf_.create(game_, vmap_, vmap_size_));
        strategy = subsolver->solve();
    }
    else
    if (solved.size() != V)
    {
        const verti num_unsolved = V - (verti)solved.size();
        mCRL2log(mcrl2::log::verbose, "DeloopSolver") << "Creating subgame with " << num_unsolved << " vertices remaining..." << std::endl;

        // Create game with remaining unsolved vertices:
        unsolved.reserve(num_unsolved);
        for (verti v = 0; v < V; ++v)
        {
            if (solved.count(v) == 0) unsolved.push_back(v);
        }
        assert(!unsolved.empty() && unsolved.size() == num_unsolved);

        subgame.make_subgame(game_, unsolved.begin(), unsolved.end());

        // Construct solver:
        if (vmap_size_ > 0)
        {
            // Need to create merged vertex map:
            submap = unsolved;
            merge_vertex_maps(submap.begin(), submap.end(), vmap_, vmap_size_);
            subsolver.reset(pgsf_.create(subgame, &submap[0], submap.size()));
        }
        else
        {
            subsolver.reset(pgsf_.create(subgame, &unsolved[0], unsolved.size()));
        }

        mCRL2log(mcrl2::log::verbose, "DeloopSolver") << "Solving..." << std::endl;
        substrat = subsolver->solve();
        if (!substrat.empty())
        {
            mCRL2log(mcrl2::log::verbose, "DeloopSolver") << "Merging strategies..." << std::endl;
            merge_strategies(strategy, substrat, unsolved);
        }
    }

    update_memory_use(
        sizeof(strategy[0])*strategy.capacity() + solved.memory_use() +
        sizeof(unsolved[0])*unsolved.capacity() + subgame.memory_use() +
        sizeof(submap[0])*submap.capacity() +
        (subsolver.get() ? subsolver->memory_use() : 0) +
        sizeof(substrat[0])*substrat.capacity() );

    return strategy;
}

ParityGameSolver *DeloopSolverFactory::create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size )
{
    return new DeloopSolver(game, pgsf_, vertex_map, vertex_map_size);
}
