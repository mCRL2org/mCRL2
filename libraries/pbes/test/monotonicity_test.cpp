// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file monotonicity_test.cpp
/// \brief Tests for the is_monotonous function for pbes expressions.

#include <iostream>
#include <string>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/pbes/monotonicity.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2::pbes_system;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

void run_monotonicity_test_case(const std::string& pbes_spec, const bool expect_success = true)
{
  pbes<> p = txt2pbes(pbes_spec);
  BOOST_CHECK(is_monotonous(p) == expect_success);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  run_monotonicity_test_case(
    "pbes nu X(m, n: Nat) =                  \n"
    "        val(n == m) && X(m + 1, n + 1); \n"
    "                                        \n"
    " init X(0, 0);                          \n"
    , true);

// Cannot run this test due to earlier checks...
//  run_monotonicity_test_case(
//    "pbes nu X(m, n: Nat) =                   \n"
//    "        val(n == m) && !X(m + 1, n + 1); \n"
//    "                                         \n"
//    " init X(0, 0);                           \n"
//  , false);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
