// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file quantifiers_propagate_test.cpp
/// \brief Test program for absinthe algorithm.

#define BOOST_TEST_MODULE quantifiers_propagate_test
#include <boost/test/included/unit_test.hpp>
#include "mcrl2/pbes/quantifier_propagate.h"
#include "mcrl2/pbes/txt2pbes.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

BOOST_AUTO_TEST_CASE(test_single_forall)
{
  std::string PBES_TEXT =
    "pbes                                                                                          \n"
    " nu X(s: List(Nat)) =                                                                         \n"
    "   forall m1: Nat. Y(s, m1);                                                                  \n"
    " mu Y(s: List(Nat), m1: Nat) =                                                                \n"
    "   forall n: Nat. val(!(n mod 6 == m1)) || val(!(n <= 50 && n div 3 == 5)) || Y(n |> s, m1);  \n"
    "init X([]);                                                                                   \n"
  ;

  std::string RESULT_TEXT =
    "pbes                                                                                                         \n"
    " nu X(s: List(Nat)) =                                                                                        \n"
    "   Y1(s);                                                                                                    \n"
    " mu Y1(s: List(Nat)) =                                                                                       \n"
    "   forall m1: Nat. forall n: Nat. val(!(n mod 6 == m1)) || val(!(n <= 50 && n div 3 == 5)) || Y(n |> s, m1); \n"
    " mu Y(s: List(Nat), m1: Nat) =                                                                               \n"
    "   forall n: Nat. val(!(n mod 6 == m1)) || val(!(n <= 50 && n div 3 == 5)) || Y(n |> s, m1);                 \n"
    "init X([]);                                                                                                  \n"
  ;

  BOOST_CHECK_EQUAL(quantifier_propagate(txt2pbes(PBES_TEXT)), txt2pbes(RESULT_TEXT));
}

BOOST_AUTO_TEST_CASE(test_two_quantifiers)
{
  std::string PBES_TEXT =
    "pbes                                                                                          \n"
    " nu X(s: List(Nat)) =                                                                         \n"
    "   exists m2: Nat. forall m1: Nat. Y(s, m1 + m2);                                             \n"
    " mu Y(s: List(Nat), m1: Nat) =                                                                \n"
    "   forall n: Nat. val(!(n mod 6 == m1)) || val(!(n <= 50 && n div 3 == 5)) || Y(n |> s, m1);  \n"
    "init X([]);                                                                                   \n"
  ;

  std::string RESULT_TEXT =
    "pbes                                                                                                                                   \n"
    " nu X(s: List(Nat)) =                                                                                                                  \n"
    "   Y1(s);                                                                                                                              \n"
    " mu Y1(s: List(Nat)) =                                                                                                                 \n"
    "   exists m2: Nat. forall m1: Nat. forall n: Nat. val(!(n mod 6 == m1 + m2)) || val(!(n <= 50 && n div 3 == 5)) || Y(n |> s, m1 + m2); \n"
    " mu Y(s: List(Nat), m1: Nat) =                                                                                                         \n"
    "   forall n: Nat. val(!(n mod 6 == m1)) || val(!(n <= 50 && n div 3 == 5)) || Y(n |> s, m1);                                           \n"
    "init X([]);                                                                                                                            \n"
  ;

  BOOST_CHECK_EQUAL(quantifier_propagate(txt2pbes(PBES_TEXT)), txt2pbes(RESULT_TEXT));
}

BOOST_AUTO_TEST_CASE(test_parameter_dependency)
{
  std::string PBES_TEXT =
    "pbes                                                                                          \n"
    " nu X(s: List(Nat)) =                                                                         \n"
    "   forall m1: Nat. Y(s, head(s) + m1);                                                        \n"
    " mu Y(s: List(Nat), m1: Nat) =                                                                \n"
    "   forall n: Nat. val(!(n mod 6 == m1)) || val(!(n <= 50 && n div 3 == 5)) || Y(n |> s, m1);  \n"
    "init X([]);                                                                                   \n"
  ;

  // No changes expected, since m1 depends on the parameter s in Y(s, head(s) + m1)
  std::string RESULT_TEXT = PBES_TEXT;

  BOOST_CHECK_EQUAL(quantifier_propagate(txt2pbes(PBES_TEXT)), txt2pbes(RESULT_TEXT));
}

BOOST_AUTO_TEST_CASE(test_no_quantifier)
{
  std::string PBES_TEXT =
    "pbes             \n"
    " nu X(s: Nat) =  \n"
    "   Y(2);         \n"
    " mu Y(s: Nat) =  \n"
    "   val(s == 2);  \n"
    "init X(0);       \n"
  ;

  // No changes expected, since there is no quantifier to propagate
  std::string RESULT_TEXT = PBES_TEXT;

  BOOST_CHECK_EQUAL(quantifier_propagate(txt2pbes(PBES_TEXT)), txt2pbes(RESULT_TEXT));
}
