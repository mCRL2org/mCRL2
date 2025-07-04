// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file replace_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE replace_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/parse.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/assignment_sequence_substitution.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/substitutions/sequence_sequence_substitution.h"

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
std::vector<data::variable> variable_context()
{
  return std::vector<data::variable>
  {
    bool_("b"),
    bool_("b1"),
    bool_("b2"),
    bool_("b3"),
    bool_("b4"),
    bool_("k"),
    bool_("m"),
    bool_("n"),
    bool_("v"),
    bool_("w"),
    bool_("x"),
    bool_("y"),
    bool_("z"),
    bool_("k1"),
    bool_("m1"),
    bool_("n1"),
    bool_("v1"),
    bool_("w1"),
    bool_("x1"),
    bool_("y1"),
    bool_("z1"),
    bool_("k2"),
    bool_("m2"),
    bool_("n2"),
    bool_("v2"),
    bool_("w2"),
    bool_("x2"),
    bool_("y2"),
    bool_("z2")
  };
}

inline
data::data_expression parse_data_expression(const std::string& text)
{
  return data::parse_data_expression(text, variable_context());
};

/// \brief Parses a string of the form "b: Bool := v, c: Bool := !w", and adds
inline
data::mutable_map_substitution<> parse_substitution(const std::string& text, const std::vector<data::variable>& variables = variable_context())
{
  data::mutable_map_substitution<> sigma;
  std::vector<std::string> substitutions = utilities::split(text, ";");
  for (const std::string& substitution: substitutions)
  {
    std::vector<std::string> words = utilities::regex_split(substitution, ":=");
    if (words.size() != 2)
    {
      continue;
    }
    data::variable v = data::parse_variable(words[0]);
    data::data_expression e = data::parse_data_expression(words[1], variables);
    sigma[v] = e;
  }
  return sigma;
}

BOOST_AUTO_TEST_CASE(test_assignment_list)
{
  using namespace mcrl2::data::sort_bool;

  data::variable d1("d1", data::basic_sort("D"));
  data::variable d2("d2", data::basic_sort("D"));
  data::variable d3("d3", data::basic_sort("D"));
  data::variable e1("e1", data::basic_sort("D"));
  data::variable e2("e2", data::basic_sort("D"));
  data::variable e3("e3", data::basic_sort("D"));

  data::assignment_vector l;
  l.emplace_back(d1, e1);
  l.emplace_back(e1, e2);
  l.emplace_back(e2, e3);

  data::data_expression t  = and_(equal_to(d1, e1), not_equal_to(e2, d3));
  data::data_expression t0 = and_(equal_to(e1, e2), not_equal_to(e3, d3));
  data::data_expression t2 = data::replace_variables(t, data::assignment_sequence_substitution(data::assignment_list(l.begin(), l.end())));
  BOOST_CHECK(t0 == t2);
}

