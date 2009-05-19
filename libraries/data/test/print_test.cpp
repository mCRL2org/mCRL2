// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost/test/minimal.hpp"

#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/bag.h"

using namespace mcrl2::data;
using namespace mcrl2::data::sort_bool_;
using namespace mcrl2::data::sort_nat;

void test_function_symbol_print() {
  function_symbol f("f", sort_bool_::bool_());

  BOOST_CHECK(pp(f) == "f");
}

void test_application_print() {
  function_symbol f("f", function_sort(bool_(), bool_()));
  function_symbol g("g", function_sort(bool_(), nat(), bool_()));

  BOOST_CHECK(pp(application(f, true_())) == "f(true)");
  BOOST_CHECK(pp(application(g, false_(), sort_nat::nat(10))) == "g(false, 10)");
}

void test_lambda_print() {
}

void test_set_print() {
}

void test_bag_print() {
}

void test_structured_sort_print() {
}

int test_main(int argc, char** argv) {
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv);

  test_function_symbol_print();
  test_application_print();
  test_lambda_print();
  test_set_print();
  test_bag_print();
  test_structured_sort_print();

  return EXIT_SUCCESS;
}
