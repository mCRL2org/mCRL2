// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "SmallProgressMeasures.h"
#include "attractor.h"
#include "SCC.h"
#include <algorithm>
#include <memory>
#include <assert.h>
#include <string.h>
#include <stdio.h>  /* printf() */

LiftingStatistics::LiftingStatistics( const ParityGame &game,
                                      long long max_lifts )
    : lifts_attempted_(0), lifts_succeeded_(0), max_lifts_(max_lifts)
{
    vertex_stats_.resize(game.graph().V());
}

void LiftingStatistics::record_lift(verti v, bool success)
{
    assert(v == NO_VERTEX || v < vertex_stats_.size());

    ++lifts_attempted_;
    if (lifts_attempted_ == max_lifts_) Abortable::abort_all();
    if (v != NO_VERTEX) ++vertex_stats_[v].first;
    if (success)
    {
        ++lifts_succeeded_;
        if (v != NO_VERTEX) ++vertex_stats_[v].second;
    }
}

SmallProgressMeasures::SmallProgressMeasures(
        const ParityGame &game, ParityGame::Player player,
        LiftingStatistics *stats, const verti *vmap, verti vmap_size )
    : game_(game), p_(player), stats_(stats),
      vmap_(vmap), vmap_size_(vmap_size),
      strategy_(game.graph().V(), NO_VERTEX), dirty_(0)
{
    assert(p_ == 0 || p_ == 1);

    // Initialize SPM vector bounds
    len_ = (game_.d() + p_)/2;
    if (len_ < 1) len_ = 1;  // ensure Top is representable
    M_ = new verti[len_];
    for (size_t n = 0; n < len_; ++n)
    {
        size_t prio = 2*n + 1 - p_;
        M_[n] = (prio < game.d()) ? game_.cardinality(prio) + 1 : 0;
    }
}

void SmallProgressMeasures::initialize_loops()
{
    const verti V = game_.graph().V();

    // Initialize vertices won by the opponent to Top. This is designed to
    // work in conjunction with preprocess_game() which should have removed
    // the non-loop outgoing edges for such vertices.

    // N.B. The DecycleSolver and DeloopSolver make this obsolete, so if we
    //      always use those, this code may well be removed!

    // (Alternatively, the winning set propagation in Friedmann's two-sided
    //  approach sets these to top, but in the current implementation this
    //  is only done after iteration, not before!)

    verti cnt = 0;
    for (verti v = 0; v < V; ++v)
    {
        if ( game_.priority(v)%2 == 1 - p_ &&
             game_.graph().outdegree(v) == 1 &&
             *game_.graph().succ_begin(v) == v )
        {
            strategy_[v] = v;
            set_top(v);
            ++cnt;
        }
    }
    mCRL2log(mcrl2::log::debug) << "Initialized " << cnt
                                << "vert" << (cnt == 1 ? "ex" : "ices") << " to top" << std::endl;
}

void SmallProgressMeasures::initialize_lifting_strategy(LiftingStrategy2 &ls)
{
    const verti V = game_.graph().V();
    if (!dirty_) dirty_ = new bool[V];
    for (verti v = 0; v < V; ++v)
    {
        if (is_top(v))
        {
            // strategy may not be valid for top vertices!
            dirty_[v] = false;
        }
        else
        {
            verti w = get_ext_succ(v, take_max(v));
            bool dirty = less_than(v, vec(w), compare_strict(v));
            strategy_[v] = w;
            dirty_[v]    = dirty;
            if (dirty) ls.push(v);
        }
    }
}

SmallProgressMeasures::~SmallProgressMeasures()
{
    delete[] M_;
    delete[] dirty_;
}

long long SmallProgressMeasures::solve_some( LiftingStrategy &ls,
                                             long long attempts )
{
    while (attempts > 0 && solve_one(ls).first != NO_VERTEX) --attempts;
    return attempts;
}

long long SmallProgressMeasures::solve_some( LiftingStrategy2 &ls,
                                             long long attempts )
{
    while (attempts > 0 && solve_one(ls) != NO_VERTEX) --attempts;
    return attempts;
}

