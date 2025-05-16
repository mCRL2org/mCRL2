// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print_test.cpp
/// \brief Tests pretty printing of state formulas.

#define BOOST_TEST_MODULE modal_formula_print_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/print.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::state_formulas;

void run_test_case(const std::string& formula, const specification& lpsspec)
{
  specification lpscopy(lpsspec);

  parse_state_formula_options options;
  options.check_monotonicity = false;
  options.translate_regular_formulas = false;
  options.resolve_name_clashes = false;
  state_formula f = parse_state_formula(formula, lpscopy, false, options);
  std::string pp_formula = state_formulas::pp(f);
  if (formula != pp_formula)
  {
    std::cerr << "Error: " << formula << " is printed as " << pp_formula << std::endl;
  }
  BOOST_TEST(formula == pp_formula);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  std::string lpstext = lps::detail::ABP_SPECIFICATION();
  specification lpsspec = remove_stochastic_operators(linearise(lpstext));

  run_test_case("delay @ 1", lpsspec);
  run_test_case("true", lpsspec);
  run_test_case("[true*]<true*>true", lpsspec);
  run_test_case("mu X. !!X", lpsspec);
  run_test_case("nu X. [true]X && <true>true", lpsspec);
  run_test_case("nu X. [true]X && (forall d: D. [r1(d)](mu Y. <true>Y || <s4(d)>true))", lpsspec);
  run_test_case("forall d: D. nu X. [!r1(d)]X && [s4(d)]false", lpsspec);
  run_test_case("nu X. [true]X && (forall d: D. [r1(d)](nu Y. [!r1(d) && !s4(d)]Y && [r1(d)]false))", lpsspec);
  run_test_case("mu X. !X", lpsspec);
  run_test_case("mu X. nu Y. X => Y", lpsspec);
  run_test_case("mu X. X || (mu X. X)", lpsspec);
  run_test_case("(mu X. X) || (mu Y. Y)", lpsspec);
  run_test_case("!(mu X. X || (mu X. X))", lpsspec);
  run_test_case("(forall d: D. nu X. X) && false", lpsspec);
  run_test_case("val(true)", lpsspec);
  run_test_case("delay @ 4", lpsspec);
  run_test_case("nu Z(i: Nat = 0). Z(2)", lpsspec);
  run_test_case("[true]true", lpsspec);
  run_test_case("[true]val(true)", lpsspec);
  run_test_case("exists d: Nat. val(d < 0)", lpsspec);
  run_test_case("exists d: Nat. val(d < 0) || val(d + 1 == 5)", lpsspec);
  run_test_case("mu X. X || val(3 > 2)", lpsspec);  // Check whether val is printed properly after a fixed point without parameters. 
  run_test_case("<true . true @ 5>true",lpsspec);   // This formula was parsed as (true.true)@5 at some point, which was clearly incorrect. 

}
