// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_utility_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <algorithm>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/core/find.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"

using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  data_variable d("d:D");
  data_variable d0("d0:D");
  data_variable d00("d00:D");
  data_expression e = and_(equal_to(d, d0), not_equal_to(d0, d00));

  fresh_variable_generator generator(e, d.sort(), "d");
  data_variable x = generator();
  BOOST_CHECK(x == data_variable("d01:D"));
  x = generator();
  BOOST_CHECK(x == data_variable("d02:D"));

  data_variable a = fresh_variable(e, sort_expression("D"), "d");
  BOOST_CHECK(a == data_variable("d01:D"));

  std::set<identifier_string> ids = find_identifiers(e);
  BOOST_CHECK(ids.size() == 8);
  BOOST_CHECK(ids.find(identifier_string("d"))    != ids.end());
  BOOST_CHECK(ids.find(identifier_string("d0"))   != ids.end());
  BOOST_CHECK(ids.find(identifier_string("d00"))  != ids.end());
  BOOST_CHECK(ids.find(identifier_string("D"))    != ids.end());
  BOOST_CHECK(ids.find(identifier_string("Bool")) != ids.end());
  BOOST_CHECK(ids.find(identifier_string("=="))   != ids.end());
  BOOST_CHECK(ids.find(identifier_string("!="))   != ids.end());
  BOOST_CHECK(ids.find(identifier_string("&&"))   != ids.end());

  data_variable f("f:F");
  x = generator(f);
  BOOST_CHECK(x == data_variable("f:F"));
  x = generator(f);
  BOOST_CHECK(x == data_variable("f00:F"));

  data_variable_list w = make_list(data_variable("d:D"), data_variable("e:E"), data_variable("f:F"));
  std::set<std::string> context;
  context.insert("e");
  context.insert("f_00");
  data_variable_list w1 = fresh_variables(w, context);
  std::cout << "w1 = " << w1 << std::endl;
  BOOST_CHECK(std::find(w1.begin(), w1.end(), data_variable("d_01:D")) != w1.end());
  BOOST_CHECK(std::find(w1.begin(), w1.end(), data_variable("e_01:E")) != w1.end());
  BOOST_CHECK(std::find(w1.begin(), w1.end(), data_variable("f_01:F")) != w1.end());

  bool bb = (d < d0);

  return 0;
}
