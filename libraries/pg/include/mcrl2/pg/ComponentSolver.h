// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_PG_COMPONENT_SOLVER_H
#define MCRL2_PG_COMPONENT_SOLVER_H

#include "mcrl2/pg/SmallProgressMeasures.h"
#include "mcrl2/pg/DenseSet.h"
#include "mcrl2/pg/SCC.h"
#include "mcrl2/utilities/logger.h"

#include <string>
#include <vector>

/*! A solver that breaks down the game graph into strongly connected components.

    The individual components are then solved from bottom to top with a
    general solver.  Whenever a component is solved, its attractor set in the
    complete graph is computed, and the graph is decomposed again, in hopes of
    generating even smaller components.
*/
class ComponentSolver : public ParityGameSolver
{
public:
    /*! Constructs a solver for the given `game` using the factory `pgsf` to
        create subsolver instances to solve subgames constructed for each
        strongly connected component found.

        When `max_depth` > 0, components identified in the main graph are
        recursively decomposed (up to the give depth) if it turns out they have
        been partially solved already (i.e. when some of their vertices lie in
        the attractor sets of winning regions identified earlier).
    */
    ComponentSolver( const ParityGame &game, ParityGameSolverFactory &pgsf,
                     int max_depth, const verti *vmap = 0, verti vmap_size = 0
                   );
    ~ComponentSolver();

    ParityGame::Strategy solve();

private:
    // SCC callback
    int operator()(const verti *vertices, std::size_t num_vertices);
    friend class SCC<ComponentSolver>;

protected:
    ParityGameSolverFactory  &pgsf_;        //!< Solver factory to use
    const int                max_depth_;    //!< Max. recusion depth
    const verti              *vmap_;        //!< Current vertex map
    const verti              vmap_size_;    //!< Size of vertex map
    ParityGame::Strategy     strategy_;     //!< Resulting strategy
    DenseSet<verti>          *winning_[2];  //!< Resulting winning sets
};

//! Factory class for ComponentSolver instances.
class ComponentSolverFactory : public ParityGameSolverFactory
{
public:
    //! \see ComponentSolver::ComponentSolver()
    ComponentSolverFactory(ParityGameSolverFactory &pgsf, int max_depth = 10)
        : pgsf_(pgsf), max_depth_(max_depth) { pgsf_.ref(); }
    ~ComponentSolverFactory() { pgsf_.deref(); }

    //! Return a new ComponentSolver instance.
    ParityGameSolver *create( const ParityGame &game,
        const verti *vertex_map, verti vertex_map_size );

protected:
    ParityGameSolverFactory &pgsf_;     //!< Factory used to create subsolvers
    const int max_depth_;               //!< Maximum recursion depth
};

#endif /* ndef MCRL2_PG_COMPONENT_SOLVER_H */
