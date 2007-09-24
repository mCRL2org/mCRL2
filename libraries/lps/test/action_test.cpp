// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file action_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "atermpp/atermpp.h"
#include "atermpp/make_list.h"
#include "mcrl2/data/function.h"
#include "mcrl2/lps/action.h"

using namespace atermpp;
using namespace lps;

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  sort X("X");
  sort Y("Y");

  action_label aX(identifier_string("a"),  make_list(X));
  action_label aY(identifier_string("a"),  make_list(Y));
  action_label aXY(identifier_string("a"), make_list(X, Y));
  action_label bX(identifier_string("b"),  make_list(X));

  data_expression x_X = data_variable("x:X");
  data_expression y_X = data_variable("y:X");
  data_expression y_Y = data_variable("y:Y");

  action a1(aX, make_list(x_X));
  action a2(aX, make_list(y_X));
  action a3(bX, make_list(x_X));

  BOOST_CHECK(equal_signatures(a1, a1));
  BOOST_CHECK(equal_signatures(a1, a2));
  BOOST_CHECK(!equal_signatures(a1, a3));

  return 0;
}
