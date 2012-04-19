// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse_test.cpp
/// \brief Add your file description here.

#include <string>
#include <sstream>
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/parse.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

const std::string PBESSPEC =
  "pbes nu X(b: Bool) = exists n: Nat. Y(n) && val(b); \n"
  "     mu Y(n: Nat)  = X(n >= 10);                    \n"
  "                                                    \n"
  "init X(true);                                       \n"
  ;

const std::string VARSPEC =
  "datavar         \n"
  "  n: Nat;       \n"
  "                \n"
  "predvar         \n"
  "  X: Bool, Pos; \n"
  "  Y: Nat;       \n"
  ;

void test_parse()
{
  pbes<> p;
  std::stringstream s(PBESSPEC);
  s >> p;

  pbes_expression x = parse_pbes_expression("X(true, 2) && Y(n+1)", VARSPEC);
  std::cout << "x = " << pbes_system::pp(x) << std::endl;
}

int test_main(int argc, char* argv[])
{
  ATinit();

  test_parse();
  core::garbage_collect();

  return 0;
}
