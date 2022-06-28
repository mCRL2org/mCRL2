// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file replace_test.cpp
/// \brief Regression test for replace functions

#define BOOST_TEST_MODULE replace_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/replace.h"

using namespace mcrl2;
using namespace mcrl2::process;

void check_result(const std::string& expression, const std::string& result, const std::string& expected_result, const std::string& title)
{
  if (result != expected_result)
  {
    std::cout << "--- failure in " << title << " ---" << std::endl;
    std::cout << "expression      = " << expression << std::endl;
    BOOST_CHECK_EQUAL(result, expected_result);
  }
}

inline
data::variable make_bool(const std::string& s)
{
  return data::variable(s, data::sort_bool::bool_());
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
process_expression parse_expression(const std::string& text)
{
  std::string DATA_DECL =
    "glob k, m, n, k1, m1, n1, k2, m2, n2: Bool;\n"
    "act a, b, c: Bool;                         \n"
    ;
  std::string PROC_DECL = "proc P(n: Nat); proc Q(n: Nat); proc R(n: Nat); proc S(n: Nat);\n";
  return process::parse_process_expression(text, DATA_DECL, PROC_DECL);
}

/// \brief Parses a string of the form "b: Bool := v, c: Bool := !w", and adds
inline
data::mutable_map_substitution<> parse_substitution(const std::string& text, const std::vector<data::variable>& variables = variable_context())
{
  data::mutable_map_substitution<> sigma;
  std::vector<std::string> substitutions = utilities::split(text, ";");
  for (auto & substitution : substitutions)
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

// Returns the free variables in the right hand side of sigma.
std::set<data::variable> sigma_variables(const data::mutable_map_substitution<>& sigma)
{
  std::set<data::variable> result;
  for (const auto & i : sigma)
  {
    std::set<data::variable> V = data::find_free_variables(i.second);
    V.erase(i.first);
    result.insert(V.begin(), V.end());
  }
  return result;
}

void test_replace_variables_capture_avoiding(const std::string& x_text, const std::string& sigma_text, const std::string& expected_result)
{
  process_expression x = parse_expression(x_text);
  data::mutable_map_substitution<> sigma = parse_substitution(sigma_text);
  std::string result = process::pp(process::replace_variables_capture_avoiding(x, sigma));
  check_result(x_text + " sigma = " + sigma_text, result, expected_result, "replace_variables_capture_avoiding");
}

BOOST_AUTO_TEST_CASE(replace_variables_capture_avoiding_test)
{
  test_replace_variables_capture_avoiding("sum n: Bool. a(n) . sum k: Bool. a(k) . a(m)", "m: Bool := n", "sum n1: Bool. a(n1) . (sum k1: Bool. a(k1) . a(n))");
}

BOOST_AUTO_TEST_CASE(process_instance_assignment_test)
{
  std::string text =
    "proc P(b: Bool, c: Bool) = P(c = true); \n"
    "                                        \n"
    "init P(true, true);                     \n"
    ;

  process_specification p = parse_process_specification(text);
  data::mutable_map_substitution<> sigma;
  data::variable b("b", data::sort_bool::bool_());
  data::variable c("c", data::sort_bool::bool_());
  sigma[c] = data::sort_bool::false_();

  process_expression x = p.equations().front().expression();
  process_expression x1 = process::replace_variables_capture_avoiding(x, sigma);
  std::cerr << process::pp(x1) << std::endl;
  BOOST_CHECK(process::pp(x1) == "P(c = true)");

  sigma[b] = data::sort_bool::false_();
  process_expression x2 = process::replace_variables_capture_avoiding(x, sigma);
  std::cerr << process::pp(x2) << std::endl;
  BOOST_CHECK(process::pp(x2) == "P(b = false, c = true)");
}

BOOST_AUTO_TEST_CASE(replace_process_identifiers_test)
{
  std::string text =
    "proc P(b: Bool, c: Bool) = P(c = true) . P(false, false); \n"
    "proc Q(b: Bool, c: Bool) = P(c = true);                   \n"
    "proc R(b: Bool, c: Bool) = Q(c = true) . Q(false, false); \n"
    "                                                          \n"
    "init P(true, true);                                       \n"
    ;

  process_specification p = parse_process_specification(text);
  process_equation eqP = p.equations()[0];
  process_equation eqQ = p.equations()[1];
  process_equation eqR = p.equations()[2];

  process_expression rhs = replace_process_identifiers(eqP.expression(), process_identifier_assignment(eqP.identifier(), eqQ.identifier()));
  BOOST_CHECK(rhs == eqR.expression());
}
