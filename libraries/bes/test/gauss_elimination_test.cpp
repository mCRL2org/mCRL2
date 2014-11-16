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
#include "mcrl2/bes/gauss_elimination.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_gauss_elimination.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

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

std::string BES9 =
  "pbes mu X = false;                                       \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES10 =
  "pbes nu X = false;                                       \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

void test_bes(std::string bes_spec, bool expected_result)
{
  pbes_system::pbes p = pbes_system::txt2pbes(bes_spec);
  int result = pbes_system::gauss_elimination(p);
  switch (result)
  {
    case 0:
      std::cout << "FALSE" << std::endl;
      break;
    case 1:
      std::cout << "true" << std::endl;
      break;
    case 2:
      std::cout << "UNKNOWN" << std::endl;
      break;
  }
  BOOST_CHECK((expected_result == false && result == 0) || (expected_result == true && result == 1));

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
  test_bes(BES9, false);
  test_bes(BES10, false);
}

void test_abp()
{
  bool timed = false;
  std::string FORMULA = "[true*]<true*>true";
  lps::specification spec=remove_stochastic_operators(lps::linearise(lps::detail::ABP_SPECIFICATION()));
  state_formulas::state_formula formula = state_formulas::parse_state_formula(FORMULA, spec);

  pbes_system::pbes p = pbes_system::lps2pbes(spec, formula, timed);
  int result = pbes_system::gauss_elimination(p);
  switch (result)
  {
    case 0:
      std::cout << "FALSE" << std::endl;
      break;
    case 1:
      std::cout << "true" << std::endl;
      break;
    case 2:
      std::cout << "UNKNOWN" << std::endl;
      break;
  }

}

void test_bes()
{
  using namespace bes;

  typedef core::term_traits<boolean_expression> tr;

  boolean_variable X("X");
  boolean_variable Y("Y");

  // empty boolean equation system
  std::vector<boolean_equation> empty;

  pbes_system::fixpoint_symbol mu = pbes_system::fixpoint_symbol::mu();
  pbes_system::fixpoint_symbol nu = pbes_system::fixpoint_symbol::nu();

  // pbes mu X = X;
  //
  // init X;
  boolean_equation e1(mu, X, X);
  boolean_equation_system bes1(empty, X);
  bes1.equations().push_back(e1);

  // pbes nu X = X;
  //
  // init X;
  boolean_equation e2(nu, X, X);
  boolean_equation_system bes2(empty, X);
  bes2.equations().push_back(e2);

  // pbes mu X = Y;
  //      nu Y = X;
  //
  // init X;
  boolean_equation e3(mu, X, Y);
  boolean_equation e4(nu, Y, X);
  boolean_equation_system bes3(empty, X);
  bes3.equations().push_back(e3);
  bes3.equations().push_back(e4);

  // pbes nu Y = X;
  //      mu X = Y;
  //
  // init X;
  boolean_equation_system bes4(empty, X);
  bes4.equations().push_back(e4);
  bes4.equations().push_back(e3);

  BOOST_CHECK(gauss_elimination(bes1) == false);
  BOOST_CHECK(gauss_elimination(bes2) == true);
  BOOST_CHECK(gauss_elimination(bes3) == false);
  BOOST_CHECK(gauss_elimination(bes4) == true);

}

inline
bool compare(const pbes_system::pbes_expression& x, const pbes_system::pbes_expression& y)
{
  return x == y;
}

typedef bool (*compare_function)(const pbes_system::pbes_expression& x, const pbes_system::pbes_expression& y);

void test_approximate()
{
  using namespace pbes_system;
  typedef core::term_traits<pbes_expression> tr;

  gauss_elimination_algorithm<pbes_traits> algorithm;
  pbes_system::pbes p = pbes_system::txt2pbes(BES4);
  algorithm.run(p.equations().begin(), p.equations().end(), approximate<pbes_traits, compare_function > (compare));
  if (tr::is_false(p.equations().front().formula()))
  {
    std::cout << "FALSE" << std::endl;
  }
  else if (tr::is_true(p.equations().front().formula()))
  {
    std::cout << "true" << std::endl;
  }
  else
  {
    std::cout << "UNKNOWN" << std::endl;
  }
}

// simple solver that only works if the PBES is a BES
struct fixpoint_equation_solver
{

  void operator()(pbes_equation& e) const
  {
    pbes_expression phi = e.symbol().is_mu() ? pbes_expr::false_() : pbes_expr::true_();
    e.formula() = replace_propositional_variables(e.formula(), propositional_variable_substitution(e.variable(), phi));
  }
};

void tutorial1()
{
  using namespace pbes_system;

  std::string txt =
    "pbes nu Y = X; \n"
    "     mu X = Y; \n"
    "               \n"
    "init X;        \n"
    ;
  pbes p = txt2pbes(txt);
  gauss_elimination_algorithm<pbes_traits> algorithm;
  algorithm.run(p.equations().begin(), p.equations().end(), fixpoint_equation_solver());
}

void tutorial2()
{
  using namespace pbes_system;

  std::string txt =
    "pbes mu X = X; \n"
    "               \n"
    "init X;        \n"
    ;
  pbes p = txt2pbes(txt);
  int solution = gauss_elimination(p);
  assert(solution == 0); // 0 indicates false
}

int test_main(int argc, char** argv)
{
  test_bes();
  test_abp();
  test_bes_examples();
  test_approximate();
  tutorial1();
  tutorial2();

  return 0;
}
