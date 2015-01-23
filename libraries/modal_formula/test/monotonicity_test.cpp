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

#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/modal_formula/is_monotonous.h"
#include "mcrl2/modal_formula/parse.h"
#include "mcrl2/modal_formula/has_name_clashes.h"
#include "mcrl2/modal_formula/resolve_name_clashes.h"
#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2::lps;
using namespace mcrl2::state_formulas;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

void run_monotonicity_test_case(const std::string& formula, const std::string& lps_spec, const bool expect_success = true)
{
  specification spec=remove_stochastic_operators(linearise(lps_spec));
  bool check_monotonicity = false;
  state_formula f = parse_state_formula(formula, spec, check_monotonicity);
  if (state_formulas::has_name_clashes(f))
  {
    std::cerr << "Error: " << state_formulas::pp(f) << " has name clashes" << std::endl;
    f = state_formulas::resolve_name_clashes(f);
    std::cerr << "resolved to " << state_formulas::pp(f) << std::endl;
  }
  BOOST_CHECK(is_monotonous(f) == expect_success);
}

BOOST_AUTO_TEST_CASE(test_abp)
{
  std::string lps_spec = lps::detail::ABP_SPECIFICATION();

  run_monotonicity_test_case("true", lps_spec, true);
  run_monotonicity_test_case("[true*]<true*>true", lps_spec, true);
  run_monotonicity_test_case("mu X. !!X", lps_spec, true);
  run_monotonicity_test_case("nu X. ([true]X && <true>true)", lps_spec, true);
  run_monotonicity_test_case("nu X. ([true]X && forall d:D. [r1(d)] mu Y. (<true>Y || <s4(d)>true))", lps_spec, true);
  run_monotonicity_test_case("forall d:D. nu X. (([!r1(d)]X && [s4(d)]false))", lps_spec, true);
  run_monotonicity_test_case("nu X. ([true]X && forall d:D. [r1(d)]nu Y. ([!r1(d) && !s4(d)]Y && [r1(d)]false))", lps_spec, true);
  run_monotonicity_test_case("mu X. !X", lps_spec, false);
  run_monotonicity_test_case("mu X. nu Y. (X => Y)", lps_spec, false);
  run_monotonicity_test_case("mu X. X || mu X. X", lps_spec, true);
  run_monotonicity_test_case("mu X. (X || mu X. X)", lps_spec, true);
  run_monotonicity_test_case("mu X. (X || mu Y. Y)", lps_spec, true);
  run_monotonicity_test_case("!(mu X. X || mu X. X)", lps_spec, true);
  run_monotonicity_test_case("!(mu X. (X || mu X. X))", lps_spec, true);
  run_monotonicity_test_case("!(mu X. (X || mu Y. Y))", lps_spec, true);
}

