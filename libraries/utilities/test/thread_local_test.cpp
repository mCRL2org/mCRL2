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

#include "mcrl2/utilities/thread_local.h"

#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <set>

using mcrl2::utilities::ThreadLocal;

BOOST_AUTO_TEST_CASE(test_basic_creation)
{
  ThreadLocal<int> tls;
  const int* val = tls.get();
  BOOST_CHECK(val == nullptr);
}

BOOST_AUTO_TEST_CASE(test_get_or_initial_creation)
{
  ThreadLocal<int> tls;
  const int* val1 = tls.get();
  BOOST_CHECK(val1 == nullptr);
  
  const int* val2 = tls.get_or([] { return 42; });
  BOOST_REQUIRE(val2 != nullptr);
  BOOST_CHECK_EQUAL(*val2, 42);
}

BOOST_AUTO_TEST_CASE(test_get_or_returns_same_value)
{
  ThreadLocal<int> tls;
  
  const int* val1 = tls.get_or([] { return 42; });
  BOOST_REQUIRE(val1 != nullptr);
  BOOST_CHECK_EQUAL(*val1, 42);
  
  const int* val2 = tls.get();
  BOOST_REQUIRE(val2 != nullptr);
  BOOST_CHECK_EQUAL(*val2, 42);
  
  const int* val3 = tls.get_or([] { return 100; });
  BOOST_REQUIRE(val3 != nullptr);
  BOOST_CHECK_EQUAL(*val3, 42);  // Should return the same value, not create a new one
}

BOOST_AUTO_TEST_CASE(test_multiple_threads)
{
  auto tls = std::make_shared<ThreadLocal<int>>();
  std::vector<std::atomic<int>> results(3);
  std::vector<std::atomic<bool>> non_null(3);

  // Note: Boost.Test assertion macros are not thread-safe (they write to a
  // shared global log), so the worker threads only record plain data and all
  // checks are performed on the main thread after the threads have joined.
  std::vector<std::thread> threads;
  for (int i = 0; i < 3; ++i)
  {
    threads.emplace_back([tls, i, &results, &non_null]() {
      const int* val = tls->get_or([i] { return i * 10; });
      non_null[i].store(val != nullptr, std::memory_order_release);
      results[i].store(val != nullptr ? *val : -1, std::memory_order_release);
    });
  }

  for (auto& t : threads)
  {
    t.join();
  }

  // Verify that each thread got a different value
  for (int i = 0; i < 3; ++i)
  {
    BOOST_CHECK(non_null[i].load(std::memory_order_acquire));
    BOOST_CHECK_EQUAL(results[i].load(std::memory_order_acquire), i * 10);
  }
}

BOOST_AUTO_TEST_CASE(test_thread_local_same_thread)
{
  ThreadLocal<int> tls;
  
  const int* val1 = tls.get_or([] { return 123; });
  BOOST_REQUIRE(val1 != nullptr);
  BOOST_CHECK_EQUAL(*val1, 123);
  
  const int* val2 = tls.get();
  BOOST_REQUIRE(val2 != nullptr);
  BOOST_CHECK_EQUAL(*val2, 123);
  
  // Same pointer should be returned
  BOOST_CHECK_EQUAL(val1, val2);
}

BOOST_AUTO_TEST_CASE(test_clear)
{
  ThreadLocal<int> tls;
  
  const int* val1 = tls.get_or([] { return 456; });
  BOOST_REQUIRE(val1 != nullptr);
  BOOST_CHECK_EQUAL(*val1, 456);
  
  tls.clear();
  
  const int* val2 = tls.get();
  BOOST_CHECK(val2 == nullptr);
}

BOOST_AUTO_TEST_CASE(test_iteration_single_thread)
{
  ThreadLocal<int> tls;
  tls.get_or([] { return 100; });
  
  int count = 0;
  int sum = 0;
  for (const auto& val : tls)
  {
    count++;
    sum += val;
  }
  
  BOOST_CHECK_EQUAL(count, 1);
  BOOST_CHECK_EQUAL(sum, 100);
}

BOOST_AUTO_TEST_CASE(test_size)
{
  ThreadLocal<int> tls;
  BOOST_CHECK_EQUAL(tls.size(), 0);
  
  tls.get_or([] { return 42; });
  BOOST_CHECK_EQUAL(tls.size(), 1);
}

BOOST_AUTO_TEST_CASE(test_iteration_multiple_values)
{
  // Regression test for the forward iterator. Iterating over two or more present
  // values must visit each value exactly once. operator* must be idempotent and
  // only operator++ may advance the cursor. The previous implementation advanced
  // inside operator*, so range-for skipped every other element and then threw
  // std::out_of_range when operator* walked past the end.
  constexpr int num_threads = 8;
  ThreadLocal<int> tls;

  // Each value can only be created from its owning thread, so populate the
  // container from several worker threads (one entry per thread).
  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i)
  {
    threads.emplace_back([&tls, i]() { tls.get_or([i] { return 1000 + i; }); });
  }
  for (auto& t : threads)
  {
    t.join();
  }

  BOOST_REQUIRE_EQUAL(tls.size(), static_cast<std::size_t>(num_threads));

  std::set<int> seen;
  int count = 0;
  for (const auto& val : tls)
  {
    seen.insert(val);
    ++count;
  }

  BOOST_CHECK_EQUAL(count, num_threads);
  BOOST_CHECK_EQUAL(seen.size(), static_cast<std::size_t>(num_threads));
  for (int i = 0; i < num_threads; ++i)
  {
    BOOST_CHECK(seen.count(1000 + i) == 1);
  }
}

BOOST_AUTO_TEST_CASE(test_iteration_empty)
{
  // An empty container must yield begin() == end() and an empty range, rather
  // than throwing when dereferenced.
  ThreadLocal<int> tls;
  BOOST_CHECK(tls.begin() == tls.end());

  int count = 0;
  for (const auto& val : tls)
  {
    (void)val;
    ++count;
  }
  BOOST_CHECK_EQUAL(count, 0);
}

BOOST_AUTO_TEST_CASE(test_iteration_postincrement)
{
  // Exercise the post-increment operator over multiple values.
  constexpr int num_threads = 4;
  ThreadLocal<int> tls;
  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i)
  {
    threads.emplace_back([&tls, i]() { tls.get_or([i] { return i; }); });
  }
  for (auto& t : threads)
  {
    t.join();
  }

  int count = 0;
  for (auto it = tls.begin(); it != tls.end(); it++)
  {
    (void)*it;
    ++count;
  }
  BOOST_CHECK_EQUAL(count, num_threads);
}
