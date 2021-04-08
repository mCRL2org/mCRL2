// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/symbolic_pbessolve.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_SYMBOLIC_PBESSOLVE_H
#define MCRL2_PBES_SYMBOLIC_PBESSOLVE_H

#include <sylvan_ldd.hpp>
#include <boost/dynamic_bitset.hpp>
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/lps/symbolic_reachability.h"
#include "mcrl2/pbes/pbesreach.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

using sylvan::ldds::ldd;

inline
std::string print_pbes_info(const srf_pbes& pbesspec)
{
  std::ostringstream out;
  pbes_equation_index equation_index(pbesspec);
  for (const auto& equation: pbesspec.equations())
  {
    const auto& name = equation.variable().name();
    out << name << " rank = " << equation_index.rank(name) << " decoration = " << (equation.is_conjunctive() ? "conjunctive" : "disjunctive") << std::endl;
  }
  return out.str();
}

// print the subgraph U of V
template <typename SummandGroup>
std::string print_graph(
  const ldd& U,
  const ldd& V,
  const std::vector<SummandGroup>& R,
  const std::vector<lps::data_expression_index>& data_index,
  const ldd& V0, // disjunctive nodes
  const std::map<std::size_t, ldd>& rank_map // maps rank to the corresponding set of nodes
)
{
  using namespace sylvan::ldds;
  using utilities::detail::contains;

  auto rank = [&](const ldd& u)
  {
    for (const auto& [r, U]: rank_map)
    {
      if (includes(U, u))
      {
        return r;
      }
    }
    throw mcrl2::runtime_error("print_graph: could not find a rank");
  };

  auto index = [](const std::vector<ldd>& v, const ldd& x)
  {
    auto i = std::find(v.begin(), v.end(), x);
    if (i == v.end())
    {
      throw mcrl2::runtime_error("print_graph: index error");
    }
    return i - v.begin();
  };

  auto values = [](const ldd& X)
  {
    std::vector<ldd> result;
    auto X_elements = ldd_solutions(X);
    for (const auto& x: X_elements)
    {
      result.push_back(cube(x));
    }
    return std::make_pair(result, X_elements);
  };

  auto succ = [&](const ldd& U)
  {
    ldd result = empty_set();
    for (std::size_t i = 0; i < R.size(); i++)
    {
      result = union_(result, alternative_relprod(U, R[i]));
    }
    return result;
  };

  auto [U_values, U_solutions] = values(U);
  auto [V_values, V_solutions] = values(V);

  std::vector<std::string> text(U_values.size());

  for (std::size_t i = 0; i < U_values.size(); i++)
  {
    ldd u = U_values[i];
    std::size_t u_index = index(V_values, u);

    ldd W = succ(u);
    auto [W_values, W_solutions] = values(W);
    std::vector<std::uint32_t> u_successors;
    for (const ldd& w: W_values)
    {
      if (contains(U_values, w))
      {
        u_successors.push_back(index(V_values, w));
      }
    }
    text[i] = std::to_string(u_index) + " " + print_state(data_index, U_solutions[i]) + ", decoration = " + (includes(V0, u) ? "disjunctive" : "conjunctive") + ", rank = " + std::to_string(rank(u)) + ", successors = " + core::detail::print_list(u_successors);
  }
  return utilities::string_join(text, "\n");
}

// print the indices of U (subset of V)
std::string print_nodes(const ldd& U, const ldd& V)
{
  using namespace sylvan::ldds;

  auto index = [](const std::vector<ldd>& v, const ldd& x)
  {
    auto i = std::find(v.begin(), v.end(), x);
    if (i == v.end())
    {
      throw mcrl2::runtime_error("print_graph: index error");
    }
    return i - v.begin();
  };

  auto values = [](const ldd& X)
  {
    std::vector<ldd> result;
    auto X_elements = ldd_solutions(X);
    for (const auto& x: X_elements)
    {
      result.push_back(cube(x));
    }
    return std::make_pair(result, X_elements);
  };

  auto [V_values, V_solutions] = values(V);
  auto [U_values, W0_solutions] = values(U);

  std::vector<std::size_t> u;
  for (const ldd& x: U_values)
  {
    u.push_back(index(V_values, x));
  }
  return core::detail::print_set(u);
}

class symbolic_pbessolve_algorithm
{
  protected:
    ldd m_V[2]; // m_V[0] is the set of even nodes, m_V[1] is the set of odd nodes
    ldd m_W[2]; // m_W[0] is the set of nodes that even won after solving and m_W[1] is the set of nodes that odd wins.
    const std::vector<summand_group>& m_summand_groups;
    std::map<std::size_t, ldd> m_rank_map;
    bool m_no_relprod = false;
    bool m_chaining = false;

    const std::vector<lps::data_expression_index>& m_data_index; // for debugging only
    ldd m_all_nodes; // for debugging only

