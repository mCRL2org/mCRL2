// Author(s): Maurice Laveaux
// Copyright (c) 2016-2016 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_PG_PRIORITYPROMOTION_SOLVER_H
#define MCRL2_PG_PRIORITYPROMOTION_SOLVER_H

#include "mcrl2/pg/ParityGameSolver.h"
#include "mcrl2/pg/RecursiveSolver.h"

#include <vector>
#include <deque>

/*! \defgroup PriorityPromotion
    Classes related to the Priority Promotion for parity games solving algorithm.
*/

/*! \ingroup PriorityPromotion

    Implementation of the priority promotion algorithm introduced in:

    Massimo Benerecetti, Daniele Dell’Erba, and Fabio Mogavero. Solving
    Parity Games via Priority Promotion, pages 270-290. Springer International
    Publishing, Cham, 2016.

    Given the current triple (region_function, strategy, prio), referred to as
    state, a region R is extracted by means of the query function. An alpha-region
    is a set of vertices R and a witness strategy sigma. So that for all plays
    consistent with sigma, they either stay within R and are winning for alpha.
    Or they escape R via vertices having the highest priority in the game. The
    initial state is (priority_function, empty_strategy, min(range(priority_function)))

    If the region R is a dominion in the whole game, the dominion is removed from
    the game and the algorithm runs on the remaining game. If the extracted region
    is open in the subgame G >= prio. Which is the subgame with only vertices
    having greater or equal priority then prio. The next state becomes
    (region_function[R -> prio], strategy*, min(range(region_function >= prio))).

    If the alpha-region is a dominion in the subgame G >= prio, the lowest priority
    region that the opponent can flee to is determined in promoteSubDominion. And
    then the next state becomes (region_function*[R -> prio*], strategy*, prio*), and
    in region_function* all regions below prio* are reset to the original priority.

    The strategy* was not presented in the paper, but was partially determined
    by the follow up paper Improving Priority Promotion for parity games. For
    the attractor for some region R the strategy is determined by the witness
    that alpha can reach R. For vertices inside the region R a witness core strategy
    sigma is given. For all vertices inside R where no strategy is defined yet
    an arbitrary successor inside R is taken to complete the strategy.
*/
class PriorityPromotionSolver : public ParityGameSolver
{
public:
    PriorityPromotionSolver(const ParityGame &game);

    /*! Compute winning strategies by means of priority promotion, follows the
        paper as closely as possible.
    */
    ParityGame::Strategy solve();

private:

    /*! From the state (region_function, strategy, prio) compute the new alpha-region
        R and update region_function[R -> p]. The strategy will be updated in
        computeAttractor. Unsolved is used to quickly iterate unsolved vertices.
        Todo is passed to be reused by computeAttractor.
    */
    void query(std::vector<priority_t>& region_function,
        std::vector<verti>& strategy,
        priority_t prio);

    /*! Compute the attractor set A for vertices in todo, with alpha being prio mod 2.
        inSubgraph indicates that only vertices in game >= prio are considered. This
        updates region_function[A -> prio]. The strategy is changed for alpha for the
        attraction witness. The remaining vertices of alpha without a strategy can pick
        any vertex inside A as witness.
    */
    void computeAttractor(std::vector<priority_t>& region_function,
        ParityGame::Strategy& strategy,
        priority_t prio,
        std::deque<verti>& todo,
        bool inSubgraph);

    /*! Determine whether the alpha-region with priority prio is open in G, or
        in G >= prio (indicated by inSubgraph). This means that for all vertices
        v with region_function[v] equal to prio, this is set R. When v belongs
        to alpha, determined by prio mod 2, there is some witness successor in R.
        For opponent vertices all successors lead to R, no witness to escape basically.
     */
    bool isOpen(std::vector<priority_t>& region_function,
        priority_t prio,
        bool inSubgraph);

    /*! Promote a sub dominion D to the maximum region lower then prio that the
        opponent can reach. This updates region_function[D -> prio*] and resets
        all priorities lower then prio* to the original priority function. The
        strategy is updated by means of computeAttractor. And lower strategies
        are set to NO_VERTEX (no strategy known).
    */
    priority_t promoteSubDominion(std::vector<priority_t>& region_function,
        ParityGame::Strategy& strategy,
        priority_t prio);

    //! Print the vertices with region_function[v] equal to prio, representing the region.
    void printRegion(std::vector<priority_t>& region_function,
        priority_t prio);

    /*! Computes min(rng(region_function >= prio)), so the next lower priority, greater then prio, that some vertex has.
        First region can be equal to prio (so use prio + 1 for next), so this function is reused for prio = 0.
     */
    priority_t nextPriority(std::vector<priority_t>& region_function,
        priority_t prio);

    /*! Store some local variables to improve readability of functions, will
        be initialized during the solving and destroyed at the end. All functions
        require the current state, but that is made explicit in parameters.
     */
    //! Stores a list of vertices not yet solved by the algorithm.
    std::vector<verti> m_unsolved;

    //! Count the number of vertices per region, to speed up nextPriority
    std::vector<verti> m_regions;

    //! This is a reused queue with vertices to compute the attractor set from.
    std::deque<verti> m_todo;

    verti m_promotions = 0; //! The number of promotions required.
    verti m_dominions = 0; //! The number of dominions found.
};

/*! \ingroup PriorityPromotion
    Factory object for PriorityPromotionSolver instances.
*/
class PriorityPromotionSolverFactory : public ParityGameSolverFactory
{
    //! Returns a new PriorityPromotionSolver instance.
    ParityGameSolver *create(const ParityGame &game,
        const verti *vertex_map,
        verti vertex_map_size);
};

#endif