// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#define BOOST_AUTO_TEST_MAIN
#include <boost/test/included/unit_test.hpp>

#include <random>
#include <unordered_set>

#include "mcrl2/utilities/hash_utility.h"
#include "mcrl2/utilities/unordered_map.h"


using namespace mcrl2::utilities;

template<typename T>
unordered_set<T> construct(std::initializer_list<T> list)
{
  unordered_set<T> set;

  for (const T& element : list)
  {
    set.emplace(element);
  }

  return set;
}

BOOST_AUTO_TEST_CASE(test_small)
{
  // Test with inserting 5, 3, 2, 5 expected { 2,3,5 }
  unordered_set<int> set = construct({5,3,2,5});

  BOOST_CHECK(set.find(5) != set.end());
  BOOST_CHECK(set.find(2) != set.end());
  BOOST_CHECK(set.find(3) != set.end());

  BOOST_CHECK(set.count(5) != 0);
  BOOST_CHECK(set.count(2) != 0);
  BOOST_CHECK(set.count(3) != 0);

  BOOST_CHECK(set.size() == 3);
  BOOST_CHECK(!set.empty());
}

BOOST_AUTO_TEST_CASE(test_large)
{
  // Test inserting a large number of elements (tests resize behaviour).
  std::random_device dev;
  std::mt19937 rng(dev());
  std::uniform_int_distribution<std::mt19937::result_type> dist(1,10000);

  unordered_set<int> test;

  // Here, we assume that the standard library implementation is correct.
  std::unordered_set<int> correct;
  for (std::size_t i = 0; i < 100000; ++i)
  {
    auto value = dist(rng);
    test.emplace(value);
    correct.emplace(value);
  }

  // Check that both contain the same elements.
  for (auto& value : correct)
  {
    BOOST_CHECK(test.find(value) != test.end());
  }

  BOOST_CHECK(test.size() == correct.size());

  for (auto& value : test)
  {
    BOOST_CHECK(correct.find(value) != correct.end());
  }
}

BOOST_AUTO_TEST_CASE(test_copy)
{
  // Test the copy constructor.
  unordered_set<int> set = construct({5,3,2,5});

  unordered_set<int> copy(set);
  set.clear();

  BOOST_CHECK(copy.find(5) != copy.end());
  BOOST_CHECK(copy.find(2) != copy.end());
  BOOST_CHECK(copy.find(3) != copy.end());
}

BOOST_AUTO_TEST_CASE(test_move)
{
  // Test the move constructor.
  unordered_set<int> set = construct({5,3,2,5});
  unordered_set<int> moved = std::move(set);
}

BOOST_AUTO_TEST_CASE(test_empty)
{
  unordered_set<int> set(0);

  BOOST_CHECK(set.empty());
  BOOST_CHECK(set.size() == 0);

  for (auto it = set.begin(); it != set.end(); ++it)
  {
    BOOST_CHECK(false);
  }
}

BOOST_AUTO_TEST_CASE(test_find_erase)
{
  // Try to erase an element using the iterator returned by find.
  unordered_set<int> set = construct({5,3,2,5});

  auto it = set.find(3);
  BOOST_CHECK(it != set.end());
  set.erase(it);
}

BOOST_AUTO_TEST_CASE(test_erase_begin)
{
  // Try to erase an element using the iterator returned by find.
  unordered_set<int> set = construct({5,3,2,5});
  set.erase(set.begin());
}

BOOST_AUTO_TEST_CASE(test_const_iterator)
{
  unordered_set<int> set = construct({5,3,2,5});
  auto it = set.begin();
  unordered_set<int>::const_iterator const_it = it;

  const unordered_set<int>::iterator it2 = set.begin();
  int value = *it2;
  BOOST_CHECK(value == *const_it);
}

class Object
{
public:
  Object(std::vector<int>&& reference)
    : m_vector(std::forward<std::vector<int>>(reference))
  {}

  bool operator==(const Object& other) const
  {
    return m_vector == other.m_vector;
  }

private:
  std::vector<int> m_vector;
};

namespace std
{

template<>
struct hash<Object>
{
  std::size_t operator()(const Object& object) const
  {
    return reinterpret_cast<std::size_t>(&object);
  }
};

}

BOOST_AUTO_TEST_CASE(test_perfect_forwarding)
{
  unordered_set<Object> objects;

  // Move it into the unordered_set.
  std::vector<int> test;
  objects.emplace(std::move(test));
}

BOOST_AUTO_TEST_CASE(test_try_emplace)
{
  unordered_map<int, std::vector<int>> mapping;

  // Move it into the unordered_set.
  std::vector<int> test;
  mapping.try_emplace(5, 1000);

  BOOST_CHECK(mapping.try_emplace(5, 1000).second == false);
}

BOOST_AUTO_TEST_CASE(test_emplace)
{
  unordered_map<int, std::vector<int>> mapping;

  // Move it into the unordered_set.
  mapping.emplace(5, 1000);

  BOOST_CHECK(mapping.emplace(5, 1000).second == false);
}
