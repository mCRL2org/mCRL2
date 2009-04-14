// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file data_expression_test.cpp
/// \brief Basic regression test for new_data expressions.

#include <iostream>
#include <boost/range/iterator_range.hpp>
#include <boost/test/minimal.hpp>

#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/new_data/detail/container_utility.h"
#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/variable.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/abstraction.h"
#include "mcrl2/new_data/lambda.h"
#include "mcrl2/new_data/forall.h"
#include "mcrl2/new_data/exists.h"
#include "mcrl2/new_data/where_clause.h"
#include "mcrl2/new_data/assignment.h"

using namespace mcrl2;
using namespace mcrl2::new_data;

/*
template <typename T>
bool equal_contents(boost::iterator_range<T> r1, boost::iterator_range<T> r2)
{
  if (r1.size() != r2.size())
  {
    return false;
  }

  T i = r1.begin();
  T j = r2.begin();
  while (i != r1.end() && j != r2.end())
  {
    if (*i != *j)
    {
      return false;
    }
    ++i;
    ++j;
  }
  return true;
}
*/

void variable_test()
{
  basic_sort s("S");
  variable x("x", s);
  BOOST_CHECK(x.name() == "x");
  BOOST_CHECK(x.sort() == s);

  core::identifier_string y_name("y");
  variable y(y_name, s);
  BOOST_CHECK(y.name() == "y");
  BOOST_CHECK(y.sort() == s);

  variable y_("y", s);
  BOOST_CHECK(y_.name() == "y");
  BOOST_CHECK(y.sort() == s);

  BOOST_CHECK(x != y);
  BOOST_CHECK(x != y_);
  BOOST_CHECK(y == y_);

  data_expression y_e(y);
  variable y_e_(y_e);
  BOOST_CHECK(y_e_ == y);
  BOOST_CHECK(y_e_.name() == y.name());
  BOOST_CHECK(y_e_.sort() == y.sort());
}

void function_symbol_test()
{
  basic_sort s0("S0");
  basic_sort s1("S1");
  basic_sort s("S");

  sort_expression_vector s01;
  s01.push_back(s0);
  s01.push_back(s1);
  boost::iterator_range<sort_expression_vector::iterator> s01_range = boost::make_iterator_range(s01);
  function_sort fs(s01_range, s);


  function_symbol f("f", fs);
  BOOST_CHECK(f.name() == "f");
  BOOST_CHECK(f.sort() == fs);

  function_symbol g("g", s0);
  BOOST_CHECK(g.name() == "g");
  BOOST_CHECK(g.sort() == s0);

  core::identifier_string g_name("g");
  function_symbol g_(g_name, s0);
  BOOST_CHECK(g_.name() == "g");
  BOOST_CHECK(g_.sort() == s0);

  BOOST_CHECK(f != g);
  BOOST_CHECK(f != g_);
  BOOST_CHECK(g == g_);

  data_expression f_e(f);
  function_symbol f_e_(f_e);
  BOOST_CHECK(f_e == f);
  BOOST_CHECK(f_e_.name() == f.name());
  BOOST_CHECK(f_e_.sort() == f.sort());
}

void application_test()
{
  basic_sort s0("S0");
  basic_sort s1("S1");
  basic_sort s("S");
  sort_expression_vector s01;
  s01.push_back(sort_expression(s0));
  s01.push_back(sort_expression(s1));
  function_sort s01s(boost::make_iterator_range(s01), s);

  function_symbol f("f", s01s);
  data_expression x(variable("x", s0));
  data_expression y(variable("y", s1));
  data_expression_vector xy = make_vector(x,y);
  boost::iterator_range<data_expression_vector::const_iterator> xy_range(boost::make_iterator_range(xy.begin(), xy.end()));
  application fxy(f, xy_range);
  BOOST_CHECK(fxy.sort() == s);
  BOOST_CHECK(fxy.head() == f);
  BOOST_CHECK(fxy.arguments() == xy_range);

  data_expression fxy_e(fxy);
  application fxy_e_(fxy_e);
  BOOST_CHECK(fxy == fxy_e_);
  BOOST_CHECK(fxy.sort() == fxy_e_.sort());
  BOOST_CHECK(fxy.head() == fxy_e_.head());
  BOOST_CHECK(fxy.arguments() == fxy_e_.arguments());

  BOOST_CHECK(fxy == application(f,x,y));

}

void abstraction_test()
{
  basic_sort s("S");

  variable x("x", s);
  variable_vector xl = make_vector(x);
  boost::iterator_range<variable_vector::const_iterator> xl_range(boost::make_iterator_range(xl.begin(), xl.end()));
  abstraction I("lambda", xl_range, x);
  BOOST_CHECK(I.binding_operator() == "lambda");
  BOOST_CHECK(I.variables() == xl_range);
  BOOST_CHECK(I.body() == x);

  data_expression I_e(I);
  abstraction I_e_(I_e);
  BOOST_CHECK(I_e_ == I);
  BOOST_CHECK(I_e_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_e_.variables() == I.variables());
  BOOST_CHECK(I_e_.body() == I.body());
}

