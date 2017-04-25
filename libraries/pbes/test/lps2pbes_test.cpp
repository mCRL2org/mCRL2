// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2pbes_test.cpp
/// \brief Add your file description here.

// Test program for timed lps2pbes.

#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/detail/pbes2bool.h"
#include "mcrl2/pbes/detail/test_utility.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriters/data_rewriter.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/utilities/text_utility.h"
#include "test_specifications.h"
#include <boost/algorithm/string.hpp>
#include <boost/test/included/unit_test_framework.hpp>
#include <iostream>
#include <iterator>

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

pbes test_lps2pbes(const std::string& lps_spec,
                   const std::string& mcf_formula,
                   bool expect_success = true,
                   bool timed = false,
                   bool structured = false,
                   bool unoptimized = false,
                   bool preprocess_modal_operators = false,
                   bool generate_counter_example = false
                  )
{
  std::cerr << "==============================================================="
            << std::endl
            << "Specification: " << std::endl
            << lps_spec << std::endl
            << "Combined with formula: " << std::endl
            << mcf_formula << std::endl
            << "using the " << (timed?"timed":"untimed") << " transformation"
            << std::endl;

  lps::specification spec = lps::parse_linear_process_specification(lps_spec);
  state_formulas::state_formula formula = state_formulas::parse_state_formula(mcf_formula, spec);

  if(expect_success)
  {
    pbes p = lps2pbes(spec, formula, timed, structured, unoptimized, preprocess_modal_operators, generate_counter_example);

    std::cerr << "Results in the following PBES:" << std::endl
              << "---------------------------------------------------------------"
              << std::endl
              << "p = " << pbes_system::pp(p) << std::endl
              << "---------------------------------------------------------------"
              << std::endl;

    BOOST_CHECK(p.is_well_typed());
    return p;
  }
  else
  {
    BOOST_CHECK_THROW(lps2pbes(spec, formula, timed), mcrl2::runtime_error);

    return pbes();
  }
}

void test_lps2pbes_and_solve(const std::string& lps_spec,
                             const std::string& mcf_formula,
                             bool expected_solution,
                             bool timed = false,
                             bool rewrite = false,
                             bool generate_counter_example = false
                            )
{
  bool structured = false;
  bool unoptimized = false;
  bool preprocess_modal_operators = false;
  bool expect_success = true;
  pbes p = test_lps2pbes(lps_spec, mcf_formula, expect_success, timed, structured, unoptimized, preprocess_modal_operators, generate_counter_example);

  // apply one point rule rewriter to p, otherwise some of the PBESs cannot be solved
  if (rewrite)
  {
    pbes_system::one_point_rule_rewriter R;
    pbes_rewrite(p, R);
  }

  bool solution = detail::pbes2bool(p);
  BOOST_CHECK(solution == expected_solution);
}

void one_point_rule_rewrite(pbes& p)
{
  data::rewriter datar(p.data());

  // apply the one point rule rewriter
  pbes_system::one_point_rule_rewriter pbesr;
  pbes_rewrite(p, pbesr);

  // post processing: apply the simplifying rewriter
  simplify_data_rewriter<data::rewriter> simp(datar);
  pbes_rewrite(p, simp);
}

void solve_pbes(const std::string& lps_spec, const std::string& mcf_formula, std::string expected_solution, bool linearize = false)
{
  bool timed = false;
  boost::trim(expected_solution);
  std::cerr << "=== solve_pbes === " << std::endl;
  std::cerr << "specification = \n" << lps_spec << std::endl;
  std::cerr << "formula = " << mcf_formula << std::endl;
  std::cerr << "expected_solution = " << expected_solution << std::endl;

  lps::specification spec;
  if (linearize)
  {
    spec=remove_stochastic_operators(lps::linearise(lps_spec));
  }
  else
  {
    spec=lps::parse_linear_process_specification(lps_spec);
  }
  state_formulas::state_formula formula;
  pbes p;

  // test formula
  formula = state_formulas::parse_state_formula(mcf_formula, spec);
  p = lps2pbes(spec, formula, timed);
  if (expected_solution != "unknown")
  {
    one_point_rule_rewrite(p);
    bool expected_result = expected_solution == "true";
    std::cerr << "solving pbes...\n" << pbes_system::pp(p) << std::endl;
    bool solution = detail::pbes2bool(p);
    BOOST_CHECK(solution == expected_result);
  }

  // test negated formula
  formula = state_formulas::not_(formula);
  p = lps2pbes(spec, formula, timed);

  if (expected_solution != "unknown")
  {
    one_point_rule_rewrite(p);
    bool expected_result = expected_solution == "false";
    std::cerr << "solving pbes...\n" << pbes_system::pp(p) << std::endl;
    bool solution = detail::pbes2bool(p);
    BOOST_CHECK(solution == expected_result);
  }
}

