// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sort_traverser_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/pbes/find.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

std::string t1 =
  "% simple test, n is constant \n"
  "                             \n"
  "pbes nu X(n: Nat) =          \n"
  "       X(n);                 \n"
  "                             \n"
  "init X(0);                   \n"
  ;

std::string t2 =
  "% simple test, n is NOT constant \n"
  "                                 \n"
  "pbes nu X(n: Nat) =              \n"
  "       X(n + 1);                 \n"
  "                                 \n"
  "init X(0);                       \n"
  ;

std::string t3 =
  "% multiple parameters: n1 is constant, n2 is not \n"
  "                                                 \n"
  "pbes nu X(n1,n2: Nat) =                          \n"
  "       X(n1, n2 + 1);                            \n"
  "                                                 \n"
  "init X(0, 1);                                    \n"
  ;

std::string t4 =
  "% conditions: n is not constant, even if conditions are enabled\n"
  "                                                               \n"
  "pbes nu X(n: Nat) =                                            \n"
  "       val(2 < n) || X(n + 1);                                 \n"
  "                                                               \n"
  "init X(0);                                                     \n"
  ;

std::string t5 =
  "% conditions: n is constant if conditions are enabled \n"
  "                                                      \n"
  "pbes nu X(n: Nat) =                                   \n"
  "       val(2 < n) || X(n + 1);                        \n"
  "                                                      \n"
  "init X(5);                                            \n"
  ;

std::string t6 =
  "% reachability: n1 is not constant, equation Y is not reachable and should be removed \n"
  "                                                                                      \n"
  "pbes nu X(n1: Nat) = X(n1+1);                                                         \n"
  "     mu Y(n2: Nat) = Y(n2+1);                                                         \n"
  "                                                                                      \n"
  "init X(5);                                                                            \n"
  ;

std::string t7 =
  "% multiple edges from one vertex, one edge invalidates the assertion of the other: no constants should be found \n"
  "                                                                                                                \n"
  "pbes                                                                                                            \n"
  "   mu X1(n1,m1:Nat) = X2(n1) || X2(m1);                                                                         \n"
  "   mu X2(n2:Nat)     = X1(n2,n2);                                                                               \n"
  "init                                                                                                            \n"
  "   X1(2,1);                                                                                                     \n"
  ;

std::string t8 =
  "% conditions: parameters b,c and d will always be removed, with conditions on, parameter \n"
  "% n will NOT be removed. changing b,c or d to false or n to a bigger number              \n"
  "% then 5 WILL result in the removal of n                                                 \n"
  "                                                                                         \n"
  "pbes nu X(b,c,d:Bool, n:Nat) =                                                           \n"
  "    val(b) || ( val(c) && ( val(d) && (val(n > 5) || X(b,c,d,n+1))));                    \n"
  "                                                                                         \n"
  "init                                                                                     \n"
  "X(false,true,true,5);                                                                    \n"
  ;

std::string t9 =
  "% conditions: universal quantification which can be solved, n1 and n2 are \n"
  "% constants if conditions are enabled                                     \n"
  "                                                                          \n"
  "pbes mu X(n1,n2:Nat) =                                                    \n"
  "    forall m:Nat. (val(n1>n2) && X(n1+1,n2+1));                           \n"
  "                                                                          \n"
  "init X(1,2);                                                              \n"
  ;

std::string t10 =
  "% conditions: existential quantification which can be solved, n1 and n2 \n"
  "% are constants if conditions are enabled                               \n"
  "                                                                        \n"
  "pbes mu X(n1,n2:Nat) =                                                  \n"
  "    exists m:Nat. (val(n1>n2) && X(n1+1,n2+1));                         \n"
  "                                                                        \n"
  "init X(1,2);                                                            \n"
  ;

std::string t11 =
  "% conditions: universal quantification which cannot be solved \n"
  "% (by the current rewriter), n1 and n2 are constants, but not \n"
  "% detected as such                                            \n"
  "                                                              \n"
  "pbes mu X(n1,n2:Nat) =                                        \n"
  "    forall m:Nat. (val(m>n2) && X(n1+1,n2+1));                \n"
  "                                                              \n"
  "init X(1,2);                                                  \n"
  ;

std::string t12 =
  "% example 4.2.1 from \"Tools for PBES\" report                                     \n"
  "% constants without conditions: o4, n2, n4, o4                                     \n"
  "% constants with conditions: everything (equations X1,X2,X3 and X4 are removed)    \n"
  "                                                                                   \n"
  "pbes                                                                               \n"
  "   mu X1(n1,m1,o1,p1:Nat) = (val(n1<m1) || X2(o1,p1)) && X3(n1) && X1(n1,m1,4,p1); \n"
  "   mu X2(n2,m2:Nat)       = X5(n2,m2) || X5(m2,n2);                                \n"
  "   nu X3(n3:Nat)          = val(n3<3) || X1(n3,n3,4,n3+1);                         \n"
  "   nu X4(n4,m4,o4:Nat)    = val(n4 <= m4+o4) || (X3(n4) && X4(n4,m4+1,n4));        \n"
  "   mu X5(n5,m5:Nat)       = val(n5>m5) || X3(n5);                                  \n"
  "                                                                                   \n"
  "init                                                                               \n"
  "   X4(0,0,0);                                                                      \n"
  ;

std::string t13 =
  "pbes nu X =           \n"
  "        Y(true);      \n"
  "      mu Y(b: Bool) = \n"
  "        X;            \n"
  "                      \n"
  "init X;               \n"
  ;

std::string t14 =
  "pbes nu X(m:Nat) =           \n"
  "        forall n:Nat . X(n); \n"
  "                             \n"
  "init X(0);                   \n"
  ;

void test_pbes(std::string text)
{
  pbes<> p = txt2pbes(text);
  std::set<data::sort_expression> sorts;
  pbes_system::find_sort_expressions(p, std::inserter(sorts, sorts.end()));
  std::cerr << "sorts: " << data::pp(data::sort_expression_list(sorts.begin(), sorts.end())) << std::endl;
}

int test_main(int argc, char* argv[])
{
  test_pbes(t1);
  test_pbes(t2);
  test_pbes(t3);
  test_pbes(t4);
  test_pbes(t5);
  test_pbes(t6);
  test_pbes(t7);
  test_pbes(t8);
  test_pbes(t9);
  test_pbes(t10);
  test_pbes(t11);
  test_pbes(t12);
  test_pbes(t13);
  test_pbes(t14);

  return 0;
}

