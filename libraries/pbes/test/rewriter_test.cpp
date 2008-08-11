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

// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/pbes_parse.h"
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

template <typename Rewriter>
void test_expressions(Rewriter R, std::string expr1, std::string expr2)
{
  if (R(expr(expr1)) != R(expr(expr2)))
  {
    BOOST_CHECK(R(expr(expr1)) == R(expr(expr2)));
    std::cout << "--- failed test --- " << expr1 << " -> " << expr2 << std::endl;
    std::cout << "expr1    " << core::pp(expr(expr1)) << std::endl;
    std::cout << "expr2    " << core::pp(expr(expr2)) << std::endl;
    std::cout << "R(expr1) " << core::pp(R(expr(expr1))) << std::endl;
    std::cout << "R(expr2) " << core::pp(R(expr(expr2))) << std::endl;
    std::cout << "R(expr1) " << R(expr(expr1)) << std::endl;
    std::cout << "R(expr2) " << R(expr(expr2)) << std::endl;
  }
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

  test_expressions(R, "false"                                                           , "val(false)");
  test_expressions(R, "true"                                                            , "val(true)");
  test_expressions(R, "true && true"                                                    , "val(true)");
  test_expressions(R, "(true && true) && true"                                          , "val(true)");
  test_expressions(R, "true && false"                                                   , "val(false)");
  test_expressions(R, "true => val(b)"                                                  , "val(b)");
  test_expressions(R, "X && true"                                                       , "X");
  test_expressions(R, "true && X"                                                       , "X");
  test_expressions(R, "X && false"                                                      , "val(false)");
  test_expressions(R, "X && val(false)"                                                 , "val(false)");
  test_expressions(R, "false && X"                                                      , "val(false)");
  test_expressions(R, "X && (false && X)"                                               , "val(false)");
  test_expressions(R, "Y(1+2)"                                                          , "Y(3)");
  test_expressions(R, "true || true"                                                    , "true");
  test_expressions(R, "(true || true) || true"                                          , "true");
  test_expressions(R, "true || false"                                                   , "true");
  test_expressions(R, "false => X"                                                      , "true");
  test_expressions(R, "Y(n+n)"                                                          , "Y(n+n)");
  test_expressions(R, "Y(n+p)"                                                          , "Y(n+p)");
  test_expressions(R, "forall m:Nat. false"                                             , "false");
  test_expressions(R, "X && X"                                                          , "X");
  test_expressions(R, "val(true)"                                                       , "true");  
  test_expressions(R, "false => (exists m:Nat. exists k:Nat. val(m*m == k && k > 20))"  , "true");

  // test_expressions(R, "exists m:Nat.true"                                               , "true");
  // test_expressions(R, "Y(n+p) && Y(p+n)"                                                , "Y(n+p)");
  // test_expressions(R, "exists m:Nat. val( m== p) && Y(m)"                               , "Y(p)");
  // test_expressions(R, "X && (Y(p) || X)"                                                , "X");
  // test_expressions(R, "X || (Y(p) && X)"                                                , "X");
  // test_expressions(R, "val(b || !b)"                                                    , "val(true)");
  // test_expressions(R, "Y(n1 + n2)"                                                      , "Y(n2 + n1)");
}

void test_enumerate_quantifiers_rewriter()
{
  // create an enumerate quantifiers rewriter R
  specification spec = mcrl22lps("init delta;");
  data::rewriter datar(spec.data());
  data::number_postfix_generator name_generator;
  data::data_enumerator<> datae(spec.data(), datar, name_generator);
  pbes_system::enumerate_quantifiers_rewriter<data::rewriter, data::data_enumerator<> > R(datar, datae);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_simplifying_rewriter();
  test_enumerate_quantifiers_rewriter();

  return 0;
}
