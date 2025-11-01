// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file stochastic_specification_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE stochastic_specification_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/lps/is_stochastic.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parelm.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/is_well_typed.h"

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

// This test checks whether a parameter in a stochastic operator are translated 
// correctely.
BOOST_AUTO_TEST_CASE(test_properly_change_bound_variables)
{
  std::string text =
    "map distribution: Bool#Pos#Pos->Real;\n"
    "\n"
    "act win, lose;\n"
    "    hold:Bool;\n"
    "\n"
    "proc Play(round_: Nat, hold3:Bool, r3: Pos) = \n"
    "       dist s3:Pos[distribution(hold3, r3, s3)].\n"
    "         (\n"
    "            (round_==1) -> lose.delta\n"
    "                        <>  sum b3: Bool. hold(b3).Play(round_+1, b3, s3 )\n"
    "        );\n"
    "\n"
    "init Play(0, false, 1);\n"
;

  std::string result1 =
    "map  distribution: Bool # Pos # Pos -> Real;\n"
    "\n"
    "act  win,lose;\n"
    "     hold: Bool;\n"
    "\n"
    "glob dc: Pos;\n"
    "     dc1: Nat;\n"
    "\n"
    "proc P(s4_Play,s1_Play: Pos, round__Play: Nat) =\n"
    "       sum b1_Play: Bool.\n"
    "         (s4_Play == 2 && !(round__Play == 1)) ->\n"
    "         hold(b1_Play) .\n"
    "         dist s2_Play: Pos[distribution(b1_Play, s1_Play, s2_Play)] .\n"
    "         P(s4_Play = 2, s1_Play = s2_Play, round__Play = round__Play + 1)\n"
    "     + (s4_Play == 2 && round__Play == 1) ->\n"
    "         lose .\n"
    "         P(s4_Play = 1, s1_Play = dc, round__Play = dc1)\n"
    "     + delta;\n"
    "\n"
    "init dist s1: Pos[distribution(false, 1, s1)] . P(2, s1, 0);\n"
    ;

  std::string result2 =
    "map  distribution: Bool # Pos # Pos -> Real;\n"
    "\n"
    "act  win,lose;\n"
    "     hold: Bool;\n"
    "\n"
    "glob dc: Pos;\n"
    "     dc1: Nat;\n"
    "\n"
    "proc P(s4_Play,s1_Play: Pos, round__Play: Nat) =\n"
    "       sum b1_Play: Bool.\n"
    "         (s4_Play == 1 && !(round__Play == 1)) ->\n"
    "         hold(b1_Play) .\n"
    "         dist s2_Play: Pos[distribution(b1_Play, s1_Play, s2_Play)] .\n"
    "         P(s4_Play = 1, s1_Play = s2_Play, round__Play = round__Play + 1)\n"
    "     + (s4_Play == 1 && round__Play == 1) ->\n"
    "         lose .\n"
    "         P(s4_Play = 2, s1_Play = dc, round__Play = dc1)\n"
    "     + delta;\n"
    "\n"
    "init dist s1: Pos[distribution(false, 1, s1)] . P(1, s1, 0);\n"
    ;

  stochastic_specification spec=linearise(text);
std::cerr << "RESULT " << spec << "\n";
  BOOST_CHECK(lps::pp(spec)==result1 ||   // The lineariser can up with multiple results, depending on how terms are stored internally. 
              lps::pp(spec)==result2);
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
  BOOST_CHECK(lps::detail::is_well_typed(spec));
}  


BOOST_AUTO_TEST_CASE(test_parelm)
{
  stochastic_specification spec = linearise(lps::detail::ABP_SPECIFICATION());
  std::set<data::variable> v = lps::find_all_variables(spec);
  parelm(spec);
  BOOST_CHECK(lps::detail::is_well_typed(spec));
}

/* This test case caused the lineariser to incorrectly handle bound variables in the distribution operator */
BOOST_AUTO_TEST_CASE(bound_variable)
{
  std::string text =
    "act a;\n"
    "init dist x0: Bool[if(x0,1/3,2/3)].(x0 -> (a . dist x1: Bool[1/2].(x1 -> a)));\n";
  stochastic_specification spec=linearise(text);
  BOOST_CHECK(lps::detail::is_well_typed(spec));
}

/* The following test case requires that the sum operator is distributed over the dist operator.
 * This is only possible, as it stands for finite domains. */
BOOST_AUTO_TEST_CASE(distributed_sum_over_dist1)
{
  std::string text =
    "act\n"
    "  c:Bool#Bool;\n"
    "\n"
    "proc\n"
    "  Q = sum b1: Bool. dist x0: Bool[if(x0,1/4,3/4)].c(b1,x0).delta;\n"
    "  \n"
    "init Q;\n";
  stochastic_specification spec=linearise(text);
  BOOST_CHECK(lps::detail::is_well_typed(spec));
}

/* A more elaborate example of the previous test case, where there are also distributions
 * inside the linear process. */
BOOST_AUTO_TEST_CASE(distributed_sum_over_dist2)
{
  std::string text =
    "act  c: Bool # Bool;\n"
    "  \n"
    "glob dc,dc1,dc2,dc3: Bool;\n"
    "\n"
    "proc P(s1_Q: Pos, x: Bool) = \n"
    "       (s1_Q == 2) ->\n"
    "         c(true, x) .\n"
    "         P(s1_Q = 1, x = dc)\n"
    "     + delta;\n"
    "         \n"
    "init dist x: Bool[if(x, 1 / 4, 3 / 4)] . P(2, x);\n";
  stochastic_specification spec=linearise(text);
  BOOST_CHECK(lps::detail::is_well_typed(spec));
}

/* The following example shows an initial distribution with variables
 * that are not used in the body. */
BOOST_AUTO_TEST_CASE(non_bound_initial_stochastic_variables)
{
  std::string text =
    "act a;\n"
    "\n"
    "proc P = (true -> delta <> (dist x0: Bool[1 / 2] . x0 -> a)) . P;\n"
    "\n"
    "init P;\n";
  stochastic_specification spec=linearise(text);
  BOOST_CHECK(lps::detail::is_well_typed(spec));
}

/* The following example was not linearised correctly, as
   an expression sum x:D.a.P was returned
   unchanged when P had an initial distribution. 
 */
BOOST_AUTO_TEST_CASE(distribution_insided_sum_bug)
{
  std::string text =
    "act a,b;\n"
    "\n"
    "proc Environment = \n"
    "        sum realReading: Nat . (realReading<2) ->\n"
    "            a . EnvironmentSensorProcess;\n"
    "\n"
    "proc EnvironmentSensorProcess =\n"
    "        dist f:Bool[if(f, 1/4, 3/4)].f->b.delta;\n"
    "\n"
    "init Environment;\n";
  stochastic_specification spec=linearise(text);
  BOOST_CHECK(lps::detail::is_well_typed(spec));
}


