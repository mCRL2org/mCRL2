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
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/process/alphabet.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/utilities/test_utilities.h"

using mcrl2::utilities::collect_after_test_case;
using namespace mcrl2;
using namespace mcrl2::process;

BOOST_GLOBAL_FIXTURE(collect_after_test_case)

struct LogDebug
{
  LogDebug()
  {
    log::mcrl2_logger::set_reporting_level(log::debug);
  }
};
BOOST_GLOBAL_FIXTURE(LogDebug);

inline
multi_action_name name(const lps::action& x)
{
  multi_action_name result;
  result.insert(x.label().name());
  return result;
}

inline
multi_action_name name(const multi_action& x)
{
  multi_action_name result;
  lps::action_list a = x.actions();
  for (lps::action_list::iterator i = a.begin(); i != a.end(); ++i)
  {
    result.insert(i->label().name());
  }
  return result;
}

inline
multi_action_name name(const core::identifier_string& x)
{
  multi_action_name result;
  result.insert(x);
  return result;
}

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

action_name_multiset_list parse_allow_set(const std::string& text)
{
  atermpp::vector<action_name_multiset> result;
  std::string s = text.substr(1, text.size() - 2);
  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
  {
    std::string word = utilities::regex_replace("\\s*\\|\\s*", "", *i);
    multi_action_name alpha = parse_multi_action_name(word);
    result.push_back(action_name_multiset(core::identifier_string_list(alpha.begin(), alpha.end())));
  }
  return action_name_multiset_list(result.begin(), result.end());
}

core::identifier_string_list parse_block_set(const std::string& text)
{
  atermpp::vector<core::identifier_string> result;
  std::string s = text.substr(1, text.size() - 2);
  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
  {
    result.push_back(core::identifier_string(*i));
  }
  return core::identifier_string_list(result.begin(), result.end());
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

rename_expression_list parse_rename_set(const std::string& text)
{
  atermpp::vector<rename_expression> result;
  std::string s = text.substr(1, text.size() - 2);
  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
  {
    std::vector<std::string> w = utilities::regex_split(*i, "\\s*->\\s*");
    result.push_back(rename_expression(core::identifier_string(w[0]), core::identifier_string(w[1])));
  }
  return rename_expression_list(result.begin(), result.end());
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

BOOST_AUTO_TEST_CASE(test_parse)
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

BOOST_AUTO_TEST_CASE(test_includes)
{
  multi_action_name alpha = parse_multi_action_name("abb");
  multi_action_name beta = parse_multi_action_name("aabb");
  BOOST_CHECK(!detail::includes(alpha, beta));
  BOOST_CHECK(detail::includes(beta, alpha));
}

BOOST_AUTO_TEST_CASE(test_alphabet_reduce)
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

BOOST_AUTO_TEST_CASE(test_alphabet1)
{
  test_alphabet("a || b", "{a, ab, b}");
  test_alphabet("allow({ a, a | b }, a || b)", "{a, ab}");
  test_alphabet("allow({a}, a || a)", "{a}");
  test_alphabet("a", "{a}");
  test_alphabet("c|c", "{cc}");
  test_alphabet("a.c|c", "{a, cc}");
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

BOOST_AUTO_TEST_CASE(test_alphabet_operations)
{
  test_alphabet_operation("{a}", "{b}", "{ab}", process::concat, "concat");
  test_alphabet_operation("{ab}", "{b, c}", "{abb, abc}", process::concat, "concat");
  test_alphabet_operation("{ab, aabc}", "{b, bc}", "{a, aa, aac}", process::left_arrow1, "left_arrow1");
  test_alphabet_operation("{aa, b}", "{a}", "{a}", process::left_arrow1, "left_arrow1");
  test_alphabet_operation("{ab, b}", "{b}", "{a}", process::left_arrow1, "left_arrow1"); // N.B. tau is excluded!
}

void test_push_allow(const std::string& expression, const std::string& Atext, bool A_includes_subsets, const std::string& expected_result, const std::string& equations = "")
{
  std::string text = "act a, b, c, d;\n" + equations + "\ninit " + expression + ";\n";
  process_specification procspec = parse_process_specification(text);
  multi_action_name_set A = parse_multi_action_name_set(Atext);
  process::detail::push_allow_node node = process::detail::push_allow(procspec.init(), A, A_includes_subsets, procspec.equations());
  node.finish(A, A_includes_subsets);
  std::string result = process::pp(node.m_expression);
  check_result(expression, result, expected_result, "push_allow");
}

BOOST_AUTO_TEST_CASE(test_push_allow1)
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

BOOST_AUTO_TEST_CASE(test_comm_operations)
{
  test_comm_operation("{a|b -> c}", "{c}", "{ab, c}", process::apply_comm_inverse, "apply_comm_inverse");
  test_comm_operation("{a|a -> b}", "{b, bb}", "{aa, aaaa, aab, b, bb}", process::apply_comm_inverse, "apply_comm_inverse");
  test_comm_operation("{a|b -> c}", "{ab, aab, aabb, abd}", "{aab, aabb, ab, abc, abd, ac, c, cc, cd}", process::apply_comm, "apply_comm");
}

template <typename Operation>
void test_rename_operation(const std::string& rename_text, const std::string& Atext, const std::string& expected_result, Operation op, const std::string& title)
{
  rename_expression_list R = parse_rename_set(rename_text);
  multi_action_name_set A = parse_multi_action_name_set(Atext);
  multi_action_name_set A1 = op(R, A);
  std::string result = print(A1);
  check_result(rename_text + ", " + Atext, result, expected_result, title);
}

BOOST_AUTO_TEST_CASE(test_rename_operations)
{
  test_rename_operation("{a -> b, c -> d}", "{ab, aacc}", "{bb, bbdd}", process::apply_rename, "apply_rename");
  test_rename_operation("{a -> b, c -> d}", "{abd, bcdd}", "{aac, accc}", process::apply_rename_inverse, "apply_rename_inverse");
}

void test_allow(const std::string& allow_text, const std::string& Atext, const std::string& expected_result, const std::string& title)
{
  action_name_multiset_list V = parse_allow_set(allow_text);
  multi_action_name_set A = parse_multi_action_name_set(Atext);
  multi_action_name_set A1 = apply_allow(V, A);
  std::string result = print(A1);
  check_result(allow_text + ", " + Atext, result, expected_result, title);
}

BOOST_AUTO_TEST_CASE(test_allow1)
{
  test_allow("{a|b, a|b|b, c}", "{ab, abbc, c}", "{ab, c}", "apply_allow");
}

void test_block(const std::string& block_text, const std::string& Atext, const std::string& expected_result, const std::string& title)
{
  core::identifier_string_list B = parse_block_set(block_text);
  multi_action_name_set A = parse_multi_action_name_set(Atext);
  multi_action_name_set A1 = apply_block(B, A);
  std::string result = print(A1);
  check_result(block_text + ", " + Atext, result, expected_result, title);
}

BOOST_AUTO_TEST_CASE(test_block1)
{
  test_block("{b}", "{ab, abbc, c}", "{c}", "apply_block");
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv);
  return EXIT_SUCCESS;
}
