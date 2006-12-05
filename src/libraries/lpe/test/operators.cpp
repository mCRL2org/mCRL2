#include <iostream>
#include <set>
#include <boost/test/minimal.hpp>
#include "atermpp/atermpp.h"
#include "lpe/data.h"
#include "lpe/function.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

class D: public aterm_appl
{
};

// This program is for checking if some standard operators on aterms
// are defined and don't lead to ambiguities.

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions();

  aterm t      = make_term("[1,2]");
  aterm_appl a = data_variable("d:D");
  aterm_list l = make_term("[3]");
  ATerm T      = t;
  ATermAppl A  = a; 
  ATermList L  = l;  

  data_variable v;
  v = a;

  set<data_variable> variables;
  variables.insert(a);
  insert_iterator<set<data_variable> > i = inserter(variables, variables.end());
  *i = a;
  find_all_if(t, is_data_variable, inserter(variables, variables.end()));

  set<function> functions;
  find_all_if(t, is_function, inserter(functions, functions.end()));

/*
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
*/

  return 0;
}
