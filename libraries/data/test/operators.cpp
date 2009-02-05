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
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_operation.h"

using namespace std;
using namespace atermpp;
using namespace mcrl2::data;

class D: public aterm_appl
{
};

void f(data_expression d)
{
}

// This program is for checking if some standard operators on aterms
// are defined and don't lead to ambiguities.

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  aterm t      = make_term("[1,2]");
  aterm_appl a = data_variable("d:D");
  aterm_list l = make_term("[3]");
  ATerm T      = t;
  ATermAppl A  = a;
  ATermList L  = l;

  data_variable v;
  f(v);
  v = a;

  set<data_variable> variables;
  variables.insert(a);
  insert_iterator<set<data_variable> > i = inserter(variables, variables.end());
  *i = a;
  find_all_if(t, is_data_variable, inserter(variables, variables.end()));

  set<data_operation> functions;
  find_all_if(t, is_data_operation, inserter(functions, functions.end()));

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
  t == T;
  T == t;

  a == a;
  a == A;
  A == a;

  l == l;
  l == L;
  L == l;

  t == a;
  t == l;
  l == a;

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

  // set
  set<aterm> st;
  set<aterm_appl> sa;
  set<aterm_list> sl;
  set<ATerm> sT;
  set<ATermAppl> sA;
  set<ATermList> sL;
  set<D> sD;

  return 0;
}
