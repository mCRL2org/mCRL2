// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file operators.cpp
/// \brief Add your file description here.

#define BOOST_TEST_MODULE operators
#include <boost/test/included/unit_test.hpp>

#include "mcrl2/atermpp/aterm_io.h"

using namespace atermpp;

class D: public aterm
{
};

// This program is for checking if some standard operators on aterms
// are defined and don't lead to ambiguities.

BOOST_AUTO_TEST_CASE(test_main)
{
  aterm t (read_term_from_string("[1,2]"));
  aterm a (read_appl_from_string("f(x)"));
  aterm_list l (read_list_from_string("[3]"));
  aterm T      = t;
  aterm A  = a;
  aterm_list L  = l;

  // assignment
  t = T;
  T = t;

  a = A;
  A = a;

  l = L;
  L = l;

  t = a;
  t = l;
  // l = a;  this will give a runtime assertion failure

  // equality
  bool b;
  b = t == t;

  b = a == a;
  b = a == A;
  b = A == a;

  b = l == l;
  b = l == L;
  b = L == l;

  b = t == a;
  b = t == l;
  b = l == a;

  // inequality
  b = t != t;

  b = a != a;
  b = a != A;
  b = A != a;

  b = l != l;
  b = l != L;
  b = L != l;

  b = t != a;
  b = t != l;
  b = l != a;

  // operator<
  b = t < t;
  b = t < T;
  b = T < t;
  b = a < a;
  b = a < A;
  b = A < a;
  b = l < l;
  b = l < L;
  b = L < l;
  static_cast<void>(b); // Silence compiler/static analysis warnings

  // set
  std::set<atermpp::aterm> st;
  std::set<aterm> sa;
  std::set<aterm_list> sl;
  std::set<aterm> sT;
  std::set<aterm> sA;
  std::set<aterm_list> sL;
  std::set<D> sD;
}
