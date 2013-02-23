// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file operators.cpp
/// \brief Add your file description here.

#include <iostream>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_appl.h"

using namespace std;
using namespace atermpp;

class D: public aterm_appl
{
};

// This program is for checking if some standard operators on aterms
// are defined and don't lead to ambiguities.

int test_main(int argc, char* argv[])
{
  atermpp::aterm t      = read_term_from_string("[1,2]");
  aterm_appl a (read_term_from_string("f(x)"));
  aterm_list l (read_term_from_string("[3]"));
  aterm T      = t;
  aterm_appl A  = a;
  aterm_list L  = l;

  // assignment
  t = t;
  t = T;
  T = t;

  a = a;
  a = A;
  A = a;

  l = l;
  l = L;
  L = l;

  t = a;
  t = l;
  // l = a;  this will give a runtime assertion failure

  // equality
  t == t;
  // t == T; Disabled this test as ATerms should not be compared to aterms.
  // T == t;

  a == a;
  a == A;
  A == a;

  l == l;
  l == L;
  L == l;

  t == a;
  t == l;
  // l == a;  Temporarily disabled. Should be reanabled in due time.

  // inequality
  t != t;
  // t != T; Disabled this test as ATerms should not be compared to aterms.
  // T != t;

  a != a;
  a != A;
  A != a;

  l != l;
  l != L;
  L != l;

  t != a;
  t != l;
  // l != a;  Temporarily disabled. Should be reanabled in due time.

  // operator<
  bool b;
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
  set<atermpp::aterm> st;
  set<aterm_appl> sa;
  set<aterm_list> sl;
  set<aterm> sT;
  set<aterm_appl> sA;
  set<aterm_list> sL;
  set<D> sD;

  return 0;
}
