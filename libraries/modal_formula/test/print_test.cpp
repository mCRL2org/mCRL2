// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print_test.cpp
/// \brief Tests pretty printing of state formulas.

#include <iostream>
#include <string>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/monotonicity.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/print.h"
#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2::lps;
using namespace mcrl2::state_formulas;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

void run_test_case(const std::string& formula, const std::string& lps_spec)
{
  specification spec = linearise(lps_spec);
  bool check_monotonicity = false;
  bool translate_regular = false;
  state_formula f = parse_state_formula(formula, spec, check_monotonicity, translate_regular);
  std::string pp_formula = state_formulas::pp(f);
  if (formula != pp_formula)
  {
    std::cerr << "Error: " << formula << " is printed as " << pp_formula << std::endl;
  }
  BOOST_CHECK(formula == pp_formula);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  std::string lps_spec = lps::detail::ABP_SPECIFICATION();

  run_test_case("delay @ 1", lps_spec);
  run_test_case("true", lps_spec);
  run_test_case("[true*]<true*>true", lps_spec);
  run_test_case("mu X. !!X", lps_spec);
  run_test_case("nu X. [true]X && <true>true", lps_spec);
  run_test_case("nu X. [true]X && forall d: D. [r1(d)]mu Y. <true>Y || <s4(d)>true", lps_spec);
  run_test_case("forall d: D. nu X. [!r1(d)]X && [s4(d)]false", lps_spec);
  run_test_case("nu X. [true]X && forall d: D. [r1(d)]nu Y. [!r1(d) && !s4(d)]Y && [r1(d)]false", lps_spec);
  run_test_case("mu X. !X", lps_spec);
  run_test_case("mu X. nu Y. X => Y", lps_spec);
  run_test_case("mu X. X || mu X. X", lps_spec);
  run_test_case("mu X. X || mu X. X", lps_spec);
  run_test_case("(mu X. X) || mu Y. Y", lps_spec);
  run_test_case("!(mu X. X || mu X. X)", lps_spec);
  run_test_case("(forall d: D. nu X. X) && false", lps_spec);
  run_test_case("val(true)", lps_spec);
  run_test_case("delay @ 4", lps_spec);
  run_test_case("nu Z(i: Nat = 0). Z(2)", lps_spec);
  run_test_case("[true]true", lps_spec);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
