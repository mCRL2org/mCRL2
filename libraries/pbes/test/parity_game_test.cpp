// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parity_game_test.cpp
/// \brief Test for parity game solver. N.B. Currently no real
/// testing is done. Instead graph representations are produced
/// in a format that can be read by a python script.

// #define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
#define MCRL2_PARITY_GAME_DEBUG

#include <fstream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/detail/python_parity_game_generator.h"

using namespace mcrl2;

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

void test_bes(std::string bes_spec, std::string output_file, bool expected_result)
{
  pbes_system::pbes<> p = pbes_system::txt2pbes(bes_spec);
  pbes_system::detail::python_parity_game_generator pgg(p);
  std::string text = pgg.run();
  std::ofstream to(output_file.c_str());
  to << text << std::endl;
}

void test_bes_examples()
{
  test_bes(BES1, "bes1.pg", false);
  test_bes(BES2, "bes2.pg", true);
  test_bes(BES3, "bes3.pg", true);
  test_bes(BES4, "bes4.pg", true);
  test_bes(BES5, "bes5.pg", false);
  test_bes(BES6, "bes6.pg", true);
  test_bes(BES7, "bes7.pg", false);
  test_bes(BES8, "bes8.pg", true);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)
  test_bes_examples();

  return 0;
}
