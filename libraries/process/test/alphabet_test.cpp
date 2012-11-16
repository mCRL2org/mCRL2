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
#include <boost/algorithm/string/predicate.hpp>
#include <boost/tuple/tuple.hpp>
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

std::pair<multi_action_name_set, bool> parse_multi_action_name_set(const std::string& text)
{
  multi_action_name_set result;
  bool result_includes_subsets = boost::algorithm::ends_with(text, "*");

  // remove {}*
  std::string s = utilities::regex_replace("[{}*]", "", text);

  std::vector<std::string> v = utilities::regex_split(s, "\\s*,\\s*");
  for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
  {
    result.insert(parse_multi_action_name(*i));
  }
  return std::make_pair(result, result_includes_subsets);
}

action_name_multiset_list parse_allow_set(const std::string& text)
{
  std::vector<action_name_multiset> result;
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
  std::vector<core::identifier_string> result;
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
  std::vector<communication_expression> result;
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
  std::vector<rename_expression> result;
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
  if (alpha.empty())
  {
    return "tau";
  }
  std::multiset<std::string> A;
  for (multi_action_name::const_iterator i = alpha.begin(); i != alpha.end(); ++i)
  {
    A.insert(std::string(*i));
  }
  return print_container(A);
}

std::string print(const multi_action_name_set& A, bool A_includes_subsets = false)
{
  std::multiset<std::string> V;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    V.insert(print(*i));
  }
  return print_set(V) + (A_includes_subsets ? "*" : "");
}

BOOST_AUTO_TEST_CASE(test_print)
{
  multi_action_name_set A;
  bool dummy;
  boost::tuples::tie(A, dummy) = parse_multi_action_name_set("{a}");
  multi_action_name tau;
  A.insert(tau);
  std::cout << print(A) << std::endl;
  BOOST_CHECK(print(A) == "{a, tau}");
}

