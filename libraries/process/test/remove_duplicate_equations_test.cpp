// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file remove_duplicate_equations_test.cpp
/// \brief Tests for remove_duplicate_equations.

#define BOOST_TEST_MODULE remove_duplicate_equations_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/process/parse.h"
#include "mcrl2/process/print.h"
#include "mcrl2/process/remove_equations.h"

using namespace mcrl2;
using namespace mcrl2::process;

static std::vector<std::string> print_equations(const std::vector<process_equation>& equations)
{
  std::vector<std::string> result;
  for (const process_equation& equation: equations)
  {
    result.push_back(std::string(equation.identifier().name()) + " = " + process::pp(equation.expression()));
  }
  return result;
}

// Removes the duplicate equations of spec, in their order in spec, keeping the equation of initial, and returns the
// remaining equations printed as "name = body", in order.
static std::vector<std::string> removed_duplicates(const std::string& spec, const std::string& initial)
{
  const process_specification procspec = parse_process_specification(spec);
  std::vector<process_equation> equations = procspec.equations();
  const std::vector<process_equation>::const_iterator i = std::find_if(equations.begin(),
    equations.end(),
    [&](const process_equation& equation) { return std::string(equation.identifier().name()) == initial; });
  BOOST_REQUIRE(i != equations.end());
  const process_identifier initial_process = i->identifier();
  remove_duplicate_equations(equations, initial_process);
  return print_equations(equations);
}

static void check(const std::vector<std::string>& result, const std::vector<std::string>& expected)
{
  BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(), expected.begin(), expected.end());
}

// P3 and P4 are removed in favour of P1 and P2, and P1 then refers to P2.
BOOST_AUTO_TEST_CASE(test_chain)
{
  check(removed_duplicates("act a, b, c;\n"
                           "proc P1 = a . P2;\n"
                           "     P3 = a . P4;\n"
                           "     P2 = b . Q;\n"
                           "     P4 = b . Q;\n"
                           "     Q = c . Q;\n"
                           "init P1 + P3;\n",
          "P1"),
    {"P1 = a . P2", "P2 = b . Q", "Q = c . Q"});
}

// The first of two duplicate equations is kept, unless the second is the initial process.
BOOST_AUTO_TEST_CASE(test_initial_process_is_kept)
{
  const std::string spec = "act a, b;\n"
                           "proc P = a . R;\n"
                           "     Q = a . R;\n"
                           "     R = b . R;\n"
                           "init P;\n";
  check(removed_duplicates(spec, "P"), {"P = a . R", "R = b . R"});
  check(removed_duplicates(spec, "Q"), {"Q = a . R", "R = b . R"});
  check(removed_duplicates(spec, "R"), {"P = a . R", "R = b . R"});
}

// Equations that are equal if they are identified are duplicates, also if their right hand sides are not
// syntactically equal; equations with different parameters are not.
BOOST_AUTO_TEST_CASE(test_bisimulation)
{
  check(removed_duplicates("act a;\n"
                           "proc P = a . P;\n"
                           "     Q = a . Q;\n"
                           "init P;\n",
          "P"),
    {"P = a . P"});
  check(removed_duplicates("act a, b;\n"
                           "proc P = a . Q;\n"
                           "     Q = b . P;\n"
                           "     R = a . S;\n"
                           "     S = b . R;\n"
                           "init S;\n",
          "S"),
    {"P = a . S", "S = b . P"});
  check(removed_duplicates("act a: Nat;\n"
                           "proc P(n: Nat) = a(0) . delta;\n"
                           "     Q(m: Nat) = a(0) . delta;\n"
                           "init P(0);\n",
          "P"),
    {"P = a(0) . delta", "Q = a(0) . delta"});
}

// Equations whose right hand sides differ only in identifiers of different classes are not duplicates.
BOOST_AUTO_TEST_CASE(test_split)
{
  check(removed_duplicates("act a, b, c;\n"
                           "proc P1 = a . P2;\n"
                           "     P3 = a . P4;\n"
                           "     P2 = b . delta;\n"
                           "     P4 = c . delta;\n"
                           "init P1;\n",
          "P1"),
    {"P1 = a . P2", "P3 = a . P4", "P2 = b . delta", "P4 = c . delta"});
}

// A removed identifier can occur in the right hand side of the kept equation, which then becomes recursive.
BOOST_AUTO_TEST_CASE(test_removed_into_recursion)
{
  check(removed_duplicates("act a;\n"
                           "proc P = a . R;\n"
                           "     Q = a . R;\n"
                           "     R = a . R;\n"
                           "init P;\n",
          "P"),
    {"P = a . P"});
}

// On a process specification the first equation of every class is kept, also in the initial process, and the
// order of the equations is kept.
BOOST_AUTO_TEST_CASE(test_process_specification)
{
  process_specification procspec = parse_process_specification("act a, b;\n"
                                                               "proc Z = b . Z;\n"
                                                               "     P = a . Q;\n"
                                                               "     Q = a . P;\n"
                                                               "     Y = b . Y;\n"
                                                               "init Q + Y;\n");
  remove_duplicate_equations(procspec);
  check(print_equations(procspec.equations()), {"Z = b . Z", "P = a . P"});
  BOOST_CHECK_EQUAL(process::pp(procspec.init()), "P + Z");
}
