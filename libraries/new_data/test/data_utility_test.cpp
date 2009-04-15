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
#include <boost/bind.hpp>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/core/find.h"
#include "mcrl2/new_data/data.h"
#include "mcrl2/new_data/find.h"
#include "mcrl2/new_data/utility.h"
#include "mcrl2/new_data/standard_utility.h"
#include "mcrl2/new_data/data_expression_utility.h"

using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::new_data;
using namespace mcrl2::new_data::sort_bool_;

void test_fresh_variable_generator()
{
  variable d("d", basic_sort("D"));
  variable d0("d0", basic_sort("D"));
  variable d00("d00", basic_sort("D"));
  data_expression e = and_(equal_to(d, d0), not_equal_to(d0, d00));

  fresh_variable_generator generator(e, d.sort(), "d");
  variable x = generator();
  BOOST_CHECK(x == variable("d01", basic_sort("D")));
  x = generator();
  BOOST_CHECK(x == variable("d02", basic_sort("D")));

  variable a = fresh_variable(e, basic_sort("D"), "d");
  BOOST_CHECK(a == variable("d01", basic_sort("D")));

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

  variable f("f", basic_sort("F"));
  x = generator(f);
  BOOST_CHECK(x == variable("f", basic_sort("F")));
  x = generator(f);
  BOOST_CHECK(x == variable("f00", basic_sort("F")));

  atermpp::vector<data_expression> v;
  variable p("p", basic_sort("P"));
  variable q("q", basic_sort("P"));
  v.push_back(p);
  v.push_back(q);
  std::for_each(v.begin(), v.end(), boost::bind(&fresh_variable_generator::add_to_context<data_expression>, boost::ref(generator), _1));
  x = generator(p);
  BOOST_CHECK(x == variable("p00", basic_sort("P")));
  x = generator(q);
  BOOST_CHECK(x == variable("q00", basic_sort("P")));
}

void test_fresh_variables()
{
  variable_vector w = make_vector(variable("d", basic_sort("D")), variable("e", basic_sort("E")), variable("f", basic_sort("F")));
  std::set<std::string> context;
  context.insert("e");
  context.insert("f_00");
  variable_vector w1 = fresh_variables(w, context);
  std::cout << "w1 = " << mcrl2::new_data::pp(w1) << std::endl;
  BOOST_CHECK(std::find(w1.begin(), w1.end(), variable("d_01", basic_sort("D"))) != w1.end());
  BOOST_CHECK(std::find(w1.begin(), w1.end(), variable("e_01", basic_sort("E"))) != w1.end());
  BOOST_CHECK(std::find(w1.begin(), w1.end(), variable("f_01", basic_sort("F"))) != w1.end());
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_fresh_variable_generator();
  test_fresh_variables();

  return 0;
}
