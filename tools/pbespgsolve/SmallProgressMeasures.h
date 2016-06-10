// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SMALL_PROGRESS_MEASURES_H_INCLUDED
#define SMALL_PROGRESS_MEASURES_H_INCLUDED

#include <vector>
#include <utility>
#include "mcrl2/utilities/logger.h"
#include "ParityGameSolver.h"
#include "LiftingStrategy.h"

/*! \defgroup SmallProgressMeasures

    Classes related to the Small Progress Measures parity game solving algoritm.
*/

/*! \ingroup SmallProgressMeasures

    Object used to collect statistics when solving using the SPM algorithm */
class LiftingStatistics
{
public:
    /*! Construct a statistics object for the given game. */
    LiftingStatistics(const ParityGame &game, long long max_lifts = -1);

#if 0
    /*! Merge statistics from a given object into this object, using the given
        vertex mapping to map vertex indices (vertex v in `other` has index
        mapping[v] in this object). */
    void merge(const LiftingStatistics &other, const verti *mapping = NULL);
#endif

    long long lifts_attempted() const { return lifts_attempted_; }
    long long lifts_succeeded() const { return lifts_succeeded_; }
    long long lifts_attempted(verti v) const { return vertex_stats_[v].first; }
    long long lifts_succeeded(verti v) const { return vertex_stats_[v].second; }

    void add_lifts_attempted(long long count) { lifts_attempted_ += count; }
    void add_lifts_succeeded(long long count) { lifts_succeeded_ += count; }
    void add_lifts_attempted(verti v, long long count)
    {
        vertex_stats_[v].first += count;
    }
    void add_lifts_succeeded(verti v, long long count)
    {
        vertex_stats_[v].second += count;
    }

private:
    void record_lift(verti v, bool success);
    friend class SmallProgressMeasures;

private:
    long long lifts_attempted_, lifts_succeeded_;
    std::vector<std::pair<long long, long long> > vertex_stats_;
    long long max_lifts_;
};

/*! \ingroup SmallProgressMeasures

    Implements the core of the Small Progress Measures algorithm, which keeps
    track of progress measure vectors, and allows lifting at vertices.

    Note that besides these vectors, it tracks the current lifting strategy and
    (optionally) a statistics object.  All public methods of this class notify
    the lifting strategy whenever vectors change (due to one of the solve..()
    lift..() methods), but only when vertices are lifted through one of the
    solve..() methods, are lifting attempts recording in the statistics object.

    As a result, the lift..() methods can be used to introduce information from
    external sources into the game, without affecting the statistics for updates
    that result from local lifting attempts. These methods are thus used in the
    two-way approach to propagate information from the dual game, and the MPI
    recursive solver.

    Note that this is an abstract base class: subclasses may implement different
    storage formats for the progress measure vectors.
*/
class SmallProgressMeasures
{
public:
    //! Maximum number of lifting attempts in a row before checking timer.
    static const int work_size = 10000;

    SmallProgressMeasures(
        const ParityGame &game, ParityGame::Player player,
        LiftingStatistics *stats = 0,
        const verti *vmap = 0, verti vmap_size = 0 );

    virtual ~SmallProgressMeasures();

    /*! Performs some work on the game using the given lifting strategy (which
        must have been initialized).

        @param max_attempts Maximum number of lifts attempted.
        @return How many attempts remain; i.e. if the result is greater than
                zero, the game is succesfully solved.
        @see initialize_lifting_strategy
    */
    long long solve_some( LiftingStrategy &ls,
                          long long attempts = work_size );
    long long solve_some( LiftingStrategy2 &ls,
                          long long attempts = work_size );

    /*! Performs one lifting attempt, and returns the index of the vertex and
        whether lifting succeeded. Returns NO_VERTEX if no more vertices were
        candidates for lifting. */
    std::pair<verti, bool> solve_one(LiftingStrategy &ls);
    verti solve_one(LiftingStrategy2 &ls);

