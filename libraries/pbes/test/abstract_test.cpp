// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file abstract_test.cpp
/// \brief Test the pbes abstract algorithm.

#include <string>
#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/pbes/abstract.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/detail/pbes2bes_variable_map_parser.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test_pbesabstract(const std::string& pbes_spec, const std::string& variable_spec)
{
  pbes<> p = txt2pbes(pbes_spec);
  pbes2bes_variable_map variable_map = pbes_system::detail::parse_variable_map(p, variable_spec);
  pbes_abstract_algorithm algorithm;
  algorithm.run(p, variable_map);
  std::cout << "\n-------------------------------\n" << pp(p) << std::endl;
}

void test_pbesabstract()
{
  test_pbesabstract(
    "pbes nu X(a: Bool, b: Nat) =  \n"
    "       val(a) || X(a, b + 1); \n"
    "                              \n"
    "init X(true, 0);              \n"
    ,
    "X(b:Nat)"
  );

  test_pbesabstract(
    "pbes nu X1(b:Bool) = exists b:Bool.(X2 || val(b)); \n"
    "     mu X2 = X2;                                   \n"
    "                                                   \n"
    "init X1(true);                                     \n"
    ,
    "X1(b:Bool)"
  );

  test_pbesabstract(
    "pbes nu X1(b:Bool) = X2 || val(b); \n"
    "     mu X2 = X2;                   \n"
    "                                   \n"
    "init X1(true);                     \n"
    ,
    "X1(b:Bool)"
  );
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_pbesabstract();

  return 0;
}
