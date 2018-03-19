// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file boolean_operator.cpp
/// \brief Test for boolean operators.

#define BOOST_TEST_MODULE boolean_operator_test
#include <boost/test/included/unit_test_framework.hpp>
#include "mcrl2/data/optimized_boolean_operators.h"
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

BOOST_AUTO_TEST_CASE(test_boolean_operators)
{
  BOOST_CHECK(data::optimized_or(expr("false"), expr("X")) == expr("X"));
  BOOST_CHECK(data::optimized_and(expr("false"), expr("X")) == expr("false"));
}

