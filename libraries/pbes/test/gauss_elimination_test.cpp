// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gauss_elimination_test.cpp
/// \brief Gauss elimination tests.

#define MCRL2_GAUSS_ELIMINATION_DEBUG

#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/gauss_elimination.h"
#include "mcrl2/pbes/bes_algorithms.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;

std::string BES1 =
  "pbes mu X = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES2 =
  "pbes nu X = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES3 =
  "pbes mu X = Y;                                           \n"
  "     nu Y = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES4 =
  "pbes nu Y = X;                                           \n"
  "     mu X = Y;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES5 =
  "pbes mu X1 = X2;                                         \n"
  "     nu X2 = X1 || X3;                                   \n"
  "     mu X3 = X4 && X5;                                   \n"
  "     nu X4 = X1;                                         \n"
  "     nu X5 = X1 || X3;                                   \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES6 =
  "pbes nu X1 = X2 && X1;                                   \n"
  "     mu X2 = X1 || X3;                                   \n"
  "     nu X3 = X3;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES7 =
  "pbes nu X1 = X2 && X3;                                   \n"
  "     nu X2 = X4 && X5;                                   \n"
  "     nu X3 = true;                                       \n"
  "     nu X4 = false;                                      \n"
  "     nu X5 = X6;                                         \n"
  "     nu X6 = X5;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES8 =
  "pbes nu X1 = X2 && X1;                                   \n"
  "     mu X2 = X1;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

void test_bes(std::string bes_spec, bool expected_result)
{
  pbes_system::pbes<> p = pbes_system::txt2pbes(bes_spec);
  int result = pbes_system::pbes_gauss_elimination(p);
  switch (result) {
    case 0: std::cout << "FALSE" << std::endl; break;
    case 1: std::cout << "TRUE" << std::endl; break;
    case 2: std::cout << "UNKNOWN" << std::endl; break;
  }
  BOOST_CHECK( (expected_result == false && result == 0) || (expected_result == true && result == 1) );

  // BOOST_CHECK(pbes2bool(p) == expected_result);
  // this gives assertion failures in pbes2bool
}

void test_bes_examples()
{
  test_bes(BES1, false);
  test_bes(BES2, true);
  test_bes(BES3, false);
  test_bes(BES4, true);
  test_bes(BES5, false);
  test_bes(BES6, true);
  test_bes(BES7, false);
  test_bes(BES8, true);
}

const std::string ABP_SPECIFICATION =
  "% This file contains the alternating bit protocol, as described in W.J.    \n"
  "% Fokkink, J.F. Groote and M.A. Reniers, Modelling Reactive Systems.       \n"
  "%                                                                          \n"
  "% The only exception is that the domain D consists of two data elements to \n"
  "% facilitate simulation.                                                   \n"
  "                                                                           \n"
  "sort                                                                       \n"
  "  D     = struct d1 | d2;                                                  \n"
  "  Error = struct e;                                                        \n"
  "                                                                           \n"
  "act                                                                        \n"
  "  r1,s4: D;                                                                \n"
  "  s2,r2,c2: D # Bool;                                                      \n"
  "  s3,r3,c3: D # Bool;                                                      \n"
  "  s3,r3,c3: Error;                                                         \n"
  "  s5,r5,c5: Bool;                                                          \n"
  "  s6,r6,c6: Bool;                                                          \n"
  "  s6,r6,c6: Error;                                                         \n"
  "  i;                                                                       \n"
  "                                                                           \n"
  "proc                                                                       \n"
  "  S(b:Bool)     = sum d:D. r1(d).T(d,b);                                   \n"
  "  T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));             \n"
  "                                                                           \n"
  "  R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+                      \n"
  "                  (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);                    \n"
  "                                                                           \n"
  "  K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;           \n"
  "                                                                           \n"
  "  L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;                   \n"
  "                                                                           \n"
  "init                                                                       \n"
  "  allow({r1,s4,c2,c3,c5,c6,i},                                             \n"
  "    comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},                     \n"
  "        S(true) || K || L || R(true)                                       \n"
  "    )                                                                      \n"
  "  );                                                                       \n"
  ;

const std::string FORMULA  = "[true*]<true*>true";

void test_abp()
{
  bool timed = false;
  lps::specification spec      = lps::mcrl22lps(ABP_SPECIFICATION);
  modal_formula::state_formula formula = modal_formula::detail::mcf2statefrm(FORMULA, spec);

  pbes_system::pbes<> p = pbes_system::lps2pbes(spec, formula, timed);
  int result = pbes_system::pbes_gauss_elimination(p);
  switch (result) {
    case 0: std::cout << "FALSE" << std::endl; break;
    case 1: std::cout << "TRUE" << std::endl; break;
    case 2: std::cout << "UNKNOWN" << std::endl; break;
  }
}

void test_bes()
{
  using namespace bes;

  typedef core::term_traits<boolean_expression> tr;

  boolean_variable X("X");
  boolean_variable Y("Y");

  // empty boolean equation system
  atermpp::vector<boolean_equation> empty;

  pbes_system::fixpoint_symbol mu = pbes_system::fixpoint_symbol::mu();
  pbes_system::fixpoint_symbol nu = pbes_system::fixpoint_symbol::nu();

  // pbes mu X = X;
  //
  // init X;
  boolean_equation e1(mu, X, X);
  boolean_equation_system<> bes1(empty , X);
  bes1.equations().push_back(e1);

  // pbes nu X = X;
  //
  // init X;
  boolean_equation e2(nu, X, X);
  boolean_equation_system<> bes2(empty , X);
  bes2.equations().push_back(e2);

  // pbes mu X = Y;
  //      nu Y = X;
  //
  // init X;
  boolean_equation e3(mu, X, Y);
  boolean_equation e4(nu, Y, X);
  boolean_equation_system<> bes3(empty, X);
  bes3.equations().push_back(e3);
  bes3.equations().push_back(e4);

  // pbes nu Y = X;
  //      mu X = Y;
  //
  // init X;
  boolean_equation_system<> bes4(empty, X);
  bes4.equations().push_back(e4);
  bes4.equations().push_back(e3);

  BOOST_CHECK(gauss_elimination(bes1) == false);
  BOOST_CHECK(gauss_elimination(bes2) == true);
  BOOST_CHECK(gauss_elimination(bes3) == false);
  BOOST_CHECK(gauss_elimination(bes4) == true);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_bes();
  core::garbage_collect();
  test_abp();
  core::garbage_collect();
  test_bes_examples();
  core::garbage_collect();

  return 0;
}
