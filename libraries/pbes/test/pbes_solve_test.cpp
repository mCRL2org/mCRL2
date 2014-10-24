// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_solve_test.cpp
/// \brief Add your file description here.

#include <boost/test/minimal.hpp>
#include "mcrl2/utilities/test_utilities.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes_solver_test.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/pbespgsolve.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

std::string test01 =
  "pbes mu X = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string test02 =
  "pbes nu X = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string test03 =
  "pbes mu X = Y;                                           \n"
  "     nu Y = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string test04 =
  "pbes nu Y = X;                                           \n"
  "     mu X = Y;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string test05 =
  "pbes mu X1 = X2;                                         \n"
  "     nu X2 = X1 || X3;                                   \n"
  "     mu X3 = X4 && X5;                                   \n"
  "     nu X4 = X1;                                         \n"
  "     nu X5 = X1 || X3;                                   \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string test06 =
  "pbes nu X1 = X2 && X1;                                   \n"
  "     mu X2 = X1 || X3;                                   \n"
  "     nu X3 = X3;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string test07 =
  "pbes nu X1 = X2 && X3;                                   \n"
  "     nu X2 = X4 && X5;                                   \n"
  "     nu X3 = true;                                       \n"
  "     nu X4 = false;                                      \n"
  "     nu X5 = X6;                                         \n"
  "     nu X6 = X5;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string test08 =
  "pbes nu X1 = X2 && X1;                                   \n"
  "     mu X2 = X1;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string test09 =
  "pbes                         \n"
  "                             \n"
  "nu X(b:Bool, n:Nat) = true;  \n"
  "                             \n"
  "init X(true,0);              \n"
  ;

// Test case to check a simple forall quantifier elimination.
std::string test10 =
  "sort E=struct e1 | e2 | e3;"
  "pbes mu X=forall x:E.val(x==e1);"
  "init X;";

// Test case to check a simple exists quantifier elimination.
std::string test11 =
  "sort E=struct e1 | e2 | e3;"
  "pbes mu X=exists x:E.val(x==e1);"
  "init X;";

// Test case to check a more complex forall quantifier elimination.
std::string test12 =
  "pbes mu X=forall x:Nat.val(x<3 => x==1);"
  "init X;";

// Test case to check a more complex exists quantifier elimination.
std::string test13 =
  "pbes mu X=exists x:Nat.val(x<3 => x==1);"
  "init X;";

// Problematic case found by random pbes tests
std::string test14 =
  "pbes nu X0 = X2;        \n"
  "      nu X1 = X2;       \n"
  "      mu X2 = X3 && X1; \n"
  "      mu X3 = X1;       \n"
  "                        \n"
  "init X0;                \n"
  ;

