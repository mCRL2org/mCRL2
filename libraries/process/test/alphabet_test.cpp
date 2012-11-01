// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file alphabet_test.cpp
/// \brief Test for alphabet reduction.

#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>
#include <vector>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/process/alphabet.h"
#include "mcrl2/process/detail/alphabet_utility.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;
using namespace mcrl2::process;

multi_action_name parse_multi_action_name(const std::string& text)
{
  multi_action_name result;
  for (std::string::const_iterator i = text.begin(); i != text.end(); ++i)
  {
    result.insert(core::identifier_string(std::string(1, *i)));
  }
  return result;
}

multi_action_name_set parse_multi_action_name_set(const std::string& text)
{
  multi_action_name_set result;

  // remove {}
  std::string s = text.substr(1, text.size() - 2);

  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
  {
    result.insert(parse_multi_action_name(*i));
  }
  return result;
}

communication_expression_list parse_comm_set(const std::string& text)
{
  atermpp::vector<communication_expression> result;
  std::string s = text.substr(1, text.size() - 2);
  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
  {
    std::vector<std::string> w = utilities::regex_split(*i, "\\s*->\\s*");
    std::string lhs = utilities::regex_replace("\\s*\\|\\s*", "", w[0]);
    std::string rhs = w[1];
    multi_action_name beta = parse_multi_action_name(lhs);
    core::identifier_string_list alpha(beta.begin(), beta.end());
    core::identifier_string a(rhs);
    result.push_back(communication_expression(alpha, a));
  }
  return communication_expression_list(result.begin(), result.end());
}

template <typename Container>
std::string print_container(const Container& c, const std::string& start = "", const std::string& end = "", const std::string& sep = "")
{
  std::ostringstream out;
  out << start;
  for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
  {
    if (i != c.begin())
    {
      out << sep;
    }
    out << *i;
  }
  out << end;
  return out.str();
}

template <typename Container>
std::string print_set(const Container& c)
{
  return print_container(c, "{", "}", ", ");
}

std::string print(const multi_action_name& alpha)
{
  std::multiset<std::string> A;
  for (multi_action_name::const_iterator i = alpha.begin(); i != alpha.end(); ++i)
  {
    A.insert(std::string(*i));
  }
  return print_container(A);
}

std::string print(const multi_action_name_set& A)
{
  std::multiset<std::string> V;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    V.insert(print(*i));
  }
  return print_set(V);
}

void test_parse()
{
  lps::action_label_list act_decl = lps::parse_action_declaration("a: Nat;");
  lps::action a = lps::parse_action("a(2)", act_decl);
  multi_action_name A;
  A.insert(core::identifier_string("a"));
  BOOST_CHECK(name(a) == A);

  multi_action_name_set B = parse_multi_action_name_set("{a, ab}");
  std::cout << "B = " << print(B) << std::endl;
  BOOST_CHECK(print(B) == "{a, ab}");
}

void test_includes()
{
  multi_action_name alpha = parse_multi_action_name("abb");
  multi_action_name beta = parse_multi_action_name("aabb");
  BOOST_CHECK(!includes(alpha, beta));
  BOOST_CHECK(includes(beta, alpha));
}

void test_alphabet_reduce()
{
  std::string text =
    "act a;        \n"
    "proc P = a.P; \n"
    "init P;       \n"
    ;
  process_specification procspec = parse_process_specification(text);
  alphabet_reduce(procspec);
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

void test_alphabet(const std::string& expression, const std::string& expected_result, const std::string& equations = "")
{
  std::string text = "act a, b, c, d;\n" + equations + "\ninit " + expression + ";\n";
  process_specification procspec = parse_process_specification(text);
  multi_action_name_set A = alphabet(procspec.init(), procspec.equations());
  std::string result = print(A);
  check_result(expression, result, expected_result, "alphabet");
}

void test_alphabet()
{
  test_alphabet("a || b", "{a, ab, b}");
  test_alphabet("allow({ a, a | b }, a || b)", "{a, ab}");
  test_alphabet("allow({a}, a || a)", "{a}");
}

template <typename Operation>
void test_alphabet_operation(const std::string& text1, const std::string& text2, const std::string& expected_result, Operation op, const std::string& title)
{
  multi_action_name_set A1 = parse_multi_action_name_set(text1);
  multi_action_name_set A2 = parse_multi_action_name_set(text2);
  multi_action_name_set A3 = op(A1, A2);
  std::string result = print(A3);
  check_result(text1 + ", " + text2, result, expected_result, title);
}

void test_alphabet_operations()
{
  test_alphabet_operation("{a}", "{b}", "{ab}", process::concat, "concat");
  test_alphabet_operation("{ab}", "{b, c}", "{abb, abc}", process::concat, "concat");
  test_alphabet_operation("{ab, aabc}", "{b, bc}", "{a, aa, aac}", process::left_arrow, "left_arrow");
  test_alphabet_operation("{ab, b}", "{b}", "{, a}", process::left_arrow, "left_arrow");
}

void test_push_allow(const std::string& expression, const std::string& Atext, bool A_includes_subsets, const std::string& expected_result, const std::string& equations = "")
{
  std::string text = "act a, b, c, d;\n" + equations + "\ninit " + expression + ";\n";
  process_specification procspec = parse_process_specification(text);
  multi_action_name_set A = parse_multi_action_name_set(Atext);
  process::detail::push_allow_node node = process::detail::push_allow(procspec.init(), A, A_includes_subsets, procspec.equations());
  std::string result = process::pp(node.expression());
  check_result(expression, result, expected_result, "push_allow");
}

void test_push_allow()
{
  test_push_allow("a || a", "{a}", false, "allow({a}, a || a)");
}

template <typename Operation>
void test_comm_operation(const std::string& comm_text, const std::string& Atext, const std::string& expected_result, Operation op, const std::string& title)
{
  communication_expression_list C = parse_comm_set(comm_text);
  multi_action_name_set A = parse_multi_action_name_set(Atext);
  multi_action_name_set A1 = op(C, A);
  std::string result = print(A1);
  check_result(comm_text + ", " + Atext, result, expected_result, title);
}

void test_comm_operations()
{
  test_comm_operation("{a|b -> c}", "{c}", "{ab, c}", process::apply_comm_inverse, "comm_inverse");
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);
  log::mcrl2_logger::set_reporting_level(log::debug);

  test_includes();
  test_parse();
  test_alphabet_reduce();
  test_alphabet();
  test_alphabet_operations();
  test_comm_operations();
  test_push_allow();

  return EXIT_SUCCESS;
}
