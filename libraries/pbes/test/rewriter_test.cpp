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
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/parse.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/rewriter.h"

using namespace mcrl2;

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
  "  X;            \n"
  "  Y: Nat;       \n"
  "  Z: Bool, Pos; \n"
  ;

inline
pbes_expression expr(const std::string& text)
{
  return pbes_system::parse_pbes_expression(text, VARIABLE_SPECIFICATION);
}

void test_simplifying_rewriter()
{
  // create a simplifying rewriter R
  specification spec = mcrl22lps(
    "map MCRL2_DUMMY_1:Pos;  \n"
    "var MCRL2_DUMMY_2:Bool; \n"
    "    MCRL2_DUMMY_3:Pos;  \n"
    "    MCRL2_DUMMY_4:Nat;  \n"
    "    MCRL2_DUMMY_5:Int;  \n"
    "    MCRL2_DUMMY_6:Real; \n"
    "eqn MCRL2_DUMMY_1 = 1;  \n"
    "init delta;"
  );
  data::rewriter datar(spec.data());
  pbes_system::simplifying_rewriter<data::rewriter> R(datar);

  BOOST_CHECK(R(expr("true && true"))           == expr("true"));
  BOOST_CHECK(R(expr("(true && true) && true")) == expr("true"));
  BOOST_CHECK(R(expr("true && false"))          == expr("false"));
  BOOST_CHECK(R(expr("true => val(b)"))         == expr("val(b)"));
  BOOST_CHECK(R(expr("X && true"))              == expr("X"));
  BOOST_CHECK(R(expr("true && X"))              == expr("X"));
  BOOST_CHECK(R(expr("X && false"))             == expr("false"));
  BOOST_CHECK(R(expr("false && X"))             == expr("false"));
  BOOST_CHECK(R(expr("X && (false && X)"))      == expr("false"));
  BOOST_CHECK(R(expr("exists m:Nat.true"))      == expr("true"));
  BOOST_CHECK(R(expr("Y(1+2)"))                 == expr("Y(3)"));
}

void test_enumerate_quantifiers_rewriter()
{
  // create an enumerate quantifiers rewriter R
  specification spec = mcrl22lps("init delta;");
  data::rewriter datar(spec.data());
  data::number_postfix_generator name_generator;
  data::data_enumerator<> datae(spec.data(), datar, name_generator);
  pbes_system::enumerate_quantifiers_rewriter<data::rewriter, data::data_enumerator<> > R(datar, datae);

  BOOST_CHECK(R(expr("true && true")) == expr("true"));
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_simplifying_rewriter();
  test_enumerate_quantifiers_rewriter();

  return 0;
}
