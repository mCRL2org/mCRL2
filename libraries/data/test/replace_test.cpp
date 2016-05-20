// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file replace_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <vector>
#include <boost/test/minimal.hpp>

#include "mcrl2/data/variable.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/substitutions/assignment_sequence_substitution.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/substitutions/sequence_sequence_substitution.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;
using namespace mcrl2::data;

void test_assignment_list()
{
  using namespace mcrl2::data::sort_bool;

  std::cerr << "assignment_list replace" << std::endl;

  variable d1("d1", basic_sort("D"));
  variable d2("d2", basic_sort("D"));
  variable d3("d3", basic_sort("D"));
  variable e1("e1", basic_sort("D"));
  variable e2("e2", basic_sort("D"));
  variable e3("e3", basic_sort("D"));

  assignment_vector l;
  l.push_back(assignment(d1, e1));
  l.push_back(assignment(e1, e2));
  l.push_back(assignment(e2, e3));

  data_expression t  = and_(equal_to(d1, e1), not_equal_to(e2, d3));
  data_expression t0 = and_(equal_to(e1, e2), not_equal_to(e3, d3));
  data_expression t2 = data::replace_variables(t, assignment_sequence_substitution(assignment_list(l.begin(), l.end())));
  std::cerr << "t  == " << data::pp(t) << std::endl;
  std::cerr << "t2 == " << data::pp(t2) << std::endl;
  BOOST_CHECK(t0 == t2);
}

