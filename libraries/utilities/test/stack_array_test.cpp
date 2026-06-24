// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/stack_array.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2::utilities;

BOOST_AUTO_TEST_CASE(test_non_empty)
{
  // Regression test: empty() previously returned the negation of emptiness, so a
  // non-empty stack_array incorrectly reported empty() == true.
  MCRL2_DECLARE_STACK_ARRAY(values, int, 4);
  BOOST_CHECK_EQUAL(values.size(), 4u);
  BOOST_CHECK(!values.empty());

  for (std::size_t i = 0; i < values.size(); ++i)
  {
    values[i] = static_cast<int>(i);
  }

  int sum = 0;
  for (int v : values)
  {
    sum += v;
  }
  BOOST_CHECK_EQUAL(sum, 0 + 1 + 2 + 3);
}

BOOST_AUTO_TEST_CASE(test_empty)
{
  MCRL2_DECLARE_STACK_ARRAY(values, int, 0);
  BOOST_CHECK_EQUAL(values.size(), 0u);
  BOOST_CHECK(values.empty());
}
