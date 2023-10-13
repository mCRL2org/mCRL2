// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE rewriter_test
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/data/detail/parse_substitution.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/rewrite.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;

const std::string SPECIFICATION1 =
  "sort Natural;                                    \n"
  "                                                 \n"
  "cons _0: Natural;                                \n"
  "    S: Natural -> Natural;                       \n"
  "                                                 \n"
  "map  eq: Natural # Natural -> Bool;              \n"
  "    less: Natural # Natural -> Bool;             \n"
  "    plus: Natural # Natural -> Natural;          \n"
  "    _1, _2, _3, _4, _5, _6, _7, _8, _9: Natural; \n"
  "    P: Natural -> Natural;                       \n"
  "    even: Natural -> Bool;                       \n"
  "                                                 \n"
  "var  n, m: Natural;                              \n"
  "                                                 \n"
  "eqn  eq(n, n) = true;                            \n"
  "    eq(S(n), _0) = false;                        \n"
  "    eq(_0, S(m)) = false;                        \n"
  "    eq(S(n), S(m)) = eq(n, m);                   \n"
  "                                                 \n"
  "    less(n, n) = false;                          \n"
  "    less(n, _0) = false;                         \n"
  "    less(_0, S(m)) = true;                       \n"
  "    less(S(n), S(m)) = less(n, m);               \n"
  "                                                 \n"
  "    plus(_0, n) = n;                             \n"
  "    plus(n, _0) = n;                             \n"
  "    plus(S(n), m) = S(plus(n, m));               \n"
  "    plus(n, S(m)) = S(plus(n, m));               \n"
  "                                                 \n"
  "    even(_0) = true;                             \n"
  "    even(S(n)) = !(even(n));                     \n"
  "                                                 \n"
  "    P(S(n)) = n;                                 \n"
  "                                                 \n"
  "    _1 = S(_0);                                  \n"
  "    _2 = S(_1);                                  \n"
  "    _3 = S(_2);                                  \n"
  "    _4 = S(_3);                                  \n"
  "    _5 = S(_4);                                  \n"
  "    _6 = S(_5);                                  \n"
  "    _7 = S(_6);                                  \n"
  "    _8 = S(_7);                                  \n"
  "    _9 = S(_8);                                  \n"
  "                                                 \n"
  "act a: Natural;                                  \n"
  "                                                 \n"
  "proc P(n: Natural) = sum m: Natural. a(m). P(m); \n"
  "                                                 \n"
  "init P(_0);                                      \n"
  ;

void test1()
{
  specification spec = parse_linear_process_specification(SPECIFICATION1);
  rewriter r(spec.data());

  data_expression n1    = find_mapping(spec.data(), "_1");
  data_expression n2    = find_mapping(spec.data(), "_2");
  data_expression n3    = find_mapping(spec.data(), "_3");
  data_expression n4    = find_mapping(spec.data(), "_4");
  data_expression n5    = find_mapping(spec.data(), "_5");
  data_expression n6    = find_mapping(spec.data(), "_6");
  data_expression n7    = find_mapping(spec.data(), "_7");
  data_expression n8    = find_mapping(spec.data(), "_8");
  data_expression n9    = find_mapping(spec.data(), "_9");
  data_expression plus  = find_mapping(spec.data(), "plus");

  BOOST_CHECK(r(plus(n4, n5)) == r(plus(n2, n7)));

  specification spec1 = spec;
  lps::rewrite(spec1, r);
  lps::detail::specification_property_map<> info(spec);
  lps::detail::specification_property_map<> info1(spec1);
  BOOST_CHECK(info.to_string() == info1.to_string());

  // test destructor
  const rewriter& r1 = r;
  BOOST_CHECK(r1(n1)==r(n1));
}

const std::string SPECIFICATION2=
  "act  c:Pos#Nat;                          \n"
  "proc P(a:Pos,b:Nat)=c(a,0).P(a+1,b+1+2); \n"
  "init P(1+1,2+2);                         \n";

