// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef COMPONENT_SOLVER_H_INCLUDED
#define COMPONENT_SOLVER_H_INCLUDED

#include "SmallProgressMeasures.h"
#include "SCC.h"
#include <string>
#include <vector>

/*! A solver that breaks down the game graph into strongly connected components,
    and uses the SPM algorithm to solve independent subgames. */
class ComponentSolver : public ParityGameSolver
{
public:
    ComponentSolver( const ParityGame &game,
                     const std::string &strategy,
                     LiftingStatistics *stats );
    ~ComponentSolver();

    bool solve();
    ParityGame::Player winner(verti v) const { return winners_[v]; }
    const ParityGame &game() const { return game_; }
    size_t memory_use() const { return memory_used_; }

private:
    // SCC callback
    int operator()(const verti *vertices, size_t num_vertices);
    friend class SCC<ComponentSolver>;

protected:
    std::string                         strategy_;
    std::vector<ParityGame::Player>     winners_;
    LiftingStatistics                   *stats_;
    size_t                              memory_used_;
};

#endif /* ndef COMPONENT_SOLVER_H_INCLUDED */
