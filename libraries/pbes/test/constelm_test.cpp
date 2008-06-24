// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file constelm_test.cpp
/// \brief Add your file description here.

#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/constelm.h"

// example 4.2.1 from "tools for PBES" report
std::string EXAMPLE4_2_1 =
  "pbes                                    \n"
  "   mu X1(n1,m1:Nat) = X2(n1) || X2(m1); \n"
  "   mu X2(n2:Nat)     = X1(n2,n2);       \n"
  "init                                    \n"
  "   X1(0,1);                             \n"
;

void test_pbes(const std::string& pbes_spec)
{
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  //test_pbes(EXAMPLE4_2_1);

  return 0;
}
