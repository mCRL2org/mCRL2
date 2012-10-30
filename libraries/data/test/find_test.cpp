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
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/function_sort.h"

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
  BOOST_CHECK(search_variable(x, n1));
  BOOST_CHECK(search_variable(x, n2));
  BOOST_CHECK(search_variable(x, n3));
  BOOST_CHECK(!search_variable(x, n4));
  BOOST_CHECK(search_variable(S, n1));
  BOOST_CHECK(!search_variable(S, n2));
  BOOST_CHECK(search_variable(V, n1));
  BOOST_CHECK(!search_variable(V, n2));


  //--- find_variables ---//
  std::set<variable> v = find_variables(x);
  BOOST_CHECK(std::find(v.begin(), v.end(), n1) != v.end());
  BOOST_CHECK(std::find(v.begin(), v.end(), n2) != v.end());
  BOOST_CHECK(std::find(v.begin(), v.end(), n3) != v.end());

  std::set<variable> vS = find_variables(S);
  std::set<variable> vV = find_variables(V);
  BOOST_CHECK(vS == vV);

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


  //--- data_specification ---//
  // TODO: discuss whether this test should fail or not
  //BOOST_CHECK(search_data_expression(data_specification().constructors(), sort_bool::true_()));

  return 0;
}
