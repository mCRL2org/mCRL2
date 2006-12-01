#include <iostream>
#include <set>
#include <boost/test/minimal.hpp>
#include "atermpp/atermpp.h"

using namespace std;
using namespace atermpp;

class D: public aterm_appl
{
};

// This program is for checking if some standard operators on aterms
// are defined and don't lead to ambiguities.

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);

  aterm t      = make_term("[1,2]");
  aterm_appl a = make_term("f(x)");
  aterm_list l = make_term("[3]");
  ATerm T      = t;
  ATermAppl A  = a; 
  ATermList L  = l;  

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
