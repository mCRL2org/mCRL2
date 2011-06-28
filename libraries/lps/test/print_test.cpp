// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pp_test.cpp
/// \brief Test for parser + pretty printer

#include <string>
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/atermpp/aterm_init.h"

#include "mcrl2/utilities/test_utilities.h"

using namespace mcrl2;
using namespace mcrl2::lps;

using mcrl2::utilities::collect_after_test_case;
BOOST_GLOBAL_FIXTURE(collect_after_test_case)

BOOST_AUTO_TEST_CASE(rational)
{
  std::string input(
      "% This is an mCRL2 process specification which can be used for testing\n"
      "% operations on rational numbers.\n"
      "\n"
      "% Tests for unary operations floor, ceil, round, succ, pred, -, abs, succ, pred\n"
      "act  unary_operations : Real # Int # Int # Int # Real # Real # Real # Real;\n"
      "proc Test_unary_operations =\n"
      "       sum x: Int, p: Pos, r: Real. (x >= -5 && x <= 5 && p <= 10 && r == x/p)\n"
      "       -> unary_operations(r, floor(r), ceil(r), round(r), abs(r), -r, succ(r), pred(r))\n"
      "       <> delta;\n"
      "\n"
      "% Tests for binary operations <, <=, >=, >, max, min, +, -, *, /\n"
      "act  binary_operations: Real # Real # Bool # Bool # Bool # Bool # Real # Real # Real # Real # Real # Real;\n"
      "     left_operand: Real;\n"
      "proc Test_binary_operations =\n"
      "       sum x: Int, p: Pos, r: Real. (x >= -5 && x <= 5 && p <= 10 && r == x/p) ->\n"
      "         left_operand(r) .\n"
      "         (sum y: Int, q: Pos, s: Real. (y >= -5 && y <= 5 && q <= 10 && s == y/q)\n"
      "         -> binary_operations(r, s, r < s, r <= s, r >= s, r > s, min(r, s), max(r, s), r + s, r - s, r * s, r / s)\n"
      "         <> delta\n"
      "         )\n"
      "       <> delta;\n"
      "\n"
      "% Tests for exponentation\n"
      "act  base: Real;\n"
      "     exponent: Real # Int # Real;\n"
      "proc Test_exponentation =\n"
      "       sum x: Int, p: Pos, r: Real. (x >= -5 && x <= 5 && p <= 10 && r == x/p)\n"
      "       -> (base(r) . (sum y: Int. (y >= -5 && y <= 5) -> exponent(r, y, exp(r, y)) <> delta))\n"
      "       <> delta;\n"
      "\n"
      "init Test_unary_operations + Test_binary_operations + Test_exponentation;\n"
  );

  specification s;
  s = linearise(input);

  std::string output;
  output = lps::pp(s);

  // Check whether the symbol @ occurs in the pretty printed output. If this is
  // the case, still some internal symbol is exposed. As a result, our parsers
  // will not be able to handle the specification as input.
  BOOST_CHECK(output.find("@") == std::string::npos);
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return 0;
}
