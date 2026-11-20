// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesinst_fatal_attractors_test.cpp
/// \brief Regression tests for the partial solvers in pbesinst_fatal_attractors.h.

#define BOOST_TEST_MODULE pbesinst_fatal_attractors_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/pbes/pbesinst_fatal_attractors.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

using decoration = structure_graph::decoration_type;

namespace
{

structure_graph::vertex_vector make_graph(const std::vector<std::pair<decoration, std::size_t>>& nodes,
    const std::vector<std::pair<structure_graph::index_type, structure_graph::index_type>>& edges)
{
  std::vector<structure_graph::vertex> vertices;
  for (const auto& [d, rank]: nodes)
  {
    vertices.emplace_back(true_(), d, rank);
  }
  for (const auto& [from, to]: edges)
  {
    vertices[from].successors.push_back(to);
    vertices[to].predecessors.push_back(from);
  }
  structure_graph::vertex_vector V;
  for (const auto& x: vertices)
  {
    V.push_back(x);
  }
  return V;
}

} // namespace

// Counterexample for the update Y := Y \ Attr_{1-alpha}(Y \ X) computed over the whole graph. Player 1 wins every
// vertex: every play ends in the self-loop at d or cycles between w and z, both of (odd) rank 1. The buggy version
// reported v as won by player 0, with an undefined strategy.
BOOST_AUTO_TEST_CASE(test_fatal_attractors_y_not_a_trap)
{
  const structure_graph::index_type v = 0, u = 1, w = 2, z = 3, d = 4;
  structure_graph::vertex_vector V = make_graph(
      {{decoration::d_disjunction, 0}, // v
       {decoration::d_conjunction, 0}, // u
       {decoration::d_disjunction, 1}, // w
       {decoration::d_disjunction, 1}, // z
       {decoration::d_conjunction, 1}}, // d
      {{v, d}, {v, w}, {u, d}, {w, u}, {w, z}, {z, w}, {d, d}});
  simple_structure_graph G(V);

  for (int variant = 0; variant < 2; ++variant)
  {
    std::array<vertex_set, 2> S = {vertex_set(V.size()), vertex_set(V.size())};
    std::array<strategy_vector, 2> tau;
    std::size_t steps = 0;
    if (variant == 0)
    {
      detail::fatal_attractors(G, S, tau, steps, 0);
    }
    else
    {
      detail::find_loops2(G, S, tau, steps, 0);
    }

    // No vertex may be claimed for player 0.
    BOOST_CHECK(S[0].is_empty());
    for (structure_graph::index_type x: S[0].vertices())
    {
      BOOST_TEST_MESSAGE("vertex " << x << " wrongly assigned to player 0 (variant " << variant << ")");
    }
    // Every winning vertex of the owning player must have a defined strategy.
    for (std::size_t alpha = 0; alpha < 2; ++alpha)
    {
      for (structure_graph::index_type x: S[alpha].vertices())
      {
        if (static_cast<std::size_t>(G.decoration(x)) == alpha)
        {
          BOOST_CHECK(tau[alpha][x] != undefined_vertex());
        }
      }
    }
  }
}
