// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file alphabet_test.cpp
/// \brief Test for alphabet reduction.

#define BOOST_TEST_MODULE alphabet_test
#include <boost/algorithm/string/predicate.hpp>
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/process/alphabet_reduce.h"
#include "mcrl2/process/detail/alphabet_intersection.h"
#include "mcrl2/process/detail/alphabet_parse.h"
#include "mcrl2/process/detail/alphabet_push_block.h"

using namespace mcrl2;
using namespace mcrl2::process;

struct LogDebug
{
  LogDebug()
  {
    log::logger::set_reporting_level(log::debug);
  }
};
BOOST_GLOBAL_FIXTURE(LogDebug);

inline
multi_action_name name(const process::action& x)
{
  multi_action_name result;
  result.insert(x.label().name());
  return result;
}

inline
multi_action_name name(const core::identifier_string& x)
{
  multi_action_name result;
  result.insert(x);
  return result;
}

std::string print(const multi_action_name& alpha)
{
  if (alpha.empty())
  {
    return "tau";
  }
  std::ostringstream out;
  for (const auto & i : alpha)
  {
    out << std::string(i);
  }
  std::string result = out.str();
  std::sort(result.begin(), result.end());
  return result;
}

std::string print(const multi_action_name_set& A, bool A_includes_subsets = false)
{
  std::multiset<std::string> V;
  for (const auto & i : A)
  {
    V.insert(print(i));
  }
  return core::detail::print_set(V, "", false, false) + (A_includes_subsets ? "@" : "");
}

std::string print(const allow_set& x)
{
  return print(x.A, x.A_includes_subsets);
}

BOOST_AUTO_TEST_CASE(test_print)
{
  auto [A, dummy] = detail::parse_simple_multi_action_name_set("{a}");
  multi_action_name tau;
  A.insert(tau);
  std::cout << print(A) << std::endl;
  BOOST_CHECK(print(A) == "{a, tau}");
}

BOOST_AUTO_TEST_CASE(test_parse)
{
  multi_action_name_set B;
  bool dummy;
  std::tie(B, dummy) = detail::parse_simple_multi_action_name_set("{a, ab}");
  std::cout << "B = " << print(B) << std::endl;
  BOOST_CHECK(print(B) == "{a, ab}");
}

BOOST_AUTO_TEST_CASE(test_includes)
{
  multi_action_name alpha = detail::parse_simple_multi_action_name("abb");
  multi_action_name beta = detail::parse_simple_multi_action_name("aabb");
  BOOST_CHECK(!alphabet_operations::includes(alpha, beta));
  BOOST_CHECK(alphabet_operations::includes(beta, alpha));
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
  test_alphabet("tau.a", "{a, tau}");
}

template <typename Operation>
void test_alphabet_operation(const std::string& text1, const std::string& text2, const std::string& expected_result, Operation op, const std::string& title)
{
  bool dummy;
  multi_action_name_set A1;
  multi_action_name_set A2;
  std::tie(A1, dummy) = detail::parse_simple_multi_action_name_set(text1);
  std::tie(A2, dummy) = detail::parse_simple_multi_action_name_set(text2);
  multi_action_name_set A3 = op(A1, A2);
  std::string result = print(A3);
  check_result(text1 + ", " + text2, result, expected_result, title);
}

BOOST_AUTO_TEST_CASE(test_alphabet_operations)
{
  using namespace alphabet_operations;
  test_alphabet_operation("{a}", "{b}", "{ab}", concat, "concat");
  test_alphabet_operation("{ab}", "{b, c}", "{abb, abc}", concat, "concat");
  test_alphabet_operation("{ab, aabc}", "{b, bc}", "{a, aa, aabc, aac, ab}", left_arrow1, "left_arrow1");
  test_alphabet_operation("{aa, b}", "{a}", "{a, aa, b}", left_arrow1, "left_arrow1");
  test_alphabet_operation("{ab, b}", "{b}", "{a, ab, b}", left_arrow1, "left_arrow1"); // N.B. tau is excluded!
  test_alphabet_operation("{bc}", "{c}", "{b, bc}", left_arrow1, "left_arrow1");
  test_alphabet_operation("{a}", "{a}", "{a}", left_arrow1, "left_arrow1");
}

void test_push_allow(const std::string& expression, const std::string& Atext, const std::string& expected_result, const std::string& equations = "")
{
  std::string text = "act a, b, c, d;\n" + equations + "\ninit " + expression + ";\n";
  process_specification procspec = parse_process_specification(text);
  auto [A, A_includes_subsets] = detail::parse_simple_multi_action_name_set(Atext);
  data::set_identifier_generator id_generator;

  std::map<process_identifier, multi_action_name_set> pcrl_equation_cache = detail::compute_pcrl_equation_cache(procspec.equations());
  process::detail::push_allow_cache W(id_generator, pcrl_equation_cache);
  process::detail::push_allow_node node = process::detail::push_allow(procspec.init(), allow_set(A, A_includes_subsets), procspec.equations(), W);
  std::string result = process::pp(node.expression);
  check_result(expression, result, expected_result, "push_allow");
}

