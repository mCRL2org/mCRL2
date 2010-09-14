// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file monotonicity_test.cpp
/// \brief Tests for the is_monotonous function for state formulas.

#include <iostream>
#include <string>

#include <boost/test/included/unit_test_framework.hpp>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/modal_formula/monotonicity.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2::lps;
using namespace mcrl2::state_formulas;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

const std::string ABP_SPECIFICATION =
"% This file contains the alternating bit protocol, as described in W.J.    \n"
"% Fokkink, J.F. Groote and M.A. Reniers, Modelling Reactive Systems.       \n"
"%                                                                          \n"
"% The only exception is that the domain D consists of two data elements to \n"
"% facilitate simulation.                                                   \n"
"                                                                           \n"
"sort                                                                       \n"
"  D     = struct d1 | d2;                                                  \n"
"  Error = struct e;                                                        \n"
"                                                                           \n"
"act                                                                        \n"
"  r1,s4: D;                                                                \n"
"  s2,r2,c2: D # Bool;                                                      \n"
"  s3,r3,c3: D # Bool;                                                      \n"
"  s3,r3,c3: Error;                                                         \n"
"  s5,r5,c5: Bool;                                                          \n"
"  s6,r6,c6: Bool;                                                          \n"
"  s6,r6,c6: Error;                                                         \n"
"  i;                                                                       \n"
"                                                                           \n"
"proc                                                                       \n"
"  S(b:Bool)     = sum d:D. r1(d).T(d,b);                                   \n"
"  T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));             \n"
"                                                                           \n"
"  R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+                      \n"
"                  (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);                    \n"
"                                                                           \n"
"  K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;           \n"
"                                                                           \n"
"  L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;                   \n"
"                                                                           \n"
"init                                                                       \n"
"  allow({r1,s4,c2,c3,c5,c6,i},                                             \n"
"    comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},                     \n"
"        S(true) || K || L || R(true)                                       \n"
"    )                                                                      \n"
"  );                                                                       \n"
;

void run_monotonicity_test_case(const std::string& formula, const std::string& lps_spec, const bool expect_success = true)
{
  specification spec = linearise(lps_spec);
  state_formula f = parse_state_formula(formula, spec);
  BOOST_CHECK(is_monotonous(f) == expect_success);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  std::string lps_spec = ABP_SPECIFICATION;

  run_monotonicity_test_case("true", lps_spec, true);
  run_monotonicity_test_case("[true*]<true*>true", lps_spec, true);
  run_monotonicity_test_case("mu X. !!X", lps_spec, true);
  run_monotonicity_test_case("nu X. ([true]X && <true>true)", lps_spec, true);
  run_monotonicity_test_case("nu X. ([true]X && forall d:D. [r1(d)] mu Y. (<true>Y || <s4(d)>true))", lps_spec, true);
  run_monotonicity_test_case("forall d:D. nu X. (([!r1(d)]X && [s4(d)]false))", lps_spec, true);
  run_monotonicity_test_case("nu X. ([true]X && forall d:D. [r1(d)]nu Y. ([!r1(d) && !s4(d)]Y && [r1(d)]false))", lps_spec, true);

  // Can't run these test cases, since the type checker checks for monotonicity as well !?!?
  //run_monotonicity_test_case("mu X. !X", lps_spec, false);
  //run_monotonicity_test_case("mu X. nu Y. (X => Y)", lps_spec, false);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}
