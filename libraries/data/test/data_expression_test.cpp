// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_expression_test.cpp
/// \brief Basic regression test for data expressions.

#define BOOST_TEST_MODULE data_expression_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/where_clause.h"

using namespace mcrl2;
using namespace mcrl2::data;

BOOST_AUTO_TEST_CASE(variable_test)
{
  basic_sort s("S");
  variable x("x", s);
  BOOST_CHECK(pp(x.name()) == "x");
  BOOST_CHECK(x.sort() == s);

  core::identifier_string y_name("y");
  variable y(y_name, s);
  BOOST_CHECK(pp(y.name()) == "y");
  BOOST_CHECK(y.sort() == s);

  variable y_("y", s);
  BOOST_CHECK(pp(y_.name()) == "y");
  BOOST_CHECK(y.sort() == s);

  BOOST_CHECK(x != y);
  BOOST_CHECK(x != y_);
  BOOST_CHECK(y == y_);

  const data_expression& y_e(y);
  variable y_e_(y_e);
  BOOST_CHECK(y_e_ == y);
  BOOST_CHECK(y_e_.name() == y.name());
  BOOST_CHECK(y_e_.sort() == y.sort());
}

BOOST_AUTO_TEST_CASE(function_symbol_test)
{
  basic_sort s0("S0");
  basic_sort s1("S1");
  basic_sort s("S");

  sort_expression_vector s01;
  s01.push_back(s0);
  s01.push_back(s1);
  function_sort fs(s01, s);


  data::function_symbol f("f", fs);
  BOOST_CHECK(pp(f.name()) == "f");
  BOOST_CHECK(f.sort() == fs);

  data::function_symbol g("g", s0);
  BOOST_CHECK(pp(g.name()) == "g");
  BOOST_CHECK(g.sort() == s0);

  core::identifier_string g_name("g");
  data::function_symbol g_(g_name, s0);
  BOOST_CHECK(pp(g_.name()) == "g");
  BOOST_CHECK(g_.sort() == s0);

  BOOST_CHECK(f != g);
  BOOST_CHECK(f != g_);
  BOOST_CHECK(g == g_);

  const data_expression& f_e(f);
  data::function_symbol f_e_(f_e);
  BOOST_CHECK(f_e == f);
  BOOST_CHECK(f_e_.name() == f.name());
  BOOST_CHECK(f_e_.sort() == f.sort());
}

BOOST_AUTO_TEST_CASE(application_test)
{
  basic_sort s0("S0");
  basic_sort s1("S1");
  basic_sort s("S");
  sort_expression_vector s01;
  s01.emplace_back(s0);
  s01.emplace_back(s1);
  function_sort s01s(s01, s);

  data::function_symbol f("f", s01s);
  data_expression x(variable("x", s0));
  data_expression y(variable("y", s1));
  data_expression_list xy = { x, y };
  application fxy(f, xy);
  BOOST_CHECK(fxy.sort() == s);
  BOOST_CHECK(fxy.head() == f);
  BOOST_CHECK(data_expression_list(fxy.begin(), fxy.end()) == xy);
  BOOST_CHECK(*(fxy.begin()) == x);
  BOOST_CHECK(*(++fxy.begin()) == y);

  const data_expression& fxy_e(fxy);
  application fxy_e_(fxy_e);
  BOOST_CHECK(fxy == fxy_e_);
  BOOST_CHECK(fxy.sort() == fxy_e_.sort());
  BOOST_CHECK(fxy.head() == fxy_e_.head());
  BOOST_CHECK(fxy == f(x,y));
}

BOOST_AUTO_TEST_CASE(abstraction_test)
{
  basic_sort s("S");

  variable x("x", s);
  variable_list xl = { x };
  abstraction I(lambda_binder(), xl, x);
  BOOST_CHECK(I.binding_operator() == lambda_binder());
  BOOST_CHECK(I.variables() == xl);
  BOOST_CHECK(I.body() == x);

  const data_expression& I_e(I);
  abstraction I_e_(I_e);
  BOOST_CHECK(I_e_ == I);
  BOOST_CHECK(I_e_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_e_.variables() == I.variables());
  BOOST_CHECK(I_e_.body() == I.body());
}