  public:
    symbolic_pbessolve_algorithm(
      const ldd& V,
      const std::vector<summand_group>& summand_groups,
      const std::map<std::size_t, std::pair<std::size_t, bool>>& equation_info, // maps ldd values to (rank, is_disjunctive)
      bool no_relprod,
      bool chaining,
      const std::vector<lps::data_expression_index>& data_index
    )
      : m_summand_groups(summand_groups), m_no_relprod(no_relprod), m_chaining(chaining), m_data_index(data_index), m_all_nodes(V)
    {
      using namespace sylvan::ldds;
      using utilities::detail::contains;

      m_V[0] = empty_set();
      m_V[1] = empty_set();

      // determine the rank and owner of all states.
      for (const auto& [value, p]: equation_info)
      {
        auto rank = p.first;
        auto is_disjunctive = p.second;
        ldd X = fix_first_element(V, value);

        auto j = m_rank_map.find(rank);
        if (j == m_rank_map.end())
        {
          m_rank_map[rank] = X;
        }
        else
        {
          j->second = union_(j->second, X);
        }

        if (is_disjunctive)
        {
          m_V[0] = union_(m_V[0], X);
        }
        else
        {
          m_V[1] = union_(m_V[1], X);
        }
      }
    }

    /// \returns The set { u in U | exists v in V: u -> v }, where -> is described by the given group.
    ldd predecessors(const ldd& U, const ldd& V, const summand_group& group)
    {
      return m_no_relprod ? lps::alternative_relprev(V, group, U) : relprev(V, group.L, group.Ir, U);
    }

    /// \returns The set { u in U | exists v in V: u -> v }
    ldd predecessors(const ldd& U, const ldd& V)
    {
      using namespace sylvan::ldds;

      ldd result = m_chaining ? V : empty_set();
      for (std::size_t i = 0; i < m_summand_groups.size(); ++i)
      {
        const summand_group& group = m_summand_groups[i];

        auto group_start = std::chrono::steady_clock::now();
        result = union_(result, predecessors(U, m_chaining ? result : V, group));

        std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - group_start;
        mCRL2log(log::debug) << "added predecessors for group " << i << " out of " << m_summand_groups.size()
                               << " (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)\n";
      }
      return result;
    }

    /// \brief Compute the attractor set for U.
    /// \param alpha the current player
    /// \param V is the set of states
    /// \param Vplayer a partitioning of the nodes into the sets of even nodes V[0] and odd V[1].
    ldd attractor(const ldd& U, std::size_t alpha, const ldd& V, const std::array<const ldd, 2>& Vplayer)
    {      
      auto start = std::chrono::steady_clock::now();
      mCRL2log(log::verbose) << "start attractor set computation\n";
      mCRL2log(log::verbose) << "LDD size = " << nodecount(U) << std::endl;

      using namespace sylvan::ldds;
      const ldd& Valpha = Vplayer[alpha];

      std::size_t iter = 0;
      ldd X = U;
      ldd todo = U;

      while (todo != empty_set())
      {
        auto iter_start = std::chrono::steady_clock::now();

        // The predecessors of the todo set; we update the todo set in this iteration to only include newly added states.
        ldd P = predecessors(V, todo);

        // Determine current player's nodes that can reach X (and X itself).
        ldd Palpha = intersect(P, Valpha);
        todo = minus(Palpha, X);
        X = union_(X, Palpha);

        // Determine nodes outside of X.
        ldd Xoutside = minus(V, X);

        // The nodes of the other player in the predecessors that are not part of the attractor set (note that Xnext contains all Valpha nodes in Xpred).
        ldd Pforced = minus(P, X);
        for (std::size_t i = 0; i < m_summand_groups.size(); ++i)
        {
          const summand_group& group = m_summand_groups[i];

          auto group_start = std::chrono::steady_clock::now();
          Pforced = minus(Pforced, predecessors(Pforced, Xoutside, group));

          std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - group_start;
          mCRL2log(log::debug) << "removed 1 - alpha predecessors for group " << i << " out of " << m_summand_groups.size()
                                 << " (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)\n";
        }

        todo = union_(todo, Pforced);
        X = union_(X, Pforced);

        std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - iter_start;
        mCRL2log(log::verbose) << "attractor set iteration " << iter << " (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)\n";
        mCRL2log(log::verbose) << "attractor LDD size = " << nodecount(X) << " and todo LDD size = " << nodecount(todo) << std::endl;

        ++iter;
      }

      std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;
      mCRL2log(log::verbose) << "finished attractor set computation (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)\n";
      mCRL2log(log::verbose) << "attractor LDD size = " << nodecount(X) << std::endl;
      return X;
    }

    // Returns (min, Vmin) with
    //   min is the minimum rank in V
    //   Vmin is the set of vertices with the minimum rank in V
    std::pair<std::size_t, ldd> get_min_rank(const ldd& V)
    {
      using namespace sylvan::ldds;

      for (auto i = m_rank_map.begin(); i != m_rank_map.end(); ++i)
      {
        ldd Vmin = intersect(V, i->second);
        if (Vmin != empty_set())
        {
          std::size_t min_rank = i->first;
          return { min_rank, Vmin };
        }
      }

      throw mcrl2::runtime_error("get_min_rank did not find any nodes");
    }