    /*! After the game is solved, this returns the strategy at vertex `v` for
        the current player, or NO_VERTEX if the vertex is controlled by his
        opponent (in that case, any move is winning) or if it is won by his
        opponent (in that case, all moves are losing). */
    verti get_strategy(verti v) const;

    /*! Takes an initialized strategy vector and updates it for the current
        player. The result is valid only after the game is solved. */
    void get_strategy(ParityGame::Strategy &strat) const;

    /*! Returns the winning set for the given player by assigning the vertices
        in the set to the given output iterator. If the game is not completely
        solved yet, then this returns a subset of the winning set. */
    template<class OutputIterator>
    void get_winning_set(ParityGame::Player player, OutputIterator result);

    /*! Sets the given vertex's progress measure to top, if it isn't already,
        and returns whether it changed: */
    inline bool lift_to_top(verti v);

    /*! Sets the given vertex's progress measure to the given value, if this
        is greater than the current value, and returns whether it changed.
        val[] must be an array of length len(v). If carry is set, the new
        value must be strictly greater (or top). */
    bool lift_to(verti v, const verti vec2[], bool carry = 0);

    /*! Returns the same result as lift_to() but without changing any
        progress measure vectors. */
    bool less_than(verti v, const verti vec2[], bool carry = 0);

    /*! For debugging: print current state to stdout */
    void debug_print() const;
    void debug_print_vertex(int v) const;

    /*! For debugging: verify that the current state describes a valid SPM */
    bool verify_solution();

    /*! Return the parity to be solved. */
    const ParityGame &game() const { return game_; }

    /*! Return the player to solve for. */
    ParityGame::Player player() const { return (ParityGame::Player)p_; }

    /*! Return the length of the SPM vectors (a positive integer). */
    int len() const { return len_; }

    /*! Returns the SPM vector space; an array of len() integers. */
    const verti *M() const { return M_; }

    /*! Changes the SPM vector space. `new_M` must be an array of at least
        `len` non-negative integers. */
    void set_M(const verti *new_M) { std::copy(new_M, new_M + len_, M_); }

    /*! Decrements the i'th element of M. */
    void decr_M(int i) { assert(M_[i] > 1); --M_[i]; }

    /*! Return the number of odd priorities less than or equal to the
        priority of v. This is the length of the SPM vector for `v`. */
    int len(verti v) const { return (game_.priority(v) + 1 + p_)/2; }

    /*! Return whether the given SPM vector has top value. */
    bool is_top(const verti vec[]) const { return vec[0] == NO_VERTEX; }

    /*! Return whether the SPM vector for vertex `v` has top value. */
    bool is_top(verti v) const { return is_top(vec(v)); }

    // The following functions are implemented in derived classes that
    // implement the actual storage of progress measure vectors:

    /*! Return the SPM vector for vertex `v`.
        This array contains only the components with odd (for Even) or even
        (for Odd) indices of the vector (since the reset is fixed at zero). */
    // virtual verti *vec(verti v) = 0;
    virtual const verti *vec(verti v) const = 0;

    /*! Assign the first `len(v)` elements of the vector for vertex `w` to the
        vector for `v`, or its successor if `carry` is set. */
    virtual void set_vec(verti v, const verti src[], bool carry) = 0;

    /*! Set the value for vertex `v` to top. */
    virtual void set_vec_to_top(verti v) = 0;

    /*! Recalculates the set of dirty vertices, and pushes them into the
        given lifting strategy (which is assumed to be freshly allocated). */
    void initialize_lifting_strategy(LiftingStrategy2 &ls);

protected:
    /*! Sets odd-controlled vertices with (only) loops to top.
        Should be called by derived classes after allocating SPM data. */
    void initialize_loops();

    /*! Set the SPM vector for vertex `v` to top value. This can decrease the
        vector space, but nothing else; e.g, the lifting strategy is not
        notified of the lift. */
    inline void set_top(verti v);

private:
    SmallProgressMeasures(const SmallProgressMeasures &);
    SmallProgressMeasures &operator=(const SmallProgressMeasures &);

private:
    /*! Compares the first `N` elements of the given SPM vectors and returns
        -1, 0 or 1 to indicate that v is smaller than, equal to, or larger than
        w (respectively). */
    inline int vector_cmp(const verti vec1[], const verti vec2[], int N) const;

