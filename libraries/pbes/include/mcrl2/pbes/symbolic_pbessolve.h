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

namespace mcrl2 {

namespace pbes_system {

typedef sylvan::ldds::ldd vertex_set;
typedef sylvan::ldds::ldd vertex;

inline
std::ostream& operator<<(std::ostream& out, const sylvan::ldds::ldd& x)
{
  return out << sylvan::ldds::print_ldd(x);
}

// Returns true if V is a subset of U
inline
bool includes(const sylvan::ldds::ldd& U, const sylvan::ldds::ldd& V)
{
  using namespace sylvan::ldds;
  return intersect(U, V) == V;
}

class symbolic_pbessolve_algorithm
{
  typedef sylvan::ldds::ldd ldd;

  protected:
    ldd m_all_nodes; // the set of nodes of the parity game
    ldd m_V[2]; // m_V_[0] is the set of even nodes, m_V_[1] is the set of odd nodes
    const std::vector<summand_group>& m_summand_groups;
    const std::vector<std::size_t>& m_rank;
    std::map<std::size_t, ldd> m_rank_map;
    bool m_no_relprod = false;

  public:
    symbolic_pbessolve_algorithm(
      const ldd& V,
      std::size_t m, // the number of parameters
      const std::vector<summand_group>& summand_groups,
      const std::vector<std::size_t>& rank, // rank[i] is the rank of equation i
      const std::set<std::size_t>& even // the indices of the even equations
    )
      : m_all_nodes(V), m_summand_groups(summand_groups), m_rank(rank)
    {
      using namespace sylvan::ldds;
      using utilities::detail::contains;

      m_V[0] = empty_set();
      m_V[1] = empty_set();

      std::size_t n = rank.size();
      std::vector<std::uint32_t> I_values;
      for (std::uint32_t i = 0; i < m; i++)
      {
        I_values.push_back(i == 0 ? 0 : 1);
      }
      ldd V1 = project(V, cube(I_values)); // The LDD m_V without the first layer
      for (std::size_t i = 0; i < n; i++)
      {
        ldd P_i = intersect(V, node(i, V1)); // TODO: can this be implemented using sylvan::ldds::match?

        std::size_t rank_i = m_rank[i];
        auto j = m_rank_map.find(rank_i);
        if (j == m_rank_map.end())
        {
          m_rank_map[rank_i] = P_i;
        }
        else
        {
          j->second = union_(j->second, P_i);
        }

        if (contains(even, i))
        {
          m_V[0] = union_(m_V[0], P_i);
        }
        else
        {
          m_V[1] = union_(m_V[1], P_i);
        }
      }
    }

    std::pair<vertex_set, vertex_set> solve_recursive(const vertex_set& V, const vertex_set& A)
    {
      using namespace sylvan::ldds;
      return solve_recursive(minus(V, A));
    }

    ldd predecessors(const ldd& U)
    {
      using namespace sylvan::ldds;
      const auto& R = m_summand_groups;
      const auto& V = m_all_nodes;

      ldd result = empty_set();
      for (std::size_t i = 0; i < R.size(); i++)
      {
        ldd prev_i = m_no_relprod ? lps::alternative_relprev(U, R[i], V) : relprev(U, R[i].L, R[i].Ir, V);
        result = union_(result, prev_i);
      }
      return result;
    }

    ldd attractor(std::size_t alpha, const ldd& U)
    {
      using namespace sylvan::ldds;
      const ldd& V = m_all_nodes;
      const ldd* V_ = m_V;

      ldd X = empty_set();
      ldd X_ = U;
      while (X != X_)
      {
        X = X_;
        ldd X1 = intersect(V_[alpha], predecessors(X_));
        ldd X2 = intersect(V_[1 - alpha], minus(V, predecessors(minus(V, X_))));
        X_ = union_(X_, union_(X1, X2));
      }
      return X;
    }

    // Returns (min, Vmin) with
    //   min is the minimum rank in V
    //   Vmin is the set of vertices with the minimum rank in V
    std::pair<std::size_t, vertex_set> get_min_rank(const vertex_set& V)
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
    std::pair<vertex_set, vertex_set> solve_recursive(const vertex_set& V)
    {
      using namespace sylvan::ldds;
      mCRL2log(log::debug) << "\n  --- solve_recursive input ---\n" << V << std::endl;

      if (V == empty_set())
      {
        return { empty_set(), empty_set() };
      }

      auto [m, U] = get_min_rank(V);

      std::size_t alpha = m % 2; // 0 = disjunctive, 1 = conjunctive

      vertex_set W[2] = { empty_set(), empty_set() };
      vertex_set W_1[2];

      vertex_set A = attractor(alpha, U);
      std::tie(W_1[0], W_1[1]) = solve_recursive(V, A);

      // Original Zielonka version
      if (W_1[1 - alpha] == empty_set())
      {
        W[alpha] = union_(A, W_1[alpha]);
        W[1 - alpha] = empty_set();
      }
      else
      {
        vertex_set B = attractor(1 - alpha, W_1[1 - alpha]);
        std::tie(W[0], W[1]) = solve_recursive(V, B);
        W[1 - alpha] = union_(W[1 - alpha], B);
      }

      mCRL2log(log::debug) << "\n  --- solution for solve_recursive input ---\n" << V << std::endl;
      mCRL2log(log::debug) << "   W0 = " << W[0] << std::endl;
      mCRL2log(log::debug) << "   W1 = " << W[1] << std::endl;
      return { W[0], W[1] };
    }

  public:
    bool solve(const vertex_set& V, const vertex& initial_vertex)
    {
      using namespace sylvan::ldds;

      mCRL2log(log::verbose) << "Solving parity game..." << std::endl;
      mCRL2log(log::debug) << V << std::endl;
      auto [W0, W1] = solve_recursive(V);
      bool is_disjunctive;
      if (includes(W0, initial_vertex))
      {
        is_disjunctive = true;
      }
      else if (includes(W1, initial_vertex))
      {
        is_disjunctive = false;
      }
      else
      {
        throw mcrl2::runtime_error("No solution found!!!");
      }
      return is_disjunctive;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SYMBOLIC_PBESSOLVE_H
