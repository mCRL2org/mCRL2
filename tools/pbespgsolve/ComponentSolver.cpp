// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "ComponentSolver.h"
#include "logging.h"

#include <assert.h>
#include <memory>

ComponentSolver::ComponentSolver( const ParityGame &game,
                                  const std::string &strategy,
                                  LiftingStatistics *stats )
    : ParityGameSolver(game), strategy_(strategy),
      winners_(game.graph().V(), ParityGame::PLAYER_NONE),
      stats_(stats), memory_used_(0)
{
}

ComponentSolver::~ComponentSolver()
{
}

bool ComponentSolver::solve()
{
    return decompose_graph(game_.graph(), *this) == 0;
}

int ComponentSolver::operator()(const verti *vertices, size_t num_vertices)
{
    info("Constructing subgame with %d vertices...", (int)num_vertices);

    // Construct a subgame
    ParityGame subgame;
    subgame.make_subgame(game_, vertices, num_vertices, &winners_[0]);

    // Compress vertex priorities
    int old_d = subgame.d();
    subgame.compress_priorities();
    info( "Priority compression removed %d of %d priorities.",
          old_d - subgame.d(), old_d );

    // Solve the subgame
    info("Solving subgame...", (int)num_vertices);
    std::auto_ptr<LiftingStrategy> spm_strategy(
        LiftingStrategy::create(subgame, strategy_.c_str()) );
    assert(spm_strategy.get() != NULL);

    /* FIXME:  we mess up the vertex statistics here (since vertex indices are
               reordered); instead, we should use a new statistics object and
               then map the results back into the main statistics. */
    SmallProgressMeasures spm(subgame, *spm_strategy, stats_);
    if (!spm.solve())
    {
        error("Solving failed!\n");
        return 1;
    }

    // Copy winners from subgame
    for (size_t n = 0; n < num_vertices; ++n)
    {
        winners_[vertices[n]] = spm.winner(n);
    }

    // Update (peak) memory use
    size_t mem = subgame.memory_use() + spm.memory_use();
    if (mem > memory_used_) memory_used_ = mem;

    return aborted() ? -1 : 0;
}