std::pair<verti, bool> SmallProgressMeasures::solve_one(LiftingStrategy &ls)
{
    verti v = ls.next();
    if (v == NO_VERTEX) return std::make_pair(NO_VERTEX, false);

    bool success = false;
    if (!is_top(v))
    {
        verti w = get_ext_succ(v, take_max(v));
        if (lift_to(v, vec(w), compare_strict(v)))
        {
            ls.lifted(v);
            success = true;
        }
    }
    if (stats_ != NULL)
    {
        stats_->record_lift(vmap_ && v < vmap_size_ ? vmap_[v] : v, success);
    }
    return std::make_pair(v, success);
}

verti SmallProgressMeasures::solve_one(LiftingStrategy2 &ls)
{
    verti v = ls.pop();
    if (v == NO_VERTEX) return NO_VERTEX;

    assert(!is_top(v));

    #ifndef NDEBUG
    bool success =
    #endif // NDEBUG 
    lift_to(v, vec(get_successor(v)), compare_strict(v));
    assert(success);
    dirty_[v] = false;
    // debug_print_vertex(v);

    for ( const verti *it  = game_.graph().pred_begin(v),
                      *end = game_.graph().pred_end(v); it != end; ++it )
    {
        verti u = *it;
        if (is_top(u)) continue;

        bool changed;
        if (!take_max(u))  // even-controlled vertex: minimize
        {
            if (get_successor(u) == v)  // minimum successor increased
            {
                verti w = get_min_succ(u);
                strategy_[u] = w;
                changed = true;
            }
            else  // non-minimum successor increased -- no change!
            {
                changed = false;
            }
        }
        else  // odd-controlled vertex: maximize
        {
            if (get_successor(u) == v)  // maximum successor increased
            {
                changed = true;
            }
            else
            if (vector_cmp(vec(v), vec(get_successor(u)), len_) > 0)
            {   // maximum successor changed
                strategy_[u] = v;
                changed = true;
            }
            else  // non-maximum successor doesn't beat current maximum
            {
                changed = false;
            }
        }
        if (changed)
        {
            if (is_dirty(u))
            {
                ls.bump(u);
            }
            else
            {
                bool carry = game_.priority(u)%2 != p_;
                bool dirty = less_than(u, vec(get_successor(u)), carry);
                if (dirty)
                {
                    dirty_[u] = true;
                    ls.push(u);
                }
            }
        }
    }

    if (stats_ != NULL)
    {
        stats_->record_lift(vmap_ && v < vmap_size_ ? vmap_[v] : v, true);
    }
    return v;
}

verti SmallProgressMeasures::get_strategy(verti v) const
{
    return (!is_top(v) && game_.player(v) == p_) ? get_min_succ(v) : NO_VERTEX;
}

void SmallProgressMeasures::get_strategy(ParityGame::Strategy &strat) const
{
    verti V = game_.graph().V();
    assert(strat.size() == V);
    for (verti v = 0; v < V; ++v)
    {
        verti w = get_strategy(v);
        if (w != NO_VERTEX) strat[v] = w;
    }
}

// Returns the same result as lift_to, but doesn't actually change anything:
bool SmallProgressMeasures::less_than(verti v, const verti vec2[], bool carry)
{
    if (is_top(v)) return false;
    if (is_top(vec2)) return true;
    int comparison = vector_cmp(vec(v), vec2, len(v));
    return comparison < 0 || (comparison <= 0 && carry);
}

bool SmallProgressMeasures::lift_to(verti v, const verti vec2[], bool carry)
{
    if (is_top(v)) return false;

    if (is_top(vec2))
    {
        set_top(v);
    }
    else
    {
        int comparison = vector_cmp(vec(v), vec2, len(v));
        if (comparison > 0 || (comparison >= 0 && !carry))  return false;
        set_vec(v, vec2, carry);
    }
    return true;
}

