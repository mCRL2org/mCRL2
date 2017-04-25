// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print_test.cpp
/// \brief Add your file description here.

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/print.h"
#include "mcrl2/pbes/txt2pbes.h"
#include <boost/test/included/unit_test_framework.hpp>
#include <string>

using namespace mcrl2;
using namespace mcrl2::pbes_system;

BOOST_AUTO_TEST_CASE(pbes_with_reals)
{
  std::string input(
    "pbes nu X(T: Real) =\n"
    "       (((true && val(true)) && val(1 > T)) && true) && ((false || val(!true)) || val(!(1 > T))) || X(1);\n"
    "\n"
    "init X(0);\n"
  );

  pbes p;
  p = txt2pbes(input);

  std::string output;
  output = pbes_system::pp(p);

  BOOST_CHECK(output.find("Real;") == std::string::npos);

}

BOOST_AUTO_TEST_CASE(pbes_print)
{
  std::string PBES =
    "pbes nu X = true; \n"
    "init X;           \n"
   ;

  pbes p;
  p = txt2pbes(PBES);
  pbes_system::pp(p);
}

BOOST_AUTO_TEST_CASE(pbes_val)
{
  std::string PBES =
    "pbes mu X(d: Pos) = val(d > 0); \n"
    "init X(1);                      \n"
   ;

  std::string expected_result =
    "pbes mu X(d: Pos) =\n"
    "       val(d > 0);\n"
    "\n"
    "init X(1);\n"
    ;

  pbes p;
  p = txt2pbes(PBES);
  std::string result = pbes_system::pp(p);
  BOOST_CHECK(result == expected_result);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return nullptr;
}
