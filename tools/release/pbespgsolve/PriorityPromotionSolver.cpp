// Author(s): Maurice Laveaux
// Copyright (c) 2016-2016 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "PriorityPromotionSolver.h"

#include "mcrl2/utilities/logger.h"

const priority_t COMPUTED_REGION = -1;

PriorityPromotionSolver::PriorityPromotionSolver(const ParityGame &game) :
    ParityGameSolver(game)
{}

ParityGame::Strategy PriorityPromotionSolver::solve()
{
    // Important note, instead of actually repeatly removing dominions from
    // the game. The game is kept the same but the substrategy is used to
    // determine which vertices still are not solved. This is done
    // because make_subgame allocates new memory repeately and parity games
    // can be huge.
    StaticGraph& graph = const_cast<StaticGraph&>(game_.graph());

    // Initialize the initial state top = (priority_function, [NO_VERTEX], prio)
    std::vector<priority_t> region_function(graph.V(), 0);
    ParityGame::Strategy strategy(graph.V(), NO_VERTEX);

    // Due to cache locality the new approach will be slightly slower. Also
    // a magic region is used to indicate in region_function that some vertex is
    // computed.
    m_unsolved = std::vector<verti>(graph.V(), 0);

    // The lowest priority in the game (the highest number).
    priority_t lowestRegion = 0;

    // Set region_function to the original priorities and initialize the mapping
    for (verti v = 0; v < graph.V(); ++v) {
        region_function[v] = game().priority(v);
        m_unsolved[v] = v;

        lowestRegion = std::max(lowestRegion, game().priority(v));
    }

    // Initialize all regions that have some vertices to true
    m_regions = std::vector<verti>(lowestRegion + 1, 0);
    for (priority_t region : region_function) {
        ++m_regions[region];
    }

    // Find the lowest priority in the game
    priority_t prio = nextPriority(region_function, 0);

    // The searcher: The algorithm was tail recursive so can also be written as iteration.
    while (true) {
         query(region_function, strategy, prio);

        if (isOpen(region_function, prio, true)) {
            mCRL2log(mcrl2::log::debug) << "Newly computed region is open in the subgame, with p = " << prio << std::endl;
            printRegion(region_function, prio);

            // Keep the new region_function and substrategy, but go to the next priority
            prio = nextPriority(region_function, prio + 1);
        }
        else {
            if (!isOpen(region_function, prio, false)) {

                // Make sure we can use todo without problems.
                assert(m_todo.empty());

                // This is a dominion D in the whole game, compute the attractor
                // for this region.
                for (verti v : m_unsolved) {
                    if (region_function[v] == prio) {
                        m_todo.push_back(v);
                    }
                }

                computeAttractor(region_function, strategy, prio, m_todo, false);

                // Remove the dominion from the game and keep the unsolved vertices, also reset
                // lower priorities and set region of prio to the COMPUTED_REGION.
                mCRL2log(mcrl2::log::debug) << "Found the dominion D, with p = " << prio << std::endl;
                printRegion(region_function, prio);

                // Reset the unsolved set and remove all regions, also add one dominion to statistics
                m_unsolved.clear();
                m_regions.assign(m_regions.size(), 0);
                ++m_dominions;

                for (verti v = 0; v < game().graph().V(); ++v) {
                    if (region_function[v] == prio) {
                        // Assign a special region indicating that its solved.
                        region_function[v] = COMPUTED_REGION;
                    }
                    else if (region_function[v] != COMPUTED_REGION) {
                        region_function[v] = game().priority(v);
                        strategy[v] = NO_VERTEX;

                        // Add the not solved vertices to the unsolved set and add vertices to their region.
                        m_unsolved.push_back(v);
                        ++m_regions[game().priority(v)];
                    }
                }


                if (m_unsolved.empty()) {
                    break; // Stop the algorithm, as all the vertices were solved.
                }

                // Reset the game and find the lowest priority in the game
                prio = nextPriority(region_function, 0);
            }
            else {
                // The game is a dominion, but only in the subgame, so promote its priority.
                mCRL2log(mcrl2::log::debug) << "Promoted dominion D, with p = " << prio << " to ";
                prio = promoteSubDominion(region_function, strategy, prio);
                mCRL2log(mcrl2::log::debug) << prio << std::endl;
                printRegion(region_function, prio);
            }
        }
    }

    mCRL2log(mcrl2::log::verbose) << m_dominions << " dominions found, and " << m_promotions  << " promotions required" << std::endl;

    return strategy;
}

