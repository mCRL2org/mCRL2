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
#include "Logger.h"
#include "SCC.h"
#include <string>
#include <vector>

/*! A solver that breaks down the game graph into strongly connected components,
    and uses the SPM algorithm to solve independent subgames. */
class ComponentSolver : public ParityGameSolver, public virtual Logger
{
public:
    ComponentSolver( const ParityGame &game,
                     ParityGameSolverFactory &pgsf );
    ~ComponentSolver();

    ParityGame::Strategy solve();

private:
    // SCC callback
    int operator()(const verti *vertices, size_t num_vertices);
    friend class SCC<ComponentSolver>;

protected:
    ParityGameSolverFactory &pgsf_;     //!< Solver factory to use
    ParityGame::Strategy    strategy_;  //!< The resulting strategy
};

class ComponentSolverFactory : public ParityGameSolverFactory
{
public:
    ComponentSolverFactory(ParityGameSolverFactory &pgsf)
        : pgsf_(pgsf) { };

    ParityGameSolver *create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size );

protected:
    ParityGameSolverFactory &pgsf_;     //!< Factory used to create subsolvers
};

#endif /* ndef COMPONENT_SOLVER_H_INCLUDED */
