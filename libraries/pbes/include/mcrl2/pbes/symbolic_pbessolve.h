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
#include "mcrl2/utilities/text_utility.h"

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
  return union_(U, V) == U;
}

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
  const sylvan::ldds::ldd& U,
  const sylvan::ldds::ldd& V,
  const std::vector<SummandGroup>& R,
  const std::vector<lps::data_expression_index>& data_index,
  const sylvan::ldds::ldd& V0, // disjunctive nodes
  const sylvan::ldds::ldd& V1, // conjunctive nodes
  const std::map<std::size_t, sylvan::ldds::ldd>& rank_map // maps rank to the corresponding set of nodes
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
template <typename SummandGroup>
std::string print_nodes(const sylvan::ldds::ldd& U, const sylvan::ldds::ldd& V, const std::vector<SummandGroup>& R, const std::vector<lps::data_expression_index>& data_index)
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
  typedef sylvan::ldds::ldd ldd;

  protected:
    ldd m_V[2]; // m_V[0] is the set of even nodes, m_V[1] is the set of odd nodes
    const std::vector<summand_group>& m_summand_groups;
    std::map<std::size_t, ldd> m_rank_map;
    bool m_no_relprod = false;

    const std::vector<lps::data_expression_index>& m_data_index; // for debugging only
    ldd m_all_nodes; // for debugging only

  public:
    symbolic_pbessolve_algorithm(
      const ldd& V,
      std::size_t m, // the number of parameters
      const std::vector<summand_group>& summand_groups,
      const std::map<std::size_t, std::pair<std::size_t, bool>>& equation_info, // maps ldd values to (rank, is_disjunctive)
      bool no_relprod,
      const std::vector<lps::data_expression_index>& data_index
    )
      : m_summand_groups(summand_groups), m_no_relprod(no_relprod), m_data_index(data_index), m_all_nodes(V)
    {
      using namespace sylvan::ldds;
      using utilities::detail::contains;

      m_V[0] = empty_set();
      m_V[1] = empty_set();

      std::vector<std::uint32_t> I_values;

      // Construct m_rank_map in a very inefficient manner.
      // TODO: implement this using Sylvan
      for (const auto& v_values: ldd_solutions(V))
      {
        auto [rank, is_disjunctive] = equation_info.at(v_values[0]);
        auto j = m_rank_map.find(rank);
        if (j == m_rank_map.end())
        {
          m_rank_map[rank] = cube(v_values);
        }
        else
        {
          j->second = union_cube(j->second, v_values);
        }
        if (is_disjunctive)
        {
          m_V[0] = union_cube(m_V[0], v_values);
        }
        else
        {
          m_V[1] = union_cube(m_V[1], v_values);
        }
      }
    }

    // returns { u in U | exists v in V: u -> v }
    ldd predecessors(const ldd& U, const ldd& V)
    {
      using namespace sylvan::ldds;
      const auto& R = m_summand_groups;

      ldd result = empty_set();
      for (std::size_t i = 0; i < R.size(); i++)
      {
        ldd prev_i = m_no_relprod ? lps::alternative_relprev(V, R[i], U) : relprev(V, R[i].L, R[i].Ir, U);
        result = union_(result, prev_i);
      }
      return result;
    }

    ldd attractor(const ldd& U, std::size_t alpha, const ldd& V)
    {
      using namespace sylvan::ldds;
      const ldd* V_ = m_V;

      ldd X = empty_set();
      ldd X_ = U;
      while (X != X_)
      {
        X = X_;
        ldd X1 = intersect(intersect(V, V_[alpha]), predecessors(V, X_));
        ldd X2 = intersect(intersect(V, V_[1 - alpha]), minus(V, predecessors(V, minus(V, X_))));
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
    std::pair<vertex_set, vertex_set> zielonka(const vertex_set& V)
    {
      using namespace sylvan::ldds;

      if (V == empty_set())
      {
        return { empty_set(), empty_set() };
      }

      auto [m, U] = get_min_rank(V);

      std::size_t alpha = m % 2; // 0 = disjunctive, 1 = conjunctive

      vertex_set W[2];
      vertex_set W_1[2];

      vertex_set A = attractor(U, alpha, V);
      mCRL2log(log::debug) << "A = attractor(" << print_nodes(U, m_all_nodes, m_summand_groups, m_data_index) << ", " << print_nodes(V, m_all_nodes, m_summand_groups, m_data_index) << ") = " << print_nodes(A, m_all_nodes, m_summand_groups, m_data_index) << std::endl;
      std::tie(W_1[0], W_1[1]) = zielonka(minus(V, A));

      // Original Zielonka version
      if (W_1[1 - alpha] == empty_set())
      {
        W[alpha] = union_(A, W_1[alpha]);
        W[1 - alpha] = empty_set();
      }
      else
      {
        vertex_set B = attractor(W_1[1 - alpha], 1 - alpha, V);
        mCRL2log(log::debug) << "B = attractor(" << print_nodes(W_1[1 - alpha], m_all_nodes, m_summand_groups, m_data_index) << ", " << print_nodes(V, m_all_nodes, m_summand_groups, m_data_index) << ") = " << print_nodes(B, m_all_nodes, m_summand_groups, m_data_index) << std::endl;
        std::tie(W[0], W[1]) = zielonka(minus(V, B));
        W[1 - alpha] = union_(W[1 - alpha], B);
      }

      mCRL2log(log::debug) << "\n  --- zielonka solution for ---\n" << print_graph(V, m_all_nodes, m_summand_groups, m_data_index, m_V[0], m_V[1], m_rank_map) << std::endl;
      mCRL2log(log::debug) << "W0 = " << print_nodes(W[0], m_all_nodes, m_summand_groups, m_data_index) << std::endl;
      mCRL2log(log::debug) << "W1 = " << print_nodes(W[1], m_all_nodes, m_summand_groups, m_data_index) << std::endl;
      assert(union_(W[0], W[1]) == V);
      return { W[0], W[1] };
    }

  public:
    bool solve(const vertex_set& V, const vertex& initial_vertex)
    {
      using namespace sylvan::ldds;

      mCRL2log(log::debug) << "\n--- apply zielonka to ---\n" << print_graph(V, m_all_nodes, m_summand_groups, m_data_index, m_V[0], m_V[1], m_rank_map) << std::endl;
      auto [W0, W1] = zielonka(V);
      if (includes(W0, initial_vertex))
      {
        return true;
      }
      else if (includes(W1, initial_vertex))
      {
        return false;
      }
      else
      {
        throw mcrl2::runtime_error("No solution found!");
      }
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_SYMBOLIC_PBESSOLVE_H