const std::string TRIVIAL_FORMULA  = "[true*]<true*>true";

BOOST_AUTO_TEST_CASE(test_trivial)
{
  test_lps2pbes(lps::detail::LINEAR_ABP_SPECIFICATION(), TRIVIAL_FORMULA);
}

BOOST_AUTO_TEST_CASE(test_timed)
{
  const std::string TIMED_SPECIFICATION =
    "act a;\n"
    "proc P = a@1 . P;\n"
    "init P;\n"
  ;

  pbes p = test_lps2pbes(TIMED_SPECIFICATION, TRIVIAL_FORMULA);

  const data::basic_sort_vector user_def_sorts(p.data().user_defined_sorts());
  BOOST_CHECK(std::find(user_def_sorts.begin(), user_def_sorts.end(), data::sort_real::real_()) == user_def_sorts.end());

  const std::set<data::sort_expression> sorts(p.data().sorts());
  BOOST_CHECK(std::find(sorts.begin(), sorts.end(), data::sort_real::real_()) != sorts.end());
}

BOOST_AUTO_TEST_CASE(test_true_implies_false)
{
  const std::string SPECIFICATION =
    "act a;                                  \n"
    "proc X(n : Nat) = (n > 2) -> a. X(n+1); \n"
    "init X(3);                              \n"
    ;

  const std::string FORMULA = "true => false";
  test_lps2pbes(SPECIFICATION, FORMULA);
}

BOOST_AUTO_TEST_CASE(test_forall_nat)
{
  const std::string SPECIFICATION =
    "act a : Nat;                           \n"
    "proc X(n:Nat) = (n>2) -> a(n). X(n+1); \n"
    "init X(3);                             \n"
    ;
  const std::string FORMULA = "nu X. (X && forall m:Nat. [a(m)]false)";
  test_lps2pbes(SPECIFICATION, FORMULA);
}

BOOST_AUTO_TEST_CASE(test_mu_or_mu)
{
  const std::string SPECIFICATION =
    "act a;         \n"
    "proc X = a. X; \n"
    "init X;        \n"
    ;
  const std::string FORMULA =
    "(                                 \n"
    "  ( mu A. [!a]A)                  \n"
    "||                                \n"
    "  ( mu B. exists t3:Pos . [!a]B ) \n"
    ")                                 \n"
    ;

  test_lps2pbes(SPECIFICATION, FORMULA);
}

