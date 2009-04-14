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
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/sort_arrow.h"

using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::data_expr;

inline
data_variable nat(std::string name)
{
  return data_variable(identifier_string(name), sort_expr::nat());
}

inline
data_variable pos(std::string name)
{
  return data_variable(identifier_string(name), sort_expr::pos());
}

inline
data_variable bool_(std::string name)
{
  return data_variable(identifier_string(name), sort_expr::bool_());
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  data_variable n1 = nat("n1");
  data_variable n2 = nat("n2");
  data_variable n3 = nat("n3");
  data_variable n4 = nat("n4");

  data_variable b1 = bool_("b1");
  data_variable b2 = bool_("b2");
  data_variable b3 = bool_("b3");
  data_variable b4 = bool_("b4");

  data_variable p1 = pos("p1");
  data_variable p2 = pos("p2");
  data_variable p3 = pos("p3");
  data_variable p4 = pos("p4");

  sort_expression sexpr = sort_arrow(make_list(sort_expr::pos(), sort_expr::bool_()), sort_expr::nat());
std::cout << "<sexpr>" << sexpr << std::endl;
  data_variable q1(identifier_string("q1"), sexpr);

  data_expression x = and_(equal_to(n1, n2), not_equal_to(n2, n3));
  data_expression y = or_(equal_to(p1, p2), and_(x, b2));

  BOOST_CHECK( find_data_variable(x, n1));
  BOOST_CHECK( find_data_variable(x, n2));
  BOOST_CHECK( find_data_variable(x, n3));
  BOOST_CHECK(!find_data_variable(x, n4));

  std::set<data_variable> v = find_all_data_variables(x);
  BOOST_CHECK(std::find(v.begin(), v.end(), n1) != v.end());
  BOOST_CHECK(std::find(v.begin(), v.end(), n2) != v.end());
  BOOST_CHECK(std::find(v.begin(), v.end(), n3) != v.end());

  BOOST_CHECK( find_sort_identifier(y, sort_expr::nat()  ));
  BOOST_CHECK( find_sort_identifier(y, sort_expr::pos()  ));
  BOOST_CHECK( find_sort_identifier(y, sort_expr::bool_()));
  BOOST_CHECK(!find_sort_identifier(y, sort_expr::real() ));

  std::set<sort_identifier> s = find_all_sort_identifiers(y);
  BOOST_CHECK(std::find(s.begin(), s.end(), sort_expr::nat()) != s.end());
  BOOST_CHECK(std::find(s.begin(), s.end(), sort_expr::pos()) != s.end());
  BOOST_CHECK(std::find(s.begin(), s.end(), sort_expr::bool_()) != s.end());

  std::set<sort_expression> e = find_all_sort_expressions(q1);
  BOOST_CHECK(std::find(e.begin(), e.end(), sort_expr::nat())   != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), sort_expr::pos())   != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), sort_expr::bool_()) != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), sexpr)              != e.end());

  std::set<data_expression> d = find_all_data_expressions(make_list(make_list(q1, p1), n1));
  BOOST_CHECK(std::find(d.begin(), d.end(), q1) != d.end());
  BOOST_CHECK(std::find(d.begin(), d.end(), p1) != d.end());
  BOOST_CHECK(std::find(d.begin(), d.end(), n1) != d.end());

  return 0;
}
