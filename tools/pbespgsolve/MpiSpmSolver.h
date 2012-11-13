// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MPI_SPM_SOLVER_H_INCLUDED
#define MPI_SPM_SOLVER_H_INCLUDED

#include "GamePartition.h"
#include "mcrl2/utilities/logger.h"
#include "MpiUtils.h"
#include "SmallProgressMeasures.h"
#include "VertexPartition.h"

/*! A parity game solver based on Marcin Jurdzinski's small progress measures
    algorithm, with pluggable lifting heuristics. Implements the two-way solving
    approach due to Friedmann, and allows distributed computation using MPI. */
class MpiSpmSolver : public ParityGameSolver, public virtual Logger
{
public:
    MpiSpmSolver( const ParityGame &game, const VertexPartition *vpart,
                  LiftingStrategyFactory *lsf, LiftingStatistics *stats = 0,
                  const verti *vertex_map = 0, verti vertex_map_size = 0 );

    ~MpiSpmSolver();

    ParityGame::Strategy solve();

protected:
    /*! Calculates the global vector space for all worker processes, and updates
        the SmallProgressMeasures instance to reflect it. */
    void set_vector_space(SmallProgressMeasures &spm);

    //! Helper function to lifts the given global vertex `v' to `vec' in `spm'.
    void update(SmallProgressMeasures &spm, verti global_v, const verti vec[]);

    /*! Lifts vertices in `spm' until globally no more vertices can be lifted
        (at which point, the game is solved for one player). */
    void solve_all(SmallProgressMeasures &spm);

    /*! Propagates information about stable vertices in `src' to `dst', by
        setting vertices which stable and non-top in `src' to top in `dst'.
        This is a purely local operation. */
    void propagate_solved( SmallProgressMeasures &src,
                           SmallProgressMeasures &dst );

    /*! Combines the local strategy with the strategies from other worker
        processes. Returns the combined global strategy for the process with
        rank 0, and an empty strategy for all other processes. */
    ParityGame::Strategy combine_strategies(ParityGame::Strategy &local);

    //! Debug-print SPM vectors.
    void debug_print(const SmallProgressMeasures &spm) const;

private:
    MpiSpmSolver(const MpiSpmSolver&);
    MpiSpmSolver &operator=(const MpiSpmSolver&);

protected:
    const VertexPartition   *vpart_;      //!< the current vertex partition
    const GamePartition     part_;        //!< the game partition being solved
    LiftingStrategyFactory  *lsf_;        //!< used to create lifting strategies
    LiftingStatistics       *stats_;      //!< global lifting statistics
    const verti             *vmap_;       //!< current global vertex map
    const verti             vmap_size_;   //!< size of globalvertex map
};

class MpiSpmSolverFactory : public ParityGameSolverFactory
{
public:
    MpiSpmSolverFactory( LiftingStrategyFactory *lsf,
                         const VertexPartition *vpart,
                         LiftingStatistics *stats = 0 );
    ~MpiSpmSolverFactory();

    ParityGameSolver *create( const ParityGame &game,
                              const verti *vertex_map,
                              verti vertex_map_size );

private:
    LiftingStrategyFactory  *lsf_;
    const VertexPartition   *vpart_;
    LiftingStatistics       *stats_;
};

#endif /* ndef MPI_SPM_SOLVER_H_INCLUDED */