void lambda_test()
{
  basic_sort s("S");

  variable x("x", s);
  variable_vector xl = make_vector(x);
  boost::iterator_range<variable_vector::const_iterator> xl_range(boost::make_iterator_range(xl.begin(), xl.end()));
  lambda I(xl_range, x);
  BOOST_CHECK(I.binding_operator() == "lambda");
  BOOST_CHECK(I.is_lambda());
  BOOST_CHECK(I.variables() == xl_range);
  BOOST_CHECK(I.body() == x);
  sort_expression s_(s);
  sort_expression_vector s_l(make_vector(s_));
  BOOST_CHECK(!s_l.empty());
  boost::iterator_range<sort_expression_vector::const_iterator> s_l_range(s_l);
  function_sort fs(s_l_range, s);
  BOOST_CHECK(I.sort() == fs);

  data_expression I_e(I);
  lambda I_e_(I_e);
  BOOST_CHECK(I_e_ == I);
  BOOST_CHECK(I_e_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_e_.variables() == I.variables());
  BOOST_CHECK(I_e_.body() == I.body());
  abstraction I_("lambda", xl_range, x);
  BOOST_CHECK(I_ == I);
  BOOST_CHECK(I_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_.variables() == I.variables());
  BOOST_CHECK(I_.body() == I.body()) ;
}

void forall_test()
{
  basic_sort s("S");

  variable x("x", s);
  variable_vector xl = make_vector(x);
  boost::iterator_range<variable_vector::const_iterator> xl_range(boost::make_iterator_range(xl.begin(), xl.end()));
  forall I(xl_range, x);
  BOOST_CHECK(I.binding_operator() == "forall");
  BOOST_CHECK(I.is_forall());
  BOOST_CHECK(I.variables() == xl_range);
  BOOST_CHECK(I.body() == x);
  sort_expression s_(s);
  sort_expression_vector s_l(make_vector(s_));

  // TODO Check sort

  data_expression I_e(I);
  forall I_e_(I_e);
  BOOST_CHECK(I_e_ == I);
  BOOST_CHECK(I_e_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_e_.variables() == I.variables());
  BOOST_CHECK(I_e_.body() == I.body());
  abstraction I_("forall", xl_range, x);
  BOOST_CHECK(I_ == I);
  BOOST_CHECK(I_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_.variables() == I.variables());
  BOOST_CHECK(I_.body() == I.body()) ;
}

void exists_test()
{
  basic_sort s("S");

  variable x("x", s);
  variable_vector xl = make_vector(x);
  boost::iterator_range<variable_vector::const_iterator> xl_range(boost::make_iterator_range(xl.begin(), xl.end()));
  exists I(xl_range, x);
  BOOST_CHECK(I.binding_operator() == "exists");
  BOOST_CHECK(I.is_exists());
  BOOST_CHECK(I.variables() == xl_range);
  BOOST_CHECK(I.body() == x);
  sort_expression s_(s);
  sort_expression_vector s_l(make_vector(s_));

  // TODO Check sort

  data_expression I_e(I);
  exists I_e_(I_e);
  BOOST_CHECK(I_e_ == I);
  BOOST_CHECK(I_e_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_e_.variables() == I.variables());
  BOOST_CHECK(I_e_.body() == I.body());
  abstraction I_("exists", xl_range, x);
  BOOST_CHECK(I_ == I);
  BOOST_CHECK(I_.binding_operator() == I.binding_operator());
  BOOST_CHECK(I_.variables() == I.variables());
  BOOST_CHECK(I_.body() == I.body()) ;
}

void where_declaration_test()
{
  basic_sort s("S");
  variable x("x", s);
  variable y("y", s);
  
  assignment xy(x,y);
  assignment_vector xyl(make_vector(xy));
  boost::iterator_range<assignment_vector::const_iterator> xy_range(xyl);
  where_clause wxy(x, xy_range);
  BOOST_CHECK(wxy.body() == x);
  BOOST_CHECK(wxy.declarations() == xy_range);
  
  data_expression wxy_e(wxy);
  where_clause wxy_e_(wxy_e);
  BOOST_CHECK(wxy_e_ == wxy);
  BOOST_CHECK(wxy_e_.body() == wxy.body());
  BOOST_CHECK(wxy_e_.declarations() == wxy.declarations());
  BOOST_CHECK(wxy.sort() == x.sort());
}

void assignment_test()
{
  basic_sort s("S");
  variable x("x", s);
  variable y("y", s);

  assignment xy(x, y);
  BOOST_CHECK(xy.lhs() == x);
  BOOST_CHECK(xy.rhs() == y);
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv);

  variable_test();
  function_symbol_test();
  application_test();
  abstraction_test();
  lambda_test();
  forall_test();
  exists_test();
  where_declaration_test();
  assignment_test();

  return EXIT_SUCCESS;
}


