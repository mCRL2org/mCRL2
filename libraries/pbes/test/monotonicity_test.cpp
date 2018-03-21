// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file monotonicity_test.cpp
/// \brief Tests for the is_monotonous function for pbes expressions.

#define BOOST_TEST_MODULE monotonicity_test
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/pbes/is_monotonous.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void run_monotonicity_test_case(const pbes_expression& x, bool expected_result)
{
  bool result = is_monotonous(x);
  if (result != expected_result)
  {
    std::cerr << "--- Failing monotonicity test case ---\n";
    std::cerr << " x = " << pbes_system::pp(x) << std::endl;
    std::cerr << " expected_result = " << std::boolalpha << expected_result << std::endl;
  }
  BOOST_CHECK(result == expected_result);
}

BOOST_AUTO_TEST_CASE(test_monotonicity)
{
  std::string text =
    "pbes                                                 \n"
    " nu X0(m, n: Nat) = val(n == m) && X0(m + 1, n + 1); \n"
    " nu X1            = !X1;                             \n"
    " nu X2            = !!X2;                            \n"
    " nu X3            = !X2 => X1;                       \n"
    " nu X4            = !(forall n:Nat . (X2 => !X1));   \n"
    " nu X5            = !(forall n:Nat . (X2 || !X1));   \n"
    "                                                     \n"
    " init X0(0, 0);                                      \n"
    ;
  bool normalize = false;
  pbes p = txt2pbes(text, normalize);
  std::vector<pbes_equation> eqn = p.equations();

  std::vector<bool> expected_results(eqn.size(), true);
  expected_results[0] = true;
  expected_results[1] = false;
  expected_results[2] = true;
  expected_results[3] = true;
  expected_results[4] = true;
  expected_results[5] = false;

  for (std::size_t i = 0; i < eqn.size(); ++i)
  {
    run_monotonicity_test_case(eqn[i].formula(), expected_results[i]);
  }
}
