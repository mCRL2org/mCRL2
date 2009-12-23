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
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/deque.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/core/garbage_collection.h"

using namespace atermpp;
using namespace mcrl2;
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
  sort_expression s1 = sort_bool::bool_();
  atermpp::vector<data_expression> v = e.enumerate_finite_sort(s1);

  variable d1(core::identifier_string("d1"), sort_bool::bool_());
  variable d2(core::identifier_string("d2"), sort_bool::bool_());
  variable d3(core::identifier_string("d3"), sort_bool::bool_());
  std::vector<variable> vars;
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
  try
  {
    data_specification data_spec = parse_data_specification(DATA_SPEC1);
    rewriter rewr(data_spec);
    number_postfix_generator generator("x_");
    data_enumerator<number_postfix_generator> e(data_spec, rewr, generator);

    variable x(identifier_string("x"), sort_pos::pos());
    atermpp::vector<data_expression_with_variables> values = e.enumerate(x);
    for (atermpp::vector<data_expression_with_variables>::const_iterator i = values.begin(); i != values.end(); ++i)
    {
      std::cout << mcrl2::core::pp(*i) << " " << mcrl2::data::pp(i->variables()) << std::endl;
    }

    data_expression_with_variables expr(x, make_vector(x));
    atermpp::vector<data_expression_with_variables> y = e.enumerate(x);
    for (atermpp::vector<data_expression_with_variables>::const_iterator i = y.begin(); i != y.end(); ++i)
    {
      atermpp::vector<data_expression_with_variables> z = e.enumerate(*i);
      for (atermpp::vector<data_expression_with_variables>::const_iterator j = z.begin(); j != z.end(); ++j)
      {
        std::cout << mcrl2::core::pp(*j) << " " << mcrl2::data::pp(j->variables()) << std::endl;
      }
    }
  }
  catch(mcrl2::runtime_error)
  {
    // this is OK
    return;
  }
  BOOST_CHECK(false); // this point should not be reached
}

void test_data_enumerator2()
{
  std::string DATA_SPEC =
  "sort D;\n"
  "map f:D -> Bool;\n"
  ;

  data_specification data_spec = parse_data_specification(DATA_SPEC);
  variable x = parse_variable("d:D", data_spec);

  rewriter rewr(data_spec);
  number_postfix_generator generator("x_");
  data_enumerator<number_postfix_generator> e(data_spec, rewr, generator);

  try
  {
    atermpp::vector<data_expression_with_variables> values = e.enumerate(x);
  }
  catch(mcrl2::runtime_error)
  {
    // this is OK
    return;
  }
  BOOST_CHECK(false); // this point should not be reached
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

void f(data_expression d)
{
  std::cout << "d = " << mcrl2::core::pp(d) << std::endl;
}

void test2()
{
  variable n("n", sort_pos::pos());
  A a = n;
  f(a);
  std::cout << "a = " << mcrl2::core::pp(a) << std::endl;
}

void test3()
{
  data_specification data_spec = parse_data_specification(DATA_SPEC1);
  rewriter rewr(data_spec);
  number_postfix_generator generator("x_");
  data_enumerator<number_postfix_generator> e(data_spec, rewr, generator);

  variable   n = parse_data_expression("n", "n: Pos;\n");
  data_expression c = parse_data_expression("n < 10", "n: Pos;\n");
  data_expression_with_variables x(c, make_vector(n));
}

void test4()
{
  data_specification data_spec(parse_data_specification("sort N = Nat;")); // import Nat
  rewriter datar(data_spec);
  number_postfix_generator generator("x_");
  data_enumerator<number_postfix_generator> datae(data_spec, datar, generator);
  variable y = parse_data_expression("n", "n: Nat;\n");
  atermpp::vector<data_expression_with_variables> z = datae.enumerate(y);
  BOOST_CHECK(z.size() > 0);
}

// This test verifies that the enumerator is able to find all terms n
// that satisfy n < 3, with n:Nat.
void test5()
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_nat::nat());
  rewriter datar(data_spec);
  number_postfix_generator generator("x_");
  data_enumerator<number_postfix_generator> datae(data_spec, datar, generator);
  atermpp::deque<data_expression_with_variables> v;
  variable n("n", sort_nat::nat());
  v.push_front(data_expression_with_variables(n, make_list(n)));
  data_expression_with_variables three = sort_nat::nat(3);

  atermpp::vector< data_expression > result;

  while (!v.empty())
  {
    data_expression_with_variables e = v.back();
    v.pop_back();
    atermpp::vector<data_expression_with_variables> z = datae.enumerate(e);

    for (atermpp::vector<data_expression_with_variables>::iterator i = z.begin(); i != z.end(); ++i)
    {
      data_expression b = datar(greater(*i, three));
      if (b == sort_bool::false_())
      {
        std::clog << "found solution " << pp(*i) << std::endl;
        result.push_back(*i);
      }
      else if (b == sort_bool::true_())
      {
        std::clog << "found non-solution " << pp(*i) << std::endl;
      }
      else
      {
        v.push_front(*i);
      }
    }
  }

  BOOST_CHECK(result.size() == 4);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_data_enumerator();
  core::garbage_collect();

  test_data_enumerator2();
  core::garbage_collect();

  test2();
  core::garbage_collect();

  test3();
  core::garbage_collect();

  test4();
  core::garbage_collect();

  test5();
  core::garbage_collect();

  return 0;
}
