// Author(s): Jan Friso Groote. Based on pres_expression_test by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pres_expression_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE pres_expression_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/pres/find.h"
#include "mcrl2/pres/join.h"
#include "mcrl2/pres/detail/parse.h"

using namespace mcrl2;
using namespace mcrl2::pres_system;
using namespace mcrl2::pres_system::detail;

BOOST_AUTO_TEST_CASE(test_accessors)
{
  using namespace mcrl2::pres_system::accessors;

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

  std::vector<pres_expression> expressions = parse_pres_expressions(EXPRESSIONS).first;
  pres_expression x = expressions[0];
  pres_expression y = expressions[1];
  data::variable d(core::identifier_string("d"), data::sort_nat::nat());
  data::variable_list v = { d };
  pres_expression z = d;
  propositional_variable_instantiation X(core::identifier_string("X"), { d });

  std::set<pres_expression> q;
  q.insert(x);
  q.insert(y);
  q.insert(z);

  {
    using namespace mcrl2::pres_system::accessors;

    pres_expression a;
    pres_expression b;
    pres_expression c;
    data::variable_list w;
    core::identifier_string s;
    data::data_expression e;
    std::set<pres_expression> q1;

    a = minus(x);
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

    a = infimum(v, x);
    w = var(a);
    b = arg(a);
    BOOST_CHECK(v == w);
    BOOST_CHECK(x == b);

    a = supremum(v, x);
    w = var(a);
    b = arg(a);
    BOOST_CHECK(v == w);
    BOOST_CHECK(x == b);

    s = name(X);
    BOOST_CHECK(s == core::identifier_string("X"));

    const data::data_expression_list& f = param(X);
    data::data_expression_list g = { d };
    BOOST_CHECK(f == g);

    a = join_or(q.begin(), q.end());
    q1 = split_or(a);
    BOOST_CHECK(q == q1);

    a = join_and(q.begin(), q.end());
    q1 = split_and(a);
    BOOST_CHECK(q == q1);
  }

  {
    pres_expression a;
    core::identifier_string s;
    data::data_expression e;
    std::set<pres_expression> q1;

    a = minus(x);
    a = and_(x, y);
    a = or_(x, y);
    a = imp(x, y);
    a = infimum(v, x);
    a = supremum(v, x);
    s = name(X);
    param(X);
    a = join_or(q.begin(), q.end());
    a = join_and(q.begin(), q.end());
    q1 = split_or(a);
    q1 = split_and(a);
  }
}

BOOST_AUTO_TEST_CASE(test_term_traits)
{
  using tr = core::term_traits<pres_expression>;

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

  pres_expression x;
  pres_expression y;
  pres_expression z;
  data::variable_list v;
  data::data_expression_list e;

  // and 1
  x = parse_pres_expression("Y(1) && Y(2)", VARSPEC);
  z = pres_system::accessors::left(x);
  z = pres_system::accessors::right(x);

  // and 2
  x = parse_pres_expression("val(b && c)", VARSPEC);
  if (tr::is_and(x))
  {
    BOOST_CHECK(false);
    z = pres_system::accessors::left(x);
    z = pres_system::accessors::right(x);
  }

  // or 1
  x = parse_pres_expression("Y(1) || Y(2)", VARSPEC);
  z = pres_system::accessors::left(x);
  z = pres_system::accessors::right(x);

  // or 2
  x = parse_pres_expression("val(b || c)", VARSPEC);
  if (tr::is_or(x))
  {
    BOOST_CHECK(false);
    z = pres_system::accessors::left(x);
    z = pres_system::accessors::right(x);
  }

  // imp 1
  x = parse_pres_expression("Y(1) => -Y(2)", VARSPEC);
  z = pres_system::accessors::left(x);
  z = pres_system::accessors::right(x);

  // imp 2
  x = parse_pres_expression("val(b => c)", VARSPEC);
  if (tr::is_imp(x))
  {
    BOOST_CHECK(false);
    z = pres_system::accessors::left(x);
    z = pres_system::accessors::right(x);
  }

  // minus 1
  x = parse_pres_expression("-(Y(1) || Y(2))", VARSPEC);
  z = pres_system::accessors::arg(x);

  // minus 2
  x = parse_pres_expression("-val(n < 10)", VARSPEC);
  z = pres_system::accessors::arg(x);

  // minus 3
  x = parse_pres_expression("val(!(n < 10))", VARSPEC);
  if (tr::is_minus(x))
  {
    BOOST_CHECK(false);
    z = pres_system::accessors::arg(x);
  }

  // prop var 1
  x = parse_pres_expression("Y(1)", VARSPEC);
  e = atermpp::down_cast<propositional_variable_instantiation>(x).parameters();

  // infimum 1
  x = parse_pres_expression("inf k:Nat.Y(k)", VARSPEC);
  v = tr::var(x);
  z = pres_system::accessors::arg(x);

  // supremum 1
  x = parse_pres_expression("sup k:Nat.Y(k)", VARSPEC);
  v = tr::var(x);
  z = pres_system::accessors::arg(x);
}
