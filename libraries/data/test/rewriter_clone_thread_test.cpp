// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_clone_thread_test.cpp
/// \brief Test that a clone of a rewriter can outlive the original rewriter on another thread.

#define BOOST_TEST_MODULE rewriter_clone_thread_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter.h"

#include <future>
#include <thread>
#include <vector>

using namespace mcrl2;
using namespace mcrl2::data;

// A tool can give every worker thread its own clone of a rewriter, and the worker threads can end after the original
// rewriter is gone. The compiling rewriter's generated code accesses cached normal forms through the rewriter, so
// each clone needs its own copy of the cache in its thread-local data.
BOOST_AUTO_TEST_CASE(clone_outlives_original_on_another_thread)
{
  // Closed subterms on the right-hand sides (c and 2 * 3) make the compiling rewriter cache their normal forms.
  const data_specification spec = parse_data_specification(
      "map c: Nat; f: Nat -> Nat;"
      "var n: Nat;"
      "eqn c = 3; f(n) = n + c + 2 * 3;");

  // The compiling rewriter is the case that matters, so test it whenever it is built (not only with MCRL2_TEST_JITTYC).
  std::vector<rewrite_strategy> strategies = {rewrite_strategy::jitty};
#ifdef MCRL2_ENABLE_JITTYC
  strategies.push_back(rewrite_strategy::jitty_compiling);
#endif

  for (const rewrite_strategy strategy: strategies)
  {
    BOOST_TEST_MESSAGE("strategy " << strategy);
    std::promise<void> clone_made;
    std::promise<void> original_gone;
    std::shared_future<void> original_gone_future = original_gone.get_future();
    bool clone_rewrites_correctly = false;
    data_expression expected;
    std::thread b;

    // Thread a makes the original rewriter, waits until thread b has made its clone, and destroys the original.
    std::thread a([&]()
    {
      rewriter original(spec, strategy);
      expected = original(parse_data_expression("f(1)", spec));
      b = std::thread([&, original_rewriter = &original]()
      {
        rewriter clone = original_rewriter->clone();
        clone.thread_initialise();
        clone_rewrites_correctly = clone(parse_data_expression("f(1)", spec)) == expected;
        clone_made.set_value();
        original_gone_future.wait();
        // The clone is destroyed here, on thread b, after the original is gone.
      });
      clone_made.get_future().wait();
    });
    a.join();
    original_gone.set_value();
    b.join();

    BOOST_CHECK(clone_rewrites_correctly);
  }
}
