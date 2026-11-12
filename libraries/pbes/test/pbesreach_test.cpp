// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesreach_test.cpp
/// \brief Tests that the modes of symbolic PBES exploration and solving give the same answer and the same number of
///        vertices as the default breadth-first exploration.

#define BOOST_TEST_MODULE pbesreach_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(dummy_test)
{
  // This is an empty test since at least one test is required.
}

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/pbes/pbesreach.h"
#include "mcrl2/pbes/symbolic_parity_game.h"
#include "mcrl2/pbes/symbolic_pbessolve.h"
#include "mcrl2/pbes/txt2pbes.h"

#include <functional>
#include <string>
#include <utility>
#include <vector>

using namespace mcrl2;

// Small PBESs with alternation, quantifiers over bounded ranges, and both answers.
static const std::vector<std::string> pbess = {
  // every recursive call is guarded by a condition that bounds the parameters, so that the parity games are finite
  "pbes nu X(n: Nat) = (val(n < 3) && X(n + 1)) || (val(n >= 3) && Y(0));"
  "     mu Y(m: Nat) = val(m >= 2) || (val(m < 2) && Y(m + 1));"
  "init X(0);",

  "pbes mu X(n: Nat) = val(n < 5) && X(n + 1);"
  "init X(0);",

  "pbes nu X(b: Bool, n: Nat) = forall m: Nat . val(m < 2 && n < 4) => X(!b, n + m);"
  "init X(true, 0);",

  "pbes mu Y(n: Nat) = (val(n < 4) && X(n + 1)) || val(n == 4);"
  "     nu X(n: Nat) = Y(n) && (val(n > 2) || (val(n < 6) && Y(n + 1)));"
  "init Y(0);",

  "pbes nu X(n: Nat) = exists m: Nat . val(m < 3 && n < 6) && Y(n + m);"
  "     mu Y(n: Nat) = val(n >= 6) || (val(n < 6) && (X(n) || Y(n + 1)));"
  "init X(0);"
};

using option_setter = std::function<void(pbes_system::symbolic_reachability_options&)>;

// The modes to test, each compared with the default breadth-first exploration.
static const std::vector<std::pair<std::string, option_setter>> variants = {
  {"--parallel-learning", [](pbes_system::symbolic_reachability_options& o) { o.parallel_learning = true; }},
  {"--parallel-learning --cached", [](pbes_system::symbolic_reachability_options& o) { o.parallel_learning = true; o.cached = true; }},
  {"--parallel-groups", [](pbes_system::symbolic_reachability_options& o) { o.parallel_groups = true; }},
  {"--parallel-groups --cached", [](pbes_system::symbolic_reachability_options& o) { o.parallel_groups = true; o.cached = true; }},
  {"--parallel-groups --parallel-learning --cached", [](pbes_system::symbolic_reachability_options& o) { o.parallel_groups = true; o.parallel_learning = true; o.cached = true; }},
  {"--parallel-groups --deadlock", [](pbes_system::symbolic_reachability_options& o) { o.parallel_groups = true; o.detect_deadlocks = true; }},
  {"--recursive-saturation --cached", [](pbes_system::symbolic_reachability_options& o) { o.recursive_saturation = true; o.cached = true; }},
  {"--parallel-saturation --cached", [](pbes_system::symbolic_reachability_options& o) { o.recursive_saturation = true; o.recursive_saturation_parallel = true; o.cached = true; }},
  {"--parallel-saturation --cached --deadlock", [](pbes_system::symbolic_reachability_options& o) { o.recursive_saturation = true; o.recursive_saturation_parallel = true; o.cached = true; o.detect_deadlocks = true; }},
  {"--reach --cached", [](pbes_system::symbolic_reachability_options& o) { o.reach = true; o.cached = true; }},
  {"--reach --cached --deadlock", [](pbes_system::symbolic_reachability_options& o) { o.reach = true; o.cached = true; o.detect_deadlocks = true; }},
};

/// \returns The answer, and the number of reachable vertices.
static std::pair<bool, double> solve(const pbes_system::pbes& p, const pbes_system::symbolic_reachability_options& options)
{
  pbes_system::srf_pbes srf = pbes_system::preprocess<false>(p, options);
  pbes_system::pbesreach_algorithm reach(srf, options);
  reach.run();
  if (reach.solution_found())
  {
    return {sylvan::ldds::includes(reach.partial_solution().winning[0], reach.initial_state()), sylvan::ldds::satcount(reach.V())};
  }
  pbes_system::symbolic_parity_game G(reach.pbes(), reach.summand_groups(), reach.data_index(), reach.V(),
                                      options.no_relprod, options.chaining, options.compute_strategy);
  pbes_system::symbolic_pbessolve_algorithm solver(G, options.check_strategy, options.compute_strategy);
  auto [result, solution] = solver.solve(reach.initial_state(), reach.V(), reach.deadlocks(), reach.partial_solution());
  return {result, sylvan::ldds::satcount(reach.V())};
}

// Sylvan operations must be executed by a Lace worker, so the test body runs as a Lace task.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_DECL_0(bool, pbesreach_test_task);
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define pbesreach_test_task() RUN(pbesreach_test_task)

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_IMPL_0(bool, pbesreach_test_task)
{
  for (const std::string& text: pbess)
  {
    const pbes_system::pbes p = pbes_system::txt2pbes(text);
    const auto [expected_answer, expected_vertices] = solve(p, pbes_system::symbolic_reachability_options());
    BOOST_CHECK(expected_vertices > 0);
    for (const auto& [name, set_options]: variants)
    {
      pbes_system::symbolic_reachability_options options;
      set_options(options);
      BOOST_TEST_CONTEXT("variant " << name << " on " << text)
      {
        const auto [answer, vertices] = solve(p, options);
        BOOST_CHECK_EQUAL(answer, expected_answer);
        BOOST_CHECK_EQUAL(vertices, expected_vertices);
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

  pbesreach_test_task();

  sylvan::sylvan_quit();
  lace_stop();
}

#endif // MCRL2_ENABLE_SYLVAN
