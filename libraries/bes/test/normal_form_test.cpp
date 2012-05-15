// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file normal_form_test.cpp
/// \brief Tests for transformations into normal form.

#include <sstream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/normal_forms.h"
#include "mcrl2/bes/parse.h"
#include "mcrl2/bes/print.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::bes;

void test_standard_recursive_form(const std::string& bes_spec, bool recursive_form = false)
{
  boolean_equation_system<> b;
  std::stringstream from(bes_spec);
  from >> b;
  std::cout << "before\n" << bes::pp(b) << std::endl;

  make_standard_form(b, recursive_form);
  std::cout << "after\n" << bes::pp(b) << std::endl;
}

void test_standard_recursive_form()
{
  std::string bes1 =
    "pbes              \n"
    "                  \n"
    "nu X1 = X2 && X1; \n"
    "mu X2 = X1 || X2; \n"
    "                  \n"
    "init X1;          \n"
    ;
  test_standard_recursive_form(bes1, false);
  test_standard_recursive_form(bes1, true);

  std::string bes2 =
    "pbes                    \n"
    "                        \n"
    "nu X1 = X2 && true;     \n"
    "mu X2 = X1 || X2 && X1; \n"
    "                        \n"
    "init X1;                \n"
    ;

  test_standard_recursive_form(bes2, false);
  test_standard_recursive_form(bes2, true);

}

int test_main(int argc, char* argv[])
{
  atermpp::aterm_init();

  test_standard_recursive_form();

  return 0;
}
