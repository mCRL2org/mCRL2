// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/block_allocator.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <vector>

using namespace mcrl2::utilities;

BOOST_AUTO_TEST_CASE(test_has_extension)
{
  // This test assumes that 16 allocations all end up in the same block, if that is not true
  // this should be adapted.
  block_allocator<int, 16> allocator;

  BOOST_CHECK(allocator.consolidate() == 0); // Should be empty initially.

  // Allocate a full block and keep track of pointers
  std::vector<int*> test;
  for (std::size_t i = 0; i < 16; ++i)
  {
    test.push_back(allocator.allocate(1));
  }

  // Allocate more spaces to fill the allocator.
  std::vector<int*> extra;
  for (std::size_t i = 0; i < 1024; ++i)
  {
    extra.push_back(allocator.allocate(1));
  }

  // Remove the first 5 elements.
  int counter = 5;
  auto it = test.begin();
  for (; it != test.end(); ++it)
  {
    allocator.deallocate(*it, 1);
    if (--counter == 0)
    {
      ++it;
      break;
    }
  }

  BOOST_CHECK(allocator.consolidate() == 0); // Block still contains 9 elements.

  for (; it != test.end(); ++it)
  {
    allocator.deallocate(*it, 1);
  }

  BOOST_CHECK(allocator.consolidate() == 1); // Block is empty.

  for (auto& element : extra)
  {
    allocator.deallocate(element, 1);
  }

  BOOST_CHECK(allocator.consolidate() == 1024/16); // Whole allocator should be emptied.
}
