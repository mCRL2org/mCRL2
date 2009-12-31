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
#include "mcrl2/pbes/pbes_solver_test.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "../../../tools/pbespgsolve/pbespgsolve.h"

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


void test_pbes2bool(const std::string& pbes_spec, bool expected_result)
{
  pbes<> p = txt2pbes(pbes_spec);
  bool result = pbes2_bool_test(p);
  if (result != expected_result)
  {
    std::cout << "--- pbes2bool failed ---\n";
    std::cout << core::pp(pbes_to_aterm(p)) << std::endl;
    std::cout << "result: " << std::boolalpha << result << std::endl;
    std::cout << "expected result: " << std::boolalpha << expected_result << std::endl;
  }
  BOOST_CHECK(result == expected_result);
  core::garbage_collect();
}

void test_pbespgsolve(const std::string& pbes_spec, const pbespgsolve_options& options, bool expected_result)
{
  pbes<> p = txt2pbes(pbes_spec);
  unsigned int log_level = 0;
  pbespgsolve_algorithm algorithm(log_level, options);
  bool result;
  try
  {
    result = algorithm.run(p);
    if (result != expected_result)
    {
      std::cout << "--- pbespgsolve failed ---\n";
      std::cout << core::pp(pbes_to_aterm(p)) << std::endl;
      std::cout << "result: " << std::boolalpha << result << std::endl;
      std::cout << "expected result: " << std::boolalpha << expected_result << std::endl;
    }
    BOOST_CHECK(result == expected_result);
  }
  catch (mcrl2::runtime_error e)
  {
    std::cout << "--- pbespgsolve failed ---\n";
    std::cout << core::pp(pbes_to_aterm(p)) << std::endl;
    std::cout << e.what() << std::endl;
    BOOST_CHECK(false);
  }
  core::garbage_collect();
}

void test_pbes_solve(const std::string& pbes_spec, bool expected_result)
{
  test_pbes2bool(pbes_spec, expected_result);

  // test with and without scc decomposition
  pbespgsolve_options options;
  test_pbespgsolve(pbes_spec, options, expected_result);
  options.use_scc_decomposition = false;
  test_pbespgsolve(pbes_spec, options, expected_result);
}

void test_all()
{
  test_pbes_solve(test01, false);
  test_pbes_solve(test02, true);
  test_pbes_solve(test03, false);
  test_pbes_solve(test04, true);
  test_pbes_solve(test05, false);
  test_pbes_solve(test06, true);
  test_pbes_solve(test07, false);
  test_pbes_solve(test08, true);
  test_pbes_solve(test09, true);
  test_pbes_solve(test10, false);
  test_pbes_solve(test11, true);
  test_pbes_solve(test12, false);
  test_pbes_solve(test13, true);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_all();

  return 0;
}