// Test case provided by Jeroen Keiren, 10-9-2010
BOOST_AUTO_TEST_CASE(test_elevator)
{
  std::string lps_spec =

    "% Model of an elevator for n floors.                                                                                           \n"
    "% Originally described in 'Solving Parity Games in Practice' by Oliver                                                         \n"
    "% Friedmann and Martin Lange.                                                                                                  \n"
    "%                                                                                                                              \n"
    "% This is the version with a first in first out policy                                                                         \n"
    "                                                                                                                               \n"
    "sort Floor = Pos;                                                                                                              \n"
    "     DoorStatus = struct open | closed;                                                                                        \n"
    "     Requests = List(Floor);                                                                                                   \n"
    "                                                                                                                               \n"
    "map maxFloor: Floor;                                                                                                           \n"
    "eqn maxFloor = 3;                                                                                                              \n"
    "                                                                                                                               \n"
    "map addRequest : Requests # Floor -> Requests;                                                                                 \n"
    "                                                                                                                               \n"
    "var r: Requests;                                                                                                               \n"
    "    f,g: Floor;                                                                                                                \n"
    "    % FIFO behaviour!                                                                                                          \n"
    "eqn addRequest([], f) = [f];                                                                                                   \n"
    "    (f == g) -> addRequest(g |> r, f) = g |> r;                                                                                \n"
    "    (f != g) -> addRequest(g |> r, f) = g |> addRequest(r, f);                                                                 \n"
    "                                                                                                                               \n"
    "map removeRequest : Requests -> Requests;                                                                                      \n"
    "var r: Requests;                                                                                                               \n"
    "    f: Floor;                                                                                                                  \n"
    "eqn removeRequest(f |> r) = r;                                                                                                 \n"
    "                                                                                                                               \n"
    "map getNext : Requests -> Floor;                                                                                               \n"
    "var r: Requests;                                                                                                               \n"
    "    f: Floor;                                                                                                                  \n"
    "eqn getNext(f |> r) = f;                                                                                                       \n"
    "                                                                                                                               \n"
    "act isAt: Floor;                                                                                                               \n"
    "    request: Floor;                                                                                                            \n"
    "    close, open, up, down;                                                                                                     \n"
    "                                                                                                                               \n"
    "proc Elevator(at: Floor, status: DoorStatus, reqs: Requests, moving: Bool) =                                                   \n"
    "       isAt(at) . Elevator()                                                                                                   \n"
    "     + sum f: Floor. (f <= maxFloor) -> request(f) . Elevator(reqs = addRequest(reqs, f))                                      \n"
    "     + (status == open) -> close . Elevator(status = closed)                                                                   \n"
    "     + (status == closed && reqs != [] && getNext(reqs) > at) -> up . Elevator(at = at + 1, moving = true)                     \n"
    "     + (status == closed && reqs != [] && getNext(reqs) < at) -> down . Elevator(at = Int2Pos(at - 1), moving = true)          \n"
    "     + (status == closed && getNext(reqs) == at) -> open. Elevator(status = open, reqs = removeRequest(reqs), moving = false); \n"
    "                                                                                                                               \n"
    "init Elevator(1, open, [], false);                                                                                             \n"
    ;

  run_monotonicity_test_case("nu U. [true] U && ((mu V . nu W. !([!request(maxFloor)]!W && [request(maxFloor)]!V)) || (nu X . mu Y. [!isAt(maxFloor)] Y &&  [isAt(maxFloor)]X))", lps_spec, true);
  run_monotonicity_test_case("nu U. [true] U && ((nu V . mu W. ([!request(maxFloor)]W && [request(maxFloor)]V)) => (nu X . mu Y. [!isAt(maxFloor)] Y &&  [isAt(maxFloor)]X))", lps_spec, true);
  run_monotonicity_test_case("nu U. [true] U && (!(nu V . mu W. ([!request(maxFloor)]W && [request(maxFloor)]V)) || (nu X . mu Y. [!isAt(maxFloor)] Y &&  [isAt(maxFloor)]X))", lps_spec, true);
  run_monotonicity_test_case("(nu X . mu Y. X) => true", lps_spec, true);
  run_monotonicity_test_case("!(nu X . mu Y. X)", lps_spec, true);
  run_monotonicity_test_case("mu X . X", lps_spec, true);
  run_monotonicity_test_case("nu X . X", lps_spec, true);
  run_monotonicity_test_case("mu X . !X", lps_spec, false);
  run_monotonicity_test_case("nu X . !X", lps_spec, false);
  run_monotonicity_test_case("!(mu X . X)", lps_spec, true);
  run_monotonicity_test_case("!(nu X . X)", lps_spec, true);
  run_monotonicity_test_case("(mu X . X) => true", lps_spec, true);
  run_monotonicity_test_case("(nu X . X) => true", lps_spec, true);
  run_monotonicity_test_case("!(mu X. (mu X. X))", lps_spec, true);

  // trac ticket #1320
  run_monotonicity_test_case("!mu X. [true]X && mu X. [true]X", lps_spec, true);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
