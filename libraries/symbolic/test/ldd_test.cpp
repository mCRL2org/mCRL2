// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE ldd_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(dummy_test)
{
  // This is an empty test since at least one test is required.
}

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/symbolic/test_utility.h"
#include "mcrl2/symbolic/bdd_util.h"

#include <sylvan_ldd.hpp>

#include <vector>
#include <random>

using sylvan::ldds::ldd;
using namespace mcrl2::symbolic;

BOOST_AUTO_TEST_CASE(ldd_from_bdd_test)
{
  initialise_sylvan();

  //for (std::size_t i = 0; i < 100; ++i)
  //{
    ldd input = random_set(1, 3, 10);
    std::cerr << input <<std::endl;
    std::vector<std::uint32_t> bits = sylvan::ldds::compute_bits(sylvan::ldds::compute_highest(input));
    for (int i = 0; i < bits.size(); ++i)
    {    
      std::cerr << i << ": " << bits[i] << std::endl; 
    }

    sylvan::bdds::bdd set = sylvan::ldds::bdd_from_ldd(input, bits, 0);
    
    std::vector<uint32_t> variables;
    int i = 0;
    for (uint32_t val: bits)
    {
      for (uint32_t k = 0; k < val; ++k)
      {
        variables.push_back(2*i);
        ++i;
      }
    }
  
    // All of the state variables.
    sylvan::bdds::bdd variables_bdd = sylvan::bdds::cube(variables);
    auto solutions = bdd_solutions(set, variables_bdd);    
    for (const auto& solution : solutions)
    {
      std::cerr << mcrl2::core::detail::print_list(solution, "\n") << std::endl;
    }

    ldd result = sylvan::ldds::ldd_from_bdd(set, bits);

    BOOST_CHECK_EQUAL(input, result);
  //}

  quit_sylvan();
}

#endif // MCRL2_ENABLE_SYLVAN