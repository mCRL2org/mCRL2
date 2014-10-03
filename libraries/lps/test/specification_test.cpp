// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file specification_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <iterator>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/detail/print_utility.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/print.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/test_input.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

void test_find_sort_expressions()
{
  specification spec = linearise(lps::detail::ABP_SPECIFICATION());
  std::set<sort_expression> s;
  lps::find_sort_expressions(spec, std::inserter(s, s.end()));
  std::cout << core::detail::print_set(s) << std::endl;
}

void test_system_defined_sorts()
{
  const std::string SPEC(
    "act a;\n\n"
    "proc X(i,j: Nat)   = (i == 5) -> a. X(i,j);\n\n"
    "init X(0,1);\n");

  specification spec = linearise(SPEC);
  function_symbol_vector r = spec.data().constructors(data::sort_nat::nat());
  BOOST_CHECK(r.size() != 0);
}

void test_empty_distribution()
{
  stochastic_distribution dist;
  std::cout << "empty dist = " << dist << std::endl;
  BOOST_CHECK(!dist.is_defined());
}

// This test is added to demonstrate that context sorts need to be added to
// the data specification of an LPS. If this is not done, the resulting
// LPS is not well typed.
void test_context_sorts()
{
  std::string text =
    "proc P = sum x:Real.(x==x) -> tau.P;\n"
    "init P;\n"
    ;

  specification spec = parse_linear_process_specification(text);
  std::cout << "spec = " << spec << std::endl;
  BOOST_CHECK(is_well_typed(spec));

  stochastic_specification sspec;
  parse_lps(text, sspec);
  std::cout << "sspec = " << sspec << std::endl;
  BOOST_CHECK(is_well_typed(sspec));
}

int test_main(int argc, char* argv[])
{
  test_find_sort_expressions();
  test_system_defined_sorts();
  test_empty_distribution();
  test_context_sorts();

  return 0;
}
