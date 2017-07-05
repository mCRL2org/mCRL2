// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file print_test.cpp
/// \brief Regression test for parsing process expressions

#define MCRL2_DEBUG_EXPRESSION_BUILDER

#include <boost/test/minimal.hpp>
#include <iostream>

#include "mcrl2/process/parse.h"
#include "mcrl2/process/print.h"

using namespace mcrl2;
using namespace mcrl2::process;

void test_comm()
{
  std::string text =
    "act  a,b,c;\n"
    "\n"
    "init comm({a | b -> c}, c);\n"
    ;

  process_specification p = parse_process_specification(text);
  std::string text1 = process::pp(p);
  std::cout << text << std::endl;
  std::cout << "---" << std::endl;
  std::cout << text1 << std::endl;
  std::cout << "---" << std::endl;
  BOOST_CHECK(text == text1);
}

void test_procinst()
{
  std::string text =
    "act  a,b;\n"
    "\n"
    "proc P = delta;\n"
    "\n"
    "init a . P + b . P;\n"
    ;

  process_specification p = parse_process_specification(text);
  std::string text1 = process::pp(p);
  std::cout << text << std::endl;
  std::cout << "---" << std::endl;
  std::cout << text1 << std::endl;
  std::cout << "---" << std::endl;
  BOOST_CHECK(text == text1);
}

void test_action_name_multiset()
{
  std::vector<core::identifier_string> v;
  v.push_back(core::identifier_string("a"));
  v.push_back(core::identifier_string("b"));
  v.push_back(core::identifier_string("c"));
  core::identifier_string_list l(v.begin(), v.end());
  action_name_multiset A(l);
  std::string text = process::pp(A);
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

void test_process_expressions()
{
  std::string SPEC = "act c; init true -> c;";
  process_specification procspec = parse_process_specification(SPEC);
  BOOST_CHECK(process::pp(procspec.init()) == "true -> c");

  test_process_expression("false -> (true -> tau) <> delta", "false -> (true -> tau) <> delta");
  test_process_expression("false -> true -> tau <> delta", "false -> true -> tau <> delta");
  test_process_expression("false -> true -> a <> b <> c", "false -> (true -> a <> b) <> c");
}

int test_main(int argc, char** argv)
{
  test_comm();
  test_procinst();
  test_action_name_multiset();
  test_process_expressions();

  return EXIT_SUCCESS;
}