    // pre: V does not contain nodes with decoration true or false.
    std::pair<ldd, ldd> zielonka(const ldd& V)
    {
      using namespace sylvan::ldds;

      if (V == empty_set())
      {
        return { empty_set(), empty_set() };
      }

      auto start = std::chrono::steady_clock::now();
      mCRL2log(log::verbose) << "start zielonka recursion\n";
      auto [m, U] = get_min_rank(V);

      std::size_t alpha = m % 2; // 0 = disjunctive, 1 = conjunctive

      // Compute the partitioning of V for players 0 (in V[0]) and 1 (in V[1]).
      std::array<const ldd, 2> Vplayer = { intersect(V, m_V[0]), intersect(V, m_V[1]) };

      ldd W[2];
      ldd W_1[2];

      ldd A = attractor(U, alpha, V, Vplayer);
      mCRL2log(log::debug) << "A = attractor(" << print_nodes(U, m_all_nodes) << ", " << print_nodes(V, m_all_nodes) << ") = " << print_nodes(A, m_all_nodes) << std::endl;
      std::tie(W_1[0], W_1[1]) = zielonka(minus(V, A));

      // Original Zielonka version
      if (W_1[1 - alpha] == empty_set())
      {
        W[alpha] = union_(A, W_1[alpha]);
        W[1 - alpha] = empty_set();
      }
      else
      {
        ldd B = attractor(W_1[1 - alpha], 1 - alpha, V, Vplayer);
        mCRL2log(log::debug) << "B = attractor(" << print_nodes(W_1[1 - alpha], m_all_nodes) << ", " << print_nodes(V, m_all_nodes) << ") = " << print_nodes(B, m_all_nodes) << std::endl;
        std::tie(W[0], W[1]) = zielonka(minus(V, B));
        W[1 - alpha] = union_(W[1 - alpha], B);
      }

      std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;
      mCRL2log(log::verbose) << "finished zielonka recursion (time = " << std::setprecision(2) << std::fixed << elapsed_seconds.count() << "s)\n";

      mCRL2log(log::debug) << "\n  --- zielonka solution for ---\n" << print_graph(V, m_all_nodes, m_summand_groups, m_data_index, m_V[0], m_rank_map) << std::endl;
      mCRL2log(log::debug) << "W0 = " << print_nodes(W[0], m_all_nodes) << std::endl;
      mCRL2log(log::debug) << "W1 = " << print_nodes(W[1], m_all_nodes) << std::endl;
      assert(union_(W[0], W[1]) == V);
      return { W[0], W[1] };
    }

  public:
    bool solve(const ldd& V,
               const ldd& initial_vertex,
               const ldd& Vdeadlock = sylvan::ldds::empty_set(),
               const ldd& W0 = sylvan::ldds::empty_set(),
               const ldd& W1 = sylvan::ldds::empty_set())
    {
      using namespace sylvan::ldds;

      std::array<const ldd, 2> Vplayer = { intersect(V, m_V[0]), intersect(V, m_V[1]) };
      m_W[0] = W0;
      m_W[1] = W1;

      if (Vdeadlock != empty_set())
      {
        // Determine winners from the deadlocks (the owner loses).
        mCRL2log(log::verbose) << "preprocessing to obtain total graph" << std::endl;
        m_W[0] = union_(m_W[0], intersect(Vdeadlock, m_V[1]));
        m_W[1] = union_(m_W[1], intersect(Vdeadlock, m_V[0]));
      }

      ldd Vtotal = V;
      std::array<const ldd, 2> Vwon = { attractor(m_W[0], 0, V, Vplayer), attractor(m_W[1], 1, V, Vplayer) };

      // We have already determined the winner for the initial vertex.
      if (includes(Vwon[0], initial_vertex))
      {
        return true;
      }
      else if (includes(Vwon[1], initial_vertex))
      {
        return false;
      }

      // After removing the deadlock (winning) states the resulting set of states is a total graph.
      Vtotal = minus(V, union_(Vwon[0], Vwon[1]));

      mCRL2log(log::debug) << "\n--- apply zielonka to ---\n" << print_graph(Vtotal, m_all_nodes, m_summand_groups, m_data_index, m_V[0], m_rank_map) << std::endl;
      auto [win0, win1] = zielonka(Vtotal);
      m_W[0] = union_(m_W[0], win0);
      m_W[1] = union_(m_W[1], win1);

      if (includes(win0, initial_vertex))
      {
        return true;
      }
      else if (includes(win1, initial_vertex))
      {
        return false;
      }
      else
      {
        throw mcrl2::runtime_error("No solution found!");
      }
    }

    /// \returns The set of states won by even (excluding W0 passed to solve).
    ldd W0()
    {
      return m_W[0];
    }

    /// \returns The set of states won by odd (excluding W1 passed to solve).
    ldd W1()
    {
      return m_W[1];
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SYMBOLIC_PBESSOLVE_H
