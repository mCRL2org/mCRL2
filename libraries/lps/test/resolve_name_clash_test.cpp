// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file resolve_name_clash_test.cpp
/// \brief Add your file description here.

#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/resolve_name_clashes.h"
#include <boost/test/minimal.hpp>
#include <iostream>
#include <set>
#include <string>

using namespace mcrl2;

std::set<data::variable> find_summand_variables(const lps::specification& spec)
{
  std::set<data::variable> result;

  const lps::linear_process& proc = spec.process();
  auto action_summands = proc.action_summands();
  for (auto & action_summand : action_summands)
  {
    auto variables = action_summand.summation_variables();
    result.insert(variables.begin(), variables.end());
  }
  auto deadlock_summands = proc.deadlock_summands();
  for (auto & deadlock_summand : deadlock_summands)
  {
    auto variables = deadlock_summand.summation_variables();
    result.insert(variables.begin(), variables.end());
  }

  return result;
}

void test_resolve_name_clashes()
{
  std::string SPEC =
    "act  a;                                 \n"
    "                                        \n"
    "proc P(b: Bool, m: Nat, n: Pos) =       \n"
    "       sum m: Bool.                     \n"
    "         m ->                           \n"
    "         a .                            \n"
    "         P(b = m, m = 1, n = 2)         \n"
    "     + sum n: Nat.                      \n"
    "         a .                            \n"
    "         P(b = true, m = n + 1, n = 3)  \n"
    "     + delta;                           \n"
    "                                        \n"
    "init P(true, 4, 5);                     \n"
    ;
  lps::specification spec = lps::parse_linear_process_specification(SPEC);
  lps::resolve_summand_variable_name_clashes(spec);
  std::set<data::variable> summation_variables = find_summand_variables(spec);
  auto process_parameters = spec.process().process_parameters();
  for (const auto & process_parameter : process_parameters)
  {
    BOOST_CHECK(summation_variables.find(process_parameter) == summation_variables.end());
  }
}

int test_main(int argc, char* argv[])
{
  test_resolve_name_clashes();

  return 0;
}
