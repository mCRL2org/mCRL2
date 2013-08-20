// Copyright (c) 2009-2011 University of Twente
// Copyright (c) 2009-2011 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2011 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2011 Eindhoven University of Technology
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

LiftingStatistics::LiftingStatistics(const ParityGame &game)
    : lifts_attempted_(0), lifts_succeeded_(0)
{
    vertex_stats_.resize(game.graph().V());
}

void LiftingStatistics::record_lift(verti v, bool success)
{
    assert(v == NO_VERTEX || v < vertex_stats_.size());

    ++lifts_attempted_;
    if (v != NO_VERTEX) ++vertex_stats_[v].first;
    if (success)
    {
        ++lifts_succeeded_;
        if (v != NO_VERTEX) ++vertex_stats_[v].second;
    }
}

SmallProgressMeasures::SmallProgressMeasures(
        const ParityGame &game, ParityGame::Player player,
        LiftingStrategyFactory *lsf, LiftingStatistics *stats,
        const verti *vmap, verti vmap_size  )
    : game_(game), p_((int)player), ls_(0), stats_(stats),
      vmap_(vmap), vmap_size_(vmap_size)
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

    // Initialize SPM vector data
    size_t n = (size_t)len_*game.graph().V();
    spm_ = new verti[n];
    std::fill_n(spm_, n, 0);

    // Initialize vertices won by the opponent to Top. This is designed to work
    // in conjunction with preprocess_game() which should have removed the
    // non-loop outgoing edges for such vertices.
    // N.B. The DecycleSolver and DeloopSolver make this obsolete, so if we
    //      always use those, this code may well be removed!
    verti cnt = 0;
    for (verti v = 0; v < game_.graph().V(); ++v)
    {
        if ( game_.priority(v)%2 == 1 - p_ &&
             game_.graph().outdegree(v) == 1 &&
             *game_.graph().succ_begin(v) == v )
        {
            set_top(v);
            ++cnt;
        }
    }
    mCRL2log(mcrl2::log::verbose) << "Initialized " << cnt << (cnt == 1?" vertex":" vertices") << " to top" << std::endl;

    // Create lifting strategy
    ls_ = lsf->create(game_, *this);
    assert(ls_);
}

SmallProgressMeasures::~SmallProgressMeasures()
{
    delete ls_;
    delete[] spm_;
    delete[] M_;
}

bool SmallProgressMeasures::solve()
{
    while (solve_part(1000000) == 1000000) if (aborted()) return false;
    return true;
}

long long SmallProgressMeasures::solve_part(long long max_attempts)
{
    long long num_attempts;
    for (num_attempts = 0; num_attempts < max_attempts; ++num_attempts)
    {
        std::pair<verti, bool> lifted = solve_one();
        if (lifted.first == NO_VERTEX) break;
    }
    return num_attempts;
}