// Problematic case found by random pbes tests
std::string test15 =
  "pbes                                                                                                                                                                                                                                                                                                                                                                                                      \n"
  "nu X0(m:Nat, b:Bool) = (((forall m:Nat.((val(m < 3)) && (forall n:Nat.((val(n < 3)) && (val(m < 3)))))) || ((!X2) || (val(m > 1)))) => ((!(!X1)) || (X0(m + 1, m > 1)))) && ((val(m > 1)) || (!(val(b))));                                                                                                                                                                                                \n"
  "nu X1 = (forall n:Nat.((val(n < 3)) && ((!((forall m:Nat.((val(m < 3)) && (!(forall m:Nat.((val(m < 3)) && (val(n < 3))))))) && (val(false)))) && (!(!((forall m:Nat.((val(m < 3)) && ((forall k:Nat.((val(k < 3)) && (val(m < 2)))) => (forall k:Nat.((val(k < 3)) && (!X4)))))) && (!((!X3) => (forall k:Nat.((val(k < 3)) && (X1))))))))))) => (forall k:Nat.((val(k < 3)) && (val(false))));          \n"
  "nu X2 = (forall n:Nat.((val(n < 3)) && (forall n:Nat.((val(n < 3)) && (val(n < 2)))))) => ((!(!((val(false)) => (X2)))) && (((!X0(0, true)) => ((val(false)) && (X1))) && (forall m:Nat.((val(m < 3)) && (val(true))))));                                                                                                                                                                                 \n"
  "nu X3 = (forall m:Nat.((val(m < 3)) && (forall n:Nat.((val(n < 3)) && (exists n:Nat.((val(n < 3)) || ((forall k:Nat.((val(k < 3)) && (val(k < 2)))) && ((val(n > 1)) || (exists k:Nat.((val(k < 3)) || ((exists m:Nat.((val(m < 3)) || (!(val(m < 3))))) || (X4)))))))))))) || (!(!(exists n:Nat.((val(n < 3)) || (exists k:Nat.((val(k < 3)) || ((!(X0(k + 1, k < 3))) => ((X3) || (val(k < 3)))))))))); \n"
  "nu X4 = (((val(false)) || ((X1) && (val(true)))) || (forall k:Nat.((val(k < 3)) && ((val(k > 1)) && (X2))))) || ((forall k:Nat.((val(k < 3)) && (!(X0(0, k > 1))))) => (forall m:Nat.((val(m < 3)) && (val(false)))));                                                                                                                                                                                    \n"
  "                                                                                                                                                                                                                                                                                                                                                                                                          \n"
  "init X0(0, true);                                                                                                                                                                                                                                                                                                                                                                                         \n"
  ;

// Problematic test case involving lambda expressions
std::string test16 =
  "sort  State = struct S;\n\n"
  "pbes nu X(f: Nat -> State) =\n"
  "           val(!(f(3) == S));\n\n"
  "init X(lambda x: Nat. S);\n"
  ;

// Test case with global variables
std::string test17 =
  "sort D = struct d1 | d2;         \n"
  "                                 \n"
  "glob d: D;                       \n"
  "                                 \n"
  "pbes nu X =                      \n"
  "       forall e: D. val(d == e); \n"
  "                                 \n"
  "init X;                          \n"
  ;

// Regression test 1 for #1215
std::string test18 =
  "map  Generated_or: Bool # Bool -> Set(Bool);                 \n"
  "     Generated_and: Bool # Bool -> Set(Bool);                \n"
  "     LiftGenerated_or: Set(Bool) # Set(Bool) -> Set(Bool);   \n"
  "     LiftGenerated_and: Set(Bool) # Set(Bool) -> Set(Bool);  \n"
  "                                                             \n"
  "var x0,x1: Bool;                                             \n"
  "eqn Generated_or(x0, x1)  =  {x0 || x1};                     \n"
  "                                                             \n"
  "var  x0,x1: Bool;                                            \n"
  "     X0,X1: Set(Bool);                                       \n"
  "eqn  Generated_and(x0, x1)  =  {x0 && x1};                   \n"
  "     LiftGenerated_or(X0, X1)  =  { y: Bool | exists x0,x1: Bool. x0 in X0 && x1 in X1 && y in Generated_or(x0, x1) };\n"
  "var  X0,X1: Set(Bool);                                       \n"
  "eqn  LiftGenerated_and(X0, X1)  =  { y: Bool | exists x0,x1: Bool. x0 in X0 && x1 in X1 && y in Generated_and(x0, x1) };\n"
  "                                                             \n"
  "pbes nu Z = val(true in LiftGenerated_and(LiftGenerated_and({false}, LiftGenerated_or({true,false}, {true,false})), {false}));\n"
  "                                                             \n"
  "init Z;                                                      \n";

