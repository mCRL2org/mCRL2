// Author(s): Maurice Laveaux and Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_PBES_SYMBOLIC_PBESSOLVE_H
#define MCRL2_PBES_SYMBOLIC_PBESSOLVE_H

#ifdef MCRL2_ENABLE_SYLVAN
#include "sylvan_ldd.hpp"

#include "symbolic_parity_game.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"



namespace mcrl2::pbes_system {

using sylvan::ldds::ldd;

/// Container type for symbolic solutions, consisting of
/// winning sets and strategies for both players.
struct symbolic_solution_t
{
  /// Winning sets of both players
  std::array<ldd,2> winning;

  /// Strategies for both players
  std::array<ldd,2> strategy;

  symbolic_solution_t()
    : winning({sylvan::ldds::empty_set(), sylvan::ldds::empty_set()}),
      strategy({sylvan::ldds::empty_set(), sylvan::ldds::empty_set()})
  {}
};

/// Print solution to string
inline
std::string print_solution(const symbolic_parity_game& G, const symbolic_solution_t& solution)
{
  std::ostringstream os;
  os << "W0 = " << G.print_nodes(solution.winning[0]) << std::endl;
  os << "W1 = " << G.print_nodes(solution.winning[1]) << std::endl;
  os << "S0 = " << G.print_strategy(solution.strategy[0]) << std::endl;
  os << "S1 = " << G.print_strategy(solution.strategy[1]);
  return os.str();
}

class symbolic_pbessolve_algorithm
{
  private:
    const symbolic_parity_game& m_G;
    bool m_check_strategy = false;

  public:
    symbolic_pbessolve_algorithm(const symbolic_parity_game& G, bool check_strategy = false) :
      m_G(G),
      m_check_strategy(check_strategy)
    {}

    symbolic_solution_t zielonka(const ldd& V)
    {
      using namespace sylvan::ldds;

      if (V == empty_set())
      {
        return symbolic_solution_t();
      }

      stopwatch timer;
      mCRL2log(log::debug) << "start zielonka recursion\n";

      symbolic_solution_t solution;

      // Compute the partitioning of V for players 0 (in V[0]) and 1 (in V[1]).
      std::array<const ldd, 2> Vplayer = m_G.players(V);

      auto [m, U] = m_G.get_min_rank(V);
      std::size_t alpha = m % 2; // 0 = disjunctive, 1 = conjunctive

      const auto [A, A_strategy] = m_G.safe_attractor(U, alpha, V, Vplayer);
      mCRL2log(log::trace) << "A = attractor(" << m_G.print_nodes(U) << ", " << m_G.print_nodes(V) << ") = " << m_G.print_nodes(A) << std::endl;

      // Original Zielonka version
      symbolic_solution_t solution_V_minus_A = zielonka(minus(V, A));
      if (solution_V_minus_A.winning[1 - alpha] == empty_set())
      {
        solution.winning[alpha] = union_(A, solution_V_minus_A.winning[alpha]);
        solution.winning[1 - alpha] = empty_set();
        solution.strategy[alpha]
          = union_(union_(A_strategy, solution_V_minus_A.strategy[alpha]), merge(intersect(U, Vplayer[alpha]), V));
        solution.strategy[1 - alpha] = empty_set();

        assert(union_(solution.winning[0], solution.winning[1]) == V);
      }
      else
      {
        const auto [B, B_strategy] = m_G.safe_attractor(solution_V_minus_A.winning[1 - alpha], 1 - alpha, V, Vplayer);
        mCRL2log(log::trace) << "B = attractor(" << m_G.print_nodes(solution_V_minus_A.winning[1 - alpha]) << ", " << m_G.print_nodes(V) << ") = " << m_G.print_nodes(B) << std::endl;
        solution = zielonka(minus(V, B));
        solution.winning[1 - alpha] = union_(solution.winning[1 - alpha], B);
        solution.strategy[1 - alpha] = union_(union_(solution_V_minus_A.strategy[1 - alpha], B_strategy), solution.strategy[1 - alpha]);
        assert(union_(solution.winning[0], solution.winning[1]) == V);
      }

      mCRL2log(log::debug) << "finished zielonka recursion (time = " << std::setprecision(2) << std::fixed << timer.seconds() << "s)\n";

      mCRL2log(log::trace) << "\n  --- zielonka solution for ---\n" << m_G.print_graph(V) << std::endl;
      mCRL2log(log::trace) << print_solution(m_G, solution) << std::endl;

      assert(union_(solution.winning[0], solution.winning[1]) == V);
      return solution;
    }

