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

using namespace mcrl2;
using namespace mcrl2::pbes_system;

std::string test1 =
  "pbes                                                                              \n"
  "                                                                                  \n"
  "nu X(b:Bool, n:Nat) = (val(b) => X(!b, n)) && (val(!b) => X(!b, n+1));            \n"
  "                                                                                  \n"
  "init X(true,0);                                                                   \n"
  ;

void test_pbes_solve(const std::string& pbes_spec, bool expected_result)
{
  pbes<> p = txt2pbes(pbes_spec);
  bool result = pbes2_bool_test(p);
  if (result != expected_result)
  {
    std::cout << "--- failed test ---\n";
    std::cout << core::pp(pbes_to_aterm(p)) << std::endl;
    std::cout << "result: "          << result << std::endl;
    std::cout << "expected result: " << expected_result << std::endl;
    // std::cout << "result: "          << std::boolalpha(result) << std::endl;
    // std::cout << "expected result: " << std::boolalpha(expected_result) << std::endl;
  }
  BOOST_CHECK(result == expected_result);
}

void test_pbes_solve()
{
  test_pbes_solve(test1, true);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_pbes_solve();

  return 0;
}
