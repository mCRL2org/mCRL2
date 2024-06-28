// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/test/tools/old/interface.hpp>
#define BOOST_TEST_MODULE ldd_merge_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(dummy_test)
{
  // This is an empty test since at least one test is required.
}

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/symbolic/test_utility.h"

#include <sylvan_ldd.hpp>

using namespace mcrl2::symbolic;

BOOST_AUTO_TEST_CASE(test_ldd_merge)
{
  initialise_sylvan();

  auto v0 = random_vector_set(3, 5, 10);
  auto v1 = random_vector_set(3, 5, 10);

  BOOST_CHECK(height(to_ldd(v0)) == 3);
  BOOST_CHECK(height(to_ldd(v1)) == 3);

  sylvan::ldds::initialise();
  auto result = sylvan::ldds::merge(to_ldd(v0), to_ldd(v1));

  // Compute the naive cartesian product.
  std::vector<std::uint32_t> interleaved;
  for (const auto& vector0 : v0)
  {
    for (const auto& vector1 : v1)
    {
      interleaved.clear();

      auto it = vector1.begin();
      for (const auto& value0 : vector0)
      {
        interleaved.emplace_back(value0);
        interleaved.emplace_back(*it);

        assert(it != vector1.end());
        ++it;
      }

      BOOST_CHECK(sylvan::ldds::member_cube(result, interleaved));
    }
  }

  // TODO: Check the other way around?

  quit_sylvan();
}

#endif // MCRL2_ENABLE_SYLVAN