  public:

    /// \brief Solve the given game restricted to V with Zielonka's recursive algorithm as solver.
    ///        The remaining parameters are sinks, vertices won by even and odd respectively.
    ///        Terminates early when initial_vertex has been solved.
    /// \returns The winner and W0, W1, S0, S1. Where S0 and S1 are the strategies.
    std::tuple<bool, symbolic_solution_t> solve(const ldd& initial_vertex,
        const ldd& V,
        const ldd& Vsinks = sylvan::ldds::empty_set(),
        const symbolic_solution_t& partial_solution = symbolic_solution_t())
    {
      using namespace sylvan::ldds;
      stopwatch timer;

      symbolic_solution_t solution = partial_solution;

      ldd Vtotal = m_G.compute_total_graph(V, empty_set(), Vsinks, solution.winning, solution.strategy);

      if (!includes(solution.winning[0], initial_vertex) && !includes(solution.winning[1], initial_vertex))
      {
        // If the initial vertex has not yet been won then run the zielonka solver as well.
        mCRL2log(log::trace) << "\n--- apply zielonka to ---\n" << m_G.print_graph(Vtotal) << std::endl;
        symbolic_solution_t zielonka_solution = zielonka(Vtotal);

        // Ensure that previously solved sets are included.
        solution.winning[0] = union_(zielonka_solution.winning[0], solution.winning[0]);
        solution.winning[1] = union_(zielonka_solution.winning[1], solution.winning[1]);
        solution.strategy[0] = union_(zielonka_solution.strategy[0], solution.strategy[0]);
        solution.strategy[1] = union_(zielonka_solution.strategy[1], solution.strategy[1]);
      }

      mCRL2log(log::verbose) << "finished solving (time = " << std::setprecision(2) << std::fixed << timer.seconds() << "s)\n";
      mCRL2log(log::trace) << print_solution(m_G, solution) << std::endl;

      if (includes(solution.winning[0], initial_vertex))
      {
        if (m_check_strategy)
        {
          check_strategy(initial_vertex, V, solution.winning[0], solution.winning[1], false, solution.strategy[0]);
        }
        return std::make_tuple(true, solution);
      }
      else if (includes(solution.winning[1], initial_vertex))
      {
        if (m_check_strategy)
        {
          check_strategy(initial_vertex, V, solution.winning[0], solution.winning[1], true, solution.strategy[1]);
        }
        return std::make_tuple(false, solution);
      }
      else
      {
        throw mcrl2::runtime_error("No solution found!");
      }
    }

    /// \brief Solve the given incomplete parity game (m_G, I) restricted to V.
    ///        The remaining parameters are sinks, vertices won by even and odd respectively.
    ///        Terminates early when initial_vertex has been solved.
    symbolic_solution_t partial_solve(const ldd& initial_vertex,
      const ldd& V,
      const ldd& I,
      const ldd& Vsinks = sylvan::ldds::empty_set(),
      const symbolic_solution_t& partial_solution = symbolic_solution_t())
    {
      // Make the game total.
      using namespace sylvan::ldds;
      symbolic_solution_t solution = partial_solution;

      ldd Vtotal = m_G.compute_total_graph(V, I, Vsinks, solution.winning, solution.strategy);
      if (includes(solution.winning[0], initial_vertex) || includes(solution.winning[1], initial_vertex))
      {
        return solution;
      }

      // Solve with zielonka twice for the safe sets.
      symbolic_solution_t zielonka_solution_0 = zielonka(m_G.compute_safe_vertices(0, Vtotal, I));
      zielonka_solution_0.winning[0] = union_(zielonka_solution_0.winning[0], solution.winning[0]);
      zielonka_solution_0.strategy[0] = union_(zielonka_solution_0.strategy[0], solution.strategy[0]);

      if (includes(zielonka_solution_0.winning[0], initial_vertex))
      {
        zielonka_solution_0.winning[1] = solution.winning[1];
        zielonka_solution_0.strategy[1] = solution.strategy[1];
        return zielonka_solution_0;
      }

      symbolic_solution_t zielonka_solution_1 = zielonka(m_G.compute_safe_vertices(1, Vtotal, I));
      zielonka_solution_1.winning[1] = union_(zielonka_solution_1.winning[1], solution.winning[1]);
      zielonka_solution_1.strategy[1] = union_(zielonka_solution_1.strategy[1], solution.strategy[1]);

      zielonka_solution_1.winning[0] = zielonka_solution_0.winning[0];
      zielonka_solution_1.strategy[0] = zielonka_solution_0.strategy[0];
      return zielonka_solution_1;
    }

