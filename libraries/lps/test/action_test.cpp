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
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/utilities/aterm_ext.h"

using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;

int test_main(int argc, char** argv )
{
  MCRL2_ATERM_INIT(argc, argv)

  sort_expression X("X");
  sort_expression Y("Y");

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

  action empty = action();
  std::cerr << pp(empty) << std::endl;
  std::cerr << pp(action()) << std::endl;

  return 0;
}
