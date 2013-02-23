// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/print.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;

int test_main(int argc, char** argv)
{
  using atermpp::make_vector;

  sort_expression X(basic_sort("X"));
  sort_expression Y(basic_sort("Y"));

  action_label aX(identifier_string("a"),  make_list(X));
  action_label aY(identifier_string("a"),  make_list(Y));
  action_label aXY(identifier_string("a"), make_list(X, Y));
  action_label bX(identifier_string("b"),  make_list(X));

  data_expression x_X = variable("x", X);
  data_expression y_X = variable("y", X);
  data_expression y_Y = variable("y", Y);

  action a1(aX, make_list(x_X));
  action a2(aX, make_list(y_X));
  action a3(bX, make_list(x_X));

  BOOST_CHECK(equal_signatures(a1, a1));
  BOOST_CHECK(equal_signatures(a1, a2));
  BOOST_CHECK(!equal_signatures(a1, a3));

  /* action empty = action();
  std::cerr << lps::pp(empty) << std::endl;
  std::cerr << lps::pp(action()) << std::endl; */


  // Test whether the front of an action_label_list is working properly.
  
  action_label_list l;
  l.push_front(aX);
  BOOST_CHECK(l.front()==aX);
  l.push_front(aY);
  l.push_front(aY);
  BOOST_CHECK(l.front()==aY);
  action_label_list l1=l;
  BOOST_CHECK(l1.front()==aY);
  BOOST_CHECK(l.front()==aY);
  action_label a=l.front();
  BOOST_CHECK(a==aY);


  return 0;
}