std::pair<verti, bool> SmallProgressMeasures::solve_one()
{
    verti vertex = ls_->next();
    if (vertex == NO_VERTEX) return std::make_pair(NO_VERTEX, false);
    bool success = lift(vertex);
    if (stats_ != NULL)
    {
        verti v = vertex;
        if (vmap_ && v < vmap_size_) v = vmap_[v];
        if (stats_) stats_->record_lift(v, success);
    }
    return std::make_pair(vertex, success);
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

size_t SmallProgressMeasures::memory_use()
{
    return sizeof(*this)
        + sizeof(game_.d())*sizeof(verti)             // M_
        + sizeof(verti)*len_*(game_.graph().V() + 1)  // spm_
        + sizeof(verti)*game_.graph().V();            // strategy
}

verti SmallProgressMeasures::get_min_succ(verti v) const
{
    return get_ext_succ(v, false);
}

verti SmallProgressMeasures::get_max_succ(verti v) const
{
    return get_ext_succ(v, true);
}

bool SmallProgressMeasures::lift(verti v)
{
    if (is_top(v)) return false;

    /* Find relevant successor */
    verti w = ((int)game_.player(v) == p_) ? get_min_succ(v) : get_max_succ(v);

    if (is_top(w))
    {
        set_top(v);
        ls_->lifted(v);
        return true;
    }

    /* Check if lifting is required */
    int d = vector_cmp(v, w, len(v));
    bool carry = game_.priority(v)%2 != p_;
    if (d >= carry) return false;

    /* Assign successor */
    for (size_t m = len(v); m > 0; --m)
    {
        size_t n = m - 1;
        vec(v)[n] = vec(w)[n] + carry;
        carry = (vec(v)[n] >= M_[n]);
        if (carry) vec(v)[n] = 0;
    }
    if (carry) set_top(v);
    ls_->lifted(v);
    return true;
}

bool SmallProgressMeasures::lift_to(verti v, const verti vec2[])
{
    verti *vec1 = vec(v);
    size_t l = len(v);
    if (vector_cmp(vec1, vec2, l) >= 0) return false;
    if (is_top(vec2))
    {
        set_top(v);
    }
    else
    {
        for (size_t i = 0; i < l; ++i) vec1[i] = vec2[i];
    }
    ls_->lifted(v);
    return true;
}

void SmallProgressMeasures::debug_print(bool verify)
{
    mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << "M =";
    for (size_t p = 0; p < game_.d(); ++p)
    {
      mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << " " << ((p%2 == p_) ? 0 : M_[p/2]);
    }
    mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << std::endl;

    for (verti v = 0; v < game_.graph().V(); ++v)
    {
      mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << v << " "
        << (game_.player(v) == ParityGame::PLAYER_EVEN ? 'E' : game_.player(v) == ParityGame::PLAYER_ODD  ? 'O' : '?')
        << "p=" << game_.priority(v) << ":";
        if (is_top(v))
        {
          mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << " T";
        }
        else
        {
            for (size_t p = 0; p < game_.d(); ++p)
            {
              mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << " " << ((p%2 == p_) ? 0 : vec(v)[p/2]);
            }
        }
        mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << std::endl;
    }

    if (verify)
    {
        mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << "Internal verification "
          << (verify_solution() ? "succeeded." : "failed!") << std::endl;
    }
}

bool SmallProgressMeasures::verify_solution()
{
    const StaticGraph &graph = game_.graph();

    for (verti v = 0; v < graph.V(); ++v)
    {
        if (!is_top(v))
        {
            for (size_t p = 0; p < game_.d(); ++p)
            {
                if (p%2 == p_) continue; /* this component is not stored */

                /* Ensure vector values satisfy bounds */
                if (vec(v)[p/2] >= M_[p/2])
                {
                  mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << p << "-th"
                    << " component of SPM vector for vertex " << v << " out of bounds!" << std::endl;
                  return false;
                }

                if (p > game_.priority(v) && vec(v)[p/2] != 0)
                {
                  mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << p/2 << "-th"
                    << " component of SPM vector for vertex " << v << " should be zero!" << std::endl;
                  return false;
                }
            }
        }

        bool all_ok = true, one_ok = false;
        for ( StaticGraph::const_iterator it = graph.succ_begin(v);
              it != graph.succ_end(v); ++it )
        {
            bool ok = is_top(v) ||
                vector_cmp(v, *it, len(v)) >= (game_.priority(v)%2 != p_);
            one_ok = one_ok || ok;
            all_ok = all_ok && ok;
        }

        if (!(game_.player(v) == p_ ? one_ok : all_ok))
        {
          mCRL2log(mcrl2::log::debug, "SmallProgressMeasures") << "order constraint not satisfied for vertex "
            << v << " with priority " << game_.priority(v) << " and player "
            << (game_.player(v) == ParityGame::PLAYER_EVEN ? "even" : game_.player(v) == ParityGame::PLAYER_ODD  ? "odd"  : "???" )
            << "!" << std::endl;
          return false;
        }
    }
    return true;
}


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
        SmallProgressMeasures spm( game(), ParityGame::PLAYER_EVEN,
                                   lsf_, stats_, vmap_, vmap_size_ );
        if (!spm.solve()) return ParityGame::Strategy();
        spm.get_strategy(strategy);
        spm.get_winning_set( ParityGame::PLAYER_ODD,
            std::back_insert_iterator<std::vector<verti> >(won_by_odd) );
        update_memory_use( spm.memory_use() +
                           spm.lifting_strategy()->memory_use() +
                           sizeof(strategy[0])*strategy.capacity() +
                           sizeof(won_by_odd[0])*won_by_odd.capacity() );
        /*
        info("DEBUG: verifying small progress measures.");
        assert(spm.verify_solution());
        */
    }

    if (!won_by_odd.empty())
    {
        // Make a dual subgame of the vertices won by player Odd
        ParityGame subgame;
        mCRL2log(mcrl2::log::verbose) << "Constructing subgame of size " << won_by_odd.size() << " to solve for Odd..." << std::endl;
        subgame.make_subgame(game_, won_by_odd.begin(), won_by_odd.end());
        subgame.compress_priorities();
        assert(subgame.proper());

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
        SmallProgressMeasures spm( subgame, ParityGame::PLAYER_ODD,
                                   lsf_, stats_, submap, submap_size );
        if (!spm.solve()) return ParityGame::Strategy();
        ParityGame::Strategy substrat(won_by_odd.size(), NO_VERTEX);
        spm.get_strategy(substrat);
        merge_strategies(strategy, substrat, won_by_odd);
        update_memory_use( spm.memory_use() +
                           spm.lifting_strategy()->memory_use() +
                           sizeof(strategy[0])*strategy.capacity() +
                           sizeof(substrat[0])*substrat.capacity() +
                           sizeof(won_by_odd[0])*won_by_odd.capacity() +
                           subgame.memory_use() +
                           sizeof(submap_data[0])*submap_data.capacity() );
        /*
        info("DEBUG: verifying small progress measures.");
        assert(spm.verify_solution());
        */
    }

    return strategy;
}