BOOST_AUTO_TEST_CASE(test_mixed_complex)
{
  const std::string SPECIFICATION =
      "sort Closure = List(Bool);\n"
      "     State = struct state(closure: Closure, copy: Nat);\n"
      "map  initial,accept: State -> Bool;\n"
      "     nextstate: State # State -> Bool;\n"
      "var  q,q': State;\n"
      "eqn  initial(q)  =  closure(q) . 0 && copy(q) == 0;\n"
      "     accept(q)  =  copy(q) == 0 && (closure(q) . 0 => closure(q) . 2);\n"
      "     nextstate(q, q')  =  #closure(q) == #closure(q') && (accept(q) => copy(q') == (copy(q) + 1) mod 1) && (!accept(q) => copy(q') == copy(q)) && closure(q) . 0 == (closure(q) . 2 || closure(q) . 1 && closure(q') . 0) && (closure(q') . 0 => closure(q') . 1 || closure(q') . 2) && (closure(q') . 2 => closure(q') . 0);\n"
      "act  a,b;\n"
      "proc P(s_P: Bool) =\n"
      "       !s_P ->\n"
      "         b .\n"
      "         P()\n"
      "     + s_P ->\n"
      "         a .\n"
      "         P(s_P = false)\n"
      "     + delta;\n"
      "init P(true);\n"
    ;

  const std::string FORMULA =
    "forall c1: State .                                                                                                                                            \n"
    " (exists c0: State .                                                                                                                                          \n"
    "   (val(initial(c0) && nextstate(c0, c1)) &&                                                                                                                  \n"
    "    (((<a>true) => val(closure(c1).1)) && (val(closure(c1).1) => (<a>true)) && ((<b>true) => val(closure(c1).2)) && (val(closure(c1).2) => (<b>true)))        \n"
    "   )                                                                                                                                                          \n"
    " ) => (                                                                                                                                                       \n"
    "   mu X(c'': State = c1) . (                                                                                                                                  \n"
    "     nu Y(c: State = c'') . (                                                                                                                                 \n"
    "       forall c': State . (                                                                                                                                   \n"
    "         val(nextstate(c, c')) =>                                                                                                                             \n"
    "         [true](                                                                                                                                              \n"
    "           (((<a>true) => val(closure(c).1)) && (val(closure(c).1) => (<a>true)) && ((<b>true) => val(closure(c).2)) && (val(closure(c).2) => (<b>true))) =>  \n"
    "           ((val(accept(c)) && X(c')) || (val(!accept(c)) && Y(c')))                                                                                          \n"
    "         )                                                                                                                                                    \n"
    "       )                                                                                                                                                      \n"
    "     )                                                                                                                                                        \n"
    "   )                                                                                                                                                          \n"
    " )                                                                                                                                                            \n"
    ;

  test_lps2pbes(SPECIFICATION, FORMULA);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  std::string FORMULA;

  FORMULA = "mu X. !!X";
  test_lps2pbes(lps::detail::LINEAR_ABP_SPECIFICATION(), FORMULA);

  FORMULA = "nu X. ([true]X && <true>true)";
  test_lps2pbes(lps::detail::LINEAR_ABP_SPECIFICATION(), FORMULA);

  FORMULA = "nu X. ([true]X && forall d:D. [r1(d)] mu Y. (<true>Y || <s4(d)>true))";
  test_lps2pbes(lps::detail::LINEAR_ABP_SPECIFICATION(), FORMULA);

  FORMULA = "forall d:D. nu X. (([!r1(d)]X && [s4(d)]false))";
  test_lps2pbes(lps::detail::LINEAR_ABP_SPECIFICATION(), FORMULA);

  FORMULA = "nu X. ([true]X && forall d:D. [r1(d)]nu Y. ([!r1(d) && !s4(d)]Y && [r1(d)]false))";
  test_lps2pbes(lps::detail::LINEAR_ABP_SPECIFICATION(), FORMULA);
}

BOOST_AUTO_TEST_CASE(test_delta_mu_true_or_mu_true)
{
  std::string SPEC = "proc P = delta;\n"
                     "init P\n;";

  std::string FORMULA =
    "(mu X(n:Nat = 0) . true) \n"
    "&&                       \n"
    "(mu X(n:Nat = 0) . true) \n"
    ;

  // Expected result:
  //
  // pbes nu X1 =
  //        Y(0) && X(0);
  //      mu Y(n: Nat) =
  //        true;
  //      mu X(n: Nat) =
  //        true;
  //
  // init X1;
  test_lps2pbes(SPEC, FORMULA);
}

// Trac ticket #841, example supplied by Tim Willemse.
BOOST_AUTO_TEST_CASE(bug_841)
{
  std::string SPEC =
    "act  a: Nat;                    \n"
    "                                \n"
    "proc P(s3: Pos, n: Nat) =       \n"
    "       sum m: Nat.              \n"
    "         (s3 == 1) ->           \n"
    "         a(m) .                 \n"
    "         P(s3 = 2, n = m)       \n"
    "     + sum m0: Nat.             \n"
    "         (s3 == 2 && n < m0) -> \n"
    "         a(m0) .                \n"
    "         P(s3 = 2, n = m0)      \n"
    "     + delta;                   \n"
    "                                \n"
    "init P(1, 0);                   \n"
    ;

  std::string FORMULA = "nu X(n :Nat=0). X(n+1)";

  test_lps2pbes(SPEC, FORMULA);
}

