// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_property_map_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE pbes_property_map_test
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/pbes/detail/pbes_property_map.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

BOOST_AUTO_TEST_CASE(property_map_test)
{
  const std::string PBES_SPEC=
    "% This file contains PBES example 5, as described in:                       \n"
    "%                                                                           \n"
    "% A. van Dam, B. Ploeger and T.A.C. Willemse. Instantiation for             \n"
    "% Parameterised Boolean Equation Systems.  Computer Science Report          \n"
    "% 08-11, Eindhoven University of Technology (TU/e), 2008.                   \n"
    "                                                                            \n"
    "pbes mu X(b: Bool, n: Nat) = Y(b, n);                                       \n"
    "     nu Y(b: Bool, n: Nat) = (val(!b) || X(!b, n)) &&                       \n"
    "                             (val(!b) || Y(b, n+1)) &&                      \n"
    "                             (val(b) || val(n == 0) || Y(b, Int2Nat(n-1))); \n"
    "                                                                            \n"
    "init X(true, 0);                                                            \n"
  ;

  const std::string INFO =
    "binding_variable_names       = X, Y                                   \n"
    "binding_variables            = X(b: Bool, n: Nat), Y(b: Bool, n: Nat) \n"
    "declared_free_variable_names =                                        \n"
    "declared_free_variables      =                                        \n"
    "declared_variable_count      = 0                                      \n"
    "equation_count               = 2                                      \n"
    "mu_equation_count            = 1                                      \n"
    "nu_equation_count            = 1                                      \n"
    "occurring_variable_names     = X, Y                                   \n"
    "occurring_variables          = X(b: Bool, n: Nat), Y(b: Bool, n: Nat) \n"
    "used_free_variable_count     = 0                                      \n"
    "used_free_variables          =                                        \n"
    "used_free_variables_names    =                                        \n"
  ;

  pbes p = txt2pbes(PBES_SPEC);
  pbes_system::detail::pbes_property_map info1(p);
  std::cerr << info1.to_string() << std::endl;
  pbes_system::detail::pbes_property_map info2(INFO);
  std::string diff = info1.compare(info2);
  BOOST_CHECK(diff.empty());
}