BOOST_AUTO_TEST_CASE(test_push_allow1)
{
  test_push_allow("a || a", "{a}", "allow({a}, a || a)");
}

template <typename Operation>
void test_comm_operation(const std::string& comm_text, const std::string& Atext, const std::string& expected_result, Operation op, const std::string& title)
{
  communication_expression_list C = detail::parse_comm_set(comm_text);
  auto [A, A_includes_subsets] = detail::parse_simple_multi_action_name_set(Atext);
  multi_action_name_set A1 = op(C, A, A_includes_subsets);
  std::string result = print(A1, A_includes_subsets);
  check_result(comm_text + ", " + Atext, result, expected_result, title);

  if (title == "comm_inverse")
  {
    allow_set A2 = alphabet_operations::comm_inverse(C, allow_set(A, A_includes_subsets));
    std::string result_allow = print(A2);
    check_result(comm_text + ", " + Atext, result_allow, expected_result, title + "<allow>");
  }
}

BOOST_AUTO_TEST_CASE(test_comm_operations)
{
  // resolve ambiguity
  auto comm_inverse = [](const communication_expression_list& C, const multi_action_name_set& A, bool A_includes_subsets) { return alphabet_operations::comm_inverse(C, A, A_includes_subsets); };
  test_comm_operation("{a|b -> c}", "{c}", "{ab, c}", comm_inverse, "comm_inverse");
  test_comm_operation("{a|a -> b}", "{b, bb}", "{aa, aaaa, aab, b, bb}", comm_inverse, "comm_inverse");
  test_comm_operation("{a|b -> c}", "{ab, aab, aabb, abd}", "{aab, aabb, ab, abc, abd, ac, c, cc, cd}", alphabet_operations::comm, "comm");
  test_comm_operation("{a|b -> c}", "{ab, aab, aabb, abd}@", "{aab, aabb, ab, abc, abd, ac, c, cc, cd}@", alphabet_operations::comm, "comm");
}

template <typename Operation>
void test_rename_operation(const std::string& rename_text, const std::string& Atext, const std::string& expected_result, Operation op, const std::string& title)
{
  rename_expression_list R = detail::parse_rename_set(rename_text);
  auto [A, A_includes_subsets] = detail::parse_simple_multi_action_name_set(Atext);
  multi_action_name_set A1 = op(R, A, A_includes_subsets);
  std::string result = print(A1, A_includes_subsets);
  check_result(rename_text + ", " + Atext, result, expected_result, title);

  if (title == "rename_inverse")
  {
    allow_set A2 = alphabet_operations::rename_inverse(R, allow_set(A, A_includes_subsets));
    std::string result_allow = print(A2);
    check_result(rename_text + ", " + Atext, result_allow, expected_result, title + "<allow>");
  }
}

BOOST_AUTO_TEST_CASE(test_rename_operations)
{
  // resolve ambiguity
  auto rename_inverse = [](const rename_expression_list& R, const multi_action_name_set& A, bool A_includes_subsets) { return alphabet_operations::rename_inverse(R, A, A_includes_subsets); };
  test_rename_operation("{a -> b, c -> d}", "{ab, aacc}", "{bb, bbdd}", alphabet_operations::rename, "rename");
  test_rename_operation("{a -> b, c -> d}", "{ab, aacc}@", "{bb, bbdd}@", alphabet_operations::rename, "rename");
  test_rename_operation("{a -> b, c -> d}", "{abd, bcdd}", "{}", rename_inverse, "rename_inverse");
  test_rename_operation("{a -> b}", "{b, bb}", "{a, aa, ab, b, bb}", rename_inverse, "rename_inverse");
  test_rename_operation("{a -> b}", "{bb}@", "{aa, ab, bb}@", rename_inverse, "rename_inverse");
  test_rename_operation("{a -> b}", "{b}", "{a, b}", rename_inverse, "rename_inverse");
  test_rename_operation("{a -> b}", "{a}", "{}", rename_inverse, "rename_inverse");
  test_rename_operation("{b -> a}", "{a, bc}@", "{a, b, c}@", rename_inverse, "rename_inverse");
}

template <typename Operation>
void test_hide_operation(const std::string& hide_text, const std::string& Atext, const std::string& expected_result, Operation op, const std::string& title)
{
  core::identifier_string_list I = detail::parse_block_set(hide_text);
  auto [A, A_includes_subsets] = detail::parse_simple_multi_action_name_set(Atext);
  multi_action_name_set A1 = op(I, A, A_includes_subsets);
  std::string result = print(A1, A_includes_subsets);
  check_result(hide_text + ", " + Atext, result, expected_result, title);
}

