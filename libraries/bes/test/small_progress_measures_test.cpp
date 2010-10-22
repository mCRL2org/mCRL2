// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file small_progress_measures_test.cpp
/// \brief Test for small progress measures algorithm

#include <sstream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/bes/small_progress_measures.h"
#include "mcrl2/bes/bes_parse.h"

using namespace mcrl2;
using namespace mcrl2::bes;

void test_smp()
{
  std::string bes1 =
    "pbes              \n"
    "                  \n"
    "mu X1 = X2 && X1; \n"
    "nu X2 = X1 || X3; \n"
    "mu X3 = X1 || X2; \n"
    "                  \n"
    "init X1;          \n"
    ;

  boolean_equation_system<> b;
  std::stringstream from(bes1);
  from >> b;
  std::cout << "b = \n" << pp(b) << std::endl;
  
  small_progress_measures_algorithm algorithm(b);
  algorithm.run();

  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_smp();

  return 0;
}
