// Author(s): Aad Mathijssen, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file typecheck2_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <sstream>
#include <string>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/core/parse.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/typecheck.h"
#include "mcrl2/utilities/test_utilities.h"

using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

void test_process_specification(const std::string& ps_in, bool const expected_result = true, bool const test_type_checker = true)
{
  process::process_specification p = process::parse_process_specification_new(ps_in);
  std::string ps_out;
  if (test_type_checker)
  {
    process::process_specification ps = p;
    if (expected_result)
    {
      process::type_check(ps);
      ps_out = process::pp(ps);
      //std::cerr << "The following process specifications should be the same:" << std::endl << ps_in  << std::endl << "and" << std::endl << ps_out << std::endl;
      BOOST_CHECK_EQUAL(ps_in, ps_out);
    }
    else
    {
      BOOST_CHECK_THROW(process::type_check(ps), mcrl2::runtime_error);
    }
  }
}

BOOST_AUTO_TEST_CASE(test_process_reference_assignment)
{
  //test process specification involving process reference assignments
  test_process_specification(
    "proc P(b: Bool) = tau . P() + tau . P(b = false);\n"
    "\n"
    "init P(b = true);\n"
  );
}

BOOST_AUTO_TEST_CASE(test_global_variables)
{
  //test process specification involving global variables
  test_process_specification(
    "glob dc: Bool;\n"
    "\n"
    "proc P(b: Bool) = tau . P(dc);\n"
    "\n"
    "init P(dc);\n"
  );
}

// For bug #732
BOOST_AUTO_TEST_CASE(test_function_condition)
{
  test_process_specification(
    "map  b: Nat -> Nat;\n\n"
    "init b -> tau;\n",
    false
  );
}

// For bug #732
BOOST_AUTO_TEST_CASE(test_function_as_set_descriptor)
{
  test_process_specification(
    "map  b: Bool # Pos -> Nat;\n"
    "     s: Set(Nat);\n\n"
    "eqn  s  =  { n: Nat | b };\n\n"
    "init b -> tau;\n",
    false
  );
}

// For bug #732
BOOST_AUTO_TEST_CASE(test_function_as_equation_condition)
{
  test_process_specification(
    "map  b: Bool # Pos -> Nat;\n"
    "     n: Nat;\n\n"
    "eqn  b  ->  n  =  0;\n\n"
    "act  a: Nat;\n\n"
    "init a(n);\n",
    false
  );
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return 0;
}
