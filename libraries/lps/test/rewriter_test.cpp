// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Add your file description here.

#include <cstdlib>
#include <string>
#include <algorithm>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/detail/parse_substitutions.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/detail/lps_rewriter.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/lps/detail/specification_property_map.h"

using namespace std;
using namespace atermpp;
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

  // cout << mcrl2::core::pp(application(plus, make_list(n4, n5))) << endl;
  BOOST_CHECK(r(application(plus, n4, n5)) == r(application(plus, n2, n7)));

  specification spec1 = spec;
  lps::rewrite(spec1, r);
  lps::detail::specification_property_map info(spec);
  lps::detail::specification_property_map info1(spec1);
  BOOST_CHECK(info.to_string() == info1.to_string());

  // test destructor
  rewriter r1 = r;
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

  lps::detail::specification_property_map info1(spec1);
  lps::detail::specification_property_map info2(spec2);
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

  lps::detail::specification_property_map info1(spec1);
  lps::detail::specification_property_map info2(spec2);
  BOOST_CHECK(info1.to_string() == info2.to_string());
  BOOST_CHECK(spec1.process().summand_count() == 3);
}

// N.B. This test doesn't work due to the different representations of numbers
// that are used before and after rewriting :-(
// Checks if rewriting the specification src with the substitutions sigma
// results in the specification dest.
void test_lps_rewriter(std::string src_text, std::string dest_text, std::string sigma_text)
{
  lps::specification src  = parse_linear_process_specification(src_text);
  lps::specification dest = parse_linear_process_specification(dest_text);

  // rewrite the specification src                                         
  data::rewriter R(src.data());
  data::mutable_substitution<data::variable, data::data_expression> sigma;
  data::detail::parse_substitutions(sigma_text, src.data(), sigma);
  lps::detail::rewriter_adapter<data::rewriter, data::mutable_substitution<data::variable, data::data_expression> > Rsigma(R, sigma);
  lps::detail::make_lps_rewriter(Rsigma).rewrite(src);                   

  if (src != dest)
  {
    std::cerr << "--- test failed ---" << std::endl;
    std::cerr << pp(src.process().summands()) << std::endl;
    std::cerr << pp(src.initial_process()) << std::endl;
    std::cerr << "-------------------" << std::endl;
    std::cerr << pp(dest.process().summands()) << std::endl;
    std::cerr << pp(dest.initial_process()) << std::endl;
  }
  BOOST_CHECK(src == dest);
  core::garbage_collect();
}
                    
void test_lps_rewriter()
{
  std::string src =
    "act  c:Pos#Nat;                          \n"
    "proc P(a:Pos,b:Nat)=c(a,0).P(a+1,b+1+2); \n"
    "init P(1+1,2+2);                         \n";

  std::string dest =
    "act  c:Pos#Nat;                          \n"
    "proc P(a:Pos,b:Nat)=c(a,0).P(a+1,b+3);   \n"
    "init P(2,4);                             \n";
    
  test_lps_rewriter(src, dest, "");
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test1();
  test2();
  test3();
  //test_lps_rewriter();

  return 0;
}
