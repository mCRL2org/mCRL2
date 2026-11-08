// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsreach_test.cpp
/// \brief Tests that the modes of lpsreach find the same states as the default breadth-first exploration.

#define BOOST_TEST_MODULE lpsreach_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(dummy_test)
{
  // This is an empty test since at least one test is required.
}

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/lpsreach.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace mcrl2;

// Small specifications with data parameters, sum variables, deadlocks and parallel components.
static const std::vector<std::string> specifications = {
  // a counter that is reset
  "act a, b: Nat;"
  "proc P(n: Nat) = (n < 5) -> a(n) . P(n + 1) + (n > 2) -> b(n) . P(0);"
  "init P(0);",

  // a sum variable, and states without successors
  "act a: Nat;"
  "proc P(x: Nat, y: Bool) = sum m: Nat . (m < 3 && x < 6) -> a(m) . P(x + m, !y) + y -> tau . P(x, false);"
  "init P(0, true);",

  // independent components
  "act a, b: Nat;"
  "proc P(i: Nat) = (i < 3) -> a(i) . P(i + 1);"
  "     Q(j: Nat) = (j < 4) -> b(j) . Q(j + 1);"
  "init P(0) || Q(0) || P(1);",

  // communicating components with a list
  "act s, r, c: Nat;"
  "proc S(l: List(Nat)) = (#l < 3) -> sum v: Nat . (v < 2) -> s(v) . S(v |> l) + (l != []) -> c(head(l)) . S(tail(l));"
  "     R(n: Nat) = sum v: Nat . (v < 2) -> r(v) . R((n + v) mod 3);"
  "init allow({c, s}, comm({s | r -> s}, S([]) || R(0)));",

  // a summand that reads and writes no parameter (a self-loop in every state)
  "act a, c;"
  "proc P(n: Nat) = a . P(n) + (n < 3) -> c . P(n + 1);"
  "init P(0);"
};

using option_setter = std::function<void(symbolic::symbolic_reachability_options&)>;

// The modes to test, each compared with the default breadth-first exploration.
static const std::vector<std::pair<std::string, option_setter>> variants = {
  {"--parallel-learning", [](symbolic::symbolic_reachability_options& o) { o.parallel_learning = true; }},
  {"--parallel-learning --cached", [](symbolic::symbolic_reachability_options& o) { o.parallel_learning = true; o.cached = true; }},
  {"--parallel-groups", [](symbolic::symbolic_reachability_options& o) { o.parallel_groups = true; }},
  {"--parallel-groups --cached", [](symbolic::symbolic_reachability_options& o) { o.parallel_groups = true; o.cached = true; }},
  {"--parallel-groups --parallel-learning --cached", [](symbolic::symbolic_reachability_options& o) { o.parallel_groups = true; o.parallel_learning = true; o.cached = true; }},
  {"--parallel-groups --deadlock", [](symbolic::symbolic_reachability_options& o) { o.parallel_groups = true; o.detect_deadlocks = true; }},
};

/// \returns The number of reachable states, and the number of deadlocks (if options.detect_deadlocks).
static std::pair<double, double> explore(const lps::specification& lpsspec, const symbolic::symbolic_reachability_options& options)
{
  lps::lpsreach_algorithm algorithm(lpsspec, options);
  const double states = sylvan::ldds::satcount(algorithm.run());
  return {states, sylvan::ldds::satcount(algorithm.deadlocks())};
}

// Sylvan operations must be executed by a Lace worker, so the test body runs as a Lace task.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_DECL_0(bool, lpsreach_test_task);
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define lpsreach_test_task() RUN(lpsreach_test_task)

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_IMPL_0(bool, lpsreach_test_task)
{
  for (const std::string& text: specifications)
  {
    lps::t_lin_options lin_options;
    lin_options.ignore_time = true; // as mcrl22lps without --timed
    const lps::specification lpsspec = lps::remove_stochastic_operators(lps::linearise(text, lin_options));
    symbolic::symbolic_reachability_options default_options;
    default_options.detect_deadlocks = true;
    const auto [expected_states, expected_deadlocks] = explore(lpsspec, default_options);
    BOOST_CHECK(expected_states > 0);
    for (const auto& [name, set_options]: variants)
    {
      symbolic::symbolic_reachability_options options;
      set_options(options);
      BOOST_TEST_CONTEXT("variant " << name << " on " << text)
      {
        const auto [states, deadlocks] = explore(lpsspec, options);
        BOOST_CHECK_EQUAL(states, expected_states);
        if (options.detect_deadlocks)
        {
          BOOST_CHECK_EQUAL(deadlocks, expected_deadlocks);
        }
      }
    }
  }
  return true;
}

BOOST_AUTO_TEST_CASE(modes_agree)
{
  lace_start(4, 0); // several workers, so that the parallel code paths run
  sylvan::sylvan_set_limits(256 * 1024 * 1024, 1, 4);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_ldd();
  sylvan::ldds::initialise();

  lpsreach_test_task();

  sylvan::sylvan_quit();
  lace_stop();
}

#endif // MCRL2_ENABLE_SYLVAN