BOOST_AUTO_TEST_CASE(test_formulas)
{
  std::string SPEC =
    "act a:Nat;                             \n"
    "map smaller: Nat#Nat -> Bool;          \n"
    "var x,y : Nat;                         \n"
    "eqn smaller(x,y) = x < y;              \n"
    "proc P(n:Nat) = sum m: Nat. a(m). P(m);\n"
    "init P(0);                             \n"
    ;

  std::string formula;
  std::string expected_solution;

  formula = "exists m:Nat. <a(m)>true";
  expected_solution = "true";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "exists p:Nat. <a(p)>true";
  expected_solution = "true";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "forall m:Nat. [a(m)]false";
  expected_solution = "false";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "nu X(n:Nat = 1). [forall m:Nat. a(m)](val(n < 10)  && X(n+2))";
  expected_solution = "unknown";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "mu X(n:Nat = 1). [forall m:Nat. a(m)](val(smaller(n,10) ) && X(n+2))";
  expected_solution = "unknown";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "<exists m:Nat. a(m)>true";
  expected_solution = "unknown";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "<a(2)>[a(0)]false";
  expected_solution = "false";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "<a(2)>true";
  expected_solution = "true";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "[forall m:Nat. a(m)]false";
  expected_solution = "unknown";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "[a(0)]<a(1)>true";
  expected_solution = "true";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "[a(1)]false";
  expected_solution = "false";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "!true";
  expected_solution = "false";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "delay@11";
  expected_solution = "unknown";
  solve_pbes(SPEC, formula, expected_solution);

  formula = "yaled@10";
  expected_solution = "unknown";
  solve_pbes(SPEC, formula, expected_solution);
}

