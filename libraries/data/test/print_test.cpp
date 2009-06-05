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
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/print.h"

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

void test_abstraction_print() {
  variable_vector x(make_vector(variable("x", sort_nat::nat())));
  variable_vector xy(make_vector(x[0], variable("y", sort_nat::nat())));

  BOOST_CHECK(pp(lambda(x, equal_to(variable("x", sort_nat::nat()), sort_nat::nat(10)))) == "lambda x: Nat. x == 10");
  std::cerr << pp(lambda(x, equal_to(variable("x", sort_nat::nat()), sort_nat::nat(10)))) << std::endl;
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
  test_abstraction_print();
  test_set_print();
  test_bag_print();
  test_structured_sort_print();

  return EXIT_SUCCESS;
}