BOOST_AUTO_TEST_CASE(test_hide_operations)
{
  auto hide_inverse = [](const core::identifier_string_list& I, const multi_action_name_set& A, bool A_includes_subsets) { return alphabet_operations::hide_inverse(I, A, A_includes_subsets); };
  test_hide_operation("{b}", "{ab}", "{}", hide_inverse, "hide_inverse");
}

void test_allow(const std::string& allow_text, const std::string& Atext, const std::string& expected_result, const std::string& title)
{
  action_name_multiset_list V = detail::parse_allow_set(allow_text);
  auto [A, A_includes_subsets] = detail::parse_simple_multi_action_name_set(Atext);
  multi_action_name_set A1 = alphabet_operations::allow(V, A, A_includes_subsets);
  std::string result = print(A1);
  check_result(allow_text + ", " + Atext, result, expected_result, title);

  allow_set A2 = alphabet_operations::allow(V, allow_set(A, A_includes_subsets));
  std::string result_allow = print(A2);
  check_result(allow_text + ", " + Atext, result_allow, expected_result, title + "<allow>");
}

BOOST_AUTO_TEST_CASE(test_allow1)
{
  test_allow("{a|b, a|b|b, c}", "{ab, abbc, c}", "{ab, c}", "allow");
  test_allow("{a, b, c}", "{ab}@", "{a, b}", "allow");
  // test_allow("{a|b}", "{a}@{b, c}", "{a|b}", "allow");
}

void test_block(const std::string& block_text, const std::string& Atext, const std::string& expected_result, const std::string& title)
{
  core::identifier_string_list B = detail::parse_block_set(block_text);
  auto [A, A_includes_subsets] = detail::parse_simple_multi_action_name_set(Atext);
  multi_action_name_set A1 = alphabet_operations::block(B, A, A_includes_subsets);
  std::string result = print(A1, A_includes_subsets);
  check_result(block_text + ", " + Atext, result, expected_result, title);

  allow_set A2 = alphabet_operations::block(B, allow_set(A, A_includes_subsets));
  std::string result_allow = print(A2);
  check_result(block_text + ", " + Atext, result_allow, expected_result, title + "<allow>");
}

BOOST_AUTO_TEST_CASE(test_block1)
{
  test_block("{b}", "{ab, abbc, c}", "{c}", "block");
  test_block("{b}", "{ab, abbc, c}@", "{a, ac, c}@", "block");
}

multi_action_name make_multi_action_name(const std::string& x)
{
  multi_action_name result;
  result.insert(core::identifier_string(x));
  return result;
}

BOOST_AUTO_TEST_CASE(test_alphabet_parallel)
{
  std::string text =
    "act a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20; \n"
    "init a1 || a2 || a3 || a4 || a5 || a6 || a7 || a8 || a9 || a10 || a11 || a12 || a13 || a14 || a15 || a16 || a17 || a18 || a19 || a20;  \n"
    ;
//    "init allow({a1, a2, a3, a4, a5, a6, a7, a8, a9, a10}, a1 || a2 || a3 || a4 || a5 || a6 || a7 || a8 || a9 || a10);  \n"
  process_specification procspec = parse_process_specification(text);
  multi_action_name_set A;
  A.insert(make_multi_action_name("a1"));
  A.insert(make_multi_action_name("a2"));
  A.insert(make_multi_action_name("a3"));
  A.insert(make_multi_action_name("a4"));
  A.insert(make_multi_action_name("a5"));
  A.insert(make_multi_action_name("a6"));
  A.insert(make_multi_action_name("a7"));
  A.insert(make_multi_action_name("a8"));
  A.insert(make_multi_action_name("a9"));
  A.insert(make_multi_action_name("a11"));
  A.insert(make_multi_action_name("a12"));
  A.insert(make_multi_action_name("a13"));
  A.insert(make_multi_action_name("a14"));
  A.insert(make_multi_action_name("a15"));
  A.insert(make_multi_action_name("a16"));
  A.insert(make_multi_action_name("a17"));
  A.insert(make_multi_action_name("a18"));
  A.insert(make_multi_action_name("a19"));
  A.insert(make_multi_action_name("a20"));

  multi_action_name_set B = detail::alphabet_intersection(procspec.init(), procspec.equations(), A);
  //BOOST_CHECK_EQUAL(lps::pp(B),"{a1, a2, a3, a4, a5, a6, a7, a8, a9, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20}");
  BOOST_CHECK_EQUAL(process::pp(B), process::pp(A));
}

BOOST_AUTO_TEST_CASE(test_alphabet_new)
{
  std::string text =
    "act a: Bool;                             \n"
    "proc S(d: Bool) = sum d:Bool. a(d).S(d); \n"
    "init allow({a}, S(true)) ;               \n"
    ;
  process_specification procspec = parse_process_specification(text);
  alphabet_reduce(procspec);
}

