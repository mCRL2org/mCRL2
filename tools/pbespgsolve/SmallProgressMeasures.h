// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SMALL_PROGRESS_MEASURES_H_INCLUDED
#define SMALL_PROGRESS_MEASURES_H_INCLUDED

#include "ParityGameSolver.h"
#include "LiftingStrategy.h"
#include "Logger.h"
#include <vector>
#include <utility>

/*! Object used to collect statistics when solving using the SPM algorithm */
class LiftingStatistics
{
public:
    /*! Construct a statistics object for the given game. */
    LiftingStatistics(const ParityGame &game);

#if 0
    /*! Merge statistics from a given object into this object, using the given
        vertex mapping to map vertex indices (vertex v in `other' has index
        mapping[v] in this object). */
    void merge(const LiftingStatistics &other, const verti *mapping = NULL);
#endif

    long long lifts_attempted() const { return lifts_attempted_; }
    long long lifts_succeeded() const { return lifts_succeeded_; }
    long long lifts_attempted(verti v) const { return vertex_stats_[v].first; }
    long long lifts_succeeded(verti v) const { return vertex_stats_[v].second; }

private:
    void record_lift(verti v, bool success);
    friend class SmallProgressMeasures;

private:
    long long lifts_attempted_, lifts_succeeded_;
    std::vector<std::pair<long long, long long> > vertex_stats_;
};


/*! A parity game solver based on Marcin Jurdzinski's small progress measures
    algorithm, with pluggable lifting heuristics.

    For each node, we need to keep an SPM vector of length game->d().
    However, since all components with even indices (zero-based) are fixed at 0,
    we only store values for the odd indices.
*/
class SmallProgressMeasures : public ParityGameSolver, public virtual Logger
{
public:
    SmallProgressMeasures( const ParityGame &game,
                           LiftingStrategyFactory &lsf,
                           LiftingStatistics *stats = 0,
                           const verti *vertex_map = 0,
                           verti vertex_map_size = 0 );
    ~SmallProgressMeasures();

    ParityGame::Strategy solve();

    /*! For debugging: print current state to stdout */
    void debug_print();

    /*! For debugging: verify that the current state describes a valid SPM */
    bool verify_solution();

protected:

    /*! Preprocess the graph to speed up processing of some specific parts. */
    void preprocess_graph();

    /*! Attempt to lift a vertex (and return whether this succeeded). */
    bool lift(verti v);

    /*! Return the SPM vector for vertex `v`.
        This array contains only the components with odd indices of the vertex
        (since the reset is fixed at zero). */
    verti *vec(verti v) { return &spm_[(size_t)len_*v]; }
    const verti *vec(verti v) const { return &spm_[(size_t)len_*v]; }

    /*! Return the number of odd priorities less than or equal to the
        priority of v. This is the length of the SPM vector for `v`. */
    int len(verti v) const { return (game_.priority(v) + 1)/2; }

    /*! Return whether the SPM vector for vertex `v` has top value. */
    bool is_top(verti v) const { return vec(v)[0] == (verti)-1; }

    /*! Set the SPM vector for vertex `v` to top value. */
    void set_top(verti v) { vec(v)[0] = (verti)-1; }

    /*! Translate local into global vertex index: */
    verti map_vertex(verti v) {
        return vmap_ ? (v < vmap_size_ ? vmap_[v] : NO_VERTEX) : v;
    }

 private:
    /*! Compares the first `N` elements of the SPM vectors for the given
        vertices and returns -1, 0 or 1 to indicate that v is smaller, equal to,
        r larger than w (respectively). */
    inline int vector_cmp(verti v, verti w, int N) const;

    /*! Returns the minimum or maximum successor for vertex `v`,
        depending on whether take_max is false or true (vertex_map_size_respectively). */
    verti get_ext_succ(verti v, bool take_max);

    /*! Returns the minimum successor for vertex `v`. */
    verti get_min_succ(verti v);

    /*! Returns the maximum successor for vertex `v`. */
    verti get_max_succ(verti v);

    // Allow selected lifting strategies to access the SPM  internals:
    friend class MaxMeasureLiftingStrategy;
    friend class OldMaxMeasureLiftingStrategy;

protected:
    LiftingStrategyFactory &lsf_;   //!< factory used to create lifting strategy
    int len_;                       //!< length of SPM vectors
    verti *M_;                      //!< bounds on the SPM vector components
    verti *spm_;                    //!< array storing the SPM vector data
    LiftingStatistics *stats_;      //!< object to record lifting statistics
    const verti *vmap_;             //!< current vertex map
    const verti vmap_size_;         //!< size of vertex map
};


class SmallProgressMeasuresFactory : public ParityGameSolverFactory
{
public:
    SmallProgressMeasuresFactory( LiftingStrategyFactory &lsf,
                                  LiftingStatistics *stats = 0 )
        : lsf_(lsf), stats_(stats) { };

    ParityGameSolver *create( const ParityGame &game,
                              const verti *vertex_map,
                              verti vertex_map_size );

private:
    LiftingStrategyFactory  &lsf_;
    LiftingStatistics       *stats_;
};


int SmallProgressMeasures::vector_cmp(verti v, verti w, int N) const
{
    if (is_top(v)) return is_top(w) ? 0 : +1;   // v is top
    if (is_top(w)) return -1;                   // w is top, but v isn't

    for (int n = 0; n < N; ++n)
    {
        if (vec(v)[n] < vec(w)[n]) return -1;
        if (vec(v)[n] > vec(w)[n]) return +1;
    }

    return 0;
}

#endif /* ndef SMALL_PROGRESS_MEASURES_H_INCLUDED */
