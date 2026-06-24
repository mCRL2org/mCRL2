// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/utilities/hashtable.h"

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <vector>

using namespace mcrl2::utilities;

// The hashtable stores pointer-like keys (it uses nullptr as the empty value),
// matching the way it is used by the atermpp thread pool. We therefore test it
// with int* keys backed by a stable storage vector.

BOOST_AUTO_TEST_CASE(test_insert_find_size)
{
  std::vector<int> storage;
  storage.reserve(64);
  for (int i = 0; i < 64; ++i)
  {
    storage.push_back(i);
  }

  hashtable<int*> table(16);
  BOOST_CHECK_EQUAL(table.size(), 0);

  // Insert a number of keys, resizing explicitly when required.
  for (int i = 0; i < 10; ++i)
  {
    if (table.must_resize())
    {
      table.resize();
    }
    auto result = table.insert(&storage[i]);
    BOOST_CHECK(result.second); // Newly inserted.
  }
  BOOST_CHECK_EQUAL(table.size(), 10);

  // Re-inserting an existing key must report that nothing was inserted.
  auto again = table.insert(&storage[3]);
  BOOST_CHECK(!again.second);
  BOOST_CHECK_EQUAL(table.size(), 10);

  // find must locate inserted keys and return end() for absent keys.
  for (int i = 0; i < 10; ++i)
  {
    BOOST_CHECK(table.find(&storage[i]) != table.end());
  }
  BOOST_CHECK(table.find(&storage[50]) == table.end());
}

BOOST_AUTO_TEST_CASE(test_erase)
{
  std::vector<int> storage;
  storage.reserve(64);
  for (int i = 0; i < 64; ++i)
  {
    storage.push_back(i);
  }

  hashtable<int*> table(64);
  for (int i = 0; i < 20; ++i)
  {
    table.insert(&storage[i]);
  }
  BOOST_CHECK_EQUAL(table.size(), 20);

  table.erase(&storage[5]);
  BOOST_CHECK_EQUAL(table.size(), 19);
  BOOST_CHECK(table.find(&storage[5]) == table.end());

  // Other keys remain findable.
  BOOST_CHECK(table.find(&storage[4]) != table.end());
  BOOST_CHECK(table.find(&storage[6]) != table.end());
}

BOOST_AUTO_TEST_CASE(test_clear_reuse)
{
  // Regression test: clear() must leave the table in a usable state. Previously
  // clear() shrank the backing vector to size zero, leaving the bucket mask and
  // element count stale so that any subsequent operation indexed out of bounds.
  std::vector<int> storage;
  storage.reserve(64);
  for (int i = 0; i < 64; ++i)
  {
    storage.push_back(i);
  }

  hashtable<int*> table(64);
  for (int i = 0; i < 20; ++i)
  {
    table.insert(&storage[i]);
  }
  BOOST_CHECK_EQUAL(table.size(), 20);

  table.clear();
  BOOST_CHECK_EQUAL(table.size(), 0);

  // The table must be fully functional again after clear().
  for (int i = 0; i < 30; ++i)
  {
    auto result = table.insert(&storage[i]);
    BOOST_CHECK(result.second);
  }
  BOOST_CHECK_EQUAL(table.size(), 30);
  for (int i = 0; i < 30; ++i)
  {
    BOOST_CHECK(table.find(&storage[i]) != table.end());
  }
}

namespace
{

// Hash/equality that compare the pointed-to int value rather than the pointer
// identity. nullptr is handled explicitly so the hashtable can still detect
// empty slots (which it does via Key == nullptr).
struct deref_hash
{
  std::size_t operator()(int* p) const { return p != nullptr ? std::hash<int>()(*p) : 0; }
};

struct deref_equals
{
  bool operator()(int* a, int* b) const
  {
    if (a == nullptr || b == nullptr)
    {
      return a == b;
    }
    return *a == *b;
  }
};

} // namespace

BOOST_AUTO_TEST_CASE(test_custom_equality_used_by_find)
{
  // Regression test: find() must use the configured equality functor, not the
  // key's built-in operator==. Here equality is value-based: two distinct
  // pointers to equal ints compare equal. The previous find() compared with the
  // raw operator== (pointer identity) and therefore failed to locate a key by
  // value.
  int x = 7;
  int y = 7; // Distinct object, equal value.
  int z = 9;

  hashtable<int*, deref_hash, deref_equals> table(16);
  table.insert(&x);

  // find by an equal-valued but distinct pointer must succeed.
  BOOST_CHECK(table.find(&y) != table.end());
  // find for an absent value must fail.
  BOOST_CHECK(table.find(&z) == table.end());
}