void PriorityPromotionSolver::query(std::vector<priority_t>& region_function,
    ParityGame::Strategy& strategy,
    priority_t prio)
{
    // Make sure nothing else is stored in the todo
    assert(m_todo.empty());

    // R* = region_function^-1(prio), this results in the todo for the attractor
    // computation, the initial set essentially.
    for (verti v : m_unsolved)
    {
        if (region_function[v] == prio) {
            m_todo.push_back(v);
        }
    }

    // (region_function[R -> prio], strategy*) <- computeAttractor_G(todo, strategy
    // restriced to todo)
    computeAttractor(region_function, strategy, prio, m_todo, true);
}

void PriorityPromotionSolver::computeAttractor(std::vector<priority_t>& region_function,
    ParityGame::Strategy& strategy,
    priority_t prio,
    std::deque<verti>& todo,
    bool inSubgraph)
{
    // Name some variables for easier access
    const StaticGraph &graph = game().graph();
    const ParityGame::Player alpha = (ParityGame::Player)(prio % 2);

    // O(V): Compute the attractor set to the alpha-region:
    while (!todo.empty()) {
        const verti w = todo.front();
        todo.pop_front();

        // Check all predecessors v of w:
        for (StaticGraph::const_iterator it = graph.pred_begin(w);
            it != graph.pred_end(w); ++it) {
            const verti v = *it;

            // Skip predecessors that are already in the attractor set, also skip
            // vertices outside the subgame G >= prio. Or vertices that are computed
            if (region_function[v] == prio || region_function[v] == COMPUTED_REGION || (inSubgraph && region_function[v] < prio)) continue;

            if (game().player(v) == alpha) {
                // sigma(v) = w, a valid strategy for alpha is to pick a successor in A
                strategy[v] = w;
            }
            else {

                // Check if all successors (w, x) subset A, thus if they end up in a vertex with prio.
                bool isSubset = true;
                for (StaticGraph::const_iterator it = graph.succ_begin(v);
                    it != graph.succ_end(v); ++it) {
                    verti x = *it;

                    // Skip vertices that are not considered in the subgraph G >= prio
                    // or that already belong to COMPUTED_REGION
                    if (region_function[x] == prio || region_function[x] == COMPUTED_REGION) {
                        continue;
                    }

                    // Either only take vertices in G >= prio or all when inSubgraph is false
                    if (region_function[x] > prio || !inSubgraph) {
                        isSubset = false; break;
                    }
                }

                // opponent controls vertex
                if (isSubset) {
                    // For opponent controlled vertices no strategy exists, so
                    // every possible outgoing edge is losing.
                    strategy[v] = NO_VERTEX;
                } else {
                    continue;  // not in the attractor set yet!
                }
            }

            // Add a vertex to their new region and remove from the old one
            --m_regions[region_function[v]];
            ++m_regions[prio];

            // When this part is reached, all liberties of v are gone or v belongs
            // to alpha, so add vertex v to the attractor set:
            region_function[v] = prio;
            todo.push_back(v);

        }
    }

    // R \ domain(tau restricted to R*), essentially vertices in R belonging to
    // alpha where no strategy is defined yet. These can pick an arbritrary
    // successor that can reach R \ R*, these already have an attraction
    // strategy so that is always fine.
    for (verti v : m_unsolved)
    {
        if (region_function[v] == prio && game().player(v) == alpha && strategy[v] == NO_VERTEX) {
            for (StaticGraph::const_iterator it = graph.succ_begin(v); it != graph.succ_end(v); ++it) {
                const verti w = *it;

                if (region_function[w] == prio) {
                    // There exists some (v, w) in E such that w belongs to R (has r[w] == prio).
                    strategy[v] = w;
                }
            }
        }
    }

    // TODO: Check conflict in the paper.
    // In the lemma: by associating every position v in Stay(R) \ domain(strategy)
    //      with an arbitrary successor in R. So a vertex without a strategy can
    //      pick a successor in R.
    //
    // Later on: R \ domain(strategy restricted to R*), R* is region without
    //      attractor. So this is still the same, but then it should take a
    //      vertex in R \ R*. So it has to be outside of the region, this cannot
    //      be checked easily at this point as that information (in todo) is lost.
}