    /// \brief Detect solitair winning cycles for the given incomplete parity game (m_G, I) restricted to V.
    ///        The remaining parameters are sinks, vertices won by even and odd respectively.
    ///        Terminates early when initial_vertex has been solved.
    /// \param safe Whether to use the safe attractor variant (as opposed to computing safe vertices first).
    symbolic_solution_t detect_solitair_cycles(const ldd& initial_vertex,
      const ldd& V,
      const ldd& I,
      bool safe_variant,
      const ldd& Vsinks,
      const symbolic_solution_t& partial_solution = symbolic_solution_t())
    {
      using namespace sylvan::ldds;

      symbolic_solution_t solution = partial_solution;
      // Make the game total and removed winning sets.

      ldd Vtotal = m_G.compute_total_graph(V, I, Vsinks, solution.winning, solution.strategy);
      if (includes(solution.winning[0], initial_vertex) || includes(solution.winning[1], initial_vertex))
      {
        return solution;
      }

      mCRL2log(log::trace) << "\n--- apply solitair winning cycle detection to ---\n" << m_G.print_graph(V) << std::endl;

      // Computes two vertex sets of all even priority and odd priority nodes respectively.
      std::array<ldd, 2> parity;
      std::array<const ldd, 2> Vplayer = m_G.players(Vtotal);
      for (const auto&[rank, Vrank] : m_G.ranks())
      {
        parity[rank % 2] = union_(parity[rank % 2], Vrank);
      }

      std::array<ldd, 2> Vsafe;
      if (!safe_variant)
      {
        Vsafe = { m_G.compute_safe_vertices(0, Vtotal, I), m_G.compute_safe_vertices(1, Vtotal, I) };
      }

      for (std::size_t alpha = 0; alpha <= 1; ++alpha)
      {
        // Determine the cycles for this player.
        ldd U = empty_set();
        ldd Unext = intersect(parity[alpha], Vplayer[alpha]);
        if (!safe_variant)
        {
          Unext = intersect(Unext, Vsafe[alpha]);
        }

        mCRL2log(log::debug) << "solitair winning cycle detection for player " << alpha << "\n";

        std::size_t iter = 0;
        while (U != Unext)
        {
          stopwatch timer;
          U = Unext;
          Unext = m_G.predecessors(U, U);

          mCRL2log(log::debug) << "iteration " << iter << " (time = " << std::setprecision(2) << std::fixed << timer.seconds() << "s)\n";

          ++iter;
        }
        // At this point, all vertices in U have an edge to another vertex in U, and
        // are thus winning for player alpha. We can set the strategy to U x U.

        mCRL2log(log::debug) << "found " << std::setw(12) << satcount(U) << " states in cycles for player " << alpha << "\n";

        solution.strategy[alpha] = union_(solution.strategy[alpha], merge(U, U));

        if (safe_variant)
        {
          std::pair<ldd, ldd> attr = m_G.safe_attractor(U, alpha, Vtotal, Vplayer, I);
          solution.winning[alpha] = union_(solution.winning[alpha], attr.first);
          solution.strategy[alpha] = union_(solution.strategy[alpha], attr.second);
        }
        else
        {
          std::pair<ldd, ldd> attr = m_G.safe_attractor(U, alpha, Vsafe[alpha], Vplayer);
          solution.winning[alpha] = union_(solution.winning[alpha], attr.first);
          solution.strategy[alpha] = union_(solution.strategy[alpha], attr.second);
        }
      }

      mCRL2log(log::trace) << print_solution(m_G, solution) << std::endl;

      return solution;
    }

