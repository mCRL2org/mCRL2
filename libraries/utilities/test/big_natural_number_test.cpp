// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file big_numbers_test.cpp
/// \brief Test whether the class big_natural_numbers works properly.

#include "mcrl2/utilities/big_numbers.h"
#include <boost/test/included/unit_test_framework.hpp>

using namespace mcrl2;
using namespace mcrl2::utilities;

void test_print(const std::string& xs)
{
  const big_natural_number x(xs);
  std::stringstream ss;
  ss << x;
  std::cerr << "Check print \n" << xs << "\n" << x << "\n" << ss.str() << "\n-------------\n";
  BOOST_CHECK(xs==ss.str());
}

void test_simple(const std::string& xs, const std::string& ys)
{
  const big_natural_number x(xs);
  const big_natural_number y(ys);
  
  std::cerr << "Check simple add and subtract " << xs << " and " << ys << "\n";
  std::cerr << "LHS: " << x << "\n";
  std::cerr << "RHS: " << (x+y)-y << "\n";
  BOOST_CHECK(x==(x+y)-y);
}

void test_div_mod(const std::string& xs, const std::string& ys)
{
  const big_natural_number x(xs);
  const big_natural_number y(ys);
  
  if (!y.is_zero())
  { 
    std::cerr << "Check div mod for " << xs << " and " << ys << "\n";
    // std::cerr << "x/y " << (x/y) << "\n";
    // std::cerr << "y*(x/y) " << (y*(x/y)) << "\n";
    // std::cerr << "x%y " << (x%y) << "\n";
    std::cerr << "LHS: " << x << "\n";
    std::cerr << "RHS: " << y*(x/y)+(x % y) << "\n";
    BOOST_CHECK(x==y*(x/y)+(x % y));
  }
}

void test_multiply(const std::string& xs, const std::string& ys)
{ 
  big_natural_number x(xs);
  big_natural_number y(ys);
  
  std::cerr << "Check multiply " << xs << " and " << ys << "\n";
  std::cerr << "LHS: " << x*y << "\n";
  std::cerr << "RHS: " << y*x << "\n";
  BOOST_CHECK(x*y==y*x);
}

void test_plus_minus_multiply(const std::string& xs, const std::string& ys)
{ 
  big_natural_number x(xs);
  big_natural_number y(ys);
  
  if (y>x)
  {
    swap(x,y);
  }
  BOOST_CHECK(x>=y);

  std::cerr << "Check plus minus " << xs << " and " << ys << "\n";

  // std::cerr << "x+y: " << (x+y) << "\n";
  // std::cerr << "x-y: " << (x-y) << "\n";
  // std::cerr << "x*x: " << x*x << "\n";
  // std::cerr << "y*y: " << y*y << "\n";
  std::cerr << "LHS: " << (x+y)*(x-y) << "\n";
  std::cerr << "RHS: " << x*x-y*y << "\n";
  BOOST_CHECK((x+y)*(x-y)==x*x-y*y);
}

void test(const std::string& xs, const std::string& ys)
{
  test_print(xs);
  test_print(ys);
  test_simple(xs,ys);
  test_simple(ys,xs);
  test_multiply(xs,ys);
  test_multiply(ys,xs);
  test_plus_minus_multiply(xs,ys);
  test_plus_minus_multiply(ys,xs);
  test_div_mod(xs,ys);
  test_div_mod(ys,xs);
}

BOOST_AUTO_TEST_CASE(big_natural_number_test)
{
  const big_natural_number x(10);
  const big_natural_number y(0);
  const big_natural_number z(100);
  BOOST_CHECK(x-x==y); 
  BOOST_CHECK(x*x==z); 
}

BOOST_AUTO_TEST_CASE(cumulative_tests)
{
  test("12","14");
  test("15","9");
  test("123987498734298734987","0");
  test("123987498734298734987","1");
  test("1400000000000000000021498639574985789345798","12000000000000000000123");
  test("12000000000000000000123","1400000000000000000021498639574985789345798");
  test("349857349587453098713409835719348571930857","34908657984275902384759028475908345");
  test("34985430320129384710938471039561390847109398734601956601293846019285609853607349587453098713409835719348571930857","34908657984275902384759028475908345");
  test("34985430320129384710938471039561390847109398734601956601293846019285609853607349587453098713409835719348571930857",
       "349086579842759023847590143958749587439587209346509145610983654498365918365918650914657816458768976519846519846519846123456789000909029485610945645198435628475908345");
  test("34985431223981954640133634673587613874569183765329875682716348576138476108576387546187658127653201876510287356021876530287165023817650187635081237650812376501876350871236501287365012873650182735610237560000000000000000000000000320129384710938471039561390847109398734601956601293846019285609853607349587453098713409835719348571930857",
       "34908657984275902384759014395874958743958720934650914561098365449836591836591865091465781645876897651984651984651984612345678900090913498173498173419837409136798130865139580010203999992929934847837413984719835610934601958632091863509826159836159861938651983651348716325918635748372987429029485610945645198435628475908345");

  std::string big_number("34985431223981954640133634673587613874569183765329875682716348576138476108576387546187658127653201876510287356021876530287165023817650187635081237650812376501876350871236501287365012873650182735610237560000000000000000000000000320129384710938471039561390847109398734601956601293846019285609853607349587453098713409835719348571930857");
  test(big_number,big_number);
}


boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