// Regression test 2 for #1215
std::string test19 =
  "sort Enum5 = struct e4_5 | e3_5 | e2_5 | e1_5 | e0_5;        \n"
  "     AbsNat = struct nul | meer;                             \n"
  "                                                             \n"
  "map                                                          \n"
  "     absplus: AbsNat # AbsNat -> Set(AbsNat);                \n"
  "     abslt: AbsNat # AbsNat -> Set(Bool);                    \n"
  "     abseqn: AbsNat # AbsNat -> Set(Bool);                   \n"
  "     Generated_and: Bool # Bool -> Set(Bool);                \n"
  "     Liftabsplus: Set(AbsNat) # Set(AbsNat) -> Set(AbsNat);  \n"
  "     Liftabslt: Set(AbsNat) # Set(AbsNat) -> Set(Bool);      \n"
  "     Liftabseqn: Set(AbsNat) # Set(AbsNat) -> Set(Bool);     \n"
  "     LiftGenerated_and: Set(Bool) # Set(Bool) -> Set(Bool);  \n"
  "                                                             \n"
  "var  an,an': AbsNat;                                         \n"
  "eqn  absplus(nul, nul)  =  { nul };                          \n"
  "     absplus(meer, meer)  =  { meer };                       \n"
  "     absplus(meer, nul)  =  { meer };                        \n"
  "     absplus(nul, meer)  =  { meer };                        \n"
  "     abslt(nul, meer)  =  { true };                          \n"
  "     abslt(meer, meer)  =  { false, true };                  \n"
  "     abslt(an, nul)  =  { false };                           \n"
  "     abseqn(nul, nul)  =  { true };                          \n"
  "     an != an'  ->  abseqn(an, an')  =  { false };           \n"
  "     abseqn(meer, meer)  =  { false, true };                 \n"
  "                                                             \n"
  "var  x0,x1: Bool;                                            \n"
  "eqn  Generated_and(x0, x1)  =  {x0 && x1};                   \n"
  "                                                             \n"
  "var  X0,X1: Set(AbsNat);                                     \n"
  "eqn  Liftabsplus(X0, X1)  =  { y: AbsNat | exists a0,a1: AbsNat. a0 in X0 && a1 in X1 && y in absplus(a0, a1) };\n"
  "     Liftabslt(X0, X1)  =  { y: Bool | exists b0,b1: AbsNat. b0 in X0 && b1 in X1 && y in abslt(b0, b1) };\n"
  "                                                             \n"
  "var  X0,X1: Set(AbsNat);                                     \n"
  "eqn  Liftabseqn(X0, X1)  =  { y: Bool | exists c0,c1: AbsNat. c0 in X0 && c1 in X1 && y in abseqn(c0, c1) };\n"
  "                                                             \n"
  "var  X0,X1: Set(Bool);                                       \n"
  "eqn  LiftGenerated_and(X0, X1)  =  { y: Bool | exists d0,d1: Bool. d0 in X0 && d1 in X1 && y in Generated_and(d0, d1) };\n"
  "                                                             \n"
  "pbes nu Z = (                                                \n"
  "   val(!(false in LiftGenerated_and({false}, {true})) ||     \n"
  "   !(false in Liftabslt({nul}, Liftabsplus({nul}, {meer})))) \n"
  " );                                                          \n"
  "                                                             \n"
  "init Z;                                                      \n";

// N.B. The test cases below should not terminate, since they correspond
// to infinite BESs.
// TODO: Test that no solution for these cases is found within a certain number of steps.
//std::string test =
//    "pbes mu X(n: Nat) = X(n + 1) \n"
//    "init X(0);                   \n"
//    ;
//
//std::string test =
//    "pbes mu X(n: Nat) = X(n + 1) || forall n: Nat. val(n < 3); \n"
//    "init X(0);                                                 \n"
//    ;
//
//// Test case supplied by Jan Friso
//std::string test =
//    "pbes mu X(n: Nat) = (n<3 && X(n + 1)) || forall n: Nat. val(n<3); \n"
//    "init X(0);                                                        \n"
//    ;

