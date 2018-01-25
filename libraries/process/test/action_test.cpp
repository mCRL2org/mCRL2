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

#include "mcrl2/core/print.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/process/process_expression.h"
#include <boost/test/minimal.hpp>
#include <iostream>
#include <set>
#include <string>

using namespace mcrl2;

int test_main(int argc, char** argv)
{
  data::sort_expression X(data::basic_sort("X"));
  data::sort_expression Y(data::basic_sort("Y"));

  process::action_label aX(core::identifier_string("a"),  { X });
  process::action_label aY(core::identifier_string("a"),  { Y });
  process::action_label aXY(core::identifier_string("a"), { X, Y });
  process::action_label bX(core::identifier_string("b"),  { X });

  data::data_expression x_X = data::variable("x", X);
  data::data_expression y_X = data::variable("y", X);
  data::data_expression y_Y = data::variable("y", Y);

  process::action a1(aX, { x_X });
  process::action a2(aX, { y_X });
  process::action a3(bX, { x_X });

  BOOST_CHECK(process::equal_signatures(a1, a1));
  BOOST_CHECK(process::equal_signatures(a1, a2));
  BOOST_CHECK(!process::equal_signatures(a1, a3));

  process::action_label_list l;
  l.push_front(aX);
  BOOST_CHECK(l.front()==aX);
  l.push_front(aY);
  l.push_front(aY);
  BOOST_CHECK(l.front()==aY);
  process::action_label_list l1=l;
  BOOST_CHECK(l1.front()==aY);
  BOOST_CHECK(l.front()==aY);
  process::action_label a=l.front();
  BOOST_CHECK(a==aY);

  return 0;
}
