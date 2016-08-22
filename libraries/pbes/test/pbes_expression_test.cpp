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
#include "mcrl2/data/data_expression.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/parse.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

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

void print(std::set<pbes_expression> q)
{
  for (const pbes_expression& expr: q)
  {
    std::cout << pbes_system::pp(expr) << std::endl;
  }
}

void test_accessors()
{
  using namespace mcrl2::pbes_system::accessors;

  std::vector<pbes_expression> expressions = parse_pbes_expressions(EXPRESSIONS).first;
  pbes_expression x = expressions[0];
  pbes_expression y = expressions[1];
  data::variable d(core::identifier_string("d"), data::sort_nat::nat());
  data::variable_list v ( { d } );
  pbes_expression z = d;
  propositional_variable_instantiation X(core::identifier_string("X"), data::data_expression_list({ data::data_expression(d) }));

  std::set<pbes_expression> q;
  q.insert(x);
  q.insert(y);
  q.insert(z);

  {
    using namespace mcrl2::pbes_system::accessors;

    pbes_expression a, b, c;
    data::variable_list w;
    core::identifier_string s;
    data::data_expression e;
    std::set<pbes_expression> q1;

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
    BOOST_CHECK(s == core::identifier_string("X"));

    const data::data_expression_list& f = param(X);
    data::data_expression_list g ({ data::data_expression(d) });
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
    pbes_expression a;
    core::identifier_string s;
    data::data_expression e;
    std::set<pbes_expression> q1;

    a = not_(x);
    a = and_(x, y);
    a = or_(x, y);
    a = imp(x, y);
    a = forall(v, x);
    a = exists(v, x);
    s = name(X);
    param(X);
    a = join_or(q.begin(), q.end());
    a = join_and(q.begin(), q.end());
    q1 = split_or(a);
    q1 = split_and(a);
  }
}

void test_term_traits()
{
  typedef core::term_traits<pbes_expression> tr;

  const std::string VARSPEC =
    "datavar         \n"
    "  m: Nat;       \n"
    "  n: Nat;       \n"
    "  b: Bool;      \n"
    "  c: Bool;      \n"
    "                \n"
    "predvar         \n"
    "  X: Bool, Pos; \n"
    "  Y: Nat;       \n"
    ;

  pbes_expression x, y, z;
  data::variable_list v;
  data::data_expression_list e;

  // and 1
  x = parse_pbes_expression("Y(1) && Y(2)", VARSPEC);
  z = pbes_system::accessors::left(x);
  z = pbes_system::accessors::right(x);

  // and 2
  x = parse_pbes_expression("val(b && c)", VARSPEC);
  if (tr::is_and(x))
  {
    BOOST_CHECK(false);
    z = pbes_system::accessors::left(x);
    z = pbes_system::accessors::right(x);
  }

  // or 1
  x = parse_pbes_expression("Y(1) || Y(2)", VARSPEC);
  z = pbes_system::accessors::left(x);
  z = pbes_system::accessors::right(x);

  // or 2
  x = parse_pbes_expression("val(b || c)", VARSPEC);
  if (tr::is_or(x))
  {
    BOOST_CHECK(false);
    z = pbes_system::accessors::left(x);
    z = pbes_system::accessors::right(x);
  }

  // imp 1
  x = parse_pbes_expression("Y(1) => !Y(2)", VARSPEC);
  z = pbes_system::accessors::left(x);
  z = pbes_system::accessors::right(x);

  // imp 2
  x = parse_pbes_expression("val(b => c)", VARSPEC);
  if (tr::is_imp(x))
  {
    BOOST_CHECK(false);
    z = pbes_system::accessors::left(x);
    z = pbes_system::accessors::right(x);
  }

  // not 1
  x = parse_pbes_expression("!(Y(1) || Y(2))", VARSPEC);
  z = pbes_system::accessors::arg(x);

  // not 2
  x = parse_pbes_expression("!val(n < 10)", VARSPEC);
  z = pbes_system::accessors::arg(x);

  // not 3
  x = parse_pbes_expression("val(!(n < 10))", VARSPEC);
  if (tr::is_not(x))
  {
    BOOST_CHECK(false);
    z = pbes_system::accessors::arg(x);
  }

  // prop var 1
  x = parse_pbes_expression("Y(1)", VARSPEC);
  e = atermpp::down_cast<propositional_variable_instantiation>(x).parameters();

  // forall 1
  x = parse_pbes_expression("forall k:Nat.Y(k)", VARSPEC);
  v = tr::var(x);
  z = pbes_system::accessors::arg(x);

  // exists 1
  x = parse_pbes_expression("exists k:Nat.Y(k)", VARSPEC);
  v = tr::var(x);
  z = pbes_system::accessors::arg(x);

}

int test_main(int argc, char** argv)
{
  test_term_traits();
  test_accessors();

  return 0;
}