void test_variable_replace()
{
  using namespace mcrl2::data::sort_bool;

  std::cerr << "variable replace" << std::endl;

  variable d1("d1", basic_sort("D"));
  variable d2("d2", basic_sort("D"));
  variable d3("d3", basic_sort("D"));
  variable_vector variables;
  variables.push_back(d1);
  variables.push_back(d2);
  variables.push_back(d3);

  variable x("x", basic_sort("D"));
  variable y("y", basic_sort("D"));
  variable z("z", basic_sort("D"));
  data_expression e1 = x;
  data_expression e2 = z;
  data_expression e3 = y;
  data_expression_vector replacements;
  replacements.push_back(e1);
  replacements.push_back(e2);
  replacements.push_back(e3);

  std::vector<variable> v;
  v.push_back(d1);
  v.push_back(d2);
  v.push_back(d3);
  std::list<data_expression> l;
  l.push_back(e1);
  l.push_back(e2);
  l.push_back(e3);

  data_expression t  = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  data_expression t1 = data::replace_variables(t, make_sequence_sequence_substitution(variables, replacements));
  data_expression t2 = data::replace_variables(t, make_sequence_sequence_substitution(v, l));
  std::cerr << "t  == " << data::pp(t) << std::endl;
  std::cerr << "t1 == " << data::pp(t1) << std::endl;
  std::cerr << "t2 == " << data::pp(t2) << std::endl;
  BOOST_CHECK(t1 == t2);

  t = and_(equal_to(d1, d2), not_equal_to(d2, d3));
  BOOST_CHECK(t1 == replace_variables(t, make_sequence_sequence_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_variables(t, make_sequence_sequence_substitution(variables, replacements)));
  BOOST_CHECK(t1 == replace_variables(t, make_sequence_sequence_substitution(v, l)));
  BOOST_CHECK(t1 == replace_variables(t, make_mutable_map_substitution(variables, replacements)));
}

void test_replace_with_binders()
{
  std::cerr << "replace with binders" << std::endl;
  mutable_map_substitution< > sigma;
  data_expression input1(variable("c", sort_bool::bool_()));
  data_expression input2(parse_data_expression("exists b: Bool, c: Bool. if(b, c, b)"));

  sigma[variable("c", sort_bool::bool_())] = sort_bool::false_();

  BOOST_CHECK(replace_free_variables(input1, sigma) == sort_bool::false_());

  // variable c is bound and should not be replaced
  BOOST_CHECK(replace_free_variables(input2, sigma) == input2);
}

inline
variable make_bool(const std::string& s)
{
  return variable(s, sort_bool::bool_());
}

void test_variables()
{
  variable d1 = make_bool("d1");
  variable d2 = make_bool("d2");
  variable d3 = make_bool("d3");
  variable d4 = make_bool("d4");
  data_expression e1 = data::sort_bool::not_(d1);
  data_expression e2 = data::sort_bool::not_(d2);
  data_expression e3 = data::sort_bool::not_(d3);

  mutable_map_substitution<> sigma;
  sigma[d1] = e1;
  sigma[d2] = e2;
  sigma[d3] = e3;

  // the variable in an assignment is not replaced by replace_free_variables
  assignment a(d1, d4);
  assignment b = replace_free_variables(a, sigma);
  BOOST_CHECK(b == a);

  // the variable in an assignment is not replaced by replace_variables
// TODO: this does no longer work, so it should be fixed
//  assignment c = replace_variables(a, sigma);
//  BOOST_CHECK(c == a);

  // the variable d1 in the right hand side is replaced by replace_free_variables, since
  // we do not consider the left hand side a binding variable
  a = assignment(d1, sort_bool::and_(d1, d2));
  b = replace_free_variables(a, sigma);
  BOOST_CHECK(b == assignment(d1, sort_bool::and_(e1, e2)));

  // the variable d1 in the right hand side is replaced by replace_free_variables
// TODO: this does no longer work, so it should be fixed
//  c = replace_variables(a, sigma);
//  BOOST_CHECK(c == assignment(d1, sort_bool::and_(e1, e2)));

  // this will lead to an assertion failure, because an attempt will be made to store
  // a data expression in a variable
  sigma[d1] = sort_bool::and_(d1, d2);
  // data_expression d = replace_variables(d1, sigma);

  // therefore one should first convert d1 to a data expression:
  data_expression d = replace_variables(data_expression(d1), sigma);
  BOOST_CHECK(d == sort_bool::and_(d1, d2));
}

void check_result(const std::string& expression, const std::string& result, const std::string& expected_result, const std::string& title)
{
  if (result != expected_result)
  {
    std::cout << "--- failure in " << title << " ---" << std::endl;
    std::cout << "expression      = " << expression << std::endl;
    std::cout << "result          = " << result << std::endl;
    std::cout << "expected result = " << expected_result << std::endl;
    BOOST_CHECK(result == expected_result);
  }
}

inline
std::vector<data::variable> variable_context()
{
  std::vector<data::variable> result;
  result.push_back(make_bool("k"));
  result.push_back(make_bool("m"));
  result.push_back(make_bool("n"));
  result.push_back(make_bool("v"));
  result.push_back(make_bool("w"));
  result.push_back(make_bool("x"));
  result.push_back(make_bool("y"));
  result.push_back(make_bool("z"));
  result.push_back(make_bool("k1"));
  result.push_back(make_bool("m1"));
  result.push_back(make_bool("n1"));
  result.push_back(make_bool("v1"));
  result.push_back(make_bool("w1"));
  result.push_back(make_bool("x1"));
  result.push_back(make_bool("y1"));
  result.push_back(make_bool("z1"));
  result.push_back(make_bool("k2"));
  result.push_back(make_bool("m2"));
  result.push_back(make_bool("n2"));
  result.push_back(make_bool("v2"));
  result.push_back(make_bool("w2"));
  result.push_back(make_bool("x2"));
  result.push_back(make_bool("y2"));
  result.push_back(make_bool("z2"));
  return result;
}

inline
data::data_expression parse_expression(const std::string& text, const std::vector<data::variable>& variables = variable_context())
{
  return data::parse_data_expression(text, variables);
}

/// \brief Parses a string of the form "b: Bool := v, c: Bool := !w", and adds
inline
data::mutable_map_substitution<> parse_substitution(const std::string& text, const std::vector<data::variable>& variables = variable_context())
{
  data::mutable_map_substitution<> sigma;
  std::vector<std::string> substitutions = utilities::split(text, ";");
  for (std::vector<std::string>::iterator i = substitutions.begin(); i != substitutions.end(); ++i)
  {
    std::vector<std::string> words = utilities::regex_split(*i, ":=");
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

// Returns the free variables in the right hand side of sigma.
std::set<data::variable> sigma_variables(const data::mutable_map_substitution<>& sigma)
{
  std::set<data::variable> result;
  for (mutable_map_substitution<>::const_iterator i = sigma.begin(); i != sigma.end(); ++i)
  {
    std::set<data::variable> V = data::find_free_variables(i->second);
    V.erase(i->first);
    result.insert(V.begin(), V.end());
  }
  return result;
}

void test_replace_variables_capture_avoiding(const std::string& x_text, const std::string& sigma_text, const std::string& expected_result)
{
  data::data_expression x = parse_expression(x_text);
  data::mutable_map_substitution<> sigma = parse_substitution(sigma_text);
  std::set<data::variable> sv = sigma_variables(sigma);
  std::string result = data::pp(data::replace_variables_capture_avoiding(x, sigma, sv));
  check_result(x_text + " sigma = " + sigma_text, result, expected_result, "replace_variables_capture_avoiding");
}

void test_replace_variables_capture_avoiding()
{
  test_replace_variables_capture_avoiding("v", "v: Bool := w", "w");
  test_replace_variables_capture_avoiding("forall x: Bool . x => y", "x: Bool := z", "forall x1: Bool. x1 => y");
  test_replace_variables_capture_avoiding("forall x: Bool . x => y", "y: Bool := z", "forall x: Bool. x => z");
  test_replace_variables_capture_avoiding("forall x: Bool . x => y", "y: Bool := x", "forall x1: Bool. x1 => x");
  test_replace_variables_capture_avoiding("forall x: Bool . x => x1 => y", "y: Bool := x", "forall x2: Bool. x2 => x1 => x");
  test_replace_variables_capture_avoiding("x => x1 => y whr x = y end", "y: Bool := x", "x2 => x1 => x whr x2 = x end");
  test_replace_variables_capture_avoiding("forall n: Bool. n => forall k: Bool. k => m", "m: Bool := n", "forall n1: Bool. n1 => (forall k: Bool. k => n)");
  test_replace_variables_capture_avoiding("forall n: Bool. n => forall n: Bool. n => m", "m: Bool := n", "forall n1: Bool. n1 => (forall n2: Bool. n2 => n)");
  test_replace_variables_capture_avoiding("forall n: Bool. n => forall k: Bool. k => m", "m: Bool := n", "forall n1: Bool. n1 => (forall k: Bool. k => n)");
  test_replace_variables_capture_avoiding("forall n: Bool. n => forall n: Bool. n => m", "m: Bool := n", "forall n1: Bool. n1 => (forall n2: Bool. n2 => n)");
}

void test_replace_free_variables()
{
  data::mutable_map_substitution<> sigma;
  data::variable x("x", data::sort_bool::bool_());
  data::data_expression y = data::sort_bool::not_(x);
  sigma[x] = y;
  data::assignment a(x, x);
  data::assignment b(x, y);
  data::assignment c = data::replace_free_variables(a, sigma);
  BOOST_CHECK(b == c);

  data::assignment_list va;
  va.push_front(a);
  data::assignment_list vb;
  vb.push_front(b);
  data::assignment_list vc = data::replace_free_variables(va, sigma);
  BOOST_CHECK(vb == vc);
}

void test_ticket_1209()
{
  std::string text = "n whr n = m, m = 3 end whr m = 255 end";
  std::string expected_result = "n whr n = m2, m3 = 3 end whr m2 = 255 end";
  data::data_expression x = data::parse_data_expression(text);
  data::mutable_map_substitution<> sigma;
  data::variable m("m", data::sort_pos::pos());
  data::variable m1("m1", data::sort_pos::pos());
  sigma[m] = m1;
  std::set<data::variable> v;
  v.insert(m1);
  data::data_expression x1 = data::replace_variables_capture_avoiding(x, sigma, v);
  std::string result = data::pp(x1);
  BOOST_CHECK(result == expected_result);
}

int test_main(int argc, char** argv)
{
  test_assignment_list();
  test_variable_replace();
  test_replace_with_binders();
  test_variables();
  test_replace_variables_capture_avoiding();
  test_replace_free_variables();
  test_ticket_1209();

  return 0;
}