#ifdef MCRL2_EXTENDED_TESTS
const std::string MACHINE_SPECIFICATION =
    "sort Enum6 = struct e5_6 | e4_6 | e3_6 | e2_6 | e1_6 | e0_6;\n"
    "     Enum3 = struct e2_3 | e1_3 | e0_3;\n"
    "map  C6_: Enum6 # Pos # Pos # Pos # Pos # Pos # Pos -> Pos;\n"
    "     C6_1: Enum6 # Bool # Bool # Bool # Bool # Bool # Bool -> Bool;\n"
    "     C3_: Enum3 # Pos # Pos # Pos -> Pos;\n"
    "     C3_1: Enum3 # Bool # Bool # Bool -> Bool;\n"
    "var  x1,y6,y5,y4,y3,y2,y1,x3,y15,y14,y13: Pos;\n"
    "     e1,e2: Enum6;\n"
    "     x2,y12,y11,y10,y9,y8,y7,x4,y18,y17,y16: Bool;\n"
    "     e5,e6: Enum3;\n"
    "eqn  C6_(e1, x1, x1, x1, x1, x1, x1)  =  x1;\n"
    "     C6_(e5_6, y6, y5, y4, y3, y2, y1)  =  y6;\n"
    "     C6_(e4_6, y6, y5, y4, y3, y2, y1)  =  y5;\n"
    "     C6_(e3_6, y6, y5, y4, y3, y2, y1)  =  y4;\n"
    "     C6_(e2_6, y6, y5, y4, y3, y2, y1)  =  y3;\n"
    "     C6_(e1_6, y6, y5, y4, y3, y2, y1)  =  y2;\n"
    "     C6_(e0_6, y6, y5, y4, y3, y2, y1)  =  y1;\n"
    "     C6_1(e2, x2, x2, x2, x2, x2, x2)  =  x2;\n"
    "     C6_1(e5_6, y12, y11, y10, y9, y8, y7)  =  y12;\n"
    "     C6_1(e4_6, y12, y11, y10, y9, y8, y7)  =  y11;\n"
    "     C6_1(e3_6, y12, y11, y10, y9, y8, y7)  =  y10;\n"
    "     C6_1(e2_6, y12, y11, y10, y9, y8, y7)  =  y9;\n"
    "     C6_1(e1_6, y12, y11, y10, y9, y8, y7)  =  y8;\n"
    "     C6_1(e0_6, y12, y11, y10, y9, y8, y7)  =  y7;\n"
    "     C3_(e5, x3, x3, x3)  =  x3;\n"
    "     C3_(e2_3, y15, y14, y13)  =  y15;\n"
    "     C3_(e1_3, y15, y14, y13)  =  y14;\n"
    "     C3_(e0_3, y15, y14, y13)  =  y13;\n"
    "     C3_1(e6, x4, x4, x4)  =  x4;\n"
    "     C3_1(e2_3, y18, y17, y16)  =  y18;\n"
    "     C3_1(e1_3, y18, y17, y16)  =  y17;\n"
    "     C3_1(e0_3, y18, y17, y16)  =  y16;\n"
    "act  ch_tea,ch_cof,insq,insd,take_tea,take_cof,want_change,sel_tea,sel_cof,accq,accd,put_tea,put_cof,put_change,ok_tea,ok_coffee,quarter,dollar,tea,coffee,change;\n"
    "proc P(s3_User,s1_Mach: Pos) =\n"
    "       (s3_User == 11 && s1_Mach == 11) ->\n"
    "         coffee .\n"
    "         P(s3_User = 1, s1_Mach = 1)\n"
    "     + sum e_User,e7_Mach: Enum6.\n"
    "         (C6_1(e_User, true, true, true, true, true, true) && C6_1(e_User, s3_User == 2, s3_User == 3, s3_User == 7, s3_User == 8, s3_User == 9, s3_User == 10) && C6_1(e7_Mach, true, true, true, true, true, true) && C6_1(e7_Mach, s1_Mach == 2, s1_Mach == 3, s1_Mach == 7, s1_Mach == 8, s1_Mach == 9, s1_Mach == 10)) ->\n"
    "         quarter .\n"
    "         P(s3_User = C6_(e_User, 3, 4, 8, 9, 10, 11), s1_Mach = C6_(e7_Mach, 3, 4, 8, 9, 10, 11))\n"
    "     + sum e3_User,e8_Mach: Bool.\n"
    "         (if(e3_User, s3_User == 7, s3_User == 2) && if(e8_Mach, s1_Mach == 7, s1_Mach == 2)) ->\n"
    "         dollar .\n"
    "         P(s3_User = if(e3_User, 11, 5), s1_Mach = if(e8_Mach, 11, 5))\n"
    "     + (s3_User == 6 && s1_Mach == 6) ->\n"
    "         change .\n"
    "         P(s3_User = 1, s1_Mach = 1)\n"
    "     + sum e4_User,e9_Mach: Enum3.\n"
    "         (C3_1(e4_User, true, true, true) && C3_1(e4_User, s3_User == 4, s3_User == 5, s3_User == 6) && C3_1(e9_Mach, true, true, true) && C3_1(e9_Mach, s1_Mach == 4, s1_Mach == 5, s1_Mach == 6)) ->\n"
    "         tea .\n"
    "         P(s3_User = C3_(e4_User, 1, 6, 1), s1_Mach = C3_(e9_Mach, 1, 6, 1))\n"
    "     + (s3_User == 1 && s1_Mach == 1) ->\n"
    "         ok_coffee .\n"
    "         P(s3_User = 7, s1_Mach = 7)\n"
    "     + (s3_User == 1 && s1_Mach == 1) ->\n"
    "         ok_tea .\n"
    "         P(s3_User = 2, s1_Mach = 2)\n"
    "     + delta;\n"
    "\n"
    "init P(1, 1);\n"
  ;

const std::string MACHINE_FORMULA1 =
  "%% after choice for tea and two quarter always tea (true) \n"
  " [ true* . ok_tea . quarter . quarter . !tea ] false      \n"
  ;

const std::string MACHINE_FORMULA2 =
  "%% always eventually action ready (true)     \n"
  " [ true* . dollar . !(tea||coffee) ] false   \n"
  ;

const std::string MACHINE_FORMULA3 =
  "%% after a quarter no change directly (true) \n"
  " [ true* . quarter . change ] false          \n"
  ;

BOOST_AUTO_TEST_CASE(test_machine_formula1)
{
  test_lps2pbes(MACHINE_SPECIFICATION, MACHINE_FORMULA1);
}

BOOST_AUTO_TEST_CASE(test_machine_formula2)
{
  test_lps2pbes(MACHINE_SPECIFICATION, MACHINE_FORMULA2);
}

BOOST_AUTO_TEST_CASE(test_machine_formula3)
{
  test_lps2pbes(MACHINE_SPECIFICATION, MACHINE_FORMULA3);
}
#endif

