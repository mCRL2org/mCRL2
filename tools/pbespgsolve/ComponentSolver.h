// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef COMPONENT_SOLVER_H_INCLUDED
#define COMPONENT_SOLVER_H_INCLUDED

#include "SmallProgressMeasures.h"
#include "DenseSet.h"
#include "mcrl2/utilities/logger.h"
#include "SCC.h"
#include <string>
#include <vector>

/*! A solver that breaks down the game graph into strongly connected components,
    and uses the SPM algorithm to solve independent subgames. */
class ComponentSolver : public ParityGameSolver
{
public:
    ComponentSolver( const ParityGame &game, ParityGameSolverFactory &pgsf,
                     const verti *vmap = 0, verti vmap_size = 0 );
    ~ComponentSolver();

    ParityGame::Strategy solve();

private:
    // SCC callback
    int operator()(const verti *vertices, size_t num_vertices);
    friend class SCC<ComponentSolver>;

protected:
    ParityGameSolverFactory  &pgsf_;        //!< Solver factory to use
    const verti              *vmap_;        //!< Current vertex map
    const verti              vmap_size_;    //!< Size of vertex map
    ParityGame::Strategy     strategy_;     //!< Resulting strategy
    DenseSet<verti>          *winning_[2];  //!< Resulting winning sets
};

class ComponentSolverFactory : public ParityGameSolverFactory
{
public:
    ComponentSolverFactory(ParityGameSolverFactory &pgsf)
        : pgsf_(pgsf) { pgsf_.ref(); }
    ~ComponentSolverFactory() { pgsf_.deref(); }

    ParityGameSolver *create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size );

protected:
    ParityGameSolverFactory &pgsf_;     //!< Factory used to create subsolvers
};

#endif /* ndef COMPONENT_SOLVER_H_INCLUDED */