    /// \brief Detect forced winning cycles for the given incomplete parity game (m_G, I) restricted to V.
    ///        The remaining parameters are sinks, vertices won by even and odd respectively.
    ///        Terminates early when initial_vertex has been solved.
    /// \param safe Whether to use the safe attractor variant (as opposed to computing safe vertices first).
    symbolic_solution_t detect_forced_cycles(const ldd& initial_vertex,
      const ldd& V,
      const ldd& I,
      bool safe_variant,
      const ldd& Vsinks,
      const symbolic_solution_t& partial_solution = symbolic_solution_t())
    {
      using namespace sylvan::ldds;

      symbolic_solution_t solution = partial_solution;

      // Make the game total and removed winning sets.

      ldd Vtotal = m_G.compute_total_graph(V, I, Vsinks, solution.winning, solution.strategy);
      if (includes(solution.winning[0], initial_vertex) || includes(solution.winning[1], initial_vertex))
      {
        return solution;
      }

      mCRL2log(log::trace) << "\n--- apply forced winning cycle detection to ---\n" << m_G.print_graph(V) << std::endl;

      // Computes two vertex sets of all even priority and odd priority nodes respectively.
      std::array<ldd, 2> parity;
      std::array<const ldd, 2> Vplayer = m_G.players(Vtotal);
      for (const auto&[rank, Vrank] : m_G.ranks())
      {
        parity[rank % 2] = union_(parity[rank % 2], Vrank);
      }

      std::array<ldd, 2> Vsafe;
      if (!safe_variant)
      {
        Vsafe = { m_G.compute_safe_vertices(0, Vtotal, I), m_G.compute_safe_vertices(1, Vtotal, I) };
      }

      for (std::size_t alpha = 0; alpha <= 1; ++alpha)
      {
        // Determine the cycles for this player.
        ldd U = empty_set();
        ldd Unext = parity[alpha];
        if (!safe_variant)
        {
          Unext = intersect(Unext, Vsafe[alpha]);
        }

        mCRL2log(log::debug) << "forced winning cycle detection for player " << alpha << "\n";

        std::size_t iter = 0;
        while (U != Unext)
        {
          stopwatch timer;
          U = Unext;
          if (safe_variant)
          {
            Unext = intersect(U, m_G.safe_control_predecessors(alpha, U, Vtotal, U, Vplayer, I));
          }
          else
          {
            Unext = intersect(U, m_G.safe_control_predecessors(alpha, U, Vsafe[alpha], U, Vplayer));
          }

          mCRL2log(log::debug) << "iteration " << iter << " (time = " << std::setprecision(2) << std::fixed << timer.seconds() << "s)\n";

          ++iter;
        }

        mCRL2log(log::debug) << "found " << std::setw(12) << satcount(U) << " states in cycles for player " << alpha << "\n";

        // Overapproximate strategy for the forced winning cycles
        solution.strategy[alpha] = union_(solution.strategy[alpha], merge(U, U));

        if (safe_variant)
        {
          std::pair<ldd, ldd> attr = m_G.safe_attractor(U, alpha, Vtotal, Vplayer, I);
          solution.winning[alpha] = union_(solution.winning[alpha], attr.first);
          solution.strategy[alpha] = union_(solution.strategy[alpha], attr.second);
        }
        else
        {
          std::pair<ldd, ldd> attr = m_G.safe_attractor(U, alpha, Vsafe[alpha], Vplayer);
          solution.winning[alpha] = union_(solution.winning[alpha], attr.first);
          solution.strategy[alpha] = union_(solution.strategy[alpha], attr.second);
        }
      }

      mCRL2log(log::trace) << print_solution(m_G, solution) << std::endl;

      return solution;
    }

