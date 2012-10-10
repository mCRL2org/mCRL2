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

//#define MCRL2_PBES_TRANSLATE_DEBUG
//#define MCRL2_STATE_FORMULA_BUILDER_DEBUG

#include <iostream>
#include <iterator>
#include <boost/test/included/unit_test_framework.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/utilities/test_utilities.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/detail/test_utility.h"
#include "mcrl2/pbes/pbes_solver_test.h"
#include "test_specifications.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::state_formulas;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;
using mcrl2::utilities::collect_after_test_case;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

pbes<> test_lps2pbes(const std::string& lps_spec, const std::string& mcf_formula, const bool timed=false)
{
  using namespace pbes_system;

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
  pbes<> p = lps2pbes(spec, formula, timed);

  std::cerr << "Results in the following PBES:" << std::endl
            << "---------------------------------------------------------------"
            << std::endl
            << "p = " << pbes_system::pp(p) << std::endl
            << "---------------------------------------------------------------"
            << std::endl;

  BOOST_CHECK(p.is_well_typed());
  return p;
}

void test_lps2pbes_and_solve(const std::string& lps_spec, const std::string& mcf_formula, const bool expected_solution, const bool timed=false)
{
  pbes<> p = test_lps2pbes(lps_spec, mcf_formula, timed);

  BOOST_CHECK_EQUAL(pbes2_bool_test(p), expected_solution);
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

  pbes<> p = test_lps2pbes(TIMED_SPECIFICATION, TRIVIAL_FORMULA);

  const atermpp::vector<sort_expression> user_def_sorts(p.data().user_defined_sorts());
  BOOST_CHECK(std::find(user_def_sorts.begin(), user_def_sorts.end(), sort_real::real_()) == user_def_sorts.end());

  const atermpp::vector<sort_expression> sorts(p.data().sorts());
  BOOST_CHECK(std::find(sorts.begin(), sorts.end(), sort_real::real_()) != sorts.end());
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

#ifdef MCRL2_USE_BOOST_FILESYSTEM
void test_directory(int argc, char** argv)
{
  BOOST_CHECK(argc > 1);

  // The dummy file test.test is used to extract the full path of the test directory.
  fs::path dummy_path = fs::system_complete(fs::path(argv[1], fs::native));
  fs::path dir = dummy_path.branch_path();
  BOOST_CHECK(fs::is_directory(dir));

  fs::directory_iterator end_iter;
  for (fs::directory_iterator dir_itr(dir); dir_itr != end_iter; ++dir_itr)
  {
    if (fs::is_regular(dir_itr->status()))
    {
      std::string filename = dir_itr->path().file_string();
      if (boost::ends_with(filename, std::string(".form")))
      {
        std::string timed_result_file   = filename.substr(0, filename.find_last_of('.') + 1) + "expected_timed_result";
        std::string untimed_result_file = filename.substr(0, filename.find_last_of('.') + 1) + "expected_untimed_result";
        std::string formula = mcrl2::utilities::read_text(filename);
        if (fs::exists(timed_result_file))
        {
          try
          {
            pbes<> result = lps2pbes(SPEC1, formula, true);
            pbes<> expected_result;
            expected_result.load(timed_result_file);
            bool cmp = (result == expected_result);
            if (!cmp)
            {
              cerr << "ERROR: test " << timed_result_file << " failed!" << endl;
            }
            BOOST_CHECK(cmp);
          }
          catch (mcrl2::runtime_error e)
          {
            cerr << e.what() << endl;
          }
        }
        if (fs::exists(untimed_result_file))
        {
          try
          {
            pbes<> result = lps2pbes(SPEC1, formula, false);
            BOOST_CHECK(result.is_well_typed());
            pbes<> expected_result;
            expected_result.load(untimed_result_file);
            bool cmp = (result == expected_result);
            if (!cmp)
            {
              cerr << "ERROR: test " << untimed_result_file << " failed!" << endl;
            }
            BOOST_CHECK(cmp);
          }
          catch (mcrl2::runtime_error e)
          {
            cerr << e.what() << endl;
          }
        }
      }
    }
  }
  core::garbage_collect();
}
#endif

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

  std::vector<string> formulas;
  formulas.push_back("delay@11");
  formulas.push_back("exists m:Nat. <a(m)>true");
  formulas.push_back("exists p:Nat. <a(p)>true");
  formulas.push_back("forall m:Nat. [a(m)]false");
  formulas.push_back("nu X(n:Nat = 1). [forall m:Nat. a(m)](val(n < 10)  && X(n+2))");
  formulas.push_back("mu X(n:Nat = 1). [forall m:Nat. a(m)](val(smaller(n,10) ) && X(n+2))");
  formulas.push_back("<exists m:Nat. a(m)>true");
  formulas.push_back("<a(2)>[a(0)]false");
  formulas.push_back("<a(2)>true");
  formulas.push_back("[forall m:Nat. a(m)]false");
  formulas.push_back("[a(0)]<a(1)>true");
  formulas.push_back("[a(1)]false");
  formulas.push_back("!true");
  formulas.push_back("yaled@10");

  for (std::vector<string>::iterator i = formulas.begin(); i != formulas.end(); ++i)
  {
    test_lps2pbes(SPEC, *i, false);
    test_lps2pbes(SPEC, *i, true);
  }
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
  pbes<> p = test_lps2pbes(SPEC, FORMULA);

  std::set<data::variable> vars(pbes_system::find_variables(p));
  for(std::set<data::variable>::const_iterator i = vars.begin(); i != vars.end(); ++i)
  {
    BOOST_CHECK_NE(i->name(), core::identifier_string("d1"));
    BOOST_CHECK_NE(i->name(), core::identifier_string("d2"));
  }
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}