ParityGame::Strategy SmallProgressMeasuresSolver::solve_alternate()
{
    // Create two SPM and two lifting strategy instances:
    std::auto_ptr<SmallProgressMeasures> spm[2];
    spm[0].reset(new SmallProgressMeasures( game_, ParityGame::PLAYER_EVEN,
                                            lsf_, stats_, vmap_, vmap_size_ ));
    spm[1].reset(new SmallProgressMeasures( game_, ParityGame::PLAYER_ODD,
                                            lsf_, stats_, vmap_, vmap_size_ ));

    // Solve games alternatingly:
    int player = 0;
    long long max_lifts = game_.graph().V(), num_lifts = 0;
    do {
        mCRL2log(mcrl2::log::verbose) << "Switching to " << (player==0?"normal":"dual") << " game..." << std::endl;
        num_lifts = spm[player]->solve_part(max_lifts);
        if (aborted()) return ParityGame::Strategy();

        mCRL2log(mcrl2::log::verbose) << "Propagating solved vertices to other game..." << std::endl;
        SetToTopIterator it = { *spm[1 - player] };
        spm[player]->get_winning_set((ParityGame::Player)player, it);
        player = 1 - player;
    } while (num_lifts == max_lifts);
    // One game is solved; solve other game completely too:
    mCRL2log(mcrl2::log::verbose) << "Finishing " << (player==0?"normal":"dual") << " game..." << std::endl;
    if (!spm[player]->solve()) return ParityGame::Strategy();

    // Retrieve combined strategies:
    ParityGame::Strategy strategy(game_.graph().V(), NO_VERTEX);
    spm[0]->get_strategy(strategy);
    spm[1]->get_strategy(strategy);

    update_memory_use( spm[0]->memory_use() +
                       spm[1]->memory_use() +
                       sizeof(strategy[0])*strategy.capacity() );

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


SmallProgressMeasuresSolverFactory::SmallProgressMeasuresSolverFactory(
        LiftingStrategyFactory *lsf, bool alt, LiftingStatistics *stats )
    : lsf_(lsf), alt_(alt), stats_(stats)
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
    return new SmallProgressMeasuresSolver(
        game, lsf_, alt_, stats_, vmap, vmap_size );
}
