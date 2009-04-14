// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Add your file description here.

#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2::pbes_system;

const std::string PBESSPEC =
  "pbes nu X(b: Bool) = exists n: Nat. Y(n) && val(b); \n"
  "     mu Y(n: Nat)  = X(n >= 10);                    \n"
  "                                                    \n"
  "init X(true);                                       \n"
  ;

void test_txt2pbes()
{
  pbes<> p = txt2pbes(PBESSPEC);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_txt2pbes();

  return 0;
}