// Submitted by Tim, 2-9-2010
BOOST_AUTO_TEST_CASE(test_example)
{
  std::string SPEC =
      "sort Enum3 = struct e2_3 | e1_3 | e0_3;\n"
      "map  C3_: Enum3 # Pos # Pos # Pos -> Pos;\n"
      "     C3_1: Enum3 # Nat # Nat # Nat -> Nat;\n"
      "     C3_2: Enum3 # Bool # Bool # Bool -> Bool;\n"
      "var  x1,y3,y2,y1: Pos;\n"
      "     e1,e2,e3: Enum3;\n"
      "     x2,y6,y5,y4: Nat;\n"
      "     x3,y9,y8,y7: Bool;\n"
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
      "act  a,b;\n"
      "glob dc: Nat;\n"
      "proc P(s3_S: Pos, n_S: Nat) =\n"
      "       sum e_S: Enum3,n1_S: Nat.\n"
      "         (C3_2(e_S, n1_S == 0, n1_S == 0, true) && C3_2(e_S, s3_S == 3, s3_S == 2 && n_S == 0, s3_S == 1 && n1_S < 3)) ->\n"
      "         a .\n"
      "         P(s3_S = 2, n_S = C3_1(e_S, Int2Nat(n_S - 1), n_S, n1_S))\n"
      "     + sum e4_S: Bool.\n"
      "         if(e4_S, s3_S == 2 && n_S == 0, s3_S == 2 && 0 < n_S) ->\n"
      "         b .\n"
      "         P(s3_S = if(e4_S, 2, 3))\n"
      "     + delta;\n"
      "init P(1, dc);\n"
    ;

  std::string FORMULA = "<a>([a]false)";
  test_lps2pbes_and_solve(SPEC, FORMULA, true);
}

// Submitted by Jeroen Keiren, 10-09-2010
// Formula 2 and 3 give normalization errors.
BOOST_AUTO_TEST_CASE(test_elevator)
{
  std::string SPEC =
      "sort Floor = Pos;\n"
      "     DoorStatus = struct open | closed;\n"
      "     Requests = List(Pos);\n"
      "map  maxFloor: Pos;\n"
      "     addRequest: Requests # Pos -> Requests;\n"
      "     removeRequest: Requests -> Requests;\n"
      "     getNext: Requests -> Pos;\n"
      "var  r: Requests;\n"
      "     f,g: Pos;\n"
      "eqn  maxFloor  =  3;\n"
      "     addRequest([], f)  =  [f];\n"
      "     f == g  ->  addRequest(g |> r, f)  =  g |> r;\n"
      "     f != g  ->  addRequest(g |> r, f)  =  g |> addRequest(r, f);\n"
      "     removeRequest(f |> r)  =  r;\n"
      "     getNext(f |> r)  =  f;\n"
      "act  isAt,request: Pos;\n"
      "     close,open,up,down;\n"
      "proc P(at_Elevator: Pos, status_Elevator: DoorStatus, reqs_Elevator: List(Pos), moving_Elevator: Bool) =\n"
      "       (status_Elevator == closed && getNext(reqs_Elevator) == at_Elevator) ->\n"
      "         open .\n"
      "         P(status_Elevator = open, reqs_Elevator = removeRequest(reqs_Elevator), moving_Elevator = false)\n"
      "     + (status_Elevator == closed && !(reqs_Elevator == []) && getNext(reqs_Elevator) < at_Elevator) ->\n"
      "         down .\n"
      "         P(at_Elevator = Nat2Pos(pred(at_Elevator)), moving_Elevator = true)\n"
      "     + (status_Elevator == closed && !(reqs_Elevator == []) && at_Elevator < getNext(reqs_Elevator)) ->\n"
      "         up .\n"
      "         P(at_Elevator = succ(at_Elevator), moving_Elevator = true)\n"
      "     + (status_Elevator == open) ->\n"
      "         close .\n"
      "         P(status_Elevator = closed)\n"
      "     + sum f_Elevator: Pos.\n"
      "         (f_Elevator <= 3) ->\n"
      "         request(f_Elevator) .\n"
      "         P(reqs_Elevator = addRequest(reqs_Elevator, f_Elevator))\n"
      "     + isAt(at_Elevator) .\n"
      "         P()\n"
      "     + delta;\n"
      "init P(1, open, [], false);\n                                                                                          \n"
    ;

  std::string formula1 = "nu U. [true] U && ((mu V . nu W. !([!request(maxFloor)]!W && [request(maxFloor)]!V)) || (nu X . mu Y. [!isAt(maxFloor)] Y &&  [isAt(maxFloor)]X))";
  std::string formula2 = "nu U. [true] U && ((nu V . mu W. ([!request(maxFloor)]W && [request(maxFloor)]V)) => (nu X . mu Y. [!isAt(maxFloor)] Y &&  [isAt(maxFloor)]X))";
  std::string formula3 = "nu U. [true] U && (!(nu V . mu W. ([!request(maxFloor)]W && [request(maxFloor)]V)) || (nu X . mu Y. [!isAt(maxFloor)] Y &&  [isAt(maxFloor)]X))";
  std::string formula4 = "(nu V . mu W. V) => true";
  std::string formula5 = "!(nu V . mu W. V)";

  test_lps2pbes(SPEC, formula1);
  test_lps2pbes(SPEC, formula2);
  test_lps2pbes(SPEC, formula3);
  test_lps2pbes(SPEC, formula4);
  test_lps2pbes(SPEC, formula5);
}

