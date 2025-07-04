// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print_test.cpp
/// \brief Regression test for parsing process expressions

#define BOOST_TEST_MODULE print_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/process/parse.h"
#include "mcrl2/process/print.h"

using namespace mcrl2;
using namespace mcrl2::process;

BOOST_AUTO_TEST_CASE(test_comm)
{
  std::string text =
    "act  a,b,c;\n"
    "\n"
    "init comm({a | b -> c}, c);\n"
    ;

  process_specification p = parse_process_specification(text);
  std::string text1 = process::pp(p, true);
  std::cout << text << std::endl;
  std::cout << "---" << std::endl;
  std::cout << text1 << std::endl;
  std::cout << "---" << std::endl;
  BOOST_CHECK(text == text1);
}

BOOST_AUTO_TEST_CASE(test_procinst)
{
  std::string text =
    "act  a,b;\n"
    "\n"
    "proc P = delta;\n"
    "\n"
    "init a . P + b . P;\n"
    ;

  process_specification p = parse_process_specification(text);
  std::string text1 = process::pp(p, true);
  std::cout << text << std::endl;
  std::cout << "---" << std::endl;
  std::cout << text1 << std::endl;
  std::cout << "---" << std::endl;
  BOOST_CHECK(text == text1);
}

BOOST_AUTO_TEST_CASE(test_action_name_multiset)
{
  std::vector<core::identifier_string> v;
  v.emplace_back("a");
  v.emplace_back("b");
  v.emplace_back("c");
  core::identifier_string_list l(v.begin(), v.end());
  action_name_multiset A(l);
  std::string text = process::pp(A, true);
  BOOST_CHECK(text == "a | b | c");

  atermpp::term_list<action_name_multiset> w;
  w.push_front(A);
  w.push_front(A);
  text = process::pp(w);
  BOOST_CHECK(text == "a | b | c, a | b | c");
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

void test_process_expression(const std::string& expression, const std::string& expected_result)
{
  std::string text = "act a, b, c, d;\ninit " + expression + ";\n";
  process_specification procspec = parse_process_specification(text);
  std::string result = process::pp(procspec.init());
  check_result(expression, result, expected_result, "process::pp");
}

BOOST_AUTO_TEST_CASE(test_process_expressions)
{
  std::string SPEC = "act c; init true -> c;";
  process_specification procspec = parse_process_specification(SPEC);
  BOOST_CHECK(process::pp(procspec.init()) == "true -> c");

  test_process_expression("false -> (true -> tau) <> delta", "false -> (true -> tau) <> delta");
  test_process_expression("false -> true -> tau <> delta", "false -> true -> tau <> delta");
  test_process_expression("false -> true -> a <> b <> c", "false -> (true -> a <> b) <> c");
}