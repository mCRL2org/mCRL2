// Author(s): Tom van Dijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsreach_action_labels_test.cpp
/// \brief Tests that lpsreach labels each learned transition with the action of the summand it comes from.

#define BOOST_TEST_MODULE lpsreach_action_labels_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(dummy_test)
{
  // This is an empty test since at least one test is required.
}

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/data/bool.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/lpsreach.h"

#include <cstdint>
#include <string>
#include <vector>

using namespace mcrl2;

// Only the summand with action c is enabled in state b = false, and only the summand with
// action a in state b = true. With --groups=used both summands are in the same group.
static const std::string specification =
  "act a, c;"
  "proc P(b: Bool) = b -> a . P(!b) + !b -> c . P(!b);"
  "init P(false);";

static void check_action_labels(const lps::specification& lpsspec, const std::string& summand_groups)
{
  symbolic::symbolic_reachability_options options;
  options.summand_groups = summand_groups;
  lps::lpsreach_algorithm algorithm(lpsspec, options);
  BOOST_CHECK_EQUAL(sylvan::ldds::satcount(algorithm.run()), 2.0);

  const lps::symbolic_lts& lts = algorithm.get_symbolic_lts();
  BOOST_CHECK_EQUAL(lts.action_index.size(), 2u);

  std::size_t number_of_transitions = 0;
  for (const lps::lps_summand_group& group: lts.summand_groups)
  {
    BOOST_REQUIRE_EQUAL(group.read.size(), 1u);
    for (const std::vector<std::uint32_t>& xy: sylvan::ldds::ldd_solutions(group.L))
    {
      // The action label is the last element of a learned transition.
      const data::data_expression& b = lts.data_index[group.read[0]][xy[group.read_pos[0]]];
      const std::string expected = data::sort_bool::is_true_function_symbol(b) ? "a" : "c";
      BOOST_CHECK_EQUAL(lps::pp(lts.action_index[xy.back()]), expected);
      number_of_transitions++;
    }
  }
  BOOST_CHECK_EQUAL(number_of_transitions, 2u);
}

// Sylvan operations must be executed by a Lace worker, so the test body runs as a Lace task.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_DECL_0(bool, lpsreach_action_labels_test_task);
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define lpsreach_action_labels_test_task() RUN(lpsreach_action_labels_test_task)

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_IMPL_0(bool, lpsreach_action_labels_test_task)
{
  lps::t_lin_options lin_options;
  lin_options.ignore_time = true; // as mcrl22lps without --timed
  const lps::specification lpsspec = lps::remove_stochastic_operators(lps::linearise(specification, lin_options));
  for (const char* summand_groups: {"none", "used", "simple", "0 1"})
  {
    BOOST_TEST_CONTEXT("--groups=" << summand_groups)
    {
      check_action_labels(lpsspec, summand_groups);
    }
  }
  return true;
}

BOOST_AUTO_TEST_CASE(action_labels)
{
  lace_start(1, 0);
  sylvan::sylvan_set_limits(256 * 1024 * 1024, 1, 4);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_ldd();
  sylvan::ldds::initialise();

  lpsreach_action_labels_test_task();

  sylvan::sylvan_quit();
  lace_stop();
}

#endif // MCRL2_ENABLE_SYLVAN