void test2()
{
  specification spec = parse_linear_process_specification(SPECIFICATION2);
  rewriter r(spec.data());

  specification spec1 = spec;
  lps::rewrite(spec1, r);

  specification spec2 = spec1;
  lps::rewrite(spec2, r);

  lps::detail::specification_property_map<> info1(spec1);
  lps::detail::specification_property_map<> info2(spec2);
  BOOST_CHECK(info1.to_string() == info2.to_string());
  BOOST_CHECK(spec1.process().summand_count() == 1);
}

const std::string SPECIFICATION3 =
  "act  c:Pos#Nat;                          \n"
  "proc P(a:Pos,b:Nat)=tau.P(a+1,b+1+2)+    \n"
  "                    tau.P(a+1,pred(a))+  \n"
  "                    c(a,0).P(a,b);       \n"
  "init P(1+1,0);                           \n";
void test3()
{
  specification spec = parse_linear_process_specification(SPECIFICATION3);
  rewriter r(spec.data());

  specification spec1 = spec;
  lps::rewrite(spec1, r);

  specification spec2 = spec1;
  lps::rewrite(spec2, r);

  lps::detail::specification_property_map<> info1(spec1);
  lps::detail::specification_property_map<> info2(spec2);
  BOOST_CHECK(info1.to_string() == info2.to_string());
  BOOST_CHECK(spec1.process().summand_count() == 3);
}

// Checks if rewriting the specification src with the substitutions sigma
// results in the specification dest.
void test_lps_rewriter(const std::string& src_text, const std::string& dest_text, const std::string& sigma_text)
{
  lps::specification src  = parse_linear_process_specification(src_text);
  lps::specification dest = parse_linear_process_specification(dest_text);

  // rewrite the specification src
  data::rewriter R(src.data());
  data::mutable_map_substitution<> sigma;
  data::detail::parse_substitution(sigma_text, sigma, src.data());
  lps::rewrite(src, R, sigma);

  if (src != dest)
  {
    std::cerr << "--- test failed ---" << std::endl;
    std::cerr << lps::pp(src) << std::endl;
    std::cerr << "-------------------" << std::endl;
    std::cerr << lps::pp(dest) << std::endl;
  }
  BOOST_CHECK(src == dest);
}

void test_lps_rewriter()
{
  std::string src =
    "act  c: Bool;                                                       \n"
    "proc P(b: Bool, c:Bool) = c(true && false).P(b || true, c && true); \n"
    "init P(true || false, true && false);                               \n";

  std::string dest =
    "act  c: Bool;                                                       \n"
    "proc P(b: Bool, c:Bool) = c(false).P(true, c);                      \n"
    "init P(true, false);                                                \n";

  test_lps_rewriter(src, dest, "");
}

void test_one_point_rule_rewriter()
{
  std::string src =
    "act  a: Bool;\n"
    "\n"
    "proc P(b: Bool) =\n"
    "       (forall n: Nat. n != 1 || b) ->\n"
    "         a(exists m: Nat. m == 1) .\n"
    "         P(b = b);\n"
    "\n"
    "init P(true);";

  std::string expected_result =
    "act  a: Bool;\n"
    "\n"
    "proc P(b: Bool) =\n"
    "       (1 != 1 || b) ->\n"
    "         a(1 == 1) .\n"
    "         P(b = b);\n"
    "\n"
    "init P(true);";

  specification lpsspec = parse_linear_process_specification(src);
  specification expected_spec = parse_linear_process_specification(expected_result);
  lps::one_point_rule_rewrite(lpsspec);
  std::string result = utilities::trim_copy(lps::pp(lpsspec));
  BOOST_CHECK(result == expected_result);
}

BOOST_AUTO_TEST_CASE(test_main)
{
  test1();
  test2();
  test3();
  test_lps_rewriter();
  test_one_point_rule_rewriter();
}