// Test for bug #1092, simple P=a.b.P example with [a]<a>true, which should be false
BOOST_AUTO_TEST_CASE(test_ab)
{
  std::string SPEC =
    "act  a,b;\n"
    "proc P(s3_P: Pos) =\n"
    "       (s3_P == 2) ->\n"
    "         b .\n"
    "         P(s3_P = 1)\n"
    "     + (s3_P == 1) ->\n"
    "         a .\n"
    "         P(s3_P = 2)\n"
    "     + delta;\n"
    "init P(1);\n"
    ;

  std::string FORMULA = "[a]<a>true";
  test_lps2pbes_and_solve(SPEC, FORMULA, false);
}

// Test for bug #1092, fairness example derived from alternating bit protocol.
BOOST_AUTO_TEST_CASE(test_unfair)
{
  const std::string SPEC =
    "act  r1,s4,i,e;\n"
    "proc P(s3_P: Pos) =\n"
    "       (s3_P == 4) ->\n"
    "         e .\n"
    "         P(s3_P = 2)\n"
    "     + (s3_P == 3) ->\n"
    "         s4 .\n"
    "         P(s3_P = 1)\n"
    "     + sum e1_P: Bool.\n"
    "         (s3_P == 2) ->\n"
    "         i .\n"
    "         P(s3_P = if(e1_P, 4, 3))\n"
    "     + (s3_P == 1) ->\n"
    "         r1 .\n"
    "         P(s3_P = 2)\n"
    "     + delta;\n"
    "init P(1);\n"
    ;

  const std::string FORMULA = "[r1](nu X. mu Y. ([s4]X && [!s4]Y))";
  test_lps2pbes_and_solve(SPEC, FORMULA, false);
}

// Test for bug #1090
BOOST_AUTO_TEST_CASE(test_1090)
{
  const std::string SPEC =
      "sort D = struct d1 | d2;\n"
      "act  a: D;\n"
      "proc P =\n"
      "       a(d1) .\n"
      "         P()\n"
      "     + delta;\n"
      "init P;\n"
    ;

  const std::string FORMULA = "[!exists d:D . a(d)]false";
  pbes p = test_lps2pbes(SPEC, FORMULA);

  std::set<data::variable> vars(pbes_system::find_all_variables(p));
  for(std::set<data::variable>::const_iterator i = vars.begin(); i != vars.end(); ++i)
  {
    BOOST_CHECK_NE(i->name(), core::identifier_string("d1"));
    BOOST_CHECK_NE(i->name(), core::identifier_string("d2"));
  }
}

