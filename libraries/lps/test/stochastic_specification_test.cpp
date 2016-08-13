// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file specification_test.cpp
/// \brief Add your file description here.

#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/is_stochastic.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parelm.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/detail/test_input.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

BOOST_AUTO_TEST_CASE(test_empty_distribution)
{
  stochastic_distribution dist;
  std::cout << "empty dist = " << dist << std::endl;
  BOOST_CHECK(!dist.is_defined());
}

BOOST_AUTO_TEST_CASE(test_remove_stochastic_operators)
{
  std::string text =
    "act a;\n"
    "proc P = a.P;\n"
    "init P;"
    ;

  stochastic_specification src;
  parse_lps(text, src);
  BOOST_CHECK(!is_stochastic(src));

  specification dest = remove_stochastic_operators(src);
  std::cout << "dest = " << dest;
  BOOST_CHECK(lps::pp(src) == lps::pp(dest));
}

BOOST_AUTO_TEST_CASE(test_is_stochastic)
{
  std::string text =
    "act  throw: Bool;                       \n"
    "                                        \n"
    "proc P = dist b:Bool[1/2].throw(b) . P; \n"
    "                                        \n"
    "init P;                                 \n"
    ;
  stochastic_specification spec;
  parse_lps(text, spec);
  BOOST_CHECK(is_stochastic(spec));

  text =
    "act  throw: Bool;         \n"
    "                          \n"
    "proc P = throw(true) . P; \n"
    "                          \n"
    "init dist b:Bool[1/2] . P;\n"
    ;
  parse_lps(text, spec);
  BOOST_CHECK(is_stochastic(spec));
}

BOOST_AUTO_TEST_CASE(test_print)
{
  std::string text =
    "act  a: Bool;\n"
    "\n"
    "proc P(s3: Pos, p: Bool) =\n"
    "       (s3 == 1) ->\n"
    "         a(p) .\n"
    "         dist p: Bool[1 / 2] .\n"
    "         P(s3 = 2, p = true)\n"
    "     + delta;\n"
    "\n"
    "init dist p: Bool[1 / 2] . P(1, true);\n"
    ;
  stochastic_specification spec;
  parse_lps(text, spec);
  std::string result = lps::pp(spec);
  BOOST_CHECK_EQUAL(text, result);
}

// The following specification did not linearise, due to an infinite loop.
BOOST_AUTO_TEST_CASE(test_linearisation)
{
  std::string text =
    "act a:Bool;\n"
    "proc X=dist b:Bool[1/2].a(b).X;\n"
    "init X;\n"
    ;

  std::string result =
    "act  a: Bool;\n"
    "\n"
    "proc P(b_X: Bool) =\n"
    "       a(b_X) .\n"
    "         dist b1_X: Bool[1 / 2] .\n"
    "         P(b_X = b1_X);\n"
    "\n"
    "init dist b: Bool[1 / 2] . P(b);\n"
    ;

  stochastic_specification spec=linearise(text);
  BOOST_CHECK_EQUAL(lps::pp(spec),result);
}  

// This test checks whether multiple parameters in a stochastic operator are translated 
// correctely.
BOOST_AUTO_TEST_CASE(test_multiple_stochastic_parameters)
{
  std::string text =
    "act a:Bool#Bool;\n"
    "proc X=dist b1,b2:Bool[if(b1,1/8,3/8)].a(b1,b2).X;\n"
    "init X;\n"
    ;

  std::string result =
    "act  a: Bool # Bool;\n"
    "\n"
    "proc P(b1_X,b2_X: Bool) =\n"
    "       a(b1_X, b2_X) .\n"
    "         dist b3_X,b4_X: Bool[if(b3_X, 1 / 8, 3 / 8)] .\n"
    "         P(b1_X = b3_X, b2_X = b4_X);\n"
    "\n"
    "init dist b1,b2: Bool[if(b1, 1 / 8, 3 / 8)] . P(b1, b2);\n"
    ;

  stochastic_specification spec=linearise(text);
  BOOST_CHECK_EQUAL(lps::pp(spec),result);
}  

