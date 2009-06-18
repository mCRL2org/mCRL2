// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file summand_test.cpp
/// \brief Simple test for summands

#include <iostream>
#include <string>
#include <set>
#include <iterator>
#include <boost/test/minimal.hpp>
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/lps/summand.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

void test_assignments()
{
  variable x("x", sort_pos::pos());
  variable y("y", sort_pos::pos());
  assignment xy(x,y);
  assignment_list l(make_list(xy));

  multi_action a(action(action_label(core::identifier_string("a"), sort_expression_list()), data_expression_list()));

  summand s(variable_list(), data_expression(sort_bool::true_()), a, l);

  multi_action s_a = s.multi_action();
  assignment_list s_l = s.assignments();

  BOOST_CHECK(s.multi_action() == a);
  BOOST_CHECK(s_a == a);
  BOOST_CHECK(s.assignments() == l);
  BOOST_CHECK(s_l == l);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_assignments();
  core::garbage_collect();

  return 0;
}

