// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file solve_test.cpp
/// \brief Test for BES solvers.

#include "mcrl2/bes/gauss_elimination.h"
#include "mcrl2/bes/parse.h"
#include "mcrl2/bes/print.h"
#include "mcrl2/bes/small_progress_measures.h"
#include <boost/test/included/unit_test_framework.hpp>
#include <sstream>
#include <string>

using namespace mcrl2;
using namespace mcrl2::bes;

void run_all_algorithms(std::string const& b, bool expected_outcome)
{
  boolean_equation_system b1;
  std::stringstream from;
  from << "pbes\n" << b << std::endl;
  from >> b1;

  std::clog << "solving the following input bes: \n" << bes::pp(b1) << std::endl;

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

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
