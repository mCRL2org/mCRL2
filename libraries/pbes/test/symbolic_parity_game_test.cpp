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

#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/symbolic_parity_game.h"
#include "mcrl2/pbes/pbes_summand_group.h"
#include "mcrl2/symbolic/data_index.h"
#include "mcrl2/utilities/logger.h"

#include <sylvan_ldd.hpp>

#include <vector>
#include <random>

using namespace mcrl2::pbes_system;
using namespace mcrl2::symbolic;
using sylvan::ldds::ldd;

std::mt19937& gen()
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return gen;
}

/// \brief Generate a random (state) vector.
std::vector<std::uint32_t> random_vector(std::size_t length, std::size_t max_value)
{    
  std::uniform_int_distribution<> dist(0, max_value);

  std::vector<std::uint32_t> result(length);
  for (std::size_t i = 0; i < length; ++i)
  {
    result[i] = dist(gen());
  }
  
  return result;
}

/// \brief Generate a set of random (state) vectors.
ldd random_set(std::size_t amount, std::size_t length, std::size_t max_value)
{
  ldd result;

  for (std::size_t i = 0; i < amount; ++i)
  {
    std::vector<std::uint32_t> random = random_vector(length, max_value);
    result = union_cube(result, random);
  }

  return result;
}

/// \brief Returns a random subset of U.
ldd random_subset(const ldd& U, std::size_t amount)
{
    std::uniform_int_distribution<> dist(0, satcount(U));

    std::vector<std::vector<std::uint32_t>> contained = ldd_solutions(U);
    std::vector<std::vector<std::uint32_t>> result_vector;

    // Choose amount vectors.
    std::size_t added = 0;
    std::size_t i = 0;
    for (const auto& vector : contained)
    {
      if (dist(gen()) <= amount)
      {
        result_vector.push_back(vector);
        ++added;
      }

      if (added == amount)
      {
        break;
      }

      ++i;
    }

    // Construct the ldd.
    ldd result;
    for (const auto& vector : result_vector)
    {
      result = union_cube(result, vector);
    }

    return result;  
}

/// A single randomly generated transition group for parameter i
summand_group single_transition_group(const mcrl2::data::variable_list& parameters, std::size_t i, std::size_t N, std::size_t D)
{
  boost::dynamic_bitset<> dependencies(parameters.size()*2);
  dependencies[i] = 1;
  dependencies[i+1] = 1;

  summand_group group(parameters, dependencies);
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
ldd min_fixpoint(F pred, const ldd& U)
{  
  ldd prev;
  ldd current = pred(U);
  do 
  {
    prev = current;
    current = pred(current);
  }
  while (current != prev);

  return current;
}

/// \returns The control predecessors of U.
ldd cpre(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& U)
{
  std::array<const ldd, 2> Vplayer = G.players(V);

  return union_(
    intersect(Vplayer[alpha],        G.predecessors(V, U)),
    minus(Vplayer[1 - alpha], union_(G.predecessors(V, minus(V, U)), G.sinks(V, V)))
  );
}

/// \returns Standard attractor set computation.
ldd attr(const symbolic_parity_game& G, const ldd& V, std::size_t alpha, const ldd& U)
{
  return min_fixpoint(
    [&](const ldd& Z) -> ldd {
      return union_(U, cpre(G, V, alpha, Z));
    },
    U);
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
  return min_fixpoint(
    [&](const ldd& Z) -> ldd {
      return union_(U, spre(G, V, alpha, Z, I));
    },
    U);
}

void initialise_sylvan()
{
  mcrl2::log::logger::set_reporting_level(mcrl2::log::debug);
  lace_init(1, 1024*1024*4);
  lace_startup(0, nullptr, nullptr);
  sylvan::sylvan_set_limits(1024 * 1024 * 1024, 6, 6);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_ldd();
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
}


BOOST_AUTO_TEST_CASE(random_test_chaining_attractor)
{
  initialise_sylvan();

  for (std::size_t i = 0; i < 100; ++i)
  {
    auto [V, Veven, prio, groups, index] = compute_random_game(5000, 2);
    symbolic_parity_game G(groups, index, V, Veven, prio, false, true);

    ldd U = random_subset(V, 250);

    std::array<const ldd, 2> Vplayer = G.players(V);
    ldd result = G.safe_attractor(U, 0, V, Vplayer);
    ldd expected = attr(G, V, 0, U);
    std::cerr << satcount(result) << std::endl;

    BOOST_CHECK_EQUAL(result, expected);
  }
}