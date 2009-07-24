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
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/find.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/fresh_variable_generator.h"
#include "mcrl2/core/garbage_collection.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::data::sort_bool;

void test_fresh_variable_generator()
{
  variable d("d", basic_sort("D"));
  variable d0("d0", basic_sort("D"));
  variable d00("d00", basic_sort("D"));
  data_expression e = and_(equal_to(d, d0), not_equal_to(d0, d00));

  fresh_variable_generator< > generator(e, "d");
  variable x = generator(d.sort());
  BOOST_CHECK(x == variable("d1", basic_sort("D")));
  x = generator(d.sort());
  BOOST_CHECK(x == variable("d2", basic_sort("D")));

  variable a = fresh_variable(e, basic_sort("D"), "d");
  BOOST_CHECK(a == variable("d1", basic_sort("D")));

  std::set<identifier_string> ids = data::find_identifiers(e);

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
  BOOST_CHECK(x == variable("f1", basic_sort("F")));

  atermpp::vector<data_expression> v;
  variable p("p", basic_sort("P"));
  variable q("q", basic_sort("P"));
  v.push_back(p);
  v.push_back(q);
  generator.add_to_context(boost::make_iterator_range(v));
  x = generator(p);
  BOOST_CHECK(x == variable("p1", basic_sort("P")));
  x = generator(q);
  BOOST_CHECK(x == variable("q1", basic_sort("P")));
}

void test_fresh_variables()
{
  variable_vector w = make_vector(variable("d", basic_sort("D")), variable("e", basic_sort("E")), variable("f", basic_sort("F")));
  std::set<std::string> context;
  context.insert("e");
  context.insert("f_00");
  variable_vector w1 = convert< variable_vector >(fresh_variables(w, context));
  std::cout << "w1 = " << mcrl2::data::pp(w1) << std::endl;
  BOOST_CHECK(std::find(w1.begin(), w1.end(), variable("d1", basic_sort("D"))) != w1.end());
  BOOST_CHECK(std::find(w1.begin(), w1.end(), variable("e1", basic_sort("E"))) != w1.end());
  BOOST_CHECK(std::find(w1.begin(), w1.end(), variable("f1", basic_sort("F"))) != w1.end());
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_fresh_variable_generator();
  core::garbage_collect();

  test_fresh_variables();
  core::garbage_collect();

  return 0;
}