    /*! Compares `N` elements of the SPM vectors for the given vertices. */
    inline int vector_cmp(verti v, verti w, int N) const;

    /*! Returns whehter vertex `v` is lifted to the maximum successor (true)
        or minimum successor (false), which in turn depends on whether it is
        controlled by the player `p_` we are solving for. */
    inline bool take_max(verti v) const { return (int)game_.player(v) != p_; }

    /*! Returns whether the progress measure for vertex `v` must be strictly
        greater than its successors or not (which is true if its priority has
        parity different from the current player). */
    inline bool compare_strict(verti v) const { return game_.priority(v)%2 != p_; }

    /*! Returns the minimum or maximum successor for vertex `v`,
        depending on whether take_max is false or true (respectively). */
    inline verti get_ext_succ(verti v, bool take_max) const;

    /*! Returns the minimum or maximum successor for vertex `v` depending on
        whether it is owned by player `p_` or not. */
    inline verti get_ext_succ(verti v) const;
    
    /*! Returns the minimum successor for vertex `v`. */
    inline verti get_min_succ(verti v) const { return get_ext_succ(v, false); }

    /*! Returns the maximum successor for vertex `v`. */
    inline verti get_max_succ(verti v) const { return get_ext_succ(v, true); }

    /*! Returns whether the succesor with the extreme progress measure for the
        given vertex.  (`Extreme` means minimal for even-controlled vertices,
        and maximal for odd-controlled vertices.) */
    inline verti get_successor(verti v) const { return strategy_[v]; }

    /*! Returns whether the given vertex is dirty; i.e. its progress measure
        is less than its successor (or less than or equal to, if its priority
        is odd). */
    inline bool is_dirty(verti v) const { return dirty_[v]; }

    // Allow selected lifting strategies to access the SPM internals:
    friend class PredecessorLiftingStrategy;
    friend class MaxMeasureLiftingStrategy2;
    friend class OldMaxMeasureLiftingStrategy;

protected:
    const ParityGame       &game_;     //!< the game being solved
    const int              p_;         //!< the player to solve for
    LiftingStatistics      *stats_;    //!< statistics object to record lifts
    const verti            *vmap_;     //!< active vertex map (if any)
    verti                  vmap_size_; //!< size of vertex map
    int                    len_;       //!< length of SPM vectors
    verti                  *M_;        //!< bounds on the SPM vector components
    ParityGame::Strategy   strategy_;  //!< current strategy
    bool                   *dirty_;    //!< marks unstable vertices
};

/*! \ingroup SmallProgressMeasures

    A small progress measures implementation that stores progress measure vectors
    "densely". All `len`*`V` elements are stored in a contiguous array.

    This is memory efficient and allows fast access and updating of progress
    measure vectors.  The main limitation is that it does not support concurrent
    access.
*/
class DenseSPM : public SmallProgressMeasures
{
public:
    DenseSPM(
        const ParityGame &game, ParityGame::Player player,
        LiftingStatistics *stats = 0,
        const verti *vertex_map = 0, verti vertex_map_size = 0 );
    ~DenseSPM();

    // verti *vec(verti v) { return &spm_[(size_t)len_*v]; }
    const verti *vec(verti v) const { return &spm_[(size_t)len_*v]; }
    void set_vec(verti v, const verti src[], bool carry) ;
    void set_vec_to_top(verti v);

protected:
    verti *spm_;  //!< array storing the SPM vector data
};


/*! \ingroup SmallProgressMeasures

    A parity game solver based on Marcin Jurdzinski's small progress measures
    algorithm, with pluggable lifting heuristics. */
