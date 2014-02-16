// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gauss_elimination_test.cpp
/// \brief Gauss elimination tests.

#define MCRL2_GAUSS_ELIMINATION_DEBUG

#include <boost/test/minimal.hpp>
#include "mcrl2/bes/gauss_elimination.h"
#include "mcrl2/pbes/pbesinstconversion.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/is_bes.h"

std::string BES1 =
  "pbes mu X = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES2 =
  "pbes nu X = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES3 =
  "pbes mu X = Y;                                           \n"
  "     nu Y = X;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES4 =
  "pbes nu Y = X;                                           \n"
  "     mu X = Y;                                           \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES5 =
  "pbes mu X1 = X2;                                         \n"
  "     nu X2 = X1 || X3;                                   \n"
  "     mu X3 = X4 && X5;                                   \n"
  "     nu X4 = X1;                                         \n"
  "     nu X5 = X1 || X3;                                   \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES6 =
  "pbes nu X1 = X2 && X1;                                   \n"
  "     mu X2 = X1 || X3;                                   \n"
  "     nu X3 = X3;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES7 =
  "pbes nu X1 = X2 && X3;                                   \n"
  "     nu X2 = X4 && X5;                                   \n"
  "     nu X3 = true;                                       \n"
  "     nu X4 = false;                                      \n"
  "     nu X5 = X6;                                         \n"
  "     nu X6 = X5;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES8 =
  "pbes nu X1 = X2 && X1;                                   \n"
  "     mu X2 = X1;                                         \n"
  "                                                         \n"
  "init X1;                                                 \n"
  ;

std::string BES9 =
  "pbes mu X = false;                                       \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

std::string BES10 =
  "pbes nu X = false;                                       \n"
  "                                                         \n"
  "init X;                                                  \n"
  ;

void test_bes(std::string bes_spec, bool expected_result)
{
  mcrl2::pbes_system::pbes p = mcrl2::pbes_system:: txt2pbes(bes_spec);
  BOOST_CHECK(mcrl2::pbes_system::is_bes(p));
  mcrl2::bes::boolean_equation_system b = mcrl2::pbes_system::pbesinstconversion(p);
  int result = mcrl2::bes::gauss_elimination(b);
  switch (result)
  {
    case 0:
      std::cout << "false" << std::endl;
      break;
    case 1:
      std::cout << "true" << std::endl;
      break;
    case 2:
      std::cout << "unknown" << std::endl;
      break;
  }
  BOOST_CHECK((expected_result == false && result == 0) || (expected_result == true && result == 1));
}

void test_bes_examples()
{
  test_bes(BES1, false);
  test_bes(BES2, true);
  test_bes(BES3, false);
  test_bes(BES4, true);
  test_bes(BES5, false);
  test_bes(BES6, true);
  test_bes(BES7, false);
  test_bes(BES8, true);
  test_bes(BES9, false);
  test_bes(BES10, false);
}

int test_main(int argc, char** argv)
{
  test_bes_examples();
  return 0;
}
