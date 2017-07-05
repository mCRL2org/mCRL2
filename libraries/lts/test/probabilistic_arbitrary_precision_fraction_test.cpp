// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file big_numbers_test.cpp
/// \brief Test whether the class bbilistic_arbitrary_precision_fraction works properly.

#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/lts/probabilistic_arbitrary_precision_fraction.h"

using namespace mcrl2;
using namespace mcrl2::lts;

void test_divide_multiply(const std::string& xs, const std::string& ys)
{ 
  std::cerr << "Check multiply " << xs << " and " << ys << "\n";
  const probabilistic_arbitrary_precision_fraction x(xs,ys);

  probabilistic_arbitrary_precision_fraction one("1","1");

  if (x>(one-one))
  {
    probabilistic_arbitrary_precision_fraction y=x/x;
    BOOST_CHECK(y==one);

    y=one-x;
    std::cerr << "LHS: " << x*y << "\n";
    std::cerr << "RHS: " << y*x << "\n";
    BOOST_CHECK(x*y==y*x);
  }
}

void test_gcd(const std::string& xs)
{
  std::cerr << "GCD " << xs << "\n";
  utilities::big_natural_number n1(xs);

  BOOST_CHECK(probabilistic_arbitrary_precision_fraction::greatest_common_divisor(n1,n1)==n1);
  BOOST_CHECK(probabilistic_arbitrary_precision_fraction::greatest_common_divisor(n1+n1,n1)==n1);
}

void test(const std::string& xs, const std::string& ys)
{
  test_gcd(xs);
  test_gcd(ys);
  test_divide_multiply(xs,ys);
}

BOOST_AUTO_TEST_CASE(cumulative_tests)
{
  test("12","14");
  test("15","90");
  test("0","90");
  test("1","123987498734298734987");
  test("12000000000000000000123","1400000000000000000021498639574985789345798");
  test("12000000000000000000123","1400000000000000000021498639574985789345798");
  test("349857349587453098713409835719348571930857","34908657984275902384759029877987987979878978475908345");
  test("34985430320129384710938471039561390847109398734601956601293846019285609853607349587453098713409835719348571930857",
       "34985431223981954640133634673587613874569183765329875682716348576138476108576387546187658127653201876510287356021876530287165023817650187635081237650812376501876350871236501287365012873650182735610237560000000000000000000000000320129384710938471039561390847109398734601956601293846019285609853607349587453098713409835719348571930857");
}


boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