#if 0
void SmallProgressMeasures::debug_print_vertex(int v) const
{
    printf ( "%6d %c p=%d:", (int)v,
                game_.player(v) == ParityGame::PLAYER_EVEN ? 'E' :
                game_.player(v) == ParityGame::PLAYER_ODD  ? 'O' : '?',
                (int)game_.priority(v) );
    if (is_top(v))
    {
        printf(" T");
    }
    else
    {
        for (int p = 0; p < game_.d(); ++p)
        {
            printf(" %d", (p%2 == p_) ? 0 : vec(v)[p/2]);
        }
    }
    printf("\n");
}

void SmallProgressMeasures::debug_print() const
{
    printf("M =");
    for (int p = 0; p < game_.d(); ++p) printf(" %d", (p%2 == p_) ? 0 : M_[p/2]);
    printf("\n");
    for (verti v = 0; v < game_.graph().V(); ++v) debug_print_vertex(v);
}

bool SmallProgressMeasures::verify_solution()
{
    const StaticGraph &graph = game_.graph();

    for (verti v = 0; v < graph.V(); ++v)
    {
        if (!is_top(v))
        {
            for (int p = 0; p < game_.d(); ++p)
            {
                if (p%2 == p_) continue; /* this component is not stored */

                /* Ensure vector values satisfy bounds */
                if (vec(v)[p/2] >= M_[p/2])
                {
                    printf( "%d-th component of SPM vector for vertex %d "
                            "out of bounds!\n", p, (int)v );
                    return false;
                }

                if (p > game_.priority(v) && vec(v)[p/2] != 0)
                {
                    printf( "%d-th component of SPM vector for vertex %d "
                            "should be zero!\n", p/2, (int)v );
                    return false;
                }
            }
        }

        bool all_ok = true, one_ok = false;
        for ( StaticGraph::const_iterator it = graph.succ_begin(v);
              it != graph.succ_end(v); ++it )
        {
            bool ok = is_top(v) ||
                      vector_cmp(v, *it, len(v)) >= compare_strict(v);
            one_ok = one_ok || ok;
            all_ok = all_ok && ok;
        }

        if (!(game_.player(v) == p_ ? one_ok : all_ok))
        {
            printf( "order constraint not satisfied for vertex %d with "
                    "priority %d and player %s!\n", v, game_.priority(v),
                game_.player(v) == PLAYER_EVEN ? "even" :
                game_.player(v) == PLAYER_ODD  ? "odd"  : "???" );
            return false;
        }
    }
    return true;
}
#endif

//
//  SmallProgressMeasuresSolver
//

SmallProgressMeasuresSolver::SmallProgressMeasuresSolver(
    const ParityGame &game, LiftingStrategyFactory *lsf, bool alternate,
    LiftingStatistics *stats, const verti *vmap, verti vmap_size )
        : ParityGameSolver(game), lsf_(lsf), alternate_(alternate),
          stats_(stats), vmap_(vmap), vmap_size_(vmap_size)
{
    lsf_->ref();
}

SmallProgressMeasuresSolver::~SmallProgressMeasuresSolver()
{
    lsf_->deref();
}

ParityGame::Strategy SmallProgressMeasuresSolver::solve()
{
    return alternate_ ? solve_alternate() : solve_normal();
}

