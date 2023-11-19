// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file solve_test.cpp
/// \brief Test for BES solvers.

#define BOOST_TEST_MODULE solve_test

#include <boost/test/included/unit_test.hpp>
#include "mcrl2/pbes/pbes_gauss_elimination.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/small_progress_measures.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void run_all_algorithms(std::string const& b, bool expected_outcome)
{
  pbes b1;
  std::stringstream from;
  from << "pbes\n" << b << std::endl;
  from >> b1;

  std::clog << "solving the following input bes: \n" << pbes_system::pp(b1) << std::endl;

  BOOST_CHECK_EQUAL(small_progress_measures(b1), expected_outcome);
  BOOST_CHECK_EQUAL(gauss_elimination(b1), expected_outcome);
}

BOOST_AUTO_TEST_CASE(test_simple_nu_mu)
{
  std::string b(
    "mu X1 = X2;\n"
    "nu X2 = X1;\n"
    "init X1;\n"
  );
  run_all_algorithms(b, false);
}

BOOST_AUTO_TEST_CASE(test_simple_mu_nu)
{
  std::string b(
    "nu X1 = X2;\n"
    "mu X2 = X1;\n"
    "init X1;\n"
  );
  run_all_algorithms(b, true);
}

BOOST_AUTO_TEST_CASE(test_simple)
{
  std::string b(
    "mu X1 = X2 && X1; \n"
    "nu X2 = X1 || X3; \n"
    "mu X3 = X1 || X2; \n"
    "                  \n"
    "init X1;          \n"
  );
  run_all_algorithms(b, false);
}
