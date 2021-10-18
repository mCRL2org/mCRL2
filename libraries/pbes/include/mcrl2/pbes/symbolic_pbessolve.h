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

#include "symbolic_parity_game.h"

namespace mcrl2 {

namespace pbes_system {

using sylvan::ldds::ldd;

class symbolic_pbessolve_algorithm
{
  private:
    symbolic_parity_game m_G;

  public:
    symbolic_pbessolve_algorithm(symbolic_parity_game G) :
      m_G(G)
    {}

    std::pair<ldd, ldd> zielonka(const ldd& V)
    {
      using namespace sylvan::ldds;

      if (V == empty_set())
      {
        return { empty_set(), empty_set() };
      }

      stopwatch timer;
      mCRL2log(log::debug) << "start zielonka recursion\n";
      auto [m, U] = m_G.get_min_rank(V);

      std::size_t alpha = m % 2; // 0 = disjunctive, 1 = conjunctive

      // Compute the partitioning of V for players 0 (in V[0]) and 1 (in V[1]).
      std::array<const ldd, 2> Vplayer = m_G.players(V);

      ldd W[2];
      ldd W_1[2];

      ldd A = m_G.safe_attractor(U, alpha, V, Vplayer);
      mCRL2log(log::debug1) << "A = attractor(" << m_G.print_nodes(U) << ", " << m_G.print_nodes(V) << ") = " << m_G.print_nodes(A) << std::endl;
      std::tie(W_1[0], W_1[1]) = zielonka(minus(V, A));

      // Original Zielonka version
      if (W_1[1 - alpha] == empty_set())
      {
        W[alpha] = union_(A, W_1[alpha]);
        W[1 - alpha] = empty_set();
      }
      else
      {
        ldd B = m_G.safe_attractor(W_1[1 - alpha], 1 - alpha, V, Vplayer);
        mCRL2log(log::debug1) << "B = attractor(" << m_G.print_nodes(W_1[1 - alpha]) << ", " << m_G.print_nodes(V) << ") = " << m_G.print_nodes(B) << std::endl;
        std::tie(W[0], W[1]) = zielonka(minus(V, B));
        W[1 - alpha] = union_(W[1 - alpha], B);
      }

      mCRL2log(log::debug) << "finished zielonka recursion (time = " << std::setprecision(2) << std::fixed << timer.seconds() << "s)\n";

      mCRL2log(log::debug1) << "\n  --- zielonka solution for ---\n" << m_G.print_graph(V) << std::endl;
      mCRL2log(log::debug1) << "W0 = " << m_G.print_nodes(W[0]) << std::endl;
      mCRL2log(log::debug1) << "W1 = " << m_G.print_nodes(W[1]) << std::endl;
      assert(union_(W[0], W[1]) == V);
      return { W[0], W[1] };
    }

  public:

    /// \brief Solve the given game restricted to V with Zielonka's recursive algorithm as solver.
    ///        The remaining parameters are sinks, vertices won by even and odd respectively.
    ///        Terminates early when initial_vertex has been solved.
    bool solve(const ldd& initial_vertex,
        const ldd& V,
        const ldd& Vsinks = sylvan::ldds::empty_set(),
        const ldd& W0 = sylvan::ldds::empty_set(),
        const ldd& W1 = sylvan::ldds::empty_set())
    {
      using namespace sylvan::ldds;
      std::array<ldd, 2> winning = { W0, W1 };
      ldd Vtotal = m_G.compute_total_graph(V, empty_set(), Vsinks, winning);
      if (includes(winning[0], initial_vertex))
      {
        return true;
      }
      else if (includes(winning[1], initial_vertex))
      {
        return false;
      }

      // If the initial vertex has not yet been won then run the zielonka solver as well.
      mCRL2log(log::debug1) << "\n--- apply zielonka to ---\n" << m_G.print_graph(V) << std::endl;
      stopwatch timer;
      auto const& [solved0, solved1] = zielonka(Vtotal);
      mCRL2log(log::verbose) << "finished solving (time = " << std::setprecision(2) << std::fixed << timer.seconds() << "s)\n";
      mCRL2log(log::debug1) << "W0 = " << m_G.print_nodes(solved0) << std::endl;
      mCRL2log(log::debug1) << "W1 = " << m_G.print_nodes(solved1) << std::endl;
      if (includes(solved0, initial_vertex))
      {
        return true;
      }
      else if (includes(solved1, initial_vertex))
      {
        return false;
      }
      else
      {
        throw mcrl2::runtime_error("No solution found!");
      }
    }

    /// \brief Solve the given incomplete parity game (m_G, I) restricted to V.
    ///        The remaining parameters are sinks, vertices won by even and odd respectively.
    ///        Terminates early when initial_vertex has been solved.
    std::pair<const ldd, const ldd> partial_solve(const ldd& initial_vertex,
      const ldd& V,
      const ldd& I,
      const ldd& Vsinks = sylvan::ldds::empty_set(),
      const ldd& W0 = sylvan::ldds::empty_set(),
      const ldd& W1 = sylvan::ldds::empty_set())
    {
      using namespace sylvan::ldds;
      std::array<ldd, 2> winning = { W0, W1 };
      std::array<const ldd, 2> Vsafe = m_G.compute_safe_vertices(V, I, Vsinks, winning);
      if (includes(winning[0], initial_vertex))
      {
        return { winning[0], winning[1] };
      }
      else if (includes(winning[1], initial_vertex))
      {
        return { winning[0], winning[1] };
      }

      ldd solved0 = union_(zielonka(Vsafe[0]).first, winning[0]);
      if (includes(solved0, initial_vertex))
      {
        // Terminate early if possible.
        return { solved0, winning[1] };
      }
      ldd solved1 = union_(zielonka(Vsafe[1]).second, winning[1]);
      return { solved0, solved1 };
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SYMBOLIC_PBESSOLVE_H