    /// \returns Partial solve using the fatal attractors.
    std::pair<ldd, ldd> detect_fatal_attractors(const ldd& initial_vertex,
        const ldd& V,
        const ldd& I,
        bool safe_variant,
        const ldd& Vsinks,
        const ldd& W0 = sylvan::ldds::empty_set(),
        const ldd& W1 = sylvan::ldds::empty_set())
    {
      using namespace sylvan::ldds;
      stopwatch timer;

      std::array<ldd, 2> winning = { W0, W1 };
      std::array<ldd, 2> strategy;

      ldd Vtotal = m_G.compute_total_graph(V, I, Vsinks, winning, strategy);
      if (includes(winning[0], initial_vertex) || includes(winning[1], initial_vertex))
      {
        return { winning[0], winning[1] };
      }

      std::array<const ldd, 2> Vplayer = m_G.players(Vtotal);

      // Compute safe vertices if necessary
      std::array<ldd, 2> Vsafe;
      if (!safe_variant)
      {
        Vsafe = { m_G.compute_safe_vertices(0, Vtotal, I), m_G.compute_safe_vertices(1, Vtotal, I) };
      }

      mCRL2log(log::trace) << "\n--- apply fatal attractor detection to ---\n" << m_G.print_graph(Vtotal) << std::endl;

      // For priorities in descending order
      for (auto it = m_G.ranks().rbegin(); it != m_G.ranks().rend(); it++)
      {
        std::size_t c = it->first;
        std::size_t alpha = c % 2;
        mCRL2log(log::debug) << "fatal attractor detection for priority " << c << "\n";
        ldd X = safe_variant ? it->second : intersect(it->second, Vsafe[alpha]);
        ldd Y = empty_set();

        while (X != empty_set() && X != Y)
        {
          Y = X;
          ldd Z = m_G.safe_monotone_attractor(X, alpha, c, safe_variant ? Vtotal : Vsafe[alpha], Vplayer, safe_variant ? I : empty_set());


          if (includes(Z, X))
          {
            if (safe_variant)
            {
              winning[alpha] = union_(winning[alpha], m_G.safe_attractor(Z, alpha, Vtotal, Vplayer, I).first);
            }
            else
            {
              winning[alpha] = union_(winning[alpha], m_G.safe_attractor(Z, alpha, Vsafe[alpha], Vplayer).first);
            }
            mCRL2log(log::debug) << "found " << std::setw(12) << satcount(Z) << " states in fatal attractors for priority " << c << "\n";
            break;
          }
          else
          {
            X = intersect(X, Z);
          }
        }
      }

      mCRL2log(log::debug) << "finished fatal attractor detection (time = " << std::setprecision(2) << std::fixed << timer.seconds() << "s)\n";
      mCRL2log(log::trace) << "W0 = " << m_G.print_nodes(winning[0]) << std::endl;
      mCRL2log(log::trace) << "W1 = " << m_G.print_nodes(winning[1]) << std::endl;

      return { winning[0], winning[1] };
    }

    /// Computes an LDD that has the deadlock states in a given set of vertices V in parity game G
    /// The deadlocks are those states that are not the predecessor of another state in V.
    ldd compute_deadlocks(const ldd& V, const symbolic_parity_game& G)
    {
      ldd predecessors = G.predecessors(V, V);
      ldd deadlocks = sylvan::ldds::minus(V, predecessors);
      return deadlocks;
    }

    /// Checks whether the computed strategy is indeed a correct certificate for the winning partition.
    ///
    /// Throws an exception when the strategy is invalid.
    void check_strategy(const ldd& initial_vertex,
      const ldd& V,
      const ldd& W0,
      const ldd& W1,
      bool alpha,
      const ldd& strategy)
    {
      mCRL2log(log::debug) << "Checking the strategy of the solved parity game..." << std::endl;
      symbolic_parity_game new_G = m_G.apply_strategy(alpha, strategy);
      mCRL2log(log::trace) << "Minimal parity game G = " << new_G.print_graph(V) << std::endl;
      // there may be new sinks due to vertices whose strategy is not defined.
      ldd new_Vsinks = compute_deadlocks(V, new_G);


      symbolic_pbessolve_algorithm check(new_G);

      auto [result, solution_prime] = check.solve(initial_vertex, V, new_Vsinks);
      if (!(W0 == solution_prime.winning[0] && W1 == solution_prime.winning[1] && result != alpha))
      {
        throw mcrl2::runtime_error("Computed strategy does not match the winning partition");
      }
    }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_PBES_SYMBOLIC_PBESSOLVE_H