BOOST_AUTO_TEST_CASE(lambda_test)
{
  basic_sort s("S");

  variable x("x", s);
  variable_list xl = { x };
  lambda I(xl, x);
  BOOST_CHECK(I.binding_operator() == lambda_binder());
  BOOST_CHECK(is_lambda(I));
  BOOST_CHECK(I.variables() == xl);
  BOOST_CHECK(I.body() == x);
  const sort_expression& s_(s);
  sort_expression_vector s_l {s_};
  BOOST_CHECK(!s_l.empty());
  function_sort fs(s_l, s);
  BOOST_CHECK(I.sort() == fs);

  const data_expression& I_e(I);
  lambda I_e_(I_e);
  BOOST_CHECK(I_e_ == I);
  BOOST_CHECK(I_e_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_e_.variables() == I.variables());
  BOOST_CHECK(I_e_.body() == I.body());
  abstraction I_(lambda_binder(), xl, x);
  BOOST_CHECK(I_ == I);
  BOOST_CHECK(I_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_.variables() == I.variables());
  BOOST_CHECK(I_.body() == I.body()) ;
}

BOOST_AUTO_TEST_CASE(forall_test)
{
  basic_sort s("S");

  variable x("x", s);
  variable_list xl = { x };
  forall I(xl, x);
  BOOST_CHECK(I.binding_operator() == forall_binder());
  BOOST_CHECK(is_forall(I));
  BOOST_CHECK(I.variables() == xl);
  BOOST_CHECK(I.body() == x);
  const sort_expression& s_(s);
  sort_expression_vector s_l {s_};

  // TODO Check sort

  const data_expression& I_e(I);
  forall I_e_(I_e);
  BOOST_CHECK(I_e_ == I);
  BOOST_CHECK(I_e_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_e_.variables() == I.variables());
  BOOST_CHECK(I_e_.body() == I.body());
  abstraction I_(forall_binder(), xl, x);
  BOOST_CHECK(I_ == I);
  BOOST_CHECK(I_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_.variables() == I.variables());
  BOOST_CHECK(I_.body() == I.body()) ;
}

BOOST_AUTO_TEST_CASE(exists_test)
{
  basic_sort s("S");

  variable x("x", s);
  variable_list xl = { x };
  exists I(xl, x);
  BOOST_CHECK(I.binding_operator() == exists_binder());
  BOOST_CHECK(is_exists(I));
  BOOST_CHECK(I.variables() == xl);
  BOOST_CHECK(I.body() == x);
  const sort_expression& s_(s);
  sort_expression_vector s_l {s_};

  // TODO Check sort

  const data_expression& I_e(I);
  exists I_e_(I_e);
  BOOST_CHECK(I_e_ == I);
  BOOST_CHECK(I_e_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_e_.variables() == I.variables());
  BOOST_CHECK(I_e_.body() == I.body());
  abstraction I_(exists_binder(), xl, x);
  BOOST_CHECK(I_ == I);
  BOOST_CHECK(I_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_.variables() == I.variables());
  BOOST_CHECK(I_.body() == I.body()) ;
}

BOOST_AUTO_TEST_CASE(set_comprehension_test)
{
  basic_sort s("S");
  variable x("x", make_function_sort_(s,sort_bool::bool_()));
  data::function_symbol f("f", make_function_sort_(s, sort_bool::bool_()));
  data_expression e(sort_set::set_comprehension(s, x));
  BOOST_CHECK(e.sort() == sort_set::set_(s));
}

BOOST_AUTO_TEST_CASE(bag_comprehension_test)
{
  basic_sort s("S");
  data::function_symbol f("f", make_function_sort_(s, sort_nat::nat()));
  data_expression e(sort_bag::bag_comprehension(s, f));
  BOOST_CHECK(e.sort() == sort_bag::bag(s));
}

BOOST_AUTO_TEST_CASE(where_declaration_test)
{
  basic_sort s("S");
  variable x("x", s);
  variable y("y", s);

  assignment xy(x,y);
  assignment_expression_list xyl = { xy };
  where_clause wxy(x, xyl);
  BOOST_CHECK(wxy.body() == x);
  BOOST_CHECK(wxy.declarations() == xyl);

  const data_expression& wxy_e(wxy);
  where_clause wxy_e_(wxy_e);
  BOOST_CHECK(wxy_e_ == wxy);
  BOOST_CHECK(wxy_e_.body() == wxy.body());
  BOOST_CHECK(wxy_e_.declarations() == wxy.declarations());
  BOOST_CHECK(wxy.sort() == x.sort());
}

BOOST_AUTO_TEST_CASE(assignment_test)
{
  basic_sort s("S");
  variable x("x", s);
  variable y("y", s);

  assignment xy(x, y);
  BOOST_CHECK(xy.lhs() == x);
  BOOST_CHECK(xy.rhs() == y);
}

BOOST_AUTO_TEST_CASE(system_defined_check)
{
  BOOST_CHECK(sort_list::empty(sort_pos::pos()) != sort_list::empty(sort_nat::nat()));
}
