// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_TEST_MODULE thread_local_test

#include <boost/test/unit_test.hpp>
#include "mcrl2/utilities/thread_local.h"

#include <thread>
#include <vector>
#include <memory>
#include <atomic>

using mcrl2::utilities::ThreadLocal;

BOOST_AUTO_TEST_SUITE(thread_local_tests)

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
  
  std::vector<std::thread> threads;
  for (int i = 0; i < 3; ++i)
  {
    threads.emplace_back([tls, i, &results]() {
      const int* val = tls->get_or([i] { return i * 10; });
      BOOST_REQUIRE(val != nullptr);
      results[i].store(*val, std::memory_order_release);
    });
  }
  
  for (auto& t : threads)
  {
    t.join();
  }
  
  // Verify that each thread got a different value
  for (int i = 0; i < 3; ++i)
  {
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

BOOST_AUTO_TEST_SUITE_END()
