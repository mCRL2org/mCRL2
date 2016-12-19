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
    "proc P(b1_X: Bool) =\n"
    "       a(b1_X) .\n"
    "         dist b1_X1: Bool[1 / 2] .\n"
    "         P(b1_X = b1_X1);\n"
    "\n"
    "init dist b1: Bool[1 / 2] . P(b1);\n"
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
    "proc P(b3_X,b4_X: Bool) =\n"
    "       a(b3_X, b4_X) .\n"
    "         dist b3_X1,b4_X1: Bool[if(b3_X1, 1 / 8, 3 / 8)] .\n"
    "         P(b3_X = b3_X1, b4_X = b4_X1);\n"
    "\n"
    "init dist b3,b4: Bool[if(b3, 1 / 8, 3 / 8)] . P(b3, b4);\n"
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

  stochastic_specification spec=linearise(text);
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
