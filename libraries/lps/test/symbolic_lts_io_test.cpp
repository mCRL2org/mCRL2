// Author(s): Tom van Dijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file symbolic_lts_io_test.cpp
/// \brief Tests that a symbolic LTS learned by lpsreach is the same after writing and reading it.

#define BOOST_TEST_MODULE symbolic_lts_io_test
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(dummy_test)
{
  // This is an empty test since at least one test is required.
}

#ifdef MCRL2_ENABLE_SYLVAN

#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/lpsreach.h"
#include "mcrl2/lps/symbolic_lts_io.h"

#include <sstream>
#include <string>

using namespace mcrl2;

// Both summands use x and y, so --groups=used puts them in one group. The summand with w does not write y, which gives
// copy nodes at a write level, and the summand with v does not read x, which gives copy nodes at a read level.
static const std::string specification =
  "act w: Nat # Nat; v: Nat;"
  "proc P(x: Nat, y: Nat) = w(x, y) . P(x = (x + 1) mod 3) + v(y) . P(x = 0, y = (y + 1) mod 2);"
  "init P(0, 0);";

static bool contains_copy_node(const sylvan::ldds::ldd& U)
{
  if (U == sylvan::ldds::false_() || U == sylvan::ldds::true_())
  {
    return false;
  }
  return sylvan::lddmc_iscopy(U.get()) != 0 || contains_copy_node(U.down()) || contains_copy_node(U.right());
}

static void check_write_and_read(const lps::specification& lpsspec, const std::string& summand_groups, bool has_copy_nodes)
{
  symbolic::symbolic_reachability_options options;
  options.summand_groups = summand_groups;
  lps::lpsreach_algorithm algorithm(lpsspec, options);
  BOOST_CHECK_EQUAL(sylvan::ldds::satcount(algorithm.run()), 6.0);
  const lps::symbolic_lts& lts = algorithm.get_symbolic_lts();

  std::stringstream stream;
  stream << lts;
  lps::symbolic_lts result;
  stream >> result;

  BOOST_CHECK(result.initial_state == lts.initial_state);
  BOOST_CHECK(result.states == lts.states);
  BOOST_CHECK_EQUAL(result.summand_groups.size(), lts.summand_groups.size());
  if (result.summand_groups.size() != lts.summand_groups.size())
  {
    return; // BOOST_REQUIRE would throw inside a Lace task.
  }

  bool copy_nodes = false;
  for (std::size_t i = 0; i < lts.summand_groups.size(); ++i)
  {
    BOOST_TEST_CONTEXT("summand group " << i)
    {
      BOOST_CHECK(result.summand_groups[i].read == lts.summand_groups[i].read);
      BOOST_CHECK(result.summand_groups[i].write == lts.summand_groups[i].write);
      BOOST_CHECK(result.summand_groups[i].L == lts.summand_groups[i].L);
    }
    copy_nodes = copy_nodes || contains_copy_node(lts.summand_groups[i].L);
  }
  BOOST_CHECK_EQUAL(copy_nodes, has_copy_nodes);
}

// Sylvan operations must be executed by a Lace worker, so the test body runs as a Lace task.
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_DECL_0(bool, symbolic_lts_io_test_task);
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define symbolic_lts_io_test_task() RUN(symbolic_lts_io_test_task)

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
TASK_IMPL_0(bool, symbolic_lts_io_test_task)
{
  lps::t_lin_options lin_options;
  lin_options.ignore_time = true; // as mcrl22lps without --timed
  const lps::specification lpsspec = lps::remove_stochastic_operators(lps::linearise(specification, lin_options));
  BOOST_TEST_CONTEXT("--groups=none")
  {
    check_write_and_read(lpsspec, "none", false);
  }
  BOOST_TEST_CONTEXT("--groups=used")
  {
    check_write_and_read(lpsspec, "used", true);
  }
  return true;
}

BOOST_AUTO_TEST_CASE(write_and_read)
{
  lace_start(1, 0);
  sylvan::sylvan_set_limits(256 * 1024 * 1024, 1, 4);
  sylvan::sylvan_init_package();
  sylvan::sylvan_init_ldd();
  sylvan::ldds::initialise();

  symbolic_lts_io_test_task();

  sylvan::sylvan_quit();
  lace_stop();
}

#endif // MCRL2_ENABLE_SYLVAN
