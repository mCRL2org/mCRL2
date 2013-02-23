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
#include "mcrl2/pbes/detail/pbes_property_map.h"
#include "mcrl2/pbes/remove_equations.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

std::string p1 =
  "pbes nu X1 = X2 && X3;                                   \n"
  "     nu X2 = X4 && X1;                                   \n"
  "     nu X3 = true;                                       \n"
  "     nu X4 = false;                                      \n"
  "     nu X5 = X6;                                         \n"
  "     nu X6 = X5;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;
std::string r1 = "binding_variable_names = X1, X2, X3, X4";

std::string p2 =
  "pbes                      \n"
  " nu X(n:Nat) = Y && X(n); \n"
  " mu Y = Z;                \n"
  " nu Z = Y;                \n"
  " nu U = U;                \n"
  "                          \n"
  " init X(0);               \n"
  ;
std::string r2 = "binding_variable_names = X, Y, Z";

void test_remove_unreachable_variables(const std::string& pbes_spec, const std::string& expected_result)
{
  pbes<> p = txt2pbes(pbes_spec);
  pbes<> q = p;
  remove_unreachable_variables(q);
  BOOST_CHECK(q.is_well_typed());
  pbes_system::detail::pbes_property_map info1(q);
  pbes_system::detail::pbes_property_map info2(expected_result);
  std::string diff = info1.compare(info2);
  if (!diff.empty())
  {
    std::cerr << "\n------ FAILED TEST ------" << std::endl;
    std::cerr << "--- expected result" << std::endl;
    std::cerr << expected_result << std::endl;
    std::cerr << "--- found result" << std::endl;
    std::cerr << info1.to_string() << std::endl;
    std::cerr << "--- differences" << std::endl;
    std::cerr << diff << std::endl;
  }
  BOOST_CHECK(diff.empty());
}

void test_remove_unreachable_variables()
{
  test_remove_unreachable_variables(p1, r1);
  test_remove_unreachable_variables(p2, r2);
}

int test_main(int argc, char* argv[])
{
  test_remove_unreachable_variables();

  return 0;
}