ParityGame::Strategy SmallProgressMeasuresSolver::solve_normal()
{
    ParityGame::Strategy strategy(game_.graph().V(), NO_VERTEX);
    std::vector<verti> won_by_odd;

    {
        mCRL2log(mcrl2::log::verbose) << "Solving for Even..." << std::endl;
        DenseSPM spm( game(), PLAYER_EVEN,
                      stats_, vmap_, vmap_size_ );
        std::unique_ptr<LiftingStrategy> ls(lsf_->create(game_, spm));
        while (spm.solve_some(*ls) == 0)
        {
            if (aborted()) return ParityGame::Strategy();
        }
        spm.get_strategy(strategy);
        spm.get_winning_set( PLAYER_ODD,
            std::back_insert_iterator<std::vector<verti> >(won_by_odd) );
#ifdef DEBUG
        mCRL2log(mcrl2::log::verbose) << "Verifying small progress measures." << std::endl;
        assert(spm.verify_solution());
#endif
    }

    if (!won_by_odd.empty())
    {
        // Make a dual subgame of the vertices won by player Odd
        ParityGame subgame;
        mCRL2log(mcrl2::log::verbose) << "Constructing subgame of size "
                                      << won_by_odd.size() << " to solve for Odd..." << std::endl;
        subgame.make_subgame(game_, won_by_odd.begin(), won_by_odd.end(), true);
        subgame.compress_priorities();

        // Create vertex map to use:
        std::vector<verti> submap_data;
        verti *submap = &won_by_odd[0];
        size_t submap_size = won_by_odd.size();
        if (vmap_)
        {
            submap_data = won_by_odd;
            submap = &submap_data[0];
            merge_vertex_maps(submap, submap + submap_size, vmap_, vmap_size_);
        }

        // Second pass; solve subgame of vertices won by Odd:
        mCRL2log(mcrl2::log::verbose) << "Solving for Odd..." << std::endl;
        DenseSPM spm( subgame, PLAYER_ODD,
                      stats_, submap, submap_size );
        std::unique_ptr<LiftingStrategy> ls(lsf_->create(subgame, spm));
        while (spm.solve_some(*ls) == 0)
        {
            if (aborted()) return ParityGame::Strategy();
        }
        ParityGame::Strategy substrat(won_by_odd.size(), NO_VERTEX);
        spm.get_strategy(substrat);
        merge_strategies(strategy, substrat, won_by_odd);
#ifdef DEBUG
        mCRL2log(mcrl2::log::debug) << "Verifying small progress measures." << std::endl;
        assert(spm.verify_solution());
#endif
    }

    return strategy;
}

ParityGame::Strategy SmallProgressMeasuresSolver::solve_alternate()
{
    // Create two SPM and two lifting strategy instances:
    std::unique_ptr<SmallProgressMeasures> spm[2];
    spm[0].reset(new DenseSPM( game_, PLAYER_EVEN,
                               stats_, vmap_, vmap_size_ ));
    spm[1].reset(new DenseSPM( game_, PLAYER_ODD,
                               stats_, vmap_, vmap_size_ ));

    // Solve games alternatingly:
    int player = 0;
    bool half_solved = false;
    while (!half_solved)
    {
        mCRL2log(mcrl2::log::verbose) << "Switching to "
                                       << (player == 0 ? "normal" : "dual") << " game..." << std::endl;
        std::unique_ptr<LiftingStrategy> ls(lsf_->create(game_, *spm[player]));

        /* Note: work size should be large enough so that dumb strategies like
                 linear lifting are still able to detect termination! */
        for ( long long work = game_.graph().V(); work > 0 && !half_solved;
              work -= SmallProgressMeasures::work_size )
        {
            half_solved = spm[player]->solve_some(*ls) > 0;
            if (aborted()) return ParityGame::Strategy();
        }

        mCRL2log(mcrl2::log::verbose) << "Propagating solved vertices to other game..." << std::endl;
        spm[player]->get_winning_set( (ParityGame::Player)player,
                                      SetToTopIterator(*spm[1 - player]) );
        player = 1 - player;
    }

    // One game is solved; solve other game completely too:
    mCRL2log(mcrl2::log::verbose) << "Finishing " << (player == 0 ? "normal" : "dual") << "game..." << std::endl;
    std::unique_ptr<LiftingStrategy> ls(lsf_->create(game_, *spm[player]));
    while (spm[player]->solve_some(*ls) == 0)
    {
        if (aborted()) return ParityGame::Strategy();
    }

    // Retrieve combined strategies:
    ParityGame::Strategy strategy(game_.graph().V(), NO_VERTEX);
    spm[0]->get_strategy(strategy);
    spm[1]->get_strategy(strategy);

    return strategy;
}