BOOST_AUTO_TEST_CASE(test_parse)
{
  lps::action_label_list act_decl = lps::parse_action_declaration("a: Nat;");
  lps::action a = lps::parse_action("a(2)", act_decl);
  multi_action_name A;
  A.insert(core::identifier_string("a"));
  BOOST_CHECK(name(a) == A);

  multi_action_name_set B;
  bool dummy;
  boost::tuples::tie(B, dummy) = parse_multi_action_name_set("{a, ab}");
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

void test_subsets(const std::string& Atext, const std::string& expected_result)
{
  multi_action_name_set A;
  bool dummy;
  boost::tuples::tie(A, dummy) = parse_multi_action_name_set(Atext);
  multi_action_name_set A1 = alphabet_operations::subsets(A);
  std::string result = print(A1);
  check_result(Atext, result, expected_result, "alphabet");
}

BOOST_AUTO_TEST_CASE(test_subsets1)
{
  test_subsets("{ab, c}", "{a, ab, b, c}");
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
  test_alphabet("tau.a", "{a, tau}");
}

template <typename Operation>
void test_alphabet_operation(const std::string& text1, const std::string& text2, const std::string& expected_result, Operation op, const std::string& title)
{
  bool dummy;
  multi_action_name_set A1, A2;
  boost::tuples::tie(A1, dummy) = parse_multi_action_name_set(text1);
  boost::tuples::tie(A2, dummy) = parse_multi_action_name_set(text2);
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
  test_alphabet_operation("{bc}", "{c}", "{b}", process::left_arrow1, "left_arrow1");
}

void test_push_allow(const std::string& expression, const std::string& Atext, const std::string& expected_result, const std::string& equations = "")
{
  std::string text = "act a, b, c, d;\n" + equations + "\ninit " + expression + ";\n";
  process_specification procspec = parse_process_specification(text);
  multi_action_name_set A;
  bool A_includes_subsets;
  boost::tuples::tie(A, A_includes_subsets) = parse_multi_action_name_set(Atext);
  process::detail::push_allow_node node = process::detail::push_allow(procspec.init(), allow_set(A, A_includes_subsets), procspec.equations());
  node.finish(procspec.equations(), allow_set(A, A_includes_subsets));
  std::string result = process::pp(node.m_expression);
  check_result(expression, result, expected_result, "push_allow");
}

BOOST_AUTO_TEST_CASE(test_push_allow1)
{
  test_push_allow("a || a", "{a}", "allow({a}, a || a)");
}

template <typename Operation>
void test_comm_operation(const std::string& comm_text, const std::string& Atext, const std::string& expected_result, Operation op, const std::string& title)
{
  communication_expression_list C = parse_comm_set(comm_text);
  multi_action_name_set A;
  bool A_includes_subsets;
  boost::tuples::tie(A, A_includes_subsets) = parse_multi_action_name_set(Atext);
  multi_action_name_set A1 = op(C, A, A_includes_subsets);
  std::string result = print(A1, A_includes_subsets);
  check_result(comm_text + ", " + Atext, result, expected_result, title);
}

BOOST_AUTO_TEST_CASE(test_comm_operations)
{
  test_comm_operation("{a|b -> c}", "{c}", "{ab, c}", alphabet_operations::comm_inverse, "comm_inverse");
  test_comm_operation("{a|a -> b}", "{b, bb}", "{aa, aaaa, aab, b, bb}", alphabet_operations::comm_inverse, "comm_inverse");
  test_comm_operation("{a|b -> c}", "{ab, aab, aabb, abd}", "{aab, aabb, ab, abc, abd, ac, c, cc, cd}", alphabet_operations::comm, "comm");
  test_comm_operation("{a|b -> c}", "{ab, aab, aabb, abd}*", "{aab, aabb, ab, abc, abd, ac, c, cc, cd}*", alphabet_operations::comm, "comm");
}

template <typename Operation>
void test_rename_operation(const std::string& rename_text, const std::string& Atext, const std::string& expected_result, Operation op, const std::string& title)
{
  rename_expression_list R = parse_rename_set(rename_text);
  multi_action_name_set A;
  bool A_includes_subsets;
  boost::tuples::tie(A, A_includes_subsets) = parse_multi_action_name_set(Atext);
  multi_action_name_set A1 = op(R, A, A_includes_subsets);
  std::string result = print(A1, A_includes_subsets);
  check_result(rename_text + ", " + Atext, result, expected_result, title);
}

BOOST_AUTO_TEST_CASE(test_rename_operations)
{
  test_rename_operation("{a -> b, c -> d}", "{ab, aacc}", "{bb, bbdd}", alphabet_operations::rename, "rename");
  test_rename_operation("{a -> b, c -> d}", "{ab, aacc}*", "{bb, bbdd}*", alphabet_operations::rename, "rename");
  test_rename_operation("{a -> b, c -> d}", "{abd, bcdd}", "{aac, accc}", alphabet_operations::rename_inverse, "rename_inverse");
  test_rename_operation("{a -> b}", "{b, bb}", "{a, aa}", alphabet_operations::rename_inverse, "rename_inverse");
  test_rename_operation("{a -> b}", "{bb}*", "{aa}*", alphabet_operations::rename_inverse, "rename_inverse");
}

void test_allow(const std::string& allow_text, const std::string& Atext, const std::string& expected_result, const std::string& title)
{
  action_name_multiset_list V = parse_allow_set(allow_text);
  multi_action_name_set A;
  bool A_includes_subsets;
  boost::tuples::tie(A, A_includes_subsets) = parse_multi_action_name_set(Atext);
  multi_action_name_set A1 = alphabet_operations::allow(V, A, A_includes_subsets);
  std::string result = print(A1);
  check_result(allow_text + ", " + Atext, result, expected_result, title);
}

BOOST_AUTO_TEST_CASE(test_allow1)
{
  test_allow("{a|b, a|b|b, c}", "{ab, abbc, c}", "{ab, c}", "allow");
  test_allow("{a, b, c}", "{ab}*", "{a, b}", "allow");
}

void test_block(const std::string& block_text, const std::string& Atext, const std::string& expected_result, const std::string& title)
{
  core::identifier_string_list B = parse_block_set(block_text);
  multi_action_name_set A;
  bool A_includes_subsets;
  boost::tuples::tie(A, A_includes_subsets) = parse_multi_action_name_set(Atext);
  multi_action_name_set A1 = alphabet_operations::block(B, A, A_includes_subsets);
  std::string result = print(A1, A_includes_subsets);
  check_result(block_text + ", " + Atext, result, expected_result, title);
}

BOOST_AUTO_TEST_CASE(test_block1)
{
  test_block("{b}", "{ab, abbc, c}", "{c}", "block");
  test_block("{b}", "{ab, abbc, c}*", "{a, ac, c}*", "block");
}

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
  return EXIT_SUCCESS;
}
