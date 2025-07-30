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

#include <cstddef>
#include <sylvan_ldd.hpp>

#include <random>
#include <vector>

namespace mcrl2::symbolic
{

inline std::mt19937& gen()
{
  static thread_local std::random_device rd;
  static thread_local std::mt19937 gen(rd());
  return gen;
}

/// \brief Generate a random (state) vector.
inline std::vector<std::uint32_t> random_vector(std::size_t length, std::size_t max_value)
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
inline sylvan::ldds::ldd random_set(std::size_t amount, std::size_t length, std::size_t max_value)
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
inline sylvan::ldds::ldd random_subset(const sylvan::ldds::ldd& U, std::size_t amount)
{
    std::uniform_int_distribution<> dist(0, satcount(U));

    std::vector<std::vector<std::uint32_t>> contained = ldd_solutions(U);
    std::vector<std::vector<std::uint32_t>> result_vector;

    // Choose amount vectors.
    std::size_t added = 0;
    for (const auto& vector : contained)
    {
      if (static_cast<std::size_t>(dist(gen())) <= amount)
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

/// Returns a random vector set containing at most number_of_insertions vectors of the given length with values in 0..max_value.
inline std::set<std::vector<std::uint32_t>>
random_vector_set(std::uint32_t length, std::uint32_t max_value, std::uint32_t number_of_insertions)
{
  std::set<std::vector<std::uint32_t>> result;

  for (std::size_t i = 0; i < number_of_insertions; ++i)
  {
    result.insert(random_vector(length, max_value));
  }

  return result;
}

inline sylvan::ldds::ldd to_ldd(const std::set<std::vector<std::uint32_t>>& vector_set)
{
  sylvan::ldds::ldd result;

  for (const auto& value : vector_set)
  {
    result = union_cube(result, value);
  }

  return result;
}

/// \brief Initialise the Sylvan library.
inline void initialise_sylvan()
{
  //mcrl2::log::logger::set_reporting_level(mcrl2::log::debug);
  lace_init(1, static_cast<size_t>(1024 * 1024 * 4));
  lace_startup(0, nullptr, nullptr);
  sylvan::sylvan_set_limits(static_cast<size_t>(1024) * 1024 * 1024, 6, 6);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_ldd();
}

/// \brief Destroy the Sylvan library.
inline void quit_sylvan()
{
  sylvan::sylvan_quit();
  lace_exit();
}

} // namespace mcrl2::symbolic

#endif // MCRL2_ENABLE_SYLVAN

#endif // MCRL2_SYMBOLIC_TEST_UTILITY_H
