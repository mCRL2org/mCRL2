// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/fixed_size_cache.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2::utilities;

struct square_struct
{
  int operator()(int value)
  {
    return value*value;
  }
};

BOOST_AUTO_TEST_CASE(test_function_cache)
{
  fifo_function_cache<square_struct, int> cache;

  std::vector<int> results;
  for (int i = 0; i < 100; ++i)
  {
    results.push_back(square_struct()(i));
  }

  for (int i = 0; i < 100; ++i)
  {
    BOOST_CHECK_EQUAL(cache(i), results[i]);
  }

}
