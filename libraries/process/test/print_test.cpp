// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print_test.cpp
/// \brief Regression test for parsing process expressions

#define MCRL2_DEBUG_EXPRESSION_BUILDER

#include <iostream>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/print.h"

using namespace mcrl2;
using namespace mcrl2::process;

void test_comm()
{
  std::string text =
    "act a;\n"
    "init comm({a|a}, a);\n"
    ;
  process_specification p = parse_process_specification(text);
  std::string text1 = process::pp(p);
  BOOST_CHECK(text == text1);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  void test_comm();

  return EXIT_SUCCESS;
}