BOOST_AUTO_TEST_CASE(test_1150)
{
  const std::string SPEC1 =
    "act a,b,c;              \n"
    "proc P(s: Pos) =        \n"
    "  (s == 1) -> a . P(2)  \n"
    "+ (s == 1) -> c . P(3)  \n"
    "+ (s == 2) -> b . P(4); \n"
    "init P(1);              \n"
    ;

  const std::string SPEC2 =
    "proc P = delta; \n"
    "init P;         \n"
    ;

  test_lps2pbes_and_solve(SPEC1, "<a>!<c>true", true);
  test_lps2pbes_and_solve(SPEC1, "<a>[c]false", true);
  test_lps2pbes_and_solve(SPEC2, "mu X . X",    false);
  test_lps2pbes_and_solve(SPEC2, "!(mu X . X)", true);
  test_lps2pbes_and_solve(SPEC2, "nu X . X",    true);
  test_lps2pbes_and_solve(SPEC2, "!(nu X . X)", false);
}

BOOST_AUTO_TEST_CASE(test_elementary_formulas)
{
  std::string procspec_text =
    "act  a, b, c;         \n"
    "                      \n"
    "proc P = a.P;         \n"
    "                      \n"
    "proc Q = a.b.Q + b.Q; \n"
    "                      \n"
    "init <INIT>;          \n"
    ;

  std::string formulas =
    "a         #  true                          #   true   \n"
    "a         #  false                         #   false  \n"
    "a         #  <a>true                       #   true   \n"
    "a         #  [b]false                      #   true   \n"
    "b         #  [b]false                      #   false  \n"
    "b         #  <a>true                       #   false  \n"
    "a.(b+c)   #  <a>(<b>true && <c>true)       #   true   \n"
    "a.b+a.c   #  <a><b>true && <a><c>true      #   true   \n"
    "a.b+a.c   #  <a>(<b>true && <c>true)       #   false  \n"
    "a.(b+c)   #  [a](<b>true||<c>true)         #   true   \n"
    "a.b+a.c   #  [a]([b]false || [c]false)     #   true   \n"
    "a.(b+c)   #  [a]([b]false && [c]false)     #   false  \n"
    "a         #  mu X.X                        #   false  \n"
    "a         #  nu X.X                        #   true   \n"
    "P         #  mu X.<a>X                     #   false  \n"
    "P         #  nu X.<a>X                     #   true   \n"
    "Q         #  mu X.nu Y.[b]Y && [a]X        #   false  \n"
    "Q         #  nu X.mu Y.[b]X && [a]Y        #   true   \n"
    "Q         #  mu X.[!a]X && <true>true      #   false  \n"
    "Q         #  mu X.[!b]X && <true>true      #   true   \n"
    "a         #  [true]false                   #   false  \n"
    "a         #  [!a]false                     #   true   \n"
    "b         #  [!a]false                     #   false  \n"
    "a         #  <!a>true                      #   false  \n"
    "a         #  <a && !a>true                 #   false  \n"
    "a         #  <a && a>true                  #   true   \n"
    "a         #  <a || b>true                  #   true   \n"
    "a         #  <!(a||b)>true                 #   false  \n"
    "a         #  [!a]false                     #   true   \n"
    "a         #  [a && a && b]false            #   true   \n"
    "a         #  [a && a && !b]false           #   false  \n"
    "a         #  [a || a]false                 #   false  \n"
    ;

  std::vector<std::string> lines = utilities::regex_split(formulas, "\\n");
  for (auto & line : lines)
  {
    std::vector<std::string> words = utilities::split(line, "#");
    if (words.size() != 3)
    {
    	continue;
    }
    solve_pbes(utilities::regex_replace("<INIT>", words[0], procspec_text), words[1], words[2], true);
  }
}

BOOST_AUTO_TEST_CASE(test_counter_example)
{
  std::string lps_spec =
    "act  a,b;                              \n"
    "proc P(c: Bool) = c ->  a . P(c = !c)  \n"
    "                + !c -> b . P(c = true)\n"
    "                + delta;               \n"
    "init P(true);                          \n"
    ;

  std::string mcf_formula = "[true*.b]false";
  bool expected_solution = false;
  bool timed = false;
  bool rewrite = false;
  bool generate_counter_example;

  generate_counter_example = false;
  test_lps2pbes_and_solve(lps_spec, mcf_formula, expected_solution, timed, rewrite, generate_counter_example);

  generate_counter_example = true;
  test_lps2pbes_and_solve(lps_spec, mcf_formula, expected_solution, timed, rewrite, generate_counter_example);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