void SmallProgressMeasuresSolver::preprocess_game(ParityGame &game)
{
    StaticGraph &graph = const_cast<StaticGraph&>(game.graph());  // HACK
    StaticGraph::edge_list obsolete_edges;

    for (verti v = 0; v < graph.V(); ++v)
    {
        if (graph.has_succ(v, v))
        {
            // Decide what to do with the edges:
            if ((int)game.priority(v)%2 == (int)game.player(v))
            {
                // Self-edge is beneficial; remove other edges
                for ( StaticGraph::const_iterator it = graph.succ_begin(v);
                      it != graph.succ_end(v); ++it )
                {
                    if (*it != v)
                    {
                        obsolete_edges.push_back(std::make_pair(v, *it));
                    }
                }
            }
            else
            if (graph.outdegree(v) > 1)
            {
                // Self-edge is detrimental; remove it
                obsolete_edges.push_back(std::make_pair(v, v));
            }
        }
    }
    graph.remove_edges(obsolete_edges);
}


//
//  SmallProgressMeasuresSolver2
//

SmallProgressMeasuresSolver2::SmallProgressMeasuresSolver2(
    const ParityGame &game, LiftingStrategyFactory *lsf, bool alternate,
    LiftingStatistics *stats, const verti *vmap, verti vmap_size )
        : SmallProgressMeasuresSolver( game, lsf, alternate,
                                       stats, vmap, vmap_size)
{
}

SmallProgressMeasuresSolver2::~SmallProgressMeasuresSolver2()
{
}

ParityGame::Strategy SmallProgressMeasuresSolver2::solve_normal()
{
    ParityGame::Strategy strategy(game_.graph().V(), NO_VERTEX);
    std::vector<verti> won_by_odd;

    {
        mCRL2log(mcrl2::log::verbose) << "Solving for Even..." << std::endl;
        DenseSPM spm( game(), PLAYER_EVEN,
                      stats_, vmap_, vmap_size_ );
        std::unique_ptr<LiftingStrategy2> ls(lsf_->create2(game_, spm));
        spm.initialize_lifting_strategy(*ls);
        while (spm.solve_some(*ls) == 0)
        {
            if (aborted()) return ParityGame::Strategy();
        }
        spm.get_strategy(strategy);
        spm.get_winning_set( PLAYER_ODD,
            std::back_insert_iterator<std::vector<verti> >(won_by_odd) );
#ifdef DEBUG
        mCRL2log(mcrl2::log::debug) << "Verifying small progress measures." << std::endl;
        assert(spm.verify_solution());
#endif
    }

    if (!won_by_odd.empty())
    {
        // Make a dual subgame of the vertices won by player Odd
        ParityGame subgame;
        mCRL2log(mcrl2::log::verbose) << "Constructing subgame of size "
                                      << won_by_odd.size() << " to solve for Odd..." << std::endl;
        subgame.make_subgame(game_, won_by_odd.begin(), won_by_odd.end(), true);
        subgame.compress_priorities();

        // Create vertex map to use:
        std::vector<verti> submap_data;
        verti *submap = &won_by_odd[0];
        size_t submap_size = won_by_odd.size();
        if (vmap_)
        {
            submap_data = won_by_odd;
            submap = &submap_data[0];
            merge_vertex_maps(submap, submap + submap_size, vmap_, vmap_size_);
        }

        // Second pass; solve subgame of vertices won by Odd:
        mCRL2log(mcrl2::log::verbose) << "Solving for Odd..." << std::endl;
        DenseSPM spm( subgame, PLAYER_ODD,
                      stats_, submap, submap_size );
        std::unique_ptr<LiftingStrategy2> ls(lsf_->create2(subgame, spm));
        spm.initialize_lifting_strategy(*ls);
        while (spm.solve_some(*ls) == 0)
        {
            if (aborted()) return ParityGame::Strategy();
        }
        ParityGame::Strategy substrat(won_by_odd.size(), NO_VERTEX);
        spm.get_strategy(substrat);
        merge_strategies(strategy, substrat, won_by_odd);
#ifdef DEBUG
        mCRL2log(mcrl2::log::debug) << "Verifying small progress measures." << std::endl;
        assert(spm.verify_solution());
#endif
    }

    return strategy;
}

