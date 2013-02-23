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

using namespace mcrl2;
using namespace mcrl2::pbes_system;

const std::string PBESSPEC1 =
  "pbes nu X(b: Bool) = exists n: Nat. Y(n) && val(b); \n"
  "     mu Y(n: Nat)  = X(n >= 10);                    \n"
  "                                                    \n"
  "init X(true);                                       \n"
  ;

const std::string PBESSPEC2 =
  "sort DATA = struct d1 | d2;                         \n"
  "     Enum3 = struct e2_3 | e1_3 | e0_3;             \n"
  "     Frame = struct frame(getd: DATA, getb: DATA);  \n"
  "                                                    \n"
  "glob dc: Frame;                                     \n"
  "                                                    \n"
  "pbes nu X(s30_K: Pos, f_K: Frame) =                 \n"
  "       X(1, f_K);                                   \n"
  "                                                    \n"
  "init X(1, dc);                                      \n"
  ;

void test_txt2pbes()
{
  pbes<> p;
  p = txt2pbes(PBESSPEC1);
  BOOST_CHECK(p.is_well_typed());
  p = txt2pbes(PBESSPEC2);
  BOOST_CHECK(p.is_well_typed());
}


int test_main(int argc, char* argv[])
{
  test_txt2pbes();

  return 0;
}
