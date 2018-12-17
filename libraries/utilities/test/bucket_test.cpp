// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/detail/bucket.h"
#include <boost/test/included/unit_test_framework.hpp>

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
    Bucket::node* node = allocator.allocate(1);
    allocator.construct(node, i);
    list.push_front(node);
  }

  BOOST_CHECK(length(list) == 100);

  /// Erase the first 10 elements.
  std::size_t count = 10;
  for (Bucket::iterator it = list.before_begin(); it != list.end();)
  {
    it = list.erase_after(list.before_begin(), allocator);
    --count;
    if (count == 0)
    {
      break;
    }
  }

  BOOST_CHECK(length(list) == 90);

  /// Clean up the remaining nodes.
  for (Bucket::iterator it = list.before_begin(); it != list.end();)
  {
    it = list.erase_after(list.before_begin(), allocator);
  }

  BOOST_CHECK(length(list) == 0);
}

boost::unit_test::test_suite* init_unit_test_suite(int, char*[])
{
  return nullptr;
}
