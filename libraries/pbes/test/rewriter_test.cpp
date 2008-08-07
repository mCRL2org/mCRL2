// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewriter_test.cpp
/// \brief Test for the pbes rewriters.

#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/rewriter.h"

using namespace mcrl2;

//const std::string SPECIFICATION =
//"act a:Nat;                               \n"
//"                                         \n"
//"map smaller: Nat#Nat -> Bool;            \n"
//"                                         \n"
//"var x,y : Nat;                           \n"
//"                                         \n"
//"eqn smaller(x,y) = x < y;                \n"
//"                                         \n"
//"proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
//"                                         \n"
//"init P(0);                               \n";

const std::string SPECIFICATION = "init delta;";

const std::string VARIABLE_SPECIFICATION =
  "datavar         \n"
  "  b:  Bool;     \n"
  "  b1: Bool;     \n"
  "  b2: Bool;     \n"
  "  b3: Bool;     \n"
  "                \n"
  "  n:  Nat;      \n"
  "  n1: Nat;      \n"
  "  n2: Nat;      \n"
  "  n3: Nat;      \n"
  "                \n"
  "  p:  Pos;      \n"
  "  p1: Pos;      \n"
  "  p2: Pos;      \n"
  "  p3: Pos;      \n"
  "                \n"
  "predvar         \n"
  "  X: ;          \n"
  "  Y: Nat;       \n"
  "  Z: Bool, Pos; \n"
  ;

inline
pbes_expression expr(const std::string& text)
{
  return pbes_system::parse_pbes_expression(text, VARIABLE_SPECIFICATION);
}

void test_simplify_rewriter()
{
  using namespace pbes_expr;

  specification spec = mcrl22lps(SPECIFICATION);
  data::rewriter datar(spec.data());
  pbes_system::simplifying_rewriter<data::rewriter> pbesr(datar);

  BOOST_CHECK(pbesr(expr("true && true"))           == expr("true"));
  BOOST_CHECK(pbesr(expr("(true && true) && true")) == expr("true"));
  BOOST_CHECK(pbesr(expr("true && false"))          == expr("false"));
  BOOST_CHECK(pbesr(expr("true => val(b)"))         == expr("val(b)"));
  BOOST_CHECK(pbesr(expr("X && true"))              == expr("X"));
  BOOST_CHECK(pbesr(expr("true && X"))              == expr("X"));
  BOOST_CHECK(pbesr(expr("X && false"))             == expr("false"));
  BOOST_CHECK(pbesr(expr("false && X"))             == expr("false"));
  BOOST_CHECK(pbesr(expr("X && (false && X)"))      == expr("false"));
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_simplify_rewriter();

  return 0;
}
