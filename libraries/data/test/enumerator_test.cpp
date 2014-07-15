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
#include "mcrl2/core/print.h"
#include "mcrl2/data/detail/enumerator_variable_limit.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/detail/enumerator.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/print.h"

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


void test_data_enumerator()
{
  try
  {
    data_specification data_spec = parse_data_specification(DATA_SPEC1);
    rewriter rewr(data_spec);
    data_enumerator e(data_spec, rewr, "x_");

    variable x(identifier_string("x"), sort_pos::pos());
    std::vector<data_expression_with_variables> values = e.enumerate(x);
    for (std::vector<data_expression_with_variables>::const_iterator i = values.begin(); i != values.end(); ++i)
    {
      std::cout << *i << " " << data::pp(i->variables()) << std::endl;
    }

    data_expression_with_variables expr(x, atermpp::make_vector(x));
    std::vector<data_expression_with_variables> y = e.enumerate(x);
    for (std::vector<data_expression_with_variables>::const_iterator i = y.begin(); i != y.end(); ++i)
    {
      std::vector<data_expression_with_variables> z = e.enumerate(*i);
      for (std::vector<data_expression_with_variables>::const_iterator j = z.begin(); j != z.end(); ++j)
      {
        std::cout << *j << " " << data::pp(j->variables()) << std::endl;
      }
    }
  }
  catch (mcrl2::runtime_error)
  {
    // this is OK
    BOOST_CHECK(false); // this point should not be reached
    return;
  }
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
  data_enumerator e(data_spec, rewr, "x_");

  try
  {
    std::vector<data_expression_with_variables> values = e.enumerate(x);
  }
  catch (mcrl2::runtime_error)
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
    A(atermpp::aterm term)
      : data_expression(term)
    {}
};

void f(data_expression d)
{
  std::cout << "d = " << data::pp(d) << std::endl;
}

void test2()
{
  variable n("n", sort_pos::pos());
  A a = n;
  f(a);
  std::cout << "a = " << a << std::endl;
}

void test3()
{
  data_specification data_spec = parse_data_specification(DATA_SPEC1);
  rewriter rewr(data_spec);
  data_enumerator e(data_spec, rewr, "x_");

  variable n = atermpp::aterm_cast<variable>(parse_data_expression("n", "n: Pos;\n"));
  data_expression c = parse_data_expression("n < 10", "n: Pos;\n");
  data_expression_with_variables x(c, atermpp::make_vector(n));
}

void test4()
{
  data_specification data_spec(parse_data_specification("sort N = Nat;")); // import Nat
  rewriter datar(data_spec);
  data_enumerator datae(data_spec, datar, "x_");
  variable y = atermpp::aterm_cast<variable>(parse_data_expression("n", "n: Nat;\n"));
  std::vector<data_expression_with_variables> z = datae.enumerate(y);
  BOOST_CHECK(z.size() > 0);
}

// This test verifies that the enumerator is able to find all terms n
// that satisfy n < 3, with n:Nat.
void test5()
{
  data_specification data_spec;
  data_spec.add_context_sort(sort_nat::nat());
  rewriter datar(data_spec);
  data_enumerator datae(data_spec, datar, "x_");
  std::deque<data_expression_with_variables> v;
  variable n("n", sort_nat::nat());
  v.push_front(data_expression_with_variables(n, make_list(n)));
  data_expression_with_variables three = sort_nat::nat(3);

  std::vector< data_expression > result;

  while (!v.empty())
  {
    data_expression_with_variables e = v.back();
    v.pop_back();
    std::vector<data_expression_with_variables> z = datae.enumerate(e);

    for (std::vector<data_expression_with_variables>::iterator i = z.begin(); i != z.end(); ++i)
    {
      data_expression b = datar(greater(*i, three));
      if (b == sort_bool::false_())
      {
        std::clog << "found solution " << *i << std::endl;
        result.push_back(*i);
      }
      else if (b == sort_bool::true_())
      {
        std::clog << "found non-solution " << *i << std::endl;
      }
      else
      {
        v.push_front(*i);
      }
    }
  }

  BOOST_CHECK(result.size() == 4);
}

/// \brief Computes the range of values that a finite sort can take
/// \param s A sort expression
/// \return A sequence of all values that s can take
/// \pre The sort expression s is finite, and s is not a function sort
std::vector<data::data_expression> value_range(data::sort_expression s, const data::data_specification& data_spec, const data::rewriter& rewr)
{
  std::cout << "s = " << data::pp(s) << std::endl;
  data::variable v("dummy", s);
  std::cout << "v = " << data::pp(v) << std::endl;
  std::vector<data::data_expression> result;
  data::data_enumerator e(data_spec, rewr);
  std::vector<data::data_expression_with_variables> values = e.enumerate(v);
  for (std::vector<data::data_expression_with_variables>::iterator i = values.begin(); i != values.end(); ++i)
  {
    result.push_back(*i);
  }
  return result;
}

void test6()
{
  data_specification data_spec;
  rewriter rewr(data_spec);
  std::vector<data_expression> values = value_range(sort_bool::bool_(), data_spec, rewr);
  for (std::vector<data_expression>::const_iterator i = values.begin(); i != values.end(); ++i)
  {
    std::cout << *i << std::endl;
  }
}

void test_enumerator_variable_limit()
{
  BOOST_CHECK(data::detail::get_enumerator_variable_limit() == 1000);
  data::detail::set_enumerator_variable_limit(100);
  BOOST_CHECK(data::detail::get_enumerator_variable_limit() == 100);
}

int test_main(int argc, char* argv[])
{
  test_data_enumerator();
  test_data_enumerator2();
  test2();
  test3();
  test4();
  test5();
  test6();

  return 0;
}
