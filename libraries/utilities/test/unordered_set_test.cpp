// Author(s): Jeroen Keiren, Maurice Laveaux
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
#include <set>
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

BOOST_AUTO_TEST_CASE(test_postincrement)
{
  // Regression test: the post-increment operator previously returned a
  // dereferenced iterator (return *copy;), so any instantiation failed to
  // compile. Exercise it over several elements.
  unordered_set<int> set = construct({5, 3, 2, 1, 4});

  std::set<int> seen;
  int count = 0;
  for (int it : set)
  {
    seen.insert(it);
    ++count;
  }

  BOOST_CHECK_EQUAL(count, 5);
  BOOST_CHECK_EQUAL(seen.size(), 5u);
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
    : m_vector(std::move(reference))
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

namespace
{

/// The number of elements inserted by the rehash tests; large enough for the table to grow many times.
constexpr int rehash_test_elements = 5000;

/// The factor by which the explicit rehash tests grow the table at once.
constexpr std::size_t rehash_test_growth_factor = 16;

/// The number of low hash bits that colliding_hash keeps zero.
constexpr unsigned int colliding_hash_shift = 4;

/// A hash whose lowest bits are always zero, so that many keys share a bucket in a small table and those buckets
/// have to be split over several buckets when the table grows.
struct colliding_hash
{
  std::size_t operator()(int value) const
  {
    return static_cast<std::size_t>(value) << colliding_hash_shift;
  }
};

/// The set that the term pool uses: thread safe, and only resized by explicit calls to rehash.
using thread_safe_set = unordered_set<int, std::hash<int>, std::equal_to<int>, std::allocator<int>, true, false>;

/// \brief Checks that set contains exactly the elements of expected, that each of them can be found, and that every
///        element is stored in the bucket that its hash selects.
template<typename Set>
void check_contents(const Set& set, const std::set<int>& expected)
{
  BOOST_CHECK_EQUAL(set.size(), expected.size());

  std::size_t stored = 0;
  std::size_t misplaced = 0;
  std::size_t unexpected = 0;
  for (std::size_t bucket = 0; bucket < set.bucket_count(); ++bucket)
  {
    for (typename Set::const_local_iterator it = set.begin(bucket); it != set.end(bucket); ++it)
    {
      ++stored;
      if ((set.hash_function()(*it) & (set.bucket_count() - 1)) != bucket)
      {
        ++misplaced;
      }
      if (expected.count(*it) == 0)
      {
        ++unexpected;
      }
    }
  }
  BOOST_CHECK_EQUAL(stored, expected.size());
  BOOST_CHECK_EQUAL(misplaced, 0u);
  BOOST_CHECK_EQUAL(unexpected, 0u);

  std::size_t missing = 0;
  for (int value: expected)
  {
    if (set.find(value) == set.end())
    {
      ++missing;
    }
  }
  BOOST_CHECK_EQUAL(missing, 0u);
}

/// \brief Inserts the values 0, ..., rehash_test_elements - 1 into set and checks its contents after every rehash.
/// \returns The number of times that the number of buckets changed.
template<typename Set>
std::size_t insert_and_check_growth(Set& set, std::set<int>& expected)
{
  std::size_t number_of_rehashes = 0;
  for (int value = 0; value < rehash_test_elements; ++value)
  {
    const std::size_t buckets_before = set.bucket_count();
    set.emplace(value);
    expected.insert(value);
    if (set.bucket_count() != buckets_before)
    {
      ++number_of_rehashes;
      check_contents(set, expected);
    }
  }
  check_contents(set, expected);
  return number_of_rehashes;
}

} // namespace

BOOST_AUTO_TEST_CASE(test_rehash_growth)
{
  unordered_set<int> set;
  std::set<int> expected;

  BOOST_CHECK_GT(insert_and_check_growth(set, expected), 0u);
}

BOOST_AUTO_TEST_CASE(test_rehash_explicit)
{
  unordered_set<int> set;
  std::set<int> expected;
  insert_and_check_growth(set, expected);

  // Growing by more than a factor two at once moves the elements of one old bucket to more than two new buckets.
  const std::size_t buckets = set.bucket_count();
  set.rehash(buckets * rehash_test_growth_factor);
  BOOST_CHECK_EQUAL(set.bucket_count(), buckets * rehash_test_growth_factor);
  check_contents(set, expected);

  // Rehashing to fewer buckets has no effect.
  set.rehash(buckets);
  BOOST_CHECK_EQUAL(set.bucket_count(), buckets * rehash_test_growth_factor);
  check_contents(set, expected);

  // A number of buckets that is not a power of two is rounded up to one.
  set.rehash(buckets * rehash_test_growth_factor + 1);
  BOOST_CHECK_EQUAL(set.bucket_count(), 2 * buckets * rehash_test_growth_factor);
  check_contents(set, expected);

  set.reserve(set.size() * rehash_test_growth_factor);
  BOOST_CHECK_GE(set.bucket_count(), set.size() * rehash_test_growth_factor);
  check_contents(set, expected);
}

BOOST_AUTO_TEST_CASE(test_rehash_empty)
{
  unordered_set<int> set(0);
  set.rehash(rehash_test_elements);
  BOOST_CHECK_GE(set.bucket_count(), static_cast<std::size_t>(rehash_test_elements));
  check_contents(set, std::set<int>());

  std::set<int> expected;
  insert_and_check_growth(set, expected);
}

BOOST_AUTO_TEST_CASE(test_rehash_colliding)
{
  // Many keys share each bucket, so every rehash has to split long chains.
  unordered_set<int, colliding_hash> set;
  std::set<int> expected;
  BOOST_CHECK_GT(insert_and_check_growth(set, expected), 0u);

  // Erase the even values, grow the table and insert them again.
  for (int value = 0; value < rehash_test_elements; value += 2)
  {
    set.erase(value);
    expected.erase(value);
  }
  check_contents(set, expected);

  set.rehash(set.bucket_count() * rehash_test_growth_factor);
  check_contents(set, expected);

  for (int value = 0; value < rehash_test_elements; value += 2)
  {
    BOOST_CHECK(set.emplace(value).second);
    expected.insert(value);
  }
  check_contents(set, expected);
}

BOOST_AUTO_TEST_CASE(test_rehash_thread_safe)
{
  // The term pool resizes its tables itself, by calling rehash_if_needed before inserting.
  thread_safe_set set(0);
  std::set<int> expected;

  std::size_t number_of_rehashes = 0;
  for (int value = 0; value < rehash_test_elements; ++value)
  {
    const std::size_t buckets_before = set.bucket_count();
    set.rehash_if_needed();
    if (set.bucket_count() != buckets_before)
    {
      ++number_of_rehashes;
      check_contents(set, expected);
    }

    BOOST_CHECK(set.emplace(value).second);
    expected.insert(value);
  }
  BOOST_CHECK_GT(number_of_rehashes, 0u);
  check_contents(set, expected);

  // Inserting an element twice does not add it again after a rehash.
  set.rehash(set.bucket_count() * rehash_test_growth_factor);
  BOOST_CHECK(!set.emplace(0).second);
  check_contents(set, expected);
}

BOOST_AUTO_TEST_CASE(test_rehash_map)
{
  // The values of a map stay attached to their keys when the table grows.
  unordered_map<int, int> mapping;
  for (int key = 0; key < rehash_test_elements; ++key)
  {
    mapping.emplace(key, -key);
  }
  mapping.rehash(mapping.bucket_count() * rehash_test_growth_factor);

  BOOST_CHECK_EQUAL(mapping.size(), static_cast<std::size_t>(rehash_test_elements));
  std::size_t wrong = 0;
  for (int key = 0; key < rehash_test_elements; ++key)
  {
    unordered_map<int, int>::const_iterator it = mapping.find(key);
    if (it == mapping.end() || it->second != -key)
    {
      ++wrong;
    }
  }
  BOOST_CHECK_EQUAL(wrong, 0u);
}
