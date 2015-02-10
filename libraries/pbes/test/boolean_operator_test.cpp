// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file boolean_operator.cpp
/// \brief Test for the pbes rewriters.

#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/pbes/parse.h"

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
pbes_system::pbes_expression expr(const std::string& text)
{
  return pbes_system::parse_pbes_expression(text, VARIABLE_SPECIFICATION);
}

void test_boolean_operators()
{
  BOOST_CHECK(data::optimized_or(expr("false"), expr("X")) == expr("X"));
  BOOST_CHECK(data::optimized_and(expr("false"), expr("X")) == expr("false"));
}

int test_main(int argc, char* argv[])
{
  test_boolean_operators();

  return 0;
}
