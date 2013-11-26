// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file test1.cpp
/// \brief Add your file description here.

#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/pbes_solver_test.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test1()
{
  std::string text =
    "pbes mu X1(n,m: Nat) =                 \n"
    "        X1(m + 1, 0) || val(!(n < 2)); \n"
    "                                       \n"
    "init X1(0, 0);                         \n"
    ;
  pbes p = txt2pbes(text);

  std::cout << "p =\n" << p << "\n" << pbes_to_aterm(p) << std::endl;

  bool result = pbes2_bool_test(p);
}

int test_main(int argc, char** argv)
{
  test1();

  return 0;
}
