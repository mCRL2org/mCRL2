// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file merge_identical_equations_test.cpp
/// \brief Tests for merge_identical_equations.

#define BOOST_TEST_MODULE merge_identical_equations_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/process/merge_identical_equations.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/print.h"

using namespace mcrl2;
using namespace mcrl2::process;

// Merges the equations of spec, in their order in spec, keeping the equation of initial, and returns the remaining
// equations printed as "name = body", in order.
static std::vector<std::string> merged_equations(const std::string& spec, const std::string& initial)
{
  const process_specification procspec = parse_process_specification(spec);
  const std::vector<process_equation>& equations = procspec.equations();
  const std::vector<process_equation>::const_iterator i = std::find_if(equations.begin(),
    equations.end(),
    [&](const process_equation& equation) { return std::string(equation.identifier().name()) == initial; });
  BOOST_REQUIRE(i != equations.end());

  std::vector<std::string> result;
  for (const process_equation& equation: merge_identical_equations(equations, i->identifier()))
  {
    result.push_back(std::string(equation.identifier().name()) + " = " + process::pp(equation.expression()));
  }
  return result;
}

static void check(const std::vector<std::string>& result, const std::vector<std::string>& expected)
{
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(), expected.begin(), expected.end());
}

// The example of the documentation: P3 and P4 are merged into P1 and P2, and P1 then refers to P2.
BOOST_AUTO_TEST_CASE(test_chain)
{
  check(merged_equations("act a, b, c;\n"
                         "proc P1 = a . P2;\n"
                         "     P3 = a . P4;\n"
                         "     P2 = b . Q;\n"
                         "     P4 = b . Q;\n"
                         "     Q = c . Q;\n"
                         "init P1 + P3;\n",
          "P1"),
    {"P1 = a . P2", "P2 = b . Q", "Q = c . Q"});
}

// The first of two equal equations is kept, unless the second is the initial process.
BOOST_AUTO_TEST_CASE(test_initial_process_is_kept)
{
  const std::string spec = "act a, b;\n"
                           "proc P = a . R;\n"
                           "     Q = a . R;\n"
                           "     R = b . R;\n"
                           "init P;\n";
  check(merged_equations(spec, "P"), {"P = a . R", "R = b . R"});
  check(merged_equations(spec, "Q"), {"Q = a . R", "R = b . R"});
}

// Equations that are only equal if they are merged are not merged, nor are equations with different parameters.
BOOST_AUTO_TEST_CASE(test_not_merged)
{
  check(merged_equations("act a;\n"
                         "proc P = a . P;\n"
                         "     Q = a . Q;\n"
                         "init P;\n",
          "P"),
    {"P = a . P", "Q = a . Q"});
  check(merged_equations("act a: Nat;\n"
                         "proc P(n: Nat) = a(0) . delta;\n"
                         "     Q(m: Nat) = a(0) . delta;\n"
                         "init P(0);\n",
          "P"),
    {"P = a(0) . delta", "Q = a(0) . delta"});
}

// Merging propagates over several rounds: once P2 and P4 are merged, P1 and P3 become equal.
BOOST_AUTO_TEST_CASE(test_several_rounds)
{
  check(merged_equations("act a, b, c;\n"
                         "proc P1 = a . P2;\n"
                         "     P3 = a . P4;\n"
                         "     P2 = b . P5;\n"
                         "     P4 = b . P6;\n"
                         "     P5 = c . delta;\n"
                         "     P6 = c . delta;\n"
                         "init P3;\n",
          "P3"),
    {"P3 = a . P2", "P2 = b . P5", "P5 = c . delta"});
}

// A merged identifier can occur in the right hand side of the kept equation, which then becomes recursive.
BOOST_AUTO_TEST_CASE(test_merged_into_recursion)
{
  check(merged_equations("act a;\n"
                         "proc P = a . R;\n"
                         "     Q = a . R;\n"
                         "     R = a . R;\n"
                         "init P;\n",
          "P"),
    {"P = a . P"});
}
