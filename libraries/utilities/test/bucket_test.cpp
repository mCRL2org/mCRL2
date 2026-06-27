// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/detail/bucket_list.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

using namespace mcrl2::utilities::detail;

template<typename Element,
         typename Allocator>
std::size_t length(bucket_list<Element, Allocator>& list)
{
  std::size_t length = 0;
  for (auto& element : list)
  {
    (void)element;
    ++length;
  }
  return length;
}

BOOST_AUTO_TEST_CASE(test_list)
{
  using Bucket = bucket_list<int, std::allocator<int>>;
  Bucket::NodeAllocator allocator;
  Bucket list;

  /// Allocate and construct various nodes for in the list.
  for (int i = 0; i < 100; ++i)
  {
    list.emplace_front(allocator, i);
  }

  BOOST_CHECK_EQUAL(length(list), 100);

  /// Erase the first 10 elements.
  std::size_t count = 10;
  for (Bucket::const_iterator it = list.before_begin(); it != list.end();)
  {
    it = list.erase_after(allocator, list.before_begin());
    --count;
    if (count == 0)
    {
      break;
    }
  }

  BOOST_CHECK_EQUAL(length(list), 90);

  /// Clean up the remaining nodes.
  for (Bucket::const_iterator it = list.before_begin(); it != list.end();)
  {
    it = list.erase_after(allocator, list.before_begin());
  }

  BOOST_CHECK_EQUAL(length(list), 0);
}

BOOST_AUTO_TEST_CASE(test_emplace_front_unique_duplicate)
{
  // Regression test: emplace_front_unique constructs the node first (moving its
  // rvalue arguments into the node) and then checks for a duplicate. The check
  // must compare against the already-constructed key, not re-forward the
  // (possibly moved-from) arguments. With a movable key such as std::string,
  // re-forwarding compared against an empty moved-from string, so a duplicate was
  // not detected.
  using Bucket = bucket_list<std::string, std::allocator<std::string>>;
  Bucket::NodeAllocator allocator;
  Bucket list;

  std::equal_to<std::string> equals;

  auto [it1, added1] = list.emplace_front_unique(allocator, equals, std::string("hello"));
  (void)it1;
  BOOST_CHECK(added1);

  // Inserting the same value (as an rvalue) again must be detected as a duplicate.
  auto [it2, added2] = list.emplace_front_unique(allocator, equals, std::string("hello"));
  (void)it2;
  BOOST_CHECK(!added2);

  // A different value is still inserted.
  auto [it3, added3] = list.emplace_front_unique(allocator, equals, std::string("world"));
  (void)it3;
  BOOST_CHECK(added3);

  // list.clear(allocator); Disabled as this did not compile on 26/6/2026
}
