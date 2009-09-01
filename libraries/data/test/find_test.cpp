// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file find_test.cpp
/// \brief Test for find functions.

#include <algorithm>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::data;

inline
variable nat(std::string name)
{
  return variable(identifier_string(name), sort_nat::nat());
}

inline
variable pos(std::string name)
{
  return variable(identifier_string(name), sort_pos::pos());
}

inline
variable bool_(std::string name)
{
  return variable(identifier_string(name), sort_bool::bool_());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  variable n1 = nat("n1");
  variable n2 = nat("n2");
  variable n3 = nat("n3");
  variable n4 = nat("n4");

  variable b1 = bool_("b1");
  variable b2 = bool_("b2");
  variable b3 = bool_("b3");
  variable b4 = bool_("b4");

  variable p1 = pos("p1");
  variable p2 = pos("p2");
  variable p3 = pos("p3");
  variable p4 = pos("p4");

  sort_expression_vector domain = make_vector< sort_expression >(sort_pos::pos(), sort_bool::bool_());
  sort_expression sexpr = function_sort(domain, sort_nat::nat());
std::cout << "<sexpr>" << sexpr << std::endl;
  variable q1(identifier_string("q1"), sexpr);

  data_expression x = sort_bool::and_(equal_to(n1, n2), not_equal_to(n2, n3));
  data_expression y = sort_bool::or_(equal_to(p1, p2), sort_bool::and_(x, b2));

  BOOST_CHECK( search_variable(x, n1));
  BOOST_CHECK( search_variable(x, n2));
  BOOST_CHECK( search_variable(x, n3));
  BOOST_CHECK(!search_variable(x, n4));

  core::garbage_collect();

  std::set<variable> v = find_variables(x);
  BOOST_CHECK(std::find(v.begin(), v.end(), n1) != v.end());
  BOOST_CHECK(std::find(v.begin(), v.end(), n2) != v.end());
  BOOST_CHECK(std::find(v.begin(), v.end(), n3) != v.end());

  BOOST_CHECK( search_basic_sort(y, sort_nat::nat()  ));
  BOOST_CHECK( search_basic_sort(y, sort_pos::pos()  ));
  BOOST_CHECK( search_basic_sort(y, sort_bool::bool_()));
  BOOST_CHECK(!search_basic_sort(y, sort_real::real_() ));

  core::garbage_collect();

  std::set<basic_sort> s = find_basic_sorts(y);
  BOOST_CHECK(std::find(s.begin(), s.end(), sort_nat::nat()) != s.end());
  BOOST_CHECK(std::find(s.begin(), s.end(), sort_pos::pos()) != s.end());
  BOOST_CHECK(std::find(s.begin(), s.end(), sort_bool::bool_()) != s.end());

  core::garbage_collect();

  std::set<sort_expression> e = find_sort_expressions(q1);
  BOOST_CHECK(std::find(e.begin(), e.end(), sort_nat::nat())   != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), sort_pos::pos())   != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), sort_bool::bool_()) != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), sexpr)              != e.end());

  core::garbage_collect();

  std::set<data_expression> d = find_data_expressions(make_vector(q1, p1, n1));
  BOOST_CHECK(std::find(d.begin(), d.end(), q1) != d.end());
  BOOST_CHECK(std::find(d.begin(), d.end(), p1) != d.end());
  BOOST_CHECK(std::find(d.begin(), d.end(), n1) != d.end());

  core::garbage_collect();

  BOOST_CHECK(search_basic_sort(data_specification(), sort_bool::bool_()));
  BOOST_CHECK(search_sort_expression(data_specification(), sort_bool::bool_()));
  BOOST_CHECK(search_data_expression(data_specification(), sort_bool::true_()));
  BOOST_CHECK(!search_variable(data_specification(), variable("a", sort_bool::bool_())));

  return 0;
}
