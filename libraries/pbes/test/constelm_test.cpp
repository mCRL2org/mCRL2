// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file constelm_test.cpp
/// \brief Add your file description here.

// #define MCRL2_PBES_CONSTELM_DEBUG

#include <string>
#include <boost/foreach.hpp>
#include <boost/test/minimal.hpp>
#include "mcrl2/core/messaging.h"
#include "mcrl2/pbes/constelm.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/txt2pbes.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::pbes_system;

std::string t1 =
"% simple test, n is constant \n"
"                             \n"
"pbes nu X(n: Nat) =          \n"
"       X(n);                 \n"
"                             \n"
"init X(0);                   \n"
;
std::string x1 = "X n";

std::string t2 =
"% simple test, n is NOT constant \n"
"                                 \n"
"pbes nu X(n: Nat) =              \n"
"       X(n + 1);                 \n"
"                                 \n"
"init X(0);                       \n"
;
std::string x2 = "";

std::string t3 =
"% multiple parameters: n1 is constant, n2 is not \n"
"                                                 \n"
"pbes nu X(n1,n2: Nat) =                          \n"
"       X(n1, n2 + 1);                            \n"
"                                                 \n"
"init X(0, 1);                                    \n"
;
std::string x3 = "X n1";

std::string t4 =
"% conditions: n is not constant, even if conditions are enabled\n"
"                                                               \n"
"pbes nu X(n: Nat) =                                            \n"
"       val(2 < n) || X(n + 1);                                 \n"
"                                                               \n"
"init X(0);                                                     \n"
;
std::string x4 = "";

std::string t5 =
"% conditions: n is constant if conditions are enabled \n"
"                                                      \n"
"pbes nu X(n: Nat) =                                   \n"
"       val(2 < n) || X(n + 1);                        \n"
"                                                      \n"
"init X(5);                                            \n"
;
std::string x5 = "X n";

std::string t6 =
"% reachability: n1 is not constant, equation Y is not reachable and should be removed \n"
"                                                                                      \n"
"pbes nu X(n1: Nat) = X(n1+1);                                                         \n"
"     mu Y(n2: Nat) = Y(n2+1);                                                         \n"
"                                                                                      \n"
"init X(5);                                                                            \n"
;
std::string x6 = "Y";

std::string t7 =
"% multiple edges from one vertex, one edge invalidates the assertion of the other: no constants should be found \n"
"                                                                                                                \n"
"pbes                                                                                                            \n"
"   mu X1(n1,m1:Nat) = X2(n1) || X2(m1);                                                                         \n"
"   mu X2(n2:Nat)     = X1(n2,n2);                                                                               \n"
"init                                                                                                            \n"
"   X1(2,1);                                                                                                     \n"
;
std::string x7 = "";

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
std::string x8 = "X b c d";

std::string t9 =
"% conditions: universal quantification which can be solved, n1 and n2 are \n"
"% constants if conditions are enabled                                     \n"
"                                                                          \n"
"pbes mu X(n1,n2:Nat) =                                                    \n"
"    forall m:Nat. (val(n1>n2) && X(n1+1,n2+1));                           \n"
"                                                                          \n"
"init X(1,2);                                                              \n"
;
std::string x9 = "X n1 n2";

std::string t10 =
"% conditions: existential quantification which can be solved, n1 and n2 \n"
"% are constants if conditions are enabled                               \n"
"                                                                        \n"
"pbes mu X(n1,n2:Nat) =                                                  \n"
"    exists m:Nat. (val(n1>n2) && X(n1+1,n2+1));                         \n"
"                                                                        \n"
"init X(1,2);                                                            \n"
;
std::string x10 = "X n1 n2";

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
std::string x11 = "";

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
std::string x12 = "X1 o1\nX2 n2\nX4 n4 o4";

std::string t13 =
"pbes nu X =           \n"
"        Y(true);      \n"
"      mu Y(b: Bool) = \n"
"        X;            \n"
"                      \n"
"init X;               \n"
;
std::string x13 = "Y b";