BOOST_AUTO_TEST_CASE(test_variable_replace)
{
  using namespace mcrl2::data::sort_bool;

  data::variable d1("d1", data::basic_sort("D"));
  data::variable d2("d2", data::basic_sort("D"));
  data::variable d3("d3", data::basic_sort("D"));
  data::variable x("x", data::basic_sort("D"));
  data::variable y("y", data::basic_sort("D"));
  data::variable z("z", data::basic_sort("D"));

  data::variable_vector variables{d1, d2, d3};
  data::data_expression_vector replacements{x, y, z};
  std::vector<data::variable> v{d1, d2, d3};
  std::list<data::data_expression> l{x, y, z};

  data::data_expression t  = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  data::data_expression t1 = data::replace_variables(t, make_sequence_sequence_substitution(variables, replacements));
  data::data_expression t2 = data::replace_variables(t, make_sequence_sequence_substitution(v, l));
  BOOST_CHECK(t1 == t2);

  t = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  BOOST_CHECK(t1 == replace_variables(t, make_sequence_sequence_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_variables(t, make_sequence_sequence_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_variables(t, make_sequence_sequence_substitution(v, l)));
  BOOST_CHECK(t1 == replace_variables(t, make_mutable_map_substitution(variables, replacements)));
}

BOOST_AUTO_TEST_CASE(test_replace_with_binders)
{
  data::mutable_map_substitution< > sigma;
  data::data_expression input1(data::variable("c", data::sort_bool::bool_()));
  data::data_expression input2(data::parse_data_expression("exists b: Bool, c: Bool. if(b, c, b)"));

  sigma[data::variable("c", data::sort_bool::bool_())] = data::sort_bool::false_();

  BOOST_CHECK(replace_free_variables(input1, sigma) == data::sort_bool::false_());

  // variable c is bound and should not be replaced
  BOOST_CHECK(replace_free_variables(input2, sigma) == input2);
}

BOOST_AUTO_TEST_CASE(test_variables)
{
  data::variable d1 = bool_("d1");
  data::variable d2 = bool_("d2");
  data::variable d3 = bool_("d3");
  data::variable d4 = bool_("d4");
  data::data_expression e1 = data::sort_bool::not_(d1);
  data::data_expression e2 = data::sort_bool::not_(d2);
  data::data_expression e3 = data::sort_bool::not_(d3);

  data::mutable_map_substitution<> sigma;
  sigma[d1] = e1;
  sigma[d2] = e2;
  sigma[d3] = e3;

  // the variable in an assignment is not replaced by replace_free_variables
  data::assignment a(d1, d4);
  data::assignment b = replace_free_variables(a, sigma);
  BOOST_CHECK(b == a);

  // the variable in an assignment is not replaced by replace_variables
  data::assignment c = replace_variables(a, sigma);
  BOOST_CHECK(c == a);

  // the variable d1 in the right hand side is replaced by replace_free_variables, since
  // we do not consider the left hand side a binding variable
  a = data::assignment(d1, data::sort_bool::and_(d1, d2));
  b = replace_free_variables(a, sigma);
  BOOST_CHECK(b == data::assignment(d1, data::sort_bool::and_(e1, e2)));

  // the variable d1 in the right hand side is replaced by replace_free_variables
  c = replace_variables(a, sigma);
  BOOST_CHECK(c == data::assignment(d1, data::sort_bool::and_(e1, e2)));

  // this will lead to an assertion failure, because an attempt will be made to store
  // a data expression in a variable
  sigma[d1] = data::sort_bool::and_(d1, d2);
  // data::data_expression z1 = replace_variables(d1, sigma);

  // therefore one should first convert d1 to a data expression:
  data::data_expression z2 = replace_variables(data::data_expression(d1), sigma);
  BOOST_CHECK(z2 == data::sort_bool::and_(d1, d2));
}

void check_result(const std::string& expression, const std::string& result, const std::string& expected_result, const std::string& title)
{
  if (result != expected_result)
  {
    std::cout << "--- failure in " << title << " ---" << std::endl;
    std::cout << "expression      = " << expression << std::endl;
    BOOST_CHECK_EQUAL(result, expected_result);
  }
}

void test_replace_variables_capture_avoiding(const std::string& x_text, const std::string& sigma_text, const std::string& expected_result)
{
  data::data_expression x = parse_data_expression(x_text);
  data::mutable_map_substitution<> sigma = parse_substitution(sigma_text);
  std::string result = data::pp(data::replace_variables_capture_avoiding(x, sigma));
  check_result(x_text + " sigma = " + sigma_text, result, expected_result, "replace_variables_capture_avoiding");
}

BOOST_AUTO_TEST_CASE(replace_variables_capture_avoiding_test)
{
  test_replace_variables_capture_avoiding("v", "v: Bool := w", "w");
  test_replace_variables_capture_avoiding("forall x: Bool . x => y", "x: Bool := z", "forall x1: Bool. x1 => y");
  test_replace_variables_capture_avoiding("forall x: Bool . x => y", "y: Bool := z", "forall x1: Bool. x1 => z");
  test_replace_variables_capture_avoiding("forall x: Bool . x => y", "y: Bool := x", "forall x1: Bool. x1 => x");
  test_replace_variables_capture_avoiding("forall x: Bool . x => x1 => y", "y: Bool := x", "forall x2: Bool. x2 => x1 => x");
  test_replace_variables_capture_avoiding("x => x1 => y whr x = y end", "y: Bool := x", "x2 => x1 => x whr x2 = x end");
  test_replace_variables_capture_avoiding("forall n: Bool. n => forall k: Bool. k => m", "m: Bool := n", "forall n1: Bool. n1 => (forall k1: Bool. k1 => n)");
  test_replace_variables_capture_avoiding("forall n: Bool. n => forall n: Bool. n => m", "m: Bool := n", "forall n1: Bool. n1 => (forall n2: Bool. n2 => n)");
  test_replace_variables_capture_avoiding("forall n: Bool. n => forall k: Bool. k => m", "m: Bool := n", "forall n1: Bool. n1 => (forall k1: Bool. k1 => n)");
  test_replace_variables_capture_avoiding("forall n: Bool. n => forall n: Bool. n => m", "m: Bool := n", "forall n1: Bool. n1 => (forall n2: Bool. n2 => n)");
}

BOOST_AUTO_TEST_CASE(test_replace_free_variables)
{
  data::mutable_map_substitution<> sigma;
  data::variable x("x", data::sort_bool::bool_());
  data::data_expression y = data::sort_bool::not_(x);
  sigma[x] = y;
  data::assignment a(x, x);
  data::assignment b(x, y);
  data::assignment c = data::replace_free_variables(a, sigma);
  BOOST_CHECK(b == c);

  data::assignment_list va{a};
  data::assignment_list vb{b};
  data::assignment_list vc = data::replace_free_variables(va, sigma);
  BOOST_CHECK_EQUAL(vb, vc);
}

BOOST_AUTO_TEST_CASE(test_ticket_1209)
{
  std::string text = "n whr n = m, m = 3 end whr m = 255 end";
  std::string expected_result = "n1 whr n1 = m2, m3 = 3 end whr m2 = 255 end";
  data::data_expression x = data::parse_data_expression(text);
  data::mutable_map_substitution<> sigma;
  data::variable m("m", data::sort_pos::pos());
  data::variable m1("m1", data::sort_pos::pos());
  sigma[m] = m1;
  data::data_expression x1 = data::replace_variables_capture_avoiding(x, sigma);
  std::string result = data::pp(x1);
  BOOST_CHECK_EQUAL(result, expected_result);
}
