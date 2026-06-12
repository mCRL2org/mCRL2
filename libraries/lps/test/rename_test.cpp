// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rename_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE rename_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/linearise.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

const std::string SPECIFICATION =
  "% Test Case 3                                                     \n"
  "%                                                                 \n"
  "% rename:                                                         \n"
  "% var                                                             \n"
  "%   x:Bool;                                                       \n"
  "%   y:Nat;                                                        \n"
  "%   z:Nat;                                                        \n"
  "% rename                                                          \n"
  "%   a(x,y) => a(x,y);                                             \n"
  "                                                                  \n"
  "act                                                               \n"
  "  a: Bool#Nat;                                                    \n"
  "                                                                  \n"
  "proc                                                              \n"
  "  X(x:Bool, y:Nat)= sum z:Nat. (y<=z && z<3) -> a(x,y).X(!x,y+1); \n"
  "                                                                  \n"
  "init                                                              \n"
  "  X(true,0);                                                      \n"
  ;

const std::string SPECIFICATION2 =
  "act a:Nat;                              \n"
  "                                        \n"
  "map smaller: Nat#Nat -> Bool;           \n"
  "                                        \n"
  "var x,y : Nat;                          \n"
  "                                        \n"
  "eqn smaller(x,y) = x < y;               \n"
  "                                        \n"
  "proc P(n:Nat) = sum m: Nat. a(m). P(m); \n"
  "                                        \n"
  "init P(0);                              \n"
  ;

const std::string SPECIFICATION3 =
  "act a;                                  \n"
  "                                        \n"
  "proc P(b:Bool) = a. P(b);               \n"
  "                                        \n"
  "init P(false);                          \n"
  ;

void test_lps_rename()
{
  specification spec=remove_stochastic_operators(linearise(SPECIFICATION));
  linear_process p = spec.process();
  std::set<identifier_string> forbidden_names;
  forbidden_names.insert(identifier_string("x"));
  forbidden_names.insert(identifier_string("y"));
  forbidden_names.insert(identifier_string("z"));

}

void test_rename()
{
  specification spec=remove_stochastic_operators(linearise(SPECIFICATION3));
  std::set<identifier_string> forbidden_names;
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_rename();
  test_lps_rename();
}
