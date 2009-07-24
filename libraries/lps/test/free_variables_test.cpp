// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file free_variables_test.cpp

#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/find.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::lps;

void test_free_variables()
{
  lps::specification specification(parse_linear_process_specification(
    "act a : Bool;\n"
    "proc X = a((forall x : Nat. exists y : Nat. x < y)).X;\n"
    "init X;\n"
  ));

  std::set<data::variable> free_variables = find_free_variables(specification.process());

  BOOST_CHECK(free_variables.find(data::variable("x", data::sort_nat::nat())) == free_variables.end());
  BOOST_CHECK(free_variables.find(data::variable("y", data::sort_nat::nat())) == free_variables.end());

  specification = parse_linear_process_specification(
    "act a;\n"
    "proc X(z : Bool) = (z && forall x : Nat. exists y : Nat. x < y) -> a.X(!z);\n"
    "init X(true);\n"
  );

  free_variables = find_free_variables(specification.process());

  BOOST_CHECK(free_variables.find(data::variable("x", data::sort_nat::nat())) == free_variables.end());
  BOOST_CHECK(free_variables.find(data::variable("y", data::sort_nat::nat())) == free_variables.end());

  BOOST_CHECK(is_well_typed(specification));
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_free_variables();
  core::garbage_collect();

  return 0;
}
