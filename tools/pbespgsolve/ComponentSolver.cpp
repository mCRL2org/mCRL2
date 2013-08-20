// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "ComponentSolver.h"
#include "attractor.h"

#include <assert.h>
#include <memory>

ComponentSolver::ComponentSolver(
    const ParityGame &game, ParityGameSolverFactory &pgsf,
    const verti *vmap, verti vmap_size )
    : ParityGameSolver(game), pgsf_(pgsf), vmap_(vmap), vmap_size_(vmap_size)
{
    pgsf_.ref();
}

ComponentSolver::~ComponentSolver()
{
    pgsf_.deref();
}

ParityGame::Strategy ComponentSolver::solve()
{
    verti V = game_.graph().V();
    strategy_.assign(V, NO_VERTEX);
    DenseSet<verti> W0(0, V), W1(0, V);
    winning_[0] = &W0;
    winning_[1] = &W1;
    if (decompose_graph(game_.graph(), *this) != 0) strategy_.clear();
    winning_[0] = NULL;
    winning_[1] = NULL;
    ParityGame::Strategy result;
    result.swap(strategy_);
    return result;
}

int ComponentSolver::operator()(const verti *vertices, size_t num_vertices)
{
    if (aborted()) return -1;

    assert(num_vertices > 0);

    // Filter out solved vertices:
    std::vector<verti> unsolved;
    unsolved.reserve(num_vertices);
    for (size_t n = 0; n < num_vertices; ++n)
    {
        verti v = vertices[n];
        if (!winning_[0]->count(v) && !winning_[1]->count(v))
        {
            unsolved.push_back(vertices[n]);
        }
    }
    mCRL2log(mcrl2::log::verbose, "ComponentSolver") << "SCC of size " << num_vertices << " with "
                                                     << unsolved.size() << " unsolved vertices..." << std::endl;
    if (unsolved.empty()) return 0;

    // Construct a subgame for unsolved vertices in this component:
    mCRL2log(mcrl2::log::verbose, "ComponentSolver") << "Constructing subgame with " << unsolved.size() << " vertices..." << std::endl;
    ParityGame subgame;
    subgame.make_subgame(game_, unsolved.begin(), unsolved.end());
    //assert(subgame.proper());

    /* N.B. if unsolved.size() < num_vertices then we run the SCC decomposition
       algorithm again (because removing vertices in attractor sets of winning
       regions may cause components to fall apart into distinct SCCs).

       In some (degenerate?) cases this could lead to a lot of work spent doing
       SCC decomposition for relatively little actual solving. To prevent this,
       change the expression in the if-statement below to `true', and then the
       components solved are the strongly-connectected components identified in
       the initial graph (and not new components created after removing vertices
       in attractor sets of winning regions). */

    if (unsolved.size() == num_vertices)
    {
        // Compress vertex priorities
        {
            size_t old_d = subgame.d();
            subgame.compress_priorities();
            size_t new_d = subgame.d();
            if (old_d != new_d)
            {
              mCRL2log(mcrl2::log::verbose, "ComponentSolver") << "Priority compression removed " << old_d - new_d << " of " << old_d << " priorities" << std::endl;
            }
        }

        // Solve the subgame
        mCRL2log(mcrl2::log::verbose, "ComponentSolver")  << "Solving subgame..." << std::endl;
        std::vector<verti> submap;  // declared here so it survives subsolver
        std::auto_ptr<ParityGameSolver> subsolver;
        if (vmap_size_ > 0)
        {
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
        ParityGame::Strategy substrat = subsolver->solve();

        // Update (peak) memory use
        update_memory_use(subgame.memory_use() + subsolver->memory_use());
        if (substrat.empty()) return -1;  // solving failed
        merge_strategies(strategy_, substrat, unsolved);

        mCRL2log(mcrl2::log::verbose, "ComponentSolver") << "Building attractor sets for winning regions" << std::endl;

        // Extract winning sets from subgame:
        std::deque<verti> todo[2];
        for (size_t n = 0; n < unsolved.size(); ++n)
        {
            ParityGame::Player pl = subgame.winner(substrat, n);
            verti v = unsolved[n];
            winning_[pl]->insert(v);
            todo[pl].push_back(v);
        }

        // Extend winning sets to attractor sets:
        for (int player = 0; player < 2; ++player)
        {
            make_attractor_set( game_, (ParityGame::Player)player,
                                *winning_[player], todo[player], strategy_ );
        }
    }
    else  /* unsolved.size() < num_vertices */
    {
        mCRL2log(mcrl2::log::verbose, "ComponentSolver") << "Identifying subcomponents..." << std::endl;
        ComponentSolver subsolver(subgame, pgsf_);
        ParityGame::Strategy substrat = subsolver.solve();
        update_memory_use(subgame.memory_use() + subsolver.memory_use());
        if (substrat.empty()) return -1;
        merge_strategies(strategy_, substrat, unsolved);
    }

    return 0;
}

ParityGameSolver *ComponentSolverFactory::create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size )
{
    return new ComponentSolver(game, pgsf_, vertex_map, vertex_map_size);
}
