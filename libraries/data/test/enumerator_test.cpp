// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file enumerator_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/data_operation.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/parser.h"
#include "mcrl2/data/sort_arrow.h"
#include "mcrl2/data/sort_utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/identifier_generator.h"

using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;

const std::string DATA_SPEC1 =
  "sort A;                     \n"
  "sort B;                     \n"
  "sort C;                     \n"
  "sort D;                     \n"
  "sort S1 = A # B -> C;       \n"
  "sort S2 = (A -> B) -> C;    \n"
  "                            \n"
  "cons s1: S1;                \n"
  "cons s2: S2;                \n"
;  

// This is no longer allowed:
// sort S3 = A -> (B -> C);    
// sort S4 = A # (A -> B) -> C;
// sort S5 = A # A -> B -> C;  


/*
enumerator make_enumerator(rewriter& datar, const data_specification& data_spec)
{
  enumerator result(datar, data_spec); 
  return result;
}

void test_enumerator()
{
  using namespace data_expr;

  data_specification data = parse_data_specification(DATA_SPEC1);
  rewriter datar(data);
  enumerator e(datar, data);
  sort_expression s1 = sort_expr::bool_();
  atermpp::vector<data_expression> v = e.enumerate_finite_sort(s1);

  data_variable d1(core::identifier_string("d1"), sort_expr::bool_());
  data_variable d2(core::identifier_string("d2"), sort_expr::bool_());
  data_variable d3(core::identifier_string("d3"), sort_expr::bool_());
  std::vector<data_variable> vars;
  vars.push_back(d1);
  vars.push_back(d2);
  data_expression t = and_(data_expr::equal_to(d1, d2), data_expr::not_equal_to(d1,d3));
  atermpp::set<data_expression> w = e.enumerate_expression_values(t, vars.begin(), vars.end());
  std::cout << "<variables>" << pp(data_expression_list(w.begin(), w.end())) << std::endl;

  // copy an enumerator
  enumerator e1 = e;
  w = e1.enumerate_expression_values(t, vars.begin(), vars.end());

  // enumerator as return value
  enumerator e2 = make_enumerator(datar, data);
  w = e2.enumerate_expression_values(t, vars.begin(), vars.end());
}
*/

void test_data_enumerator()
{
  using namespace data_expr;

  data_specification data_spec = parse_data_specification(DATA_SPEC1);
  rewriter rewr(data_spec);
  number_postfix_generator generator("x_");
  data_enumerator<number_postfix_generator> e(data_spec, rewr, generator);

  data_variable x(identifier_string("x"), sort_expr::pos());
  atermpp::vector<data_expression_with_variables> values = e.enumerate(x);
  for (atermpp::vector<data_expression_with_variables>::iterator i = values.begin(); i != values.end(); ++i)
  {
    std::cout << pp(*i) << " " << pp(i->variables()) << std::endl;
  }
  
  data_expression_with_variables expr(x, make_list(x));
  atermpp::vector<data_expression_with_variables> y = e.enumerate(x);
  for (atermpp::vector<data_expression_with_variables>::iterator i = y.begin(); i != y.end(); ++i)
  {
    atermpp::vector<data_expression_with_variables> z = e.enumerate(*i);   
    for (atermpp::vector<data_expression_with_variables>::iterator j = z.begin(); j != z.end(); ++j)
    {
      std::cout << pp(*j) << " " << pp(j->variables()) << std::endl;
    }
  } 
}

class A: public data_expression
{
  public:
    /// Constructor.
    ///             
    A()
    {}

    /// Constructor.
    ///             
    A(atermpp::aterm_appl term)
      : data_expression(term)
    {}

    /// Constructor.
    ///             
    A(ATermAppl term)
      : data_expression(term)
    {}
};
MCRL2_ATERM_TRAITS_SPECIALIZATION(A)

void f(data_expression d)
{
  std::cout << "d = " << pp(d) << std::endl;
}

void test2()
{
  data_variable n = parse_data_expression("n", "n: Pos;\n"); 
  A a = n;
  f(a);
  std::cout << "a = " << pp(a) << std::endl;
}

void test3()
{
  data_specification data_spec = parse_data_specification(DATA_SPEC1);
  rewriter rewr(data_spec);
  number_postfix_generator generator("x_");
  data_enumerator<number_postfix_generator> e(data_spec, rewr, generator);

  data_variable   n = parse_data_expression("n", "n: Pos;\n");
  data_expression c = parse_data_expression("n < 10", "n: Pos;\n");
  data_expression_with_variables x(c, atermpp::make_list(n)); 
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_data_enumerator();
  test2();
  test3();

  return 0;
}