ParityGame::Strategy SmallProgressMeasuresSolver2::solve_alternate()
{
    // Create two SPM and two lifting strategy instances:
    std::unique_ptr<SmallProgressMeasures> spm[2];
    spm[0].reset(new DenseSPM( game_, PLAYER_EVEN,
                               stats_, vmap_, vmap_size_ ));
    spm[1].reset(new DenseSPM( game_, PLAYER_ODD,
                               stats_, vmap_, vmap_size_ ));

    // Solve games alternatingly:
    int player = 0;
    bool half_solved = false;
    while (!half_solved)
    {
        mCRL2log(mcrl2::log::verbose) << "Switching to " << (player == 0 ? "normal" : "dual") << " game..." << std::endl;
        std::unique_ptr<LiftingStrategy2> ls(lsf_->create2(game_, *spm[player]));
        spm[player]->initialize_lifting_strategy(*ls);

        for ( long long work = game_.graph().V(); work > 0 && !half_solved;
              work -= SmallProgressMeasures::work_size )
        {
            half_solved = spm[player]->solve_some(*ls) > 0;
            if (aborted()) return ParityGame::Strategy();
        }

        mCRL2log(mcrl2::log::verbose) << "Propagating solved vertices to other game..." << std::endl;
        spm[player]->get_winning_set( (ParityGame::Player)player,
                                      SetToTopIterator(*spm[1 - player]) );
        player = 1 - player;
    }

    // One game is solved; solve other game completely too:
    mCRL2log(mcrl2::log::verbose) << "Finishing " << (player == 0 ? "normal" : "dual") << " game..." << std::endl;
    std::unique_ptr<LiftingStrategy2> ls(lsf_->create2(game_, *spm[player]));
    spm[player]->initialize_lifting_strategy(*ls);
    while (spm[player]->solve_some(*ls) == 0)
    {
        if (aborted()) return ParityGame::Strategy();
    }

    // Retrieve combined strategies:
    ParityGame::Strategy strategy(game_.graph().V(), NO_VERTEX);
    spm[0]->get_strategy(strategy);
    spm[1]->get_strategy(strategy);

    return strategy;
}


//
//  SmallProgressMeasuresSolverFactory
//

SmallProgressMeasuresSolverFactory::SmallProgressMeasuresSolverFactory(
        LiftingStrategyFactory *lsf, int version, bool alt,
        LiftingStatistics *stats )
    : lsf_(lsf), version_(version), alt_(alt), stats_(stats)
{
    lsf_->ref();
}

SmallProgressMeasuresSolverFactory::~SmallProgressMeasuresSolverFactory()
{
    lsf_->deref();
}

ParityGameSolver *SmallProgressMeasuresSolverFactory::create(
    const ParityGame &game, const verti *vmap, verti vmap_size )
{
    assert(version_ == 1 || version_ == 2);
    if (version_ == 1)
    {
        return new SmallProgressMeasuresSolver(
            game, lsf_, alt_, stats_, vmap, vmap_size );
    }
    if (version_ == 2)
    {
        return new SmallProgressMeasuresSolver2(
            game, lsf_, alt_, stats_, vmap, vmap_size );
    }
    return 0;
}

//
//  DenseSPM
//

DenseSPM::DenseSPM( const ParityGame &game, ParityGame::Player player,
                    LiftingStatistics *stats,
                    const verti *vertex_map, verti vertex_map_size )
    : SmallProgressMeasures(game, player, stats, vertex_map, vertex_map_size),
      spm_(new verti[(size_t)len_*game.graph().V()]())
{
    initialize_loops();
}

DenseSPM::~DenseSPM()
{
    delete[] spm_;
}

void DenseSPM::set_vec(verti v, const verti src[], bool carry)
{
    verti *dst = &spm_[(size_t)len_*v];
    const int l = len(v);                   // l: vector length
    int k = l;                              // k: position of last overflow
    for (int n = l - 1; n >= 0; --n)
    {
        dst[n] = src[n] + carry;
        carry = (dst[n] >= M_[n]);
        if (carry) k = n;
    }
    while (k < l) dst[k++] = 0;
    if (carry) set_top(v);
}

void DenseSPM::set_vec_to_top(verti v)
{
    spm_[(size_t)len_*v] = NO_VERTEX;
}
