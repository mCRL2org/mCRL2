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
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/lps_rewrite.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/core/garbage_collection.h"

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
  specification spec = mcrl22lps(SPECIFICATION1);
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
  specification spec1=rewrite_lps(spec,r);
  BOOST_CHECK(spec1==rewrite_lps(spec1,r));
  BOOST_CHECK(spec1.process().summands().size()==1);
  // test destructor
  rewriter r1 = r;
}

const std::string SPECIFICATION2=
"act  c:Pos#Nat;                          \n"
"proc P(a:Pos,b:Nat)=c(a,0).P(a+1,b+1+2); \n"
"init P(1+1,2+2);                         \n";

void test2()
{ specification spec = mcrl22lps(SPECIFICATION2);
  rewriter r(spec.data());
  specification spec1=rewrite_lps(spec,r);
  BOOST_CHECK(spec1==rewrite_lps(spec1,r));
  BOOST_CHECK(spec1.process().summands().size()==1);
}

const std::string SPECIFICATION3=
"act  c:Pos#Nat;                          \n"
"proc P(a:Pos,b:Nat)=tau.P(a+1,b+1+2)+    \n"
"                    tau.P(a+1,pred(a))+  \n"
"                    c(a,0).P(a,b);       \n"
"init P(1+1,0);                           \n";

void test3()
{ specification spec = mcrl22lps(SPECIFICATION3);
  rewriter r(spec.data());
  specification spec1=rewrite_lps(spec,r);
  BOOST_CHECK(spec1==rewrite_lps(spec1,r));
  BOOST_CHECK(spec1.process().summands().size()==2);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test1();
  core::garbage_collect();
  test2();
  core::garbage_collect();
  test3();
  core::garbage_collect();

  return 0;
}