class SmallProgressMeasuresSolver
    : public ParityGameSolver
{
protected:
    /*! Helper class which is an OutputIterator that sets vertices assigned
        through it to top in the given SPM representation (which in turn updates
        the corresponding lifting strategy). */
    class SetToTopIterator
    {
    public:
        SetToTopIterator(SmallProgressMeasures &spm) : spm(spm) { }
        SetToTopIterator& operator++() { return *this; }
        SetToTopIterator& operator++(int) { return *this; }
        SetToTopIterator& operator*() { return *this; }
        SetToTopIterator& operator=(verti v)
        {
            spm.lift_to_top(v);
            return *this;
        }

    private:
        SmallProgressMeasures &spm;
    };

public:
    SmallProgressMeasuresSolver( const ParityGame &game,
                                 LiftingStrategyFactory *lsf,
                                 bool alternate = false,
                                 LiftingStatistics *stats = 0,
                                 const verti *vmap = 0,
                                 verti vmap_size = 0 );
    virtual ~SmallProgressMeasuresSolver();

    ParityGame::Strategy solve();

    /*! Solves the game by applying Jurdziński's proposed algorithm that solves
        the game for one player only, and then solves a subgame with the
        remaining vertices. This algorithm is most efficient when the original
        game is easier to solve than its dual. */
    virtual ParityGame::Strategy solve_normal();

    /*! Solves the game using Friedmann's alternate strategy. This allocates
        solving algorithms for both the normal game and its dual at once, and
        alternates working on each, exchanging information about solved vertices
        in the process. */
    virtual ParityGame::Strategy solve_alternate();

    /*! Preprocess the game so that vertices with loops either have the loop
        removed, or have all other edges removed. In the latter case, the vertex
        is necessarily won by the player corresponding with its parity.

        This preprocessing operation speeds up solving with small progress
        measures considerably, though it is superseded by the DecycleSolver
        which does more general preprocessing. */
    static void preprocess_game(ParityGame &game);

private:
    SmallProgressMeasuresSolver(const SmallProgressMeasuresSolver&);
    SmallProgressMeasuresSolver &operator=(const SmallProgressMeasuresSolver&);

protected:
    LiftingStrategyFactory *lsf_;   //!< factory used to create lifting strategy
    bool alternate_;                //!< whether to use the alternate algorithm
    LiftingStatistics *stats_;      //!< object to record lifting statistics
    const verti *vmap_;             //!< current vertex map
    const verti vmap_size_;         //!< size of vertex map
};

/*! \ingroup SmallProgressMeasures

    TODO: document this class. */
class SmallProgressMeasuresSolver2 : public SmallProgressMeasuresSolver
{
public:
    SmallProgressMeasuresSolver2( const ParityGame &game,
                                  LiftingStrategyFactory *lsf,
                                  bool alternate = false,
                                  LiftingStatistics *stats = 0,
                                  const verti *vmap = 0,
                                  verti vmap_size = 0 );
    ~SmallProgressMeasuresSolver2();

    ParityGame::Strategy solve_normal();
    ParityGame::Strategy solve_alternate();

private:
    SmallProgressMeasuresSolver2(const SmallProgressMeasuresSolver2&);
    SmallProgressMeasuresSolver2 &operator=(const SmallProgressMeasuresSolver2&);
};

/*! \ingroup SmallProgressMeasures

    Factory class for SmallProgressMeasuresSolver instances */
class SmallProgressMeasuresSolverFactory : public ParityGameSolverFactory
{
public:
    SmallProgressMeasuresSolverFactory( LiftingStrategyFactory *lsf,
        int version = 1, bool alt = false, LiftingStatistics *stats = 0 );
    ~SmallProgressMeasuresSolverFactory();

    ParityGameSolver *create( const ParityGame &game,
                              const verti *vmap,
                              verti vmap_size );

private:
    LiftingStrategyFactory  *lsf_;
    int                     version_;
    bool                    alt_;
    LiftingStatistics       *stats_;
};

#include "SmallProgressMeasures_impl.h"

#endif /* ndef SMALL_PROGRESS_MEASURES_H_INCLUDED */
