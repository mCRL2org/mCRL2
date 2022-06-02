// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file find_test.cpp
/// \brief Test for find functions.

#define BOOST_TEST_MODULE find_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/parse.h"

using namespace mcrl2;

inline
data::variable bool_(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_bool::bool_());
}

inline
data::variable nat(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_nat::nat());
}

inline
data::variable pos(const std::string& name)
{
  return data::variable(core::identifier_string(name), data::sort_pos::pos());
}

inline
data::data_expression parse_data_expression(const std::string& text)
{
  std::vector<data::variable> variable_context
  {
    nat("n"),
    nat("n1"),
    nat("n2"),
    nat("n3"),
    nat("n4"),
    bool_("b"),
    bool_("b1"),
    bool_("b2"),
    bool_("b3"),
    bool_("b4"),
    pos("p"),
    pos("p1"),
    pos("p2"),
    pos("p3"),
    pos("p4")
  };
  return data::parse_data_expression(text, variable_context);
};

template <typename T>
std::string print_set(const std::set<T>& v)
{
  std::set<std::string> s;
  for (const T& v_i: v)
  {
    s.insert(data::pp(v_i));
  }
  return core::detail::print_set(s);
}

BOOST_AUTO_TEST_CASE(test_containers)
{
  data::variable b = bool_("b");
  data::variable c = bool_("c");
  data::variable_vector v{b, c};
  BOOST_CHECK_EQUAL(print_set(find_all_variables(v)), "{ b, c }");
  BOOST_CHECK(data::search_variable(v, b));
  BOOST_CHECK(data::search_variable(v, c));

  data::variable_list l{b, c};
  BOOST_CHECK_EQUAL(print_set(find_all_variables(l)), "{ b, c }");
  BOOST_CHECK(data::search_variable(l, b));
  BOOST_CHECK(data::search_variable(l, c));
}

BOOST_AUTO_TEST_CASE(test_find)
{
  using utilities::detail::contains;

  data::variable n1 = nat("n1");
  data::variable n2 = nat("n2");
  data::variable n3 = nat("n3");
  data::variable n4 = nat("n4");

  data::variable b1 = bool_("b1");
  data::variable b2 = bool_("b2");
  data::variable b3 = bool_("b3");
  data::variable b4 = bool_("b4");

  data::variable p1 = pos("p1");
  data::variable p2 = pos("p2");
  data::variable p3 = pos("p3");
  data::variable p4 = pos("p4");

  std::set<data::variable> S{b1, p1, n1};
  BOOST_CHECK_EQUAL(print_set(find_all_variables(S)), "{ b1, n1, p1 }");
  BOOST_CHECK_EQUAL(print_set(find_sort_expressions(S)), "{ Bool, Nat, Pos }");
  BOOST_CHECK(search_variable(S, n1));
  BOOST_CHECK(!search_variable(S, n2));

  std::vector<data::variable> V{b1, p1, n1};
  BOOST_CHECK_EQUAL(print_set(find_all_variables(S)), "{ b1, n1, p1 }");
  BOOST_CHECK(search_variable(V, n1));
  BOOST_CHECK(!search_variable(V, n2));

  data::sort_expression_vector domain {data::sort_pos::pos(), data::sort_bool::bool_()};
  data::sort_expression sexpr = data::function_sort(domain, data::sort_nat::nat());
  data::variable q1(core::identifier_string("q1"), sexpr);
  BOOST_CHECK_EQUAL(print_set(find_sort_expressions(q1)), "{ Bool, Nat, Pos, Pos # Bool -> Nat }");

  data::data_expression x = parse_data_expression("(n1 == n2) && (n2 != n3)");
  BOOST_CHECK_EQUAL(print_set(find_all_variables(x)), "{ n1, n2, n3 }");
  BOOST_CHECK_EQUAL(print_set(find_free_variables(x)), "{ n1, n2, n3 }");
  BOOST_CHECK(search_variable(x, n1));
  BOOST_CHECK(search_variable(x, n2));
  BOOST_CHECK(search_variable(x, n3));
  BOOST_CHECK(!search_variable(x, n4));

  std::set<data::sort_expression> Z;
  find_sort_expressions(q1, std::inserter(Z, Z.end()));
  find_sort_expressions(S, std::inserter(Z, Z.end()));
  BOOST_CHECK_EQUAL(print_set(Z), "{ Bool, Nat, Pos, Pos # Bool -> Nat }");
}

BOOST_AUTO_TEST_CASE(find_all_variables_test)
{
  auto all_variables = [&](const std::string& text)
  {
    return print_set(data::find_all_variables(parse_data_expression(text)));
  };

  BOOST_CHECK_EQUAL(all_variables("n"), "{ n }");
  BOOST_CHECK_EQUAL(all_variables("exists n: Nat. (n == n)"), "{ n }");
  BOOST_CHECK_EQUAL(all_variables("exists n: Nat. true"), "{ n }");
  BOOST_CHECK_EQUAL(all_variables("2 whr n = n end"), "{ n }");
  BOOST_CHECK_EQUAL(all_variables("2 whr p = 3 end"), "{ p }");
}

BOOST_AUTO_TEST_CASE(find_free_variables_test)
{
  auto free_variables = [&](const std::string& text)
  {
    return print_set(data::find_free_variables(parse_data_expression(text)));
  };

  BOOST_CHECK_EQUAL(free_variables("n"), "{ n }");
  BOOST_CHECK_EQUAL(free_variables("exists n: Nat. (n == n)"), "{  }");
  BOOST_CHECK_EQUAL(free_variables("2 whr n = n end"), "{ n }");
  BOOST_CHECK_EQUAL(free_variables("2 whr p = 3 end"), "{  }");
}
