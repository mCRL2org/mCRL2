// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_parse.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::modal;
using namespace mcrl2::modal::detail;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

using mcrl2::core::pp;

std::string EXPRESSIONS =
"datavar                                  \n"
"  n: Nat;                                \n"
"                                         \n"
"predvar                                  \n"
"                                         \n"
"expressions                              \n"
"  val(n > 2);                            \n"
"  val(n > 3)                             \n"
;

void print(atermpp::set<pbes_expression> q)
{
  for (atermpp::set<pbes_expression>::iterator i = q.begin(); i != q.end(); ++i)
  {
    std::cout << pp(*i) << std::endl;
  }
}

void test_accessors()
{
  using namespace mcrl2::pbes_system::accessors;

  std::vector<pbes_expression> expressions = parse_pbes_expressions(EXPRESSIONS).first;
  pbes_expression x = expressions[0];
  pbes_expression y = expressions[1];
  data_variable d(identifier_string("d"), sort_expr::nat());
  data_variable_list v = make_list(d);
  pbes_expression z = d; 
  propositional_variable_instantiation X(identifier_string("X"), make_list(d));

  atermpp::set<pbes_expression> q;
  q.insert(x);
  q.insert(y);
  q.insert(z);

  {
    using namespace pbes_expr;
    using namespace mcrl2::pbes_system::accessors;

    pbes_expression a, b, c;
    data_variable_list w;
    identifier_string s;
    data_expression e;
    atermpp::set<pbes_expression> q1;

    e = val(z);
    
    a = not_(x);
    b = arg(a);
    BOOST_CHECK(x == b);

    a = and_(x, y);
    b = left(a);
    c = right(a);
    BOOST_CHECK(x == b);
    BOOST_CHECK(y == c);

    a = or_(x, y);
    b = left(a);
    c = right(a);
    BOOST_CHECK(x == b);
    BOOST_CHECK(y == c);

    a = imp(x, y);
    b = left(a);
    c = right(a);
    BOOST_CHECK(x == b);
    BOOST_CHECK(y == c);

    a = forall(v, x);
    w = var(a);
    b = arg(a);
    BOOST_CHECK(v == w);
    BOOST_CHECK(x == b);
   
    a = exists(v, x);
    w = var(a);
    b = arg(a);
    BOOST_CHECK(v == w);
    BOOST_CHECK(x == b);

    s = name(X);
    BOOST_CHECK(s == identifier_string("X"));
    
    data_expression_list f = param(X);
    data_expression_list g = make_list(d);
    BOOST_CHECK(f == g);

    print(q);

    a = join_or(q.begin(), q.end());
    q1 = split_or(a);    
    BOOST_CHECK(q == q1);

    print(q1);

    a = join_and(q.begin(), q.end());
    q1 = split_and(a);
    BOOST_CHECK(q == q1);

    print(q1);
  } 

  {
    using namespace pbes_expr_optimized;

    pbes_expression a, b, c;
    data_variable_list w;
    identifier_string s;
    data_expression e;
    atermpp::set<pbes_expression> q1;

    e = val(z);
    
    a = not_(x);
    a = and_(x, y);
    a = or_(x, y);
    a = imp(x, y);
    a = forall(v, x);
    a = exists(v, x);
    s = name(X);
    data_expression_list f = param(X);
    a = join_or(q.begin(), q.end());
    a = join_and(q.begin(), q.end());
    q1 = split_or(a);
    q1 = split_and(a);
  } 
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_accessors();

  return 0;
}