void test_pbes2bool(const std::string& pbes_spec, bool expected_result, data::rewrite_strategy rewrite_strategy)
{
  pbes p = txt2pbes(pbes_spec);
  bool result = pbes2_bool_test(p, rewrite_strategy);
  if (result != expected_result)
  {
    std::cout << "--- pbes2bool failed ---\n";
    std::cout << pbes_system::pp(p) << std::endl;
    std::cout << "result: " << std::boolalpha << result << std::endl;
    std::cout << "expected result: " << std::boolalpha << expected_result << std::endl;
  }
  BOOST_CHECK(result == expected_result);
}

void test_pbespgsolve(const std::string& pbes_spec, const pbespgsolve_options& options, bool expected_result)
{
  pbes p = txt2pbes(pbes_spec);
  bool result = pbespgsolve(p, options);
  if (result != expected_result)
  {
    std::cout << "--- pbespgsolve failed ---\n";
    std::cout << pbes_system::pp(p) << std::endl;
    std::cout << "result:          " << std::boolalpha << result << std::endl;
    std::cout << "expected result: " << std::boolalpha << expected_result << std::endl;
  }
  BOOST_CHECK(result == expected_result);
}

void test_pbes_solve(const std::string& pbes_spec, bool expected_result)
{
  std::vector<data::rewrite_strategy> strategies = utilities::get_test_rewrite_strategies(false);

  for(std::vector<data::rewrite_strategy>::const_iterator i = strategies.begin(); i != strategies.end(); ++i)
  {
    test_pbes2bool(pbes_spec, expected_result, *i);

    // test with and without scc decomposition
    pbespgsolve_options options;
    options.rewrite_strategy = *i;

    test_pbespgsolve(pbes_spec, options, expected_result);

    options.use_scc_decomposition = false;
    test_pbespgsolve(pbes_spec, options, expected_result);
  }
}

std::string frm_nodeadlock = "[true*]<true*>true";
std::string frm_nolivelock = "[true*]mu X.[tau]X";

void test_abp_frm(const std::string& FORMULA, bool expected_result)
{
  bool timed = false;
  lps::specification spec=remove_stochastic_operators(lps::linearise(lps::detail::ABP_SPECIFICATION()));
  state_formulas::state_formula formula = state_formulas::parse_state_formula(FORMULA, spec);
  pbes_system::pbes p = pbes_system::lps2pbes(spec, formula, timed);
  std::string abp_text = pbes_system::pp(p);
  test_pbes_solve(abp_text, expected_result);
}

void test_abp()
{
  test_abp_frm(frm_nodeadlock, true);
  test_abp_frm(frm_nolivelock, true);
}

int test_main(int argc, char** argv)
{
  std::cerr << "Test01\n";
  test_pbes_solve(test01, false);
  std::cerr << "Test02\n";
  test_pbes_solve(test02, true);
  std::cerr << "Test03\n";
  test_pbes_solve(test03, false);
  std::cerr << "Test04\n";
  test_pbes_solve(test04, true);
  std::cerr << "Test05\n";
  test_pbes_solve(test05, false);
  std::cerr << "Test06\n";
  test_pbes_solve(test06, true);
  std::cerr << "Test07\n";
  test_pbes_solve(test07, false);
  std::cerr << "Test08\n";
  test_pbes_solve(test08, true);
  std::cerr << "Test09\n";
  test_pbes_solve(test09, true);
  std::cerr << "Test10\n";
  test_pbes_solve(test10, false);
  std::cerr << "Test11\n";
  test_pbes_solve(test11, true);
  std::cerr << "Test12\n";
  test_pbes_solve(test12, false);
  std::cerr << "Test13\n";
  test_pbes_solve(test13, true);
  std::cerr << "Test14\n";
  test_pbes_solve(test14, true);
  std::cerr << "Test15\n";
  test_pbes_solve(test15, false);
  std::cerr << "Test16\n";
  test_pbes_solve(test16, false);
  std::cerr << "Test17\n";
  test_pbes_solve(test17, false);
  std::cerr << "Test18\n";
  test_pbes_solve(test18, false);
  std::cerr << "Test19\n";
  test_pbes_solve(test19, true);
  std::cerr << "Test ABP\n";
  test_abp();

  return 0;
}
