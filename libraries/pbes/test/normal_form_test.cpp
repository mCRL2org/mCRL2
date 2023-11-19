// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file normal_form_test.cpp
/// \brief Tests for transformations into normal form.

#define BOOST_TEST_MODULE normal_form_test
#include "mcrl2/pbes/normal_forms.h"
#include "mcrl2/pbes/parse.h"
#include "mcrl2/pbes/print.h"

#include <boost/test/included/unit_test.hpp>

using namespace mcrl2;
using namespace mcrl2::pbes_system;

void test_standard_recursive_form(const std::string& bes_spec, bool recursive_form = false)
{
  pbes b;
  std::stringstream from(bes_spec);
  from >> b;
  std::cout << "before\n" << pbes_system::pp(b) << std::endl;

  make_standard_form(b, recursive_form);
  std::cout << "after\n" << pbes_system::pp(b) << std::endl;
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

BOOST_AUTO_TEST_CASE(test_main)
{
  test_standard_recursive_form();
}
