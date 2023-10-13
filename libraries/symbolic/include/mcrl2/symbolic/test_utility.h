// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_SYMBOLIC_TEST_UTILITY_H
#define MCRL2_SYMBOLIC_TEST_UTILITY_H

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/utilities/logger.h"

#include <sylvan_ldd.hpp>

#include <random>
#include <vector>

namespace mcrl2::symbolic
{

std::mt19937& gen()
{
  static thread_local std::random_device rd;
  static thread_local std::mt19937 gen(rd());
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
sylvan::ldds::ldd random_set(std::size_t amount, std::size_t length, std::size_t max_value)
{
  sylvan::ldds::ldd result;

  for (std::size_t i = 0; i < amount; ++i)
  {
    std::vector<std::uint32_t> random = random_vector(length, max_value);
    result = union_cube(result, random);
  }

  return result;
}

/// \brief Returns a random subset of U.
sylvan::ldds::ldd random_subset(const sylvan::ldds::ldd& U, std::size_t amount)
{
    std::uniform_int_distribution<> dist(0, satcount(U));

    std::vector<std::vector<std::uint32_t>> contained = ldd_solutions(U);
    std::vector<std::vector<std::uint32_t>> result_vector;

    // Choose amount vectors.
    std::size_t added = 0;
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
    }

    // Construct the ldd.
    sylvan::ldds::ldd result;
    for (const auto& vector : result_vector)
    {
      result = union_cube(result, vector);
    }

    return result;  
}

/// \brief Initialise the Sylvan library.
void initialise_sylvan()
{
  //mcrl2::log::logger::set_reporting_level(mcrl2::log::debug);
  lace_start(1, 1024*1024*4);
  lace_set_stacksize(0);
  sylvan::sylvan_set_limits(1024 * 1024 * 1024, 6, 6);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_ldd();
}

/// \brief Destroy the Sylvan library.
void quit_sylvan()
{
  sylvan::sylvan_quit();
  lace_stop();
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_SYMBOLIC_TEST_UTILITY_H
