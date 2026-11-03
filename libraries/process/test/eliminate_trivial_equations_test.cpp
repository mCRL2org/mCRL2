// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file eliminate_trivial_equations_test.cpp
/// \brief Tests for eliminate_trivial_equations.

#define BOOST_TEST_MODULE eliminate_trivial_equations_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/process/eliminate_trivial_equations.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/print.h"

using namespace mcrl2;
using namespace mcrl2::process;

// Eliminates the trivial equations of spec, and checks that the result is printed as expected. The printed forms are
// compared, because the type checker may give the numbers in expected other sorts than the substitutions give them.
static void check_eliminate_trivial_equations(const std::string& spec, const std::string& expected)
{
  process_specification result = parse_process_specification(spec);
  eliminate_trivial_equations(result);
  const std::string expected_result = process::pp(parse_process_specification(expected));
  BOOST_CHECK_MESSAGE(process::pp(result) == expected_result,
    "result\n"
      << process::pp(result) << "expected\n"
      << expected_result);
}

BOOST_AUTO_TEST_CASE(test_chain)
{
  check_eliminate_trivial_equations("act a: Nat;\n"
                                    "proc P3(n: Nat) = a(n) . P3(n);\n"
                                    "     P2(k: Nat) = P3(k + 10);\n"
                                    "     P1(i: Nat) = P2(i + 100);\n"
                                    "init P1(0);\n",
    "act a: Nat;\n"
    "proc P3(n: Nat) = a(n) . P3(n);\n"
    "init P3(0 + 100 + 10);\n");
}

// Two chains P1 = P2 = P3 = P4 and P5 = P2 = P3 = P4 that share the tail P2 = P3 = P4. The second chain was computed
// from the already completed substitution of the first one, which gave P4(m + 1) for P5(0), with m the parameter of
// P3.
BOOST_AUTO_TEST_CASE(test_chains_with_a_shared_tail)
{
  check_eliminate_trivial_equations("act a: Nat;\n"
                                    "proc P4(n: Nat) = a(n) . P4(n);\n"
                                    "     P3(m: Nat) = P4(m + 1);\n"
                                    "     P2(k: Nat) = P3(k + 10);\n"
                                    "     P1(i: Nat) = P2(i + 100);\n"
                                    "     P5(j: Nat) = P2(j + 1000);\n"
                                    "init P1(0) + P5(0);\n",
    "act a: Nat;\n"
    "proc P4(n: Nat) = a(n) . P4(n);\n"
    "init P4(0 + 100 + 10 + 1) + P4(0 + 1000 + 10 + 1);\n");
}

// Trivial equations that form a cycle cannot be eliminated. Both equations were removed, while P3 still referred to
// them.
BOOST_AUTO_TEST_CASE(test_cycle)
{
  process_specification spec = parse_process_specification("act a;\n"
                                                           "proc P1 = P2;\n"
                                                           "     P2 = P1;\n"
                                                           "     P3 = a . P1;\n"
                                                           "init P3;\n");
  BOOST_CHECK_THROW(eliminate_trivial_equations(spec), mcrl2::runtime_error);
}
