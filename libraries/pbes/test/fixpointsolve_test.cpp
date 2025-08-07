// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file iteration_test.cpp
/// \brief Add your file description here.

#include "mcrl2/pbes/tools/pbesfixpointsolve.h"
#define BOOST_TEST_MODULE iteration_test
#include <boost/regex.hpp>
#include <boost/test/included/unit_test.hpp>
#include <regex>

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/tools/pbesfixpointsolve.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace pbes_system;

std::string replace_whitespace(std::string str)
{
  return std::regex_replace(
      std::string(boost::algorithm::replace_all_copy(boost::algorithm::replace_all_copy(str, "\r\n", " "), "\n", " ")),
      std::regex("\\s+"),
      " ");
}

void test_result(std::string input, std::string expected_result, bool option_check_global_invariant)
{
  bool normalize = false;
  pbes p = txt2pbes(input, normalize);
  pbesfixpointsolve_pbes_fixpoint_iterator fixpoint_iterator;
  pbesfixpointsolve_options options;
  options.check_global_invariant = option_check_global_invariant;
  fixpoint_iterator.run(p, options);
  std::string result = pbes_system::pp(p);
  std::cout << replace_whitespace(result) << std::endl;
  std::cout << replace_whitespace(expected_result) << std::endl;
  BOOST_CHECK(replace_whitespace(result) == replace_whitespace(expected_result));
}

BOOST_AUTO_TEST_CASE(test_fixpointsolve)
{
  std::string text;
  std::string expected_result;
  text = "pbes nu X(n: Pos, b: Bool) = (val(!(n == 2)) && (X(2, b)) && (X(3, b)));"
         "init X(1, false);";
  expected_result = "pbes nu X(n: Pos, b: Bool) =\n"
                    "     val(!(n == 2)) && val(false) && val(true);\n"
                    "init X(1, false);\n";

  test_result(text, expected_result, false);
  test_result(text, expected_result, true);

  text = "sort Colour = struct red | green;"
         "Sluice = struct s1 | s2;"
         "map  init_colour: Sluice -> Colour;"
         ""
         "var  s: Sluice;"
         "eqn  init_colour(s)  =  red;"
         ""
         "glob dc: Sluice;"
         ""
         "pbes nu Z(s3_P: Pos, s_P: Sluice, col_P1,col_P2: Colour) ="
         "(val(!(s_P == s1)) || val(!(s3_P == 2)) || X0(1, s1, red, col_P2)) &&"
         "(val(!(s3_P == 3)) || Z(1, dc, if(s_P == s1, green, col_P1), if(s_P == s2, green, col_P2))) &&"
         "(val(!(s3_P == 2)) || Z(1, dc, if(s_P == s1, red, col_P1), if(s_P == s2, red, col_P2))) &&"
         "(val(!(s3_P == 1 && !(col_P1 == green))) || Z(1, dc, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && col_P1 == green)) || Z(2, s1, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && !(col_P2 == green))) || Z(1, dc, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && col_P2 == green)) || Z(2, s2, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && !(col_P1 == red))) || Z(1, dc, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && col_P1 == red)) || Z(3, s1, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && !(col_P2 == red))) || Z(1, dc, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && col_P2 == red)) || Z(3, s2, col_P1, col_P2)); "
         ""
         "nu X0(s3_P: Pos, s_P: Sluice, col_P1,col_P2: Colour) ="
         "(val(!(s_P == s1)) || val(!(s3_P == 2))) &&"
         "(val(s_P == s1) || val(!(s3_P == 3)) || X0(1, s1, col_P1, green)) &&"
         "(val(!(s3_P == 2)) || X0(1, s1, if(s_P == s1, red, col_P1), if(s_P == s2, red, col_P2))) &&"
         "(val(!(s3_P == 1 && !(col_P1 == green))) || X0(1, s1, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && col_P1 == green)) || X0(2, s1, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && !(col_P2 == green))) || X0(1, s1, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && col_P2 == green)) || X0(2, s2, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && !(col_P1 == red))) || X0(1, s1, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && col_P1 == red)) || X0(3, s1, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && !(col_P2 == red))) || X0(1, s1, col_P1, col_P2)) &&"
         "(val(!(s3_P == 1 && col_P2 == red)) || X0(3, s2, col_P1, col_P2));"
         ""
         "init Z(1, dc, red, red);";

  expected_result = "sort Colour = struct red | green;\n"
                    "Sluice = struct s1 | s2;\n"
                    "\n"
                    "map  init_colour: Sluice -> Colour;\n"
                    "\n"
                    "var  s: Sluice;\n"
                    "eqn  init_colour(s)  =  red;\n"
                    "\n"
                    "glob dc: Sluice;\n"
                    "\n"
                    "pbes nu Z(s3_P: Pos, s_P: Sluice, col_P1,col_P2: Colour) =\n"
                    "val(true);\n"
                    "nu X0(s3_P: Pos, s_P: Sluice, col_P1,col_P2: Colour) =\n"
                    "(val(!(s_P == s1)) || val(!(s3_P == 2))) && (val(s_P == s1) || val(!(s3_P == 3)) || val(!(col_P1 == green))) && (val(!(s3_P == 2)) || val(!(if(s_P == s1, red, col_P1) == green))) && (val(!(s3_P == 1 && !(col_P1 == green))) || val(!(col_P1 == green))) && (val(!(s3_P == 1 && col_P1 == green)) || val(false)) && (val(!(s3_P == 1 && !(col_P2 == green))) || val(!(col_P1 == green))) && (val(!(s3_P == 1 && col_P2 == green)) || val(true)) && (val(!(s3_P == 1 && !(col_P1 == red))) || val(!(col_P1 == green))) && (val(!(s3_P == 1 && col_P1 == red)) || val(true)) && (val(!(s3_P == 1 && !(col_P2 == red))) || val(!(col_P1 == green))) && (val(!(s3_P == 1 && col_P2 == red)) || val(true));\n"
                    "\n"
                    "init Z(1, dc, red, red);\n";
  test_result(text, expected_result, false);
  test_result(text, expected_result, true);
}
