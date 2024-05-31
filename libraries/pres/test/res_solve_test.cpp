// Author(s): Jan Friso Groote. Based on bes_solve by Wieger Wesselink
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

#include "mcrl2/pres/parse.h"
#include "mcrl2/pres/rewrite.h"
#include "mcrl2/pres/ressolve_gauss_elimination.h"
#include "mcrl2/pres/ressolve_numerical.h"
#include "mcrl2/pres/ressolve_numerical_directed.h"  // This include must be last. 

using namespace mcrl2;
using namespace mcrl2::pres_system;

constexpr double infinity = std::numeric_limits<double>::infinity();

void run_all_algorithms(std::string const& b, double expected_outcome)
{
  pres b1;
  std::stringstream from;
  from << "pres\n" << b << std::endl;
  from >> b1;

  data::rewriter datar(b1.data());
  simplify_data_rewriter presrewr(b1.data(), datar);
  pres_rewrite(b1,presrewr);
  pressolve_options options;
  ressolve_by_numerical_iteration_directed solver1(options, b1);
  double result=solver1.run();

  if (float(result)!=float(expected_outcome))
  {
    std::cerr << "Solving the following res using numerical directed fails: \nExpected outcome: "<< expected_outcome 
              << "\nObtained outcome: " << result << "\n" << pres_system::pp(b1) << std::endl;
    BOOST_CHECK_EQUAL(float(result), float(expected_outcome));
  }

  ressolve_by_numerical_iteration solver2(options, b1);
  result=solver2.run();
  
  if (float(result)!=float(expected_outcome))
  { 
    std::cerr << "Solving the following res using the plain numerical algorithm fails: \nExpected outcome: "<< expected_outcome 
              << "\nObtained outcome: " << result << "\n" << pres_system::pp(b1) << std::endl;
    BOOST_CHECK_EQUAL(float(result), float(expected_outcome));
  }

  ressolve_by_gauss_elimination_algorithm solver3(options, b1);
  pres_expression result_as_pres=solver3.run();
  if (!(data::is_data_expression(result_as_pres)||is_true(result_as_pres)||is_false(result_as_pres)))
  {
    std::cerr << "Expected data expression when solving pres: " << result_as_pres << "\n" << pres_system::pp(b1) << std::endl;
    BOOST_CHECK(data::is_data_expression(result_as_pres));
  }
  else 
  {
    if (is_true(result_as_pres))
    {
      result=infinity;
    }
    else if (is_false(result_as_pres))
    {
      result=-infinity;
    }
    else 
    {
      result=data::sort_real::value<double>(atermpp::down_cast<data::data_expression>(result_as_pres));
    }
    
    if (result!=expected_outcome)
    { 
      std::cerr << "Solving the following res using the Gauss-elimination algorithm fails: \nExpected outcome: "<< expected_outcome 
                << "\nObtained outcome: " << result << "\n" << pres_system::pp(b1) << std::endl;
      BOOST_CHECK_EQUAL(result, expected_outcome);
    }
  }
}

BOOST_AUTO_TEST_CASE(test_simple_nu_mu)
{
  std::string b(
    "mu X1 = X2;\n"
    "nu X2 = X1;\n"
    "init X1;\n"
  );
  run_all_algorithms(b, -infinity);
}

BOOST_AUTO_TEST_CASE(test_simple_mu_nu)
{
  std::string b(
    "nu X1 = X2;\n"
    "mu X2 = X1;\n"
    "init X1;\n"
  );
  run_all_algorithms(b, infinity);
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
  run_all_algorithms(b, -infinity);
}

BOOST_AUTO_TEST_CASE(addplusminusinfinity)
{
  std::string b(
    "mu X1 = X1 + val(true); \n"
    "                  \n"
    "init X1;          \n"
  );
  run_all_algorithms(b, infinity);
}

BOOST_AUTO_TEST_CASE(numerical_approximation)
{
  std::string b(
    "mu X1 = (val(1/2)*X1 + val(1))||val(0); \n"
    "                  \n"
    "init X1;          \n"
  );
  run_all_algorithms(b, 2.0);
}