bool PriorityPromotionSolver::isOpen(std::vector<priority_t>& region_function,
    priority_t prio,
    bool inSubgraph)
{
    // Name some variables for easier access
    const StaticGraph &graph = game().graph();
    ParityGame::Player alpha = (ParityGame::Player)(prio % 2);

    // O(V): Loop over unsolved vertices and find vertices belonging to region with prio.
    for (verti v : m_unsolved) {
        if (region_function[v] == prio) {
            // If the vertex belong to the opponent
            if (game().player(v) != alpha) {
                // For all (v, u) in E, u should belong to R
                for (StaticGraph::const_iterator it = graph.succ_begin(v);
                    it != graph.succ_end(v); ++it)
                {
                    const verti u = *it;

                    // There is an edge from opponent to a vertex in the subgraph or in the whole graph
                    if (region_function[u] != COMPUTED_REGION
                        && ((inSubgraph && region_function[u] > prio)
                        || (!inSubgraph && region_function[u] != prio))) {
                        return true;
                    }
                }
            }
            else {
                // If there exists a (v, u) to R its closed
                bool isOpen = true;

                for (StaticGraph::const_iterator it = graph.succ_begin(v);
                    it != graph.succ_end(v); ++it)
                {
                    const verti u = *it;

                    // There is an edge from a player to the region
                    if (region_function[u] == prio) {
                        isOpen = false; break;
                    }
                }

                if (isOpen) {
                    return true;
                }
            }
        }
    }

    return false;
}

priority_t PriorityPromotionSolver::promoteSubDominion(std::vector<priority_t>& region_function,
    ParityGame::Strategy& strategy,
    priority_t prio)
{
    const StaticGraph &graph = game().graph();
    ParityGame::Player alpha = (ParityGame::Player)(prio % 2);

    // O(V): It is only a dominion in the subgraph, determine highest p < prio
    // that opponent can escape to
    priority_t promotion = 0;

    // This is referred to as r* = bep(R, r) in the paper (best escape priority).
    // For every opponent vertex this is the lowest priority (highest value in
    // min-prio games) that it can reach. The region it can reach belongs to alpha,
    // otherwise it would be attracted in some earlier state.
    for (verti v : m_unsolved) {
        if (region_function[v] == prio && game().player(v) != alpha) {
            // For all (v, u) in E collect the highest priority smaller then prio that opponent can flee to.
            for (StaticGraph::const_iterator it = graph.succ_begin(v);
                it != graph.succ_end(v); ++it) {
                const verti u = *it;

                if (region_function[u] < prio) {
                    promotion = std::max(promotion, region_function[u]);
                }
            }
        }
    }

    ++m_promotions;

    // Here the prio region is promoted to the new priority and all lower positions
    // are reset.
    for (verti v : m_unsolved) {
        // Promote the current region to the promotion priority
        if (region_function[v] == prio) {
            region_function[v] = promotion;

            // Add the vertex to the new region.
            ++m_regions[region_function[v]];
        }
        else if (region_function[v] > promotion) {
            // Remove the vertex from the old region
            --m_regions[region_function[v]];

            // Reset all vertices lower to the original priorities, remove the strategy.
            region_function[v] = game().priority(v);
            strategy[v] = NO_VERTEX;

            // Add the vertex to the new region.
            ++m_regions[region_function[v]];
        }
    }

    // This was promoted so no longer exists
    m_regions[prio] = 0;

    return promotion;
}

void PriorityPromotionSolver::printRegion(std::vector<priority_t>& region_function,
    priority_t prio)
{
    // This costs O(V) so only enable this in debug, prints all vertices v where region_function[v] == prio.
    if (mCRL2logEnabled(mcrl2::log::debug)) {
        mCRL2log(mcrl2::log::debug) << "alpha-region[" << prio << "] = { ";
        bool first = true;
        for (verti v : m_unsolved)
        {
            if (region_function[v] == prio) {
                if (!first) {
                    mCRL2log(mcrl2::log::debug) << ",";
                }

                mCRL2log(mcrl2::log::debug) << v;
                first = false;
            }

        }
        mCRL2log(mcrl2::log::debug) << " }" << std::endl;
    }
}

priority_t PriorityPromotionSolver::nextPriority(std::vector<priority_t>& region_function,
    priority_t prio)
{
    // Starting from the current priority, find the next region that exists
    // greater or equal to rio. This should never go out of bounds as the lowest
    // region will always be dominion.
    while (m_regions[prio] == 0)
    {
        ++prio;
        assert(prio < m_regions.size());
    }

    return prio;
}

ParityGameSolver *PriorityPromotionSolverFactory::create(const ParityGame &game,
    const verti *vertex_map,
    verti vertex_map_size)
{
    return new PriorityPromotionSolver(game);
}