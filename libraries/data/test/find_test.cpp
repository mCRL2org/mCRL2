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
#include <iterator>
#include <set>
#include <vector>
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

  std::set<variable> S;
  S.insert(b1);
  S.insert(p1);
  S.insert(n1);

  std::vector<variable> V;
  V.push_back(b1);
  V.push_back(p1);
  V.push_back(n1);

  sort_expression_vector domain = atermpp::make_vector< sort_expression >(sort_pos::pos(), sort_bool::bool_());
  sort_expression sexpr = function_sort(domain, sort_nat::nat());
  variable q1(identifier_string("q1"), sexpr);

  data_expression x = sort_bool::and_(equal_to(n1, n2), not_equal_to(n2, n3));
  data_expression y = sort_bool::or_(equal_to(p1, p2), sort_bool::and_(x, b2));

  //--- search_variable ---//
  BOOST_CHECK( search_variable(x, n1));
  BOOST_CHECK( search_variable(x, n2));
  BOOST_CHECK( search_variable(x, n3));
  BOOST_CHECK(!search_variable(x, n4));
  BOOST_CHECK( search_variable(S, n1));
  BOOST_CHECK(!search_variable(S, n2));
  BOOST_CHECK( search_variable(V, n1));
  BOOST_CHECK(!search_variable(V, n2));

  core::garbage_collect();

  //--- find_variables ---//
  std::set<variable> v = find_variables(x);
  BOOST_CHECK(std::find(v.begin(), v.end(), n1) != v.end());
  BOOST_CHECK(std::find(v.begin(), v.end(), n2) != v.end());
  BOOST_CHECK(std::find(v.begin(), v.end(), n3) != v.end());

  std::set<variable> vS = find_variables(S);
  std::set<variable> vV = find_variables(V);
  BOOST_CHECK(vS == vV);

  //--- search_basic_sort ---//
  BOOST_CHECK( search_basic_sort(y, sort_nat::nat()  ));
  BOOST_CHECK( search_basic_sort(y, sort_pos::pos()  ));
  BOOST_CHECK( search_basic_sort(y, sort_bool::bool_()));
  BOOST_CHECK(!search_basic_sort(y, sort_real::real_() ));
  BOOST_CHECK( search_basic_sort(S, sort_bool::bool_()));
  BOOST_CHECK(!search_basic_sort(S, sort_real::real_() ));
  BOOST_CHECK( search_basic_sort(V, sort_bool::bool_()));
  BOOST_CHECK(!search_basic_sort(V, sort_real::real_() ));

  core::garbage_collect();

  //--- find_basic_sorts ---//
  std::set<basic_sort> s = find_basic_sorts(y);
  BOOST_CHECK(std::find(s.begin(), s.end(), sort_nat::nat()) != s.end());
  BOOST_CHECK(std::find(s.begin(), s.end(), sort_pos::pos()) != s.end());
  BOOST_CHECK(std::find(s.begin(), s.end(), sort_bool::bool_()) != s.end());

  std::set<basic_sort> sS = find_basic_sorts(S);
  std::set<basic_sort> sV = find_basic_sorts(V);
  BOOST_CHECK(sS == sV);

  core::garbage_collect();

  //--- find_sort_expressions ---//
  std::set<sort_expression> e = find_sort_expressions(q1);
  BOOST_CHECK(std::find(e.begin(), e.end(), sort_nat::nat())   != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), sort_pos::pos())   != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), sort_bool::bool_()) != e.end());
  BOOST_CHECK(std::find(e.begin(), e.end(), sexpr)              != e.end());

  std::set<sort_expression> eS = find_sort_expressions(S);
  std::set<sort_expression> eV = find_sort_expressions(V);
  BOOST_CHECK(eS == eV);

  std::set<sort_expression> Z;
  find_sort_expressions(q1, std::inserter(Z, Z.end()));
  find_sort_expressions(S, std::inserter(Z, Z.end()));

  core::garbage_collect();

  //--- find_data_expressions ---//
  std::set<data_expression> d = find_data_expressions(atermpp::make_vector(q1, p1, n1));
  BOOST_CHECK(std::find(d.begin(), d.end(), q1) != d.end());
  BOOST_CHECK(std::find(d.begin(), d.end(), p1) != d.end());
  BOOST_CHECK(std::find(d.begin(), d.end(), n1) != d.end());

  std::set<data_expression> dS = find_data_expressions(S);
  std::set<data_expression> dV = find_data_expressions(V);
  BOOST_CHECK(dS == dV);

  core::garbage_collect();

  //--- data_specification ---//
  BOOST_CHECK(search_basic_sort(data_specification().sorts(), sort_bool::bool_()));
  BOOST_CHECK(search_sort_expression(data_specification().sorts(), sort_bool::bool_()));
  BOOST_CHECK(search_data_expression(data_specification().constructors(), sort_bool::true_()));
  BOOST_CHECK(!search_variable(data_specification().sorts(), variable("a", sort_bool::bool_())));

  return 0;
}
