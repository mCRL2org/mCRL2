// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file remove_parameters_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/lps/detail/test_input.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

std::string SPEC =
  "act  action: Nat;         \n"
  "                          \n"
  "proc P(s: Pos, i: Nat) =  \n"
  "       (s == 2) ->        \n"
  "         action(3) .      \n"
  "         P(1, 4)          \n"
  "     + (s == 1) ->        \n"
  "         action(4) .      \n"
  "         P(2, 4)          \n"
  "     + true ->            \n"
  "         delta;           \n"
  "                          \n"
  "init P(1, 0);             \n"
  ;

void test_remove_parameters()
{
  specification spec = parse_linear_process_specification(SPEC);

  // remove parameters
  std::set<data::variable> to_be_removed;
  to_be_removed.insert(variable("s", sort_pos::pos()));
  to_be_removed.insert(variable("i", sort_nat::nat()));

  // check the result
  std::string expected_result = "process_parameter_names =";
  lps::detail::specification_property_map<> info(spec);

  specification spec2 = parse_linear_process_specification(SPEC);
  remove_parameters(spec2, to_be_removed);
  lps::detail::specification_property_map<> info2(spec2);
  BOOST_CHECK(data::detail::compare_property_maps("test_remove_parameters", info2, expected_result));
}

void test_instantiate_free_variables()
{
  specification spec = remove_stochastic_operators(linearise(lps::detail::ABP_SPECIFICATION()));
  lps::detail::lps_algorithm<> algorithm(spec);
  algorithm.instantiate_free_variables();
}

void test_remove_rundant_assignments()
{
  std::string lpsspec_text =
    "act  a;\n"
    "proc P(x: Nat) = a.P(x = x);\n"
    "init P(0);"
  ;
  std::string expected_result_text =
    "act  a;\n"
    "proc P(x: Nat) = a.P();\n"
    "init P(0);"
  ;

  specification result = parse_linear_process_specification(lpsspec_text);
  remove_redundant_assignments(result);
  specification expected_result = parse_linear_process_specification(expected_result_text);
  BOOST_CHECK(result == expected_result);

  lpsspec_text =
    "act  a;\n"
    "proc P(x: Nat) = sum x: Nat. a.P(x = x);\n"
    "init P(0);"
  ;
  expected_result_text =
    "act  a;\n"
    "proc P(x: Nat) = sum x: Nat. a.P(x = x);\n"
    "init P(0);"
  ;
  result = parse_linear_process_specification(lpsspec_text);
  remove_redundant_assignments(result);
  expected_result = parse_linear_process_specification(expected_result_text);
  BOOST_CHECK(result == expected_result);
}

int test_main(int argc, char* argv[])
{
  test_remove_parameters();
  test_instantiate_free_variables();
  test_remove_rundant_assignments();

  return 0;
}
