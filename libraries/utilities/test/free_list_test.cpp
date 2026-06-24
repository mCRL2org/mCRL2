// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/detail/free_list.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <array>

using namespace mcrl2::utilities::detail;

// The free_list does not own its slots; the caller supplies the storage. These
// tests also force instantiation of erase_after() and contains(), which
// previously did not compile (they referenced a non-existent get_node() and
// compared a slot pointer with a dereferenced element respectively).

BOOST_AUTO_TEST_CASE(test_push_pop_count)
{
  free_list<int> list;
  BOOST_CHECK(list.empty());
  BOOST_CHECK_EQUAL(list.count(), 0u);

  using slot = free_list<int>::slot;
  std::array<slot, 4> storage{};

  for (auto& s : storage)
  {
    list.push_front(s);
  }
  BOOST_CHECK(!list.empty());
  BOOST_CHECK_EQUAL(list.count(), 4u);

  // contains() must compile and find a slot that is part of the list.
  BOOST_CHECK(list.contains(reinterpret_cast<int*>(&storage[0])));

  list.pop_front();
  BOOST_CHECK_EQUAL(list.count(), 3u);
}

BOOST_AUTO_TEST_CASE(test_erase_after)
{
  free_list<int> list;

  using slot = free_list<int>::slot;
  std::array<slot, 4> storage{};
  for (auto& s : storage)
  {
    list.push_front(s);
  }
  BOOST_CHECK_EQUAL(list.count(), 4u);

  // erase_after() must compile (it previously called get_node()) and remove one
  // element.
  list.erase_after(list.before_begin());
  BOOST_CHECK_EQUAL(list.count(), 3u);
}