// This test checks whether the outward distribution of dist operators is going 
// correctely.
BOOST_AUTO_TEST_CASE(test_push_dist_outward)
{
  std::string text =
    "sort Coin = struct head_ | tail_;\n"
    "\n"
    "act throw:Coin;\n"
    "    success;\n"
    "\n"
    "proc X(head_seen:Bool) =  dist c:Coin[1/2].throw(c).\n"
    "                           ((c==head_ && head_seen) -> success.delta+\n"
    "                            (c==head_ && !head_seen) -> X(true)+\n"
    "                            (c==tail_) -> X(false));\n"
    "\n"
    "init X(false);\n"
    ;

  std::string result1 =
    "sort Coin = struct head_ | tail_;\n"
    "     Enum3 = struct e2_3 | e1_3 | e0_3;\n"
    "\n"
    "map  C3_: Enum3 # Coin # Coin # Coin -> Coin;\n"
    "     C3_1: Enum3 # Pos # Pos # Pos -> Pos;\n"
    "     C3_2: Enum3 # Bool # Bool # Bool -> Bool;\n"
    "     C3_3: Enum3 # Real # Real # Real -> Real;\n"
    "\n"
    "var  x1,y3,y2,y1: Coin;\n"
    "     e1,e2,e3,e4: Enum3;\n"
    "     x2,y6,y5,y4: Pos;\n"
    "     x3,y9,y8,y7: Bool;\n"
    "     x4,y12,y11,y10: Real;\n"
    "eqn  C3_(e1, x1, x1, x1)  =  x1;\n"
    "     C3_(e2_3, y3, y2, y1)  =  y3;\n"
    "     C3_(e1_3, y3, y2, y1)  =  y2;\n"
    "     C3_(e0_3, y3, y2, y1)  =  y1;\n"
    "     C3_1(e2, x2, x2, x2)  =  x2;\n"
    "     C3_1(e2_3, y6, y5, y4)  =  y6;\n"
    "     C3_1(e1_3, y6, y5, y4)  =  y5;\n"
    "     C3_1(e0_3, y6, y5, y4)  =  y4;\n"
    "     C3_2(e3, x3, x3, x3)  =  x3;\n"
    "     C3_2(e2_3, y9, y8, y7)  =  y9;\n"
    "     C3_2(e1_3, y9, y8, y7)  =  y8;\n"
    "     C3_2(e0_3, y9, y8, y7)  =  y7;\n"
    "     C3_3(e4, x4, x4, x4)  =  x4;\n"
    "     C3_3(e2_3, y12, y11, y10)  =  y12;\n"
    "     C3_3(e1_3, y12, y11, y10)  =  y11;\n"
    "     C3_3(e0_3, y12, y11, y10)  =  y10;\n"
    "\n"
    "act  throw: Coin;\n"
    "     success;\n"
    "\n"
    "glob dc5: Bool;\n"
    "     dc4,dc3,dc2,dc1,dc: Coin;\n"
    "\n"
    "proc P(s1_X: Pos, c1_X,c2_X,c_X: Coin, head_seen_X: Bool) =\n"
    "       (s1_X == 1 && c_X == head_ && head_seen_X) ->\n"
    "         success .\n"
    "         P(s1_X = 3, c1_X = dc2, c2_X = dc3, c_X = dc4, head_seen_X = dc5)\n"
    "     + sum e_X: Enum3.\n"
    "         C3_2(e_X, s1_X == 2, s1_X == 1 && c_X == tail_, s1_X == 1 && c_X == head_ && !head_seen_X) ->\n"
    "         throw(C3_(e_X, c_X, c2_X, c1_X)) .\n"
    "         dist c4_X,c5_X: Coin[1 / 4] .\n"
    "         P(s1_X = 1, c1_X = c4_X, c2_X = c5_X, c_X = C3_(e_X, c_X, c2_X, c1_X), head_seen_X = C3_2(e_X, head_seen_X, false, true));\n"
    "\n"
    "init dist c: Coin[1 / 2] . P(2, dc1, dc, c, false);\n"
    ;

std::string result2 =
    "sort Coin = struct head_ | tail_;\n"
    "     Enum3 = struct e2_3 | e1_3 | e0_3;\n"
    "\n"
    "map  C3_: Enum3 # Coin # Coin # Coin -> Coin;\n"
    "     C3_1: Enum3 # Pos # Pos # Pos -> Pos;\n"
    "     C3_2: Enum3 # Bool # Bool # Bool -> Bool;\n"
    "     C3_3: Enum3 # Real # Real # Real -> Real;\n"
    "\n"
    "var  x1,y3,y2,y1: Coin;\n"
    "     e1,e2,e3,e4: Enum3;\n"
    "     x2,y6,y5,y4: Pos;\n"
    "     x3,y9,y8,y7: Bool;\n"
    "     x4,y12,y11,y10: Real;\n"
    "eqn  C3_(e1, x1, x1, x1)  =  x1;\n"
    "     C3_(e2_3, y3, y2, y1)  =  y3;\n"
    "     C3_(e1_3, y3, y2, y1)  =  y2;\n"
    "     C3_(e0_3, y3, y2, y1)  =  y1;\n"
    "     C3_1(e2, x2, x2, x2)  =  x2;\n"
    "     C3_1(e2_3, y6, y5, y4)  =  y6;\n"
    "     C3_1(e1_3, y6, y5, y4)  =  y5;\n"
    "     C3_1(e0_3, y6, y5, y4)  =  y4;\n"
    "     C3_2(e3, x3, x3, x3)  =  x3;\n"
    "     C3_2(e2_3, y9, y8, y7)  =  y9;\n"
    "     C3_2(e1_3, y9, y8, y7)  =  y8;\n"
    "     C3_2(e0_3, y9, y8, y7)  =  y7;\n"
    "     C3_3(e4, x4, x4, x4)  =  x4;\n"
    "     C3_3(e2_3, y12, y11, y10)  =  y12;\n"
    "     C3_3(e1_3, y12, y11, y10)  =  y11;\n"
    "     C3_3(e0_3, y12, y11, y10)  =  y10;\n"
    "\n"
    "act  throw: Coin;\n"
    "     success;\n"
    "\n"
    "glob dc5: Bool;\n"
    "     dc4,dc3,dc2,dc1,dc: Coin;\n"
    "\n"
    "proc P(s1_X: Pos, c1_X,c2_X,c_X: Coin, head_seen_X: Bool) =\n"
    "       (s1_X == 2 && c_X == head_ && head_seen_X) ->\n"
    "         success .\n"
    "         P(s1_X = 3, c1_X = dc2, c2_X = dc3, c_X = dc4, head_seen_X = dc5)\n"
    "     + sum e_X: Enum3.\n"
    "         C3_2(e_X, s1_X == 2 && c_X == tail_, s1_X == 2 && c_X == head_ && !head_seen_X, s1_X == 1) ->\n"
    "         throw(C3_(e_X, c2_X, c1_X, c_X)) .\n"
    "         dist c3_X,c4_X: Coin[1 / 4] .\n"
    "         P(s1_X = 2, c1_X = c3_X, c2_X = c4_X, c_X = C3_(e_X, c2_X, c1_X, c_X), head_seen_X = C3_2(e_X, false, true, head_seen_X));\n"
    "\n"
    "init dist c: Coin[1 / 2] . P(1, dc1, dc, c, false);\n"
    ;



  stochastic_specification spec=linearise(text);
  // The result can either be result1 or result2.
  if (lps::pp(spec)!=result1)
  {
    BOOST_CHECK_EQUAL(lps::pp(spec),result2);
  }
}  


BOOST_AUTO_TEST_CASE(test_parelm)
{
  stochastic_specification spec = linearise(lps::detail::ABP_SPECIFICATION());
  std::set<data::variable> v = lps::find_all_variables(spec);
  parelm(spec);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
