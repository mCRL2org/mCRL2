// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE symbolic_parity_game_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(dummy_test)
{
  // This is an empty test since at least one test is required.
}

/*
// Commented this test since Sylvan causes some address sanitizer failures that I cannot resolve.
#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/symbolic_parity_game.h"
#include "mcrl2/pbes/symbolic_pbessolve.h"
#include "mcrl2/pbes/pbes_summand_group.h"
#include "mcrl2/symbolic/data_index.h"
#include "mcrl2/symbolic/test_utility.h"

#include <sylvan_ldd.hpp>

#include <vector>
#include <random>

using namespace mcrl2::pbes_system;
using namespace mcrl2::symbolic;
using sylvan::ldds::ldd;

/// A single randomly generated transition group for parameter i
summand_group single_transition_group(const mcrl2::data::variable_list& parameters, std::size_t i, std::size_t N, std::size_t D)
{
  boost::dynamic_bitset<> dependencies(parameters.size()*2);
  dependencies[i] = 1;
  dependencies[i+1] = 1;

  summand_group group(parameters, dependencies, false);
  group.L = random_set(N, 2, D);

  return group;
}

/// \brief Generates a random parity game with N vertices and at most P priorities.
std::tuple<ldd, ldd, std::vector<ldd>, std::vector<summand_group>, std::vector<data_expression_index>> compute_random_game(std::size_t N, std::size_t P)
{  
  std::size_t D = 10; // Size of the state space domain.
  std::size_t length = 4; // Length of the state vector.

  ldd V = random_set(N, length, D);
  ldd Veven = random_subset(V, N / 2);

  ldd remaining = V;
  std::vector<ldd> prio(P);
  for (std::size_t i = 0; i < P; ++i)
  {
    prio[i] = random_subset(remaining, N/P);
    remaining = minus(remaining, prio[i]);
  }

  // Define some list of parameters.
  mcrl2::data::variable_list parameters;
  for (std::size_t j = 0; j < length; ++j)
  {
    parameters.emplace_front(mcrl2::data::variable());
  }

  // Generate random transition groups.
  std::vector<summand_group> groups;
  groups.emplace_back(single_transition_group(parameters, 0, 3*D, D));
  groups.emplace_back(single_transition_group(parameters, 1, 3*D, D));
  groups.emplace_back(single_transition_group(parameters, 2, 3*D, D));
  groups.emplace_back(single_transition_group(parameters, 3, 3*D, D));

  std::vector<data_expression_index> index;

  return std::make_tuple(V, Veven, prio, groups, index);
}

/// \returns Minimal fixpoint of the given (monotone) predicate transformer A -> A.
template<typename F>
ldd least_fixpoint(F pred)
{  
  ldd prev, current;
  do 
  {
    prev = current;
    current = pred(current);
  }
  while (current != prev);

  return current;
}

/// \returns Maximal fixpoint of the given (monotone) predicate transformer A -> A.
template<typename F>
ldd greatest_fixpoint(F pred, ldd V)
{  
  ldd prev;
  ldd current = V;
  do 
  {
    prev = current;
    current = pred(current);
  }
  while (current != prev);

  return current;
}

/// \returns The predecessors of U.
ldd pre(const symbolic_parity_game& G, const ldd& V, const ldd& U)
{
  return G.predecessors(V, U);
}

/// \returns The control predecessors of U.
ldd cpre(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& U)
{
  std::array<const ldd, 2> Vplayer = G.players(V);

  return union_(
    intersect(Vplayer[alpha],        pre(G, V, U)),
    minus(Vplayer[1 - alpha], union_(pre(G, V, minus(V, U)), G.sinks(V, V)))
  );
}

/// \returns Standard attractor set computation.
ldd attr(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& U)
{
  return least_fixpoint(
    [&](const ldd& Z) -> ldd {
      return union_(U, cpre(G, V, alpha, Z));
    });
}

/// \returns The safe control predecessors of U.
ldd spre(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& U, const ldd& I)
{
  std::array<const ldd, 2> Vplayer = G.players(V);

  return union_(
    intersect(Vplayer[alpha], G.predecessors(V, U)),
    minus(Vplayer[1 - alpha], union_(union_(G.predecessors(V, minus(V, U)), G.sinks(V, V)), I))
  );
}

/// \returns Safe attractor set computation.
ldd sattr(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& U, const ldd& I)
{
  return least_fixpoint(
    [&](const ldd& Z) -> ldd {
      return union_(U, spre(G, V, alpha, Z, I));
    });
}

/// \brief Computes the monotone control predecessor.
ldd Mcpre(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& Z, const ldd& U, std::size_t c)
{
  return intersect(G.prio_above(V, c), cpre(G, V, alpha, union_(Z, U)));
}

/// \brief Computes monotone attractor for priority c.
ldd Mattr(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& U, std::size_t c)
{
  return least_fixpoint(
    [&](const ldd& Z) -> ldd
    {
      return Mcpre(G, V, alpha, Z, U, c);
    }
  );
}

/// \returns Safe vertices computation.
ldd safe(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& I)
{
  std::array<const ldd, 2> Vplayer = G.players(V);
  return minus(V, attr(G, V, 1-alpha, intersect(Vplayer[1-alpha], I)));
}

/// \brief Computes fatal attractor for priority c.
ldd fatal_attractor(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, std::size_t c, const ldd& I)
{
  ldd Pequal = intersect(V, G.ranks().at(c));
  const ldd Vsafe = safe(G, V, alpha, I);

  return greatest_fixpoint(
    [&](const ldd& Z) -> ldd
    {
      // G \cap safe(G) is here represented by restricting V to Vsafe.
      return intersect(intersect(Pequal, Vsafe), Mattr(G, Vsafe, alpha, Z, c));
    },
    Vsafe
  );
}

/// \brief Computes the safe monotone control predecessor.
ldd sMcpre(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& Z, const ldd& U, std::size_t c, const ldd& I)
{
  return intersect(G.prio_above(V, c), spre(G, V, alpha, union_(Z, U), I));
}

/// \brief Computes monotone attractor for priority c.
ldd sMattr(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& U, std::size_t c, const ldd& I)
{
  return least_fixpoint(
    [&](const ldd& Z) -> ldd
    {
      return sMcpre(G, V, alpha, Z, U, c, I);
    }
  );
}

/// \brief Computes fatal attractor for priority c.
ldd safe_fatal_attractor(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, std::size_t c, const ldd& I)
{
  ldd Pequal = intersect(V, G.ranks().at(c));

  return greatest_fixpoint(
    [&](const ldd& Z) -> ldd
    {
      return intersect(Pequal, sMattr(G, V, alpha, Z, c, I));
    },
    V
  );
}

/// \brief Returns the solitair cycles.
ldd solitair_cycles(const symbolic_parity_game& G, const ldd& V, std::size_t alpha)
{
  std::array<const ldd, 2> Vplayer = G.players(V);
  std::array<const ldd, 2> parity = G.parity(V);

  return greatest_fixpoint(
    [&](const ldd& Z) -> ldd {
      return intersect(intersect(Vplayer[alpha], parity[alpha]), pre(G, V, Z));
    },
    V
  );
}

/// \brief Returns the forced cycles.
ldd forced_cycles(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& I)
{
  std::array<const ldd, 2> parity = G.parity(V);
  const ldd Vsafe = safe(G, V, alpha, I);

  return greatest_fixpoint(
    [&](const ldd& Z) -> ldd {
      return intersect(intersect(parity[alpha], Vsafe), cpre(G, V, alpha, Z));
    },
    V
  );
}

BOOST_AUTO_TEST_CASE(random_test_attractor)
{
  initialise_sylvan();

  for (std::size_t i = 0; i < 100; ++i)
  {
    auto [V, Veven, prio, groups, index] = compute_random_game(5000, 2);
    symbolic_parity_game G(groups, index, V, Veven, prio, false, false);

    ldd U = random_subset(V, 250);

    std::array<const ldd, 2> Vplayer = G.players(V);
    ldd result = G.safe_attractor(U, 0, V, Vplayer);
    ldd expected = attr(G, V, 0, U);
    std::cerr << satcount(result) << std::endl;

    BOOST_CHECK_EQUAL(result, expected);
  }
  
  quit_sylvan();
}

BOOST_AUTO_TEST_CASE(random_test_safe_attractor)
{
  initialise_sylvan();

  for (std::size_t i = 0; i < 100; ++i)
  {
    auto [V, Veven, prio, groups, index] = compute_random_game(5000, 2);
    symbolic_parity_game G(groups, index, V, Veven, prio, false, false);

    ldd U = random_subset(V, 250);
    ldd I = random_subset(minus(V, U), 250);

    std::array<const ldd, 2> Vplayer = G.players(V);
    ldd result = G.safe_attractor(U, 0, V, Vplayer, I);
    ldd expected = sattr(G, V, 0, U, I);
    std::cerr << satcount(result) << std::endl;

    BOOST_CHECK_EQUAL(result, expected);
  }
  
  quit_sylvan();
}

BOOST_AUTO_TEST_CASE(random_test_chaining_safe_attractor)
{
  initialise_sylvan();

  for (std::size_t i = 0; i < 100; ++i)
  {
    auto [V, Veven, prio, groups, index] = compute_random_game(5000, 2);
    symbolic_parity_game G(groups, index, V, Veven, prio, false, true);

    ldd U = random_subset(V, 250);
    ldd I = random_subset(minus(V, U), 250);

    std::array<const ldd, 2> Vplayer = G.players(V);
    ldd result = G.safe_attractor(U, 0, V, Vplayer, I);
    ldd expected = sattr(G, V, 0, U, I);
    std::cerr << satcount(result) << std::endl;

    BOOST_CHECK_EQUAL(result, expected);
  }
  
  quit_sylvan();
}

BOOST_AUTO_TEST_CASE(random_test_monotone_attractor)
{
  initialise_sylvan();

  for (std::size_t i = 0; i < 100; ++i)
  {
    auto [V, Veven, prio, groups, index] = compute_random_game(5000, 2);
    symbolic_parity_game G(groups, index, V, Veven, prio, false, false);

    ldd U = random_subset(V, 250);

    std::array<const ldd, 2> Vplayer = G.players(V);
    ldd result = G.safe_monotone_attractor(U, 0, 0, V, Vplayer);
    ldd expected = Mattr(G, V, 0, U, 0);
    std::cerr << satcount(result) << std::endl;

    BOOST_CHECK_EQUAL(result, expected);
  }
  
  quit_sylvan();
}

BOOST_AUTO_TEST_CASE(random_test_safe_monotone_attractor)
{
  initialise_sylvan();

  for (std::size_t i = 0; i < 100; ++i)
  {
    auto [V, Veven, prio, groups, index] = compute_random_game(5000, 2);
    symbolic_parity_game G(groups, index, V, Veven, prio, false, false);

    ldd U = random_subset(V, 250);
    ldd I = random_subset(minus(V, U), 250);

    std::array<const ldd, 2> Vplayer = G.players(V);
    ldd result = G.safe_monotone_attractor(U, 0, 0, V, Vplayer, I);
    ldd expected = sMattr(G, V, 0, U, 0, I);
    std::cerr << satcount(result) << std::endl;

    BOOST_CHECK_EQUAL(result, expected);
  }

  quit_sylvan();
}

BOOST_AUTO_TEST_CASE(random_test_chaining_monotone_attractor)
{
  initialise_sylvan();

  for (std::size_t i = 0; i < 100; ++i)
  {
    auto [V, Veven, prio, groups, index] = compute_random_game(5000, 2);
    symbolic_parity_game G(groups, index, V, Veven, prio, false, true);

    ldd U = random_subset(V, 250);
    ldd I = random_subset(minus(V, U), 250);

    std::array<const ldd, 2> Vplayer = G.players(V);
    ldd result = G.safe_monotone_attractor(U, 0, 0, V, Vplayer, I);
    ldd expected = sMattr(G, V, 0, U, 0, I);
    std::cerr << satcount(result) << std::endl;

    BOOST_CHECK_EQUAL(result, expected);
  }

  quit_sylvan();
}

BOOST_AUTO_TEST_CASE(random_test_solitair_cycles)
{
  initialise_sylvan();
  
  for (std::size_t i = 0; i < 1; ++i)
  {
    auto [V, Veven, prio, groups, index] = compute_random_game(100, 2);
    symbolic_parity_game G(groups, index, V, Veven, prio, false, false);
    mcrl2::pbes_system::symbolic_pbessolve_algorithm solver(G);

    ldd initial = random_subset(V, 1);
    ldd I;

    // Use V as initial vertex so it does not terminate early.
    std::pair<ldd, ldd> Vresult = solver.detect_solitair_cycles(V, V, I, false, G.sinks(V, V));

    std::array<ldd, 2> winning;
    ldd Vtotal = G.compute_total_graph(V, sylvan::ldds::empty_set(), G.sinks(V, V), winning);

    std::pair<ldd, ldd> Vexpected = { union_(winning[0], attr(G, V, 0, solitair_cycles(G, V, 0))), 
                                      union_(winning[1], attr(G, V, 1, solitair_cycles(G, V, 1))) };
    BOOST_CHECK_EQUAL(Vresult.first, Vexpected.first);
    BOOST_CHECK_EQUAL(Vresult.second, Vexpected.second);
  }
  
  quit_sylvan();
}

BOOST_AUTO_TEST_CASE(random_test_solitair_cycles)
{
  initialise_sylvan();
  
  for (std::size_t i = 0; i < 1; ++i)
  {
    auto [V, Veven, prio, groups, index] = compute_random_game(500, 2);
    symbolic_parity_game G(groups, index, V, Veven, prio, false, false);
    mcrl2::pbes_system::symbolic_pbessolve_algorithm solver(G);

    ldd initial = random_subset(V, 1);
    ldd I;

    // Use V as initial vertex so it does not terminate early.
    std::pair<ldd, ldd> Vresult = solver.detect_forced_cycles(V, V, I, false, G.sinks(V, V));

    std::array<ldd, 2> winning;
    ldd Vtotal = G.compute_total_graph(V, sylvan::ldds::empty_set(), G.sinks(V, V), winning);

    std::pair<ldd, ldd> Vexpected = { union_(winning[0], attr(G, V, 0, forced_cycles(G, V, 0, I))), 
                                      union_(winning[1], attr(G, V, 1, forced_cycles(G, V, 1, I))) };
    BOOST_CHECK_EQUAL(Vresult.first, Vexpected.first);
    BOOST_CHECK_EQUAL(Vresult.second, Vexpected.second);
  }
  
  quit_sylvan();
}

#endif // MCRL2_ENABLE_SYLVAN
*/
