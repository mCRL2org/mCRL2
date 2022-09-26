// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file one_point_condition_rewriter_test.cpp
/// \brief Tests for rewriter that uses LPS condition to simplify the right
/// hand side of a summand.

#define BOOST_TEST_MODULE one_point_condition_rewriter_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/detail/parse_substitution.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/lps/rewriters/one_point_condition_rewrite.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/rewrite.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

// Checks if rewriting the specification src with the substitutions sigma
// results in the specification dest.
void test_one_point_condition_rewriter(const std::string& src_text, const std::string& dest_text)
{
  lps::specification src  = parse_linear_process_specification(src_text);
  lps::specification dest = parse_linear_process_specification(dest_text);

  // rewrite the specification src
  data::rewriter R(src.data());
  lps::one_point_condition_rewrite(src, R);

  if (src != dest)
  {
    std::cerr << "--- test failed ---" << std::endl;
    std::cerr << lps::pp(src) << std::endl;
    std::cerr << "-------------------" << std::endl;
    std::cerr << lps::pp(dest) << std::endl;
  }
  BOOST_CHECK(src == dest);
}

void test_one_point_condition_rule_rewriter()
{
  std::string src =
      "act a: Nat;\n"
      "\n"
      "proc P(n: Nat) = (n == 0) -> a(n + 1). P(n + 2);\n"
      "\n"
      "init P(0);\n";

  std::string expected_result =
      "act a: Nat;\n"
      "\n"
      "proc P(n: Nat) = (n == 0) -> a(1). P(2);\n"
      "\n"
      "init P(0);\n";

  test_one_point_condition_rewriter(src, expected_result);
}

void test_parunfold_example()
{
  std::string src =
    "sort ListNat;\n"
    "\n"
    "cons c_,c_1: ListNat;\n"
    "\n"
    "map  C_ListNat: ListNat # List(Nat) # List(Nat) -> List(Nat);\n"
    "Det_ListNat: List(Nat) -> ListNat;\n"
    "pi_ListNat: List(Nat) -> Nat;\n"
    "pi_ListNat1: List(Nat) -> List(Nat);\n"
    "C_ListNat: ListNat # Bool # Bool -> Bool;\n"
    "C_ListNat: ListNat # ListNat # ListNat -> ListNat;\n"
    "C_ListNat: ListNat # Nat # Nat -> Nat;\n"
    "\n"
    "var  d1,d2: List(Nat);\n"
    "d,d6,d7: ListNat;\n"
    "d3,d8: Nat;\n"
    "d4,d5: Bool;\n"
    "eqn  C_ListNat(c_, d1, d2)  =  d1;\n"
    "C_ListNat(c_1, d1, d2)  =  d2;\n"
    "C_ListNat(d, d2, d2)  =  d2;\n"
    "Det_ListNat([])  =  c_;\n"
    "Det_ListNat(d3 |> d1)  =  c_1;\n"
    "pi_ListNat(d3 |> d1)  =  d3;\n"
    "pi_ListNat([])  =  0;\n"
    "pi_ListNat1(d3 |> d1)  =  d1;\n"
    "pi_ListNat1([])  =  [];\n"
    "C_ListNat(c_, d4, d5)  =  d4;\n"
    "C_ListNat(c_1, d4, d5)  =  d5;\n"
    "C_ListNat(d, d5, d5)  =  d5;\n"
    "C_ListNat(d, d4, d5)  =  d4 && d == c_ || d5 && d == c_1;\n"
    "C_ListNat(c_, d6, d7)  =  d6;\n"
    "C_ListNat(c_1, d6, d7)  =  d7;\n"
    "C_ListNat(d, d7, d7)  =  d7;\n"
    "C_ListNat(c_, d3, d8)  =  d3;\n"
    "C_ListNat(c_1, d3, d8)  =  d8;\n"
    "C_ListNat(d, d8, d8)  =  d8;\n"
    "\n"
    "act  a: Nat;\n"
    "\n"
    "proc P(stack_pp: ListNat, stack_pp1: Nat, stack_pp2: List(Nat)) =\n"
    "    (stack_pp == c_1) ->\n"
    "    a(C_ListNat(stack_pp, head([]), stack_pp1)) .\n"
    "    P(stack_pp = C_ListNat(stack_pp, Det_ListNat(tail([])), Det_ListNat(stack_pp2)), stack_pp1 = C_ListNat(stack_pp, pi_ListNat(tail([])), pi_ListNat(stack_pp2)), stack_pp2 = C_ListNat(stack_pp, pi_ListNat1(tail([])), pi_ListNat1(stack_pp2)));\n"
    "\n"
    "init P(c_1, 1, []);\n";

    std::string expected_result =
      "sort ListNat;\n"
      "\n"
      "cons c_,c_1: ListNat;\n"
      "\n"
      "map  C_ListNat: ListNat # List(Nat) # List(Nat) -> List(Nat);\n"
      "Det_ListNat: List(Nat) -> ListNat;\n"
      "pi_ListNat: List(Nat) -> Nat;\n"
      "pi_ListNat1: List(Nat) -> List(Nat);\n"
      "C_ListNat: ListNat # Bool # Bool -> Bool;\n"
      "C_ListNat: ListNat # ListNat # ListNat -> ListNat;\n"
      "C_ListNat: ListNat # Nat # Nat -> Nat;\n"
      "\n"
      "var  d1,d2: List(Nat);\n"
      "d,d6,d7: ListNat;\n"
      "d3,d8: Nat;\n"
      "d4,d5: Bool;\n"
      "eqn  C_ListNat(c_, d1, d2)  =  d1;\n"
      "C_ListNat(c_1, d1, d2)  =  d2;\n"
      "C_ListNat(d, d2, d2)  =  d2;\n"
      "Det_ListNat([])  =  c_;\n"
      "Det_ListNat(d3 |> d1)  =  c_1;\n"
      "pi_ListNat(d3 |> d1)  =  d3;\n"
      "pi_ListNat([])  =  0;\n"
      "pi_ListNat1(d3 |> d1)  =  d1;\n"
      "pi_ListNat1([])  =  [];\n"
      "C_ListNat(c_, d4, d5)  =  d4;\n"
      "C_ListNat(c_1, d4, d5)  =  d5;\n"
      "C_ListNat(d, d5, d5)  =  d5;\n"
      "C_ListNat(d, d4, d5)  =  d4 && d == c_ || d5 && d == c_1;\n"
      "C_ListNat(c_, d6, d7)  =  d6;\n"
      "C_ListNat(c_1, d6, d7)  =  d7;\n"
      "C_ListNat(d, d7, d7)  =  d7;\n"
      "C_ListNat(c_, d3, d8)  =  d3;\n"
      "C_ListNat(c_1, d3, d8)  =  d8;\n"
      "C_ListNat(d, d8, d8)  =  d8;\n"
      "\n"
      "act  a: Nat;\n"
      "\n"
      "proc P(stack_pp: ListNat, stack_pp1: Nat, stack_pp2: List(Nat)) =\n"
      "    (stack_pp == c_1) ->\n"
      "    a(stack_pp1) .\n"
      "    P(stack_pp = Det_ListNat(stack_pp2), stack_pp1 = pi_ListNat(stack_pp2), stack_pp2 = pi_ListNat1(stack_pp2));\n"
      "\n"
      "init P(c_1, 1, []);\n";

    test_one_point_condition_rewriter(src, expected_result);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test_one_point_condition_rule_rewriter();
  test_parunfold_example();
}