std::string t14 =
"pbes nu X(m:Nat) =           \n"
"        forall n:Nat . X(n); \n"
"                             \n"
"init X(0);                   \n"
;
std::string x14 = "";

void test_pbes(const std::string& pbes_spec, std::string expected_result, bool compute_conditions, bool remove_equations = true)
{
  typedef simplifying_rewriter<pbes_expression, data::rewriter> my_pbes_rewriter;

  std::cout << "----------------------------------" << std::endl;
  std::cout << pbes_spec << std::endl;
  std::cout << "---" << std::endl;

  core::gsSetNormalMsg();
  pbes<> p = txt2pbes(pbes_spec);
  pbes<> q = p;
  core::gsSetVerboseMsg();

  // data rewriter
  data::rewriter datar(q.data());

  // pbes rewriter
  my_pbes_rewriter pbesr(datar);

  // constelm algorithm
  pbes_constelm_algorithm<pbes_expression, data::rewriter, my_pbes_rewriter> algorithm(datar, pbesr);

  // run the algorithm
  algorithm.run(q, compute_conditions, remove_equations);
  std::map<propositional_variable, std::vector<variable> > removed_parameters = algorithm.redundant_parameters();
  std::set<propositional_variable> removed_equations = algorithm.redundant_equations();

  std::set<std::string> lines1;
  for (std::map<propositional_variable, std::vector<variable> >::const_iterator i = removed_parameters.begin(); i != removed_parameters.end(); ++i)
  {
    std::string line = core::pp(i->first.name());
    std::set<std::string> v;
    for (std::vector<variable>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
    {
      v.insert(core::pp(*j));
    }
    for (std::set<std::string>::const_iterator j = v.begin(); j != v.end(); ++j)
    {
      line = line + " " + *j;
    }
    lines1.insert(line);
  }
  for (std::set<propositional_variable>::iterator i = removed_equations.begin(); i != removed_equations.end(); ++i)
  {
    lines1.insert(core::pp(i->name()));
  }

  std::set<std::string> lines2;
  boost::algorithm::split(lines2, expected_result, boost::algorithm::is_any_of("\n"));
  lines2.erase("");

  std::string s1;
  for (std::set<std::string>::iterator i = lines1.begin(); i != lines1.end(); ++i)
  {
    s1 = s1 + *i + "\n";
  }

  std::string s2;
  for (std::set<std::string>::iterator i = lines2.begin(); i != lines2.end(); ++i)
  {
    s2 = s2 + *i + "\n";
  }

  if (s1 != s2)
  {
    std::cout << "s1 = #" << s1 << "#" << std::endl;
    std::cout << "s2 = #" << s2 << "#" << std::endl;
  }
  BOOST_CHECK(s1 == s2);

  std::cout << core::pp(q) << std::endl;
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_pbes(t1 , x1 , false);
  core::garbage_collect();
  test_pbes(t2 , x2 , false);
  core::garbage_collect();
  test_pbes(t3 , x3 , false);
  core::garbage_collect();
  test_pbes(t4 , x4 , true);
  core::garbage_collect();
  test_pbes(t5 , x5 , true);
  core::garbage_collect();
  test_pbes(t6 , x6 , false);
  core::garbage_collect();
  test_pbes(t7 , x7 , false);
  core::garbage_collect();
  test_pbes(t8 , x8 , true);
  core::garbage_collect();
  test_pbes(t9 , x9 , true);
  core::garbage_collect();
  test_pbes(t10, x10, true);
  core::garbage_collect();
  test_pbes(t11, x11, true);
  core::garbage_collect();
  test_pbes(t12, x12, false);
  core::garbage_collect();
  test_pbes(t13, x13, false);
  core::garbage_collect();
  test_pbes(t14, x14, false);
  core::garbage_collect();

  return 0;
}
