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
#include "mcrl2/lps/detail/lps_parameter_remover.h"
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
  lps::detail::lps_parameter_remover<std::set<data::variable> > remover(to_be_removed);

  data::variable_list v;
  remover.remove_list(v);
  remover(spec);

  // check the result
  std::string expected_result = "process_parameter_names =";
  lps::detail::specification_property_map info(spec);
  BOOST_CHECK(data::detail::compare_property_maps("test_remove_parameters", info, expected_result));

  specification spec2 = parse_linear_process_specification(SPEC);
  remove_parameters(spec2, to_be_removed);
  lps::detail::specification_property_map info2(spec);
  BOOST_CHECK(data::detail::compare_property_maps("test_remove_parameters", info2, expected_result));
}

void test_instantiate_free_variables()
{
  specification spec = linearise(lps::detail::ABP_SPECIFICATION());
  lps::detail::lps_algorithm algorithm(spec);
  algorithm.instantiate_free_variables();
}

int test_main(int argc, char* argv[])
{
  test_remove_parameters();
  test_instantiate_free_variables();

  return 0;
}